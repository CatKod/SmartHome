#include <gui/settingscreen_screen/SettingScreenView.hpp>

SettingScreenView::SettingScreenView()
{

}

void SettingScreenView::setupScreen()
{
    SettingScreenViewBase::setupScreen();
}

void SettingScreenView::tearDownScreen()
{
    SettingScreenViewBase::tearDownScreen();
}

/* USER CODE BEGIN */
void SettingScreenView::updateTimeDisplay(int hour, int minute)
{
    Unicode::snprintf(this->txtTimeBuffer1, sizeof(this->txtTimeBuffer1) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", hour);
    Unicode::snprintf(this->txtTimeBuffer2, sizeof(this->txtTimeBuffer2) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", minute);
    this->txtTime.invalidate(); 
}
/* USER CODE END */