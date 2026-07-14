#include <gui/cardscreen_screen/CardScreenView.hpp>

CardScreenView::CardScreenView()
{
}

void CardScreenView::setupScreen()
{
    CardScreenViewBase::setupScreen();
}

void CardScreenView::tearDownScreen()
{
    CardScreenViewBase::tearDownScreen();
}

/* USER CODE BEGIN */
void CardScreenView::updateTimeDisplay(int hour, int minute)
{
    Unicode::snprintf(this->txtTimeBuffer1, sizeof(this->txtTimeBuffer1) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", hour);
    Unicode::snprintf(this->txtTimeBuffer2, sizeof(this->txtTimeBuffer2) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", minute);
    this->txtTime.invalidate(); 
}

void CardScreenView::navigateToHomeActive()
{
    // Execute screen transition ONLY when the presenter receives successful auth signal from H7
    application().gotoHomeActiveScreenScreenNoTransition();
}
/* USER CODE END */