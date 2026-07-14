#include <gui/settingscreen_screen/SettingScreenView.hpp>
#include <gui/settingscreen_screen/SettingScreenPresenter.hpp>

SettingScreenPresenter::SettingScreenPresenter(SettingScreenView& v)
    : view(v)
{

}

void SettingScreenPresenter::activate()
{

}

void SettingScreenPresenter::deactivate()
{

}

/* USER CODE BEGIN */
void SettingScreenPresenter::updateTime(int hour, int minute)
{
    view.updateTimeDisplay(hour, minute); // Forward time data to the view
}
/* USER CODE END */