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
    touchgfx::Unicode::strncpy(this->txtNotiBuffer, "Access Granted!", sizeof(this->txtNotiBuffer) / sizeof(touchgfx::Unicode::UnicodeChar));
    this->txtNoti.invalidate();

    // Execute screen transition securely via MVP framework
    application().gotoHomeActiveScreenScreenNoTransition();
}

void CardScreenView::showAuthFailedMessage()
{
    touchgfx::Unicode::strncpy(this->txtNotiBuffer, "Invalid Card!", sizeof(this->txtNotiBuffer) / sizeof(touchgfx::Unicode::UnicodeChar));
    this->txtNoti.invalidate();
}
/* USER CODE END */