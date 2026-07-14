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
    virtual void checkPassword() override;
    void navigateToHomeActive();
    /* USER CODE END */
protected:
};

#endif // PASSWORDSCREENVIEW_HPP