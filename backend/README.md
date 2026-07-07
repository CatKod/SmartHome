# SmartHome Backend

A stateless backend API for the SmartHome project, built with
[Hono](https://hono.dev) and
[`@supabase/server`](https://github.com/supabase/server). It handles Supabase
auth verification and creates request-scoped Supabase clients so the Flutter app
and the ESP32/STM32 gateways can talk to Supabase through a single API.

## Requirements

- Node.js >= 20 (developed on Node 22)
- A Supabase project (URL, publishable key, secret key, JWKS URL)

## Setup

1. Install dependencies:

```bash
npm install
```

2. Configure environment variables. Copy the template and fill in your values:

```bash
cp .env.example .env
```

`.env` is gitignored. Set the following (see Supabase dashboard → API Keys):

| Variable                   | Description                                  |
| -------------------------- | -------------------------------------------- |
| `SUPABASE_URL`             | Project URL (`https://<ref>.supabase.co`)    |
| `SUPABASE_PUBLISHABLE_KEY` | Publishable key (`sb_publishable_...`)       |
| `SUPABASE_SECRET_KEY`      | Secret key (`sb_secret_...`) — keep private  |
| `SUPABASE_JWKS_URL`        | Remote JWKS endpoint for JWT verification    |
| `PORT`                     | Local HTTP port (default `3000`)             |

> The secret key pasted during setup was masked, so `.env` currently contains a
> placeholder — replace it with your full `sb_secret_...` key before using the
> `secret`-auth endpoints.

## Run

```bash
npm run dev        # watch mode (tsx)
npm run build      # compile TypeScript to dist/
npm start          # run the compiled server
npm run typecheck  # type-check without emitting
```

The server listens on `http://localhost:3000`.

## Endpoints

| Method | Path             | Auth mode     | Notes                                             |
| ------ | ---------------- | ------------- | ------------------------------------------------- |
| GET    | `/health`        | none          | Uptime/health probe.                              |
| GET    | `/api/catalog`   | `publishable` | Public catalog; send key in `apikey` header.      |
| GET    | `/api/me`        | `user`        | Returns identity from the caller's JWT.           |
| GET    | `/api/devices`   | `user`        | Lists the signed-in user's devices (RLS-scoped).  |
| POST   | `/api/telemetry` | `secret`      | Server-to-server ingest; `supabaseAdmin` (no RLS).|

### Auth modes (from `@supabase/server`)

- `user` — requires a valid Supabase JWT in the `Authorization: Bearer` header.
- `publishable` — validates the `apikey` header against the publishable key; the
  client stays anonymous (RLS still applies).
- `secret` — validates the `apikey` header against the secret key; grants an
  admin client that bypasses RLS. Use only for trusted server-to-server calls.
- `none` — open, no credentials required.

The example routes reference tables (`device_catalog`, `devices`, `telemetry`)
that don't exist yet — create them in Supabase (with appropriate RLS policies)
or adjust the handlers to match your schema.

## Notes

The example tables reference names for illustration; endpoints will return a
Supabase error until the corresponding tables and RLS policies exist. See the
[`@supabase/server` docs](https://github.com/supabase/server) for adapters,
auth modes, named keys, and primitives.
