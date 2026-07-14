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

    /* USER CODE BEGIN */
    void updateTimeDisplay(int hour, int minute);
    /* USER CODE END */
protected:
};

#endif // SETTINGSCREENVIEW_HPP
