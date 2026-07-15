#ifndef PASSWORDSCREENVIEW_HPP
#define PASSWORDSCREENVIEW_HPP

#include <gui_generated/passwordscreen_screen/PasswordScreenViewBase.hpp>
#include <gui/passwordscreen_screen/PasswordScreenPresenter.hpp>

class PasswordScreenView : public PasswordScreenViewBase
{
public:
    PasswordScreenView();
    virtual ~PasswordScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    /* USER CODE BEGIN */
    void updateTimeDisplay(int hour, int minute);
    void navigateToHomeActive();
    void checkPassword();

    // Function to handle physical keypad inputs forwarded from Model/Presenter
    void handleKeyInput(char key);
    void showAuthNotification(const char* message);
    void showAuthFailedMessage();

private:
    char pinValue[5];         // Store 4 digits + null terminator
    uint8_t currentPinIndex;   // Track current input box (0 to 3)
    uint16_t wrongPinCount;    // Counter for invalid PIN attempts

    // Helper functions to change focus and refresh border colors
    void updateBoxFocus();
    void clearPinFields();
/* USER CODE END */

protected:
};

#endif // PASSWORDSCREENVIEW_HPP