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
protected:
};

#endif // PASSWORDSCREENVIEW_HPP
