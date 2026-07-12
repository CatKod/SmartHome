#ifndef SECURITYSCREENVIEW_HPP
#define SECURITYSCREENVIEW_HPP

#include <gui_generated/securityscreen_screen/SecurityScreenViewBase.hpp>
#include <gui/securityscreen_screen/SecurityScreenPresenter.hpp>

class SecurityScreenView : public SecurityScreenViewBase
{
public:
    SecurityScreenView();
    virtual ~SecurityScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SECURITYSCREENVIEW_HPP
