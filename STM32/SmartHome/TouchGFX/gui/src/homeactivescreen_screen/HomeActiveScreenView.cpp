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

    // FIXED: Synchronize clock buffers into the dynamic clock text field
    this->txtTime.setWildcard1(this->txtTimeBuffer1);
    this->txtTime.setWildcard2(this->txtTimeBuffer2);

    this->txtTime.invalidate(); 
}

void HomeActiveScreenView::updateEnvDisplay(float temp, int humi, int light)
{
    // Convert current parameters into safe array sequences
    Unicode::snprintfFloat(txtTempBuffer, TXTTEMP_SIZE, "%.1f", temp);
    Unicode::snprintf(txtHumiBuffer, TXTHUMI_SIZE, "%d", humi);
    Unicode::snprintf(txtBrightBuffer, TXTBRIGHT_SIZE, "%d", light);

    // Bind mutated data arrays into active screen panel elements
    txtTemp.setWildcard(txtTempBuffer);
    txtHumi.setWildcard(txtHumiBuffer);
    txtBright.setWildcard(txtBrightBuffer);

    // Trigger visual memory redraw
    txtTemp.invalidate(); 
    txtHumi.invalidate();
    txtBright.invalidate();
}

void HomeActiveScreenView::activateSecurityMode()
{
    presenter->sendSecurityChangeCommand(false);
    application().gotoAuthScreenScreenNoTransition();
}
/* USER CODE END */
