#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

/* USER CODE BEGIN */
#ifdef __cplusplus
extern "C" {
#endif

// Global variables for receiving data from F4 via UART
extern float f4_rx_temperature;
extern int f4_rx_humidity;
extern int f4_rx_light;
extern bool f4_rx_is_logged_in;
extern bool f4_rx_warning_trigger;

#ifdef __cplusplus
}
#endif
/* USER CODE END */

Model::Model() : modelListener(0)
{
}

void Model::tick()
{
    /* USER CODE BEGIN */
    if (modelListener != 0)
    {
        // update the environmental data to the view
        modelListener->updateEnvironmentalData(f4_rx_temperature, f4_rx_humidity, f4_rx_light);

        // check if the login state has changed
        static bool lastLoginState = false;
        if (f4_rx_is_logged_in != lastLoginState)
        {
            lastLoginState = f4_rx_is_logged_in;
            if (f4_rx_is_logged_in)
            {
                modelListener->onAuthorizationSuccess();
            }
        }

        // check if the security alert state has changed
        static bool lastWarningState = false;
        if (f4_rx_warning_trigger != lastWarningState)
        {
            lastWarningState = f4_rx_warning_trigger;
            modelListener->onSecurityAlert(f4_rx_warning_trigger);
        }
    }
    /* USER CODE END */
}