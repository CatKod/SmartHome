#include <gui/authscreen_screen/AuthScreenView.hpp>

AuthScreenView::AuthScreenView()
{

}

void AuthScreenView::setupScreen()
{
    AuthScreenViewBase::setupScreen();
}

void AuthScreenView::tearDownScreen()
{
    AuthScreenViewBase::tearDownScreen();
}

/* USER CODE BEGIN */
void AuthScreenView::updateTimeDisplay(int hour, int minute)
{
    // Update wildcards safely using this-> pointer to clear IntelliSense errors
    Unicode::snprintf(this->txtTimeBuffer1, sizeof(this->txtTimeBuffer1) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", hour);
    Unicode::snprintf(this->txtTimeBuffer2, sizeof(this->txtTimeBuffer2) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", minute);
    this->txtTime.invalidate(); // Redraw the clock widget
}
/* USER CODE END */