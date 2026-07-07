import 'dotenv/config'

import { serve } from '@hono/node-server'
import { Hono } from 'hono'
import { cors } from 'hono/cors'
import { HTTPException } from 'hono/http-exception'
import { AuthError } from '@supabase/server'
import { withSupabase } from '@supabase/server/adapters/hono'

const app = new Hono()

app.use('*', cors())

// Public — no credentials required. Useful for uptime/health probes.
app.get('/health', (c) =>
  c.json({ status: 'ok', service: 'smarthome-backend' }),
)

// Publishable-key gated — the mobile app can browse public data before login.
// Send the key in the `apikey` header. RLS still applies (anon role).
app.get('/api/catalog', withSupabase({ auth: 'publishable' }), async (c) => {
  const { supabase } = c.var.supabaseContext
  const { data, error } = await supabase.from('device_catalog').select()
  if (error) return c.json({ error: error.message }, 400)
  return c.json({ catalog: data })
})

// Authenticated user — returns the identity decoded from the JWT.
app.get('/api/me', withSupabase({ auth: 'user' }), (c) => {
  const { userClaims } = c.var.supabaseContext
  return c.json({ user: userClaims })
})

// Authenticated user — lists the signed-in user's devices (RLS-scoped).
app.get('/api/devices', withSupabase({ auth: 'user' }), async (c) => {
  const { supabase } = c.var.supabaseContext
  const { data, error } = await supabase.from('devices').select()
  if (error) return c.json({ error: error.message }, 400)
  return c.json({ devices: data })
})

// Server-to-server — an ESP32/STM32 gateway or cron pushes telemetry using the
// secret key. `supabaseAdmin` bypasses RLS, so guard this endpoint carefully.
app.post('/api/telemetry', withSupabase({ auth: 'secret' }), async (c) => {
  const body = await c.req.json().catch(() => null)
  if (body === null) return c.json({ error: 'Invalid JSON body' }, 400)

  const { supabaseAdmin } = c.var.supabaseContext
  const { data, error } = await supabaseAdmin
    .from('telemetry')
    .insert(body)
    .select()
  if (error) return c.json({ error: error.message }, 400)
  return c.json({ inserted: data }, 201)
})

// Turn auth failures (and other HTTP exceptions) into clean JSON responses.
app.onError((err, c) => {
  if (err instanceof HTTPException && err.cause instanceof AuthError) {
    const authError = err.cause
    return c.json(
      { error: authError.message, code: authError.code },
      authError.status as 401 | 500,
    )
  }
  if (err instanceof HTTPException) {
    return c.json({ error: err.message }, err.status)
  }
  console.error(err)
  return c.json({ error: 'Internal server error' }, 500)
})

const port = Number(process.env.PORT ?? 3000)
serve({ fetch: app.fetch, port }, (info) => {
  console.log(`SmartHome backend listening on http://localhost:${info.port}`)
})
