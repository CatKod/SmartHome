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