#include <gui/homeactivescreen_screen/HomeActiveScreenView.hpp>

HomeActiveScreenView::HomeActiveScreenView()
{

}

void HomeActiveScreenView::setupScreen()
{
    HomeActiveScreenViewBase::setupScreen();
}

void HomeActiveScreenView::tearDownScreen()
{
    HomeActiveScreenViewBase::tearDownScreen();
}

/* USER CODE BEGIN */
void HomeActiveScreenView::activateSecurityMode()
{
    // Send a command to the F4 MCU to activate security mode
    presenter->sendSecurityChangeCommand(false);

    // Navigate to the main screen without transition
    application().gotoMainScreenNoTransition();
}

void HomeActiveScreenView::updateEnvDisplay(float temp, int humi, int light)
{
    // push the environmental data to the text areas
    Unicode::snprintfFloat(txtTempBuffer, TXTTEMP_SIZE, "%.1f", temp);
    txtTemp.invalidate(); 

    Unicode::snprintf(txtHumiBuffer, TXTHUMI_SIZE, "%03d", humi);
    txtHumi.invalidate();

    Unicode::snprintf(txtBrightBuffer, TXTBRIGHT_SIZE, "%03d", light);
    txtBright.invalidate();
}
/* USER CODE END */