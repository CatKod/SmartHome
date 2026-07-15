#include <gui/passwordscreen_screen/PasswordScreenView.hpp>
#include <touchgfx/Color.hpp>
#include <stdio.h>
#include <string.h>

// Color definitions for Box widgets (RGB888)
#define COLOR_BORDER_FOCUS   touchgfx::Color::getColorFromRGB(0, 255, 255)   // Bright Cyan color
#define COLOR_BORDER_NORMAL  touchgfx::Color::getColorFromRGB(64, 64, 64)    // Dim Gray color

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

void PasswordScreenView::updateBoxFocus()
{
    // Array of pointers to easily loop through your predefined UI components
    touchgfx::Box* boxes[] = { &boxPin1, &boxPin2, &boxPin3, &boxPin4 };
    
    for (int i = 0; i < 4; i++)
    {
        // If it's the active box being typed, brighten its border background
        if (i == currentPinIndex) {
            boxes[i]->setColor(COLOR_BORDER_FOCUS);
        } else {
            boxes[i]->setColor(COLOR_BORDER_NORMAL);
        }
        boxes[i]->invalidate();
    }
}

void PasswordScreenView::handleKeyInput(char key)
{
    touchgfx::TextAreaWithOneWildcard* pinTexts[] = { &txtPin1, &txtPin2, &txtPin3, &txtPin4 };
    touchgfx::Unicode::UnicodeChar* buffers[] = { txtPin1Buffer, txtPin2Buffer, txtPin3Buffer, txtPin4Buffer };

    if (key >= '0' && key <= '9' && currentPinIndex < 4)
    {
        pinValue[currentPinIndex] = key;
        Unicode::snprintf(buffers[currentPinIndex], 2, "*"); // Display masked password character
        pinTexts[currentPinIndex]->invalidate();
        
        currentPinIndex++;
        updateBoxFocus();
    }
    else if (key == '*' && currentPinIndex > 0) // Backspace logic mapped to asterisk symbol
    {
        currentPinIndex--;
        pinValue[currentPinIndex] = '\0';
        Unicode::snprintf(buffers[currentPinIndex], 2, ""); // Clear the character mask
        pinTexts[currentPinIndex]->invalidate();
        updateBoxFocus();
    }
}

void PasswordScreenView::clearPinFields()
{
    touchgfx::TextAreaWithOneWildcard* pinTexts[] = { &txtPin1, &txtPin2, &txtPin3, &txtPin4 };
    touchgfx::Unicode::UnicodeChar* buffers[] = { txtPin1Buffer, txtPin2Buffer, txtPin3Buffer, txtPin4Buffer };

    memset(pinValue, 0, sizeof(pinValue));
    currentPinIndex = 0;
    
    for (int i = 0; i < 4; i++)
    {
        Unicode::snprintf(buffers[i], 2, "");
        pinTexts[i]->invalidate();
    }
    updateBoxFocus();
}

void PasswordScreenView::showAuthFailedMessage()
{
    // Increment the local mismatch counter
    this->wrongPinCount++;
    
    char alertMsg[40];
    // Format the failure message with current invalid count
    snprintf(alertMsg, sizeof(alertMsg), "Wrong PIN! Failures: %d", this->wrongPinCount);
    
    // Display on the txtNoti area and reset input boxes
    showAuthNotification(alertMsg);
    clearPinFields();
}

void PasswordScreenView::showAuthNotification(const char* message)
{
    touchgfx::Unicode::strncpy(this->txtNotiBuffer, message, sizeof(this->txtNotiBuffer) / sizeof(touchgfx::Unicode::UnicodeChar));
    
    this->txtNoti.invalidate(); // Redraw the text area to show the notification
}

void PasswordScreenView::checkPassword()
{
    if (currentPinIndex < 4)
    {
        showAuthNotification("PIN must be 4 digits!");
        return;
    }
    
    pinValue[4] = '\0';
    showAuthNotification("Verifying PIN...");
    
    // Send the extracted PIN buffer to H7 via Presenter for physical verification
    // presenter->sendPinToH7(pinValue); 
    
    // For test fallback: if UART fails or you want to track wrong attempts locally
    // If Model reports failure later through Presenter, call tracking logic:
    // wrongPinCount++;
    // char alertMsg[40];
    // snprintf(alertMsg, sizeof(alertMsg), "Wrong PIN! Failures: %d", wrongPinCount);
    // showAuthNotification(alertMsg);
    // clearPinFields();
}

void PasswordScreenView::navigateToHomeActive()
{
    showAuthNotification("Success! Welcome.");
    application().gotoHomeActiveScreenScreenNoTransition();
}
/* USER CODE END */