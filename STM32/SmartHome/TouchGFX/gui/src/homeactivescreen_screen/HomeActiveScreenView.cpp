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
void HomeActiveScreenView::updateTimeDisplay(int hour, int minute)
{
    Unicode::snprintf(this->txtTimeBuffer1, sizeof(this->txtTimeBuffer1) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", hour);
    Unicode::snprintf(this->txtTimeBuffer2, sizeof(this->txtTimeBuffer2) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", minute);
    this->txtTime.invalidate(); 
}

void HomeActiveScreenView::updateEnvDisplay(float temp, int humi, int light)
{
    // Seamlessly update text wildcards while swiping between dashboard containers
    Unicode::snprintfFloat(txtTempBuffer, TXTTEMP_SIZE, "%.1f", temp);
    txtTemp.invalidate(); 

    Unicode::snprintf(txtHumiBuffer, TXTHUMI_SIZE, "%d", humi);
    txtHumi.invalidate();

    Unicode::snprintf(txtBrightBuffer, TXTBRIGHT_SIZE, "%d", light);
    txtBright.invalidate();
}

void HomeActiveScreenView::activateSecurityMode()
{
    // Transmit command to H7 to toggle the physical security relay/lock
    presenter->sendSecurityChangeCommand(false);

    // Return to the Authentication screen to require re-login
    application().gotoAuthScreenScreenNoTransition();
}
/* USER CODE END */