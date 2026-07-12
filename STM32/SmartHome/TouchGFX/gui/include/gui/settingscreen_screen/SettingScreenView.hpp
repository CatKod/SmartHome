#ifndef SETTINGSCREENVIEW_HPP
#define SETTINGSCREENVIEW_HPP

#include <gui_generated/settingscreen_screen/SettingScreenViewBase.hpp>
#include <gui/settingscreen_screen/SettingScreenPresenter.hpp>

class SettingScreenView : public SettingScreenViewBase
{
public:
    SettingScreenView();
    virtual ~SettingScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SETTINGSCREENVIEW_HPP
