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
    // txtTemp
    Unicode::snprintfFloat(txtTempBuffer, TXTTEMP_SIZE, "%.1f", temp);
    txtTemp.invalidate();

    // txtHumi
    Unicode::snprintf(txtHumiBuffer, TXTHUMI_SIZE, "%03d", humi);
    txtHumi.invalidate();

    // txtBright
    Unicode::snprintf(txtBrightBuffer, TXTBRIGHT_SIZE, "%03d", light);
    txtBright.invalidate();
}

void MainScreenView::navigateToHomeActive()
{
    application().gotoHomeActiveScreenNoTransition();
}
/* USER CODE END */