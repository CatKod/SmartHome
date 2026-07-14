#include <gui/passwordscreen_screen/PasswordScreenView.hpp>

PasswordScreenView::PasswordScreenView()
{
}

void PasswordScreenView::setupScreen()
{
    PasswordScreenViewBase::setupScreen();
}

void PasswordScreenView::tearDownScreen()
{
    PasswordScreenViewBase::tearDownScreen();
}

/* USER CODE BEGIN */
void PasswordScreenView::updateTimeDisplay(int hour, int minute)
{
    Unicode::snprintf(this->txtTimeBuffer1, sizeof(this->txtTimeBuffer1) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", hour);
    Unicode::snprintf(this->txtTimeBuffer2, sizeof(this->txtTimeBuffer2) / sizeof(touchgfx::Unicode::UnicodeChar), "%02d", minute);
    this->txtTime.invalidate(); 
}

void PasswordScreenView::checkPassword()
{
    // TODO: Extract the entered PIN from txtPin elements
    // Pass the PIN array/string to the presenter to transmit via UART
    // Example: presenter->sendPinForVerification(enteredPin);
    
    // NOTE: Do NOT call change screen here. The transition is governed by the H7 logic.
}

void PasswordScreenView::navigateToHomeActive()
{
    // Execute screen transition ONLY when the H7 verifies the PIN is correct
    application().gotoHomeActiveScreenScreenNoTransition();
}
/* USER CODE END */