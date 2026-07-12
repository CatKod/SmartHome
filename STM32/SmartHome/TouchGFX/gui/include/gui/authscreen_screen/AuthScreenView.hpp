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
protected:
};

#endif // AUTHSCREENVIEW_HPP
