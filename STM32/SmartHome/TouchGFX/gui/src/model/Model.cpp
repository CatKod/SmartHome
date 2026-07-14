#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

/* USER CODE BEGIN */
#ifndef SIMULATOR
#ifdef __cplusplus
extern "C" {
#endif

// Global variables imported from f4_uart_link.c
extern float f4_rx_temperature;
extern int f4_rx_humidity;
extern int f4_rx_light;
extern bool f4_rx_is_logged_in;
extern bool f4_rx_warning_trigger;

// Transmit command function from f4_uart_link.c
extern void F4_UART_SendHomeMode(bool active);

#ifdef __cplusplus
}
#endif
#endif // SIMULATOR
/* USER CODE END */

Model::Model() : modelListener(0), currentTemp(0.0f), currentHumi(0), currentLight(0), lastLoginState(false), lastWarningState(false)
{
}

void Model::tick()
{
    /* USER CODE BEGIN */
    bool loggedIn = false;
    bool isWarning = false;

#ifndef SIMULATOR
    // Running on physical STM32 MCU: Fetch real-time data from UART
    currentTemp = f4_rx_temperature;
    currentHumi = f4_rx_humidity;
    currentLight = f4_rx_light;
    loggedIn = f4_rx_is_logged_in;
    isWarning = f4_rx_warning_trigger;
#else
    // Running on PC Simulator: Generate mock data for UI testing
    static int tickCount = 0;
    if (tickCount++ % 60 == 0)
    {
        currentTemp = 26.5f;
        currentHumi = 60;
        currentLight = 85;
    }
    loggedIn = false; // Manually toggle in debugger if testing navigation
    isWarning = false;
#endif

    if (modelListener != 0)
    {
        // 1. Dispatch real-time environmental data to the active view
        modelListener->updateEnvironmentalData(currentTemp, currentHumi, currentLight);

        // 2. Trigger authorization success event if login state flips to true
        if (loggedIn != lastLoginState)
        {
            lastLoginState = loggedIn;
            if (loggedIn)
            {
                modelListener->onAuthorizationSuccess();
            }
        }

        // 3. Trigger security alert event if warning state changes
        if (isWarning != lastWarningState)
        {
            lastWarningState = isWarning;
            modelListener->onSecurityAlert(isWarning);
        }
    }
    /* USER CODE END */
}

/* USER CODE BEGIN */
void Model::sendHomeModeCmd(bool active)
{
#ifndef SIMULATOR
    // Route the command via UART strictly when deployed on the physical board
    F4_UART_SendHomeMode(active);
#endif
}
/* USER CODE END */