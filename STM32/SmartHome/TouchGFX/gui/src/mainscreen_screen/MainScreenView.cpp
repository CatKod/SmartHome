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
    // Format numeric values into the designated Unicode buffers
    Unicode::snprintf(txtTimeBuffer1, sizeof(txtTimeBuffer1) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", hour);
    Unicode::snprintf(txtTimeBuffer2, sizeof(txtTimeBuffer2) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", minute);
    
    // Explicitly force TouchGFX to reload text from the wildcards before rendering
    txtTime.setWildcard1(txtTimeBuffer1);
    txtTime.setWildcard2(txtTimeBuffer2);

    // Request graphic hardware to redraw the modified text area
    txtTime.invalidate();
}

void MainScreenView::updateEnvDisplay(float temp, int humi, int light)
{
    // Format environmental variables into the text buffers
    Unicode::snprintfFloat(txtTempBuffer, TXTTEMP_SIZE, "%.1f", temp);
    Unicode::snprintf(txtHumiBuffer, TXTHUMI_SIZE, "%d", humi);
    Unicode::snprintf(txtBrightBuffer, TXTBRIGHT_SIZE, "%d", light);

    // Synchronize buffers to widgets to ensure the display updates on physical MCU
    txtTemp.setWildcard(txtTempBuffer);
    txtHumi.setWildcard(txtHumiBuffer);
    txtBright.setWildcard(txtBrightBuffer);

    // Invalidate widgets to force Chrome-ART (DMA2D) memory refresh
    txtTemp.invalidate();
    txtHumi.invalidate();
    txtBright.invalidate();
}

void MainScreenView::navigateToHomeActive()
{
    application().gotoHomeActiveScreenScreenNoTransition();
}
/* USER CODE END */
