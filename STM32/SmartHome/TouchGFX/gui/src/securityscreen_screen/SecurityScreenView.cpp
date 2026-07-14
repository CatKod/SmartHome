#include <gui/securityscreen_screen/SecurityScreenView.hpp>

SecurityScreenView::SecurityScreenView()
{

}

void SecurityScreenView::setupScreen()
{
    SecurityScreenViewBase::setupScreen();
}

void SecurityScreenView::tearDownScreen()
{
    SecurityScreenViewBase::tearDownScreen();
}

/* USER CODE BEGIN */
void SecurityScreenView::updateTimeDisplay(int hour, int minute)
{
    Unicode::snprintf(this->txtTimeBuffer1, sizeof(this->txtTimeBuffer1) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", hour);
    Unicode::snprintf(this->txtTimeBuffer2, sizeof(this->txtTimeBuffer2) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", minute);
    this->txtTime.invalidate(); 
}
/* USER CODE END */