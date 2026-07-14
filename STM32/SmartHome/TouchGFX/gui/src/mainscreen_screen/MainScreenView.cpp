#include <gui/mainscreen_screen/MainScreenView.hpp>

MainScreenView::MainScreenView()
{

}

void MainScreenView::setupScreen()
{
    MainScreenViewBase::setupScreen();
}

void MainScreenView::tearDownScreen()
{
    MainScreenViewBase::tearDownScreen();
}

/* USER CODE BEGIN */
void MainScreenView::updateTimeDisplay(int hour, int minute)
{
    // Use sizeof to securely get buffer size if macro identifiers are not cached yet
    Unicode::snprintf(txtTimeBuffer1, sizeof(txtTimeBuffer1) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", hour);
    Unicode::snprintf(txtTimeBuffer2, sizeof(txtTimeBuffer2) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", minute);
    
    txtTime.invalidate();
}

void MainScreenView::updateEnvDisplay(float temp, int humi, int light)
{
    // Update environmental metrics on the locked screen
    Unicode::snprintfFloat(txtTempBuffer, TXTTEMP_SIZE, "%.1f", temp);
    txtTemp.invalidate();

    Unicode::snprintf(txtHumiBuffer, TXTHUMI_SIZE, "%d", humi);
    txtHumi.invalidate();

    Unicode::snprintf(txtBrightBuffer, TXTBRIGHT_SIZE, "%d", light);
    txtBright.invalidate();
}

void MainScreenView::navigateToHomeActive()
{
    // Jump to dashboard bypassing the login process (e.g. valid external bypass)
    application().gotoHomeActiveScreenScreenNoTransition();
}
/* USER CODE END */