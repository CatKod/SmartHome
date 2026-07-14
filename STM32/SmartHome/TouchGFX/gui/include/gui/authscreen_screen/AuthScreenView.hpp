#ifndef AUTHSCREENVIEW_HPP
#define AUTHSCREENVIEW_HPP

#include <gui_generated/authscreen_screen/AuthScreenViewBase.hpp>
#include <gui/authscreen_screen/AuthScreenPresenter.hpp>

class AuthScreenView : public AuthScreenViewBase
{
public:
    AuthScreenView();
    virtual ~AuthScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    /* USER CODE BEGIN */
    void updateTimeDisplay(int hour, int minute);
    /* USER CODE END */
protected:
};

#endif // AUTHSCREENVIEW_HPP
