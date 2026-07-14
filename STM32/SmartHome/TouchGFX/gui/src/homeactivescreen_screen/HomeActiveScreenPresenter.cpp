#include <gui/homeactivescreen_screen/HomeActiveScreenView.hpp>
#include <gui/homeactivescreen_screen/HomeActiveScreenPresenter.hpp>

HomeActiveScreenPresenter::HomeActiveScreenPresenter(HomeActiveScreenView& v)
    : view(v)
{

}

void HomeActiveScreenPresenter::activate()
{

}

void HomeActiveScreenPresenter::deactivate()
{

}

/* USER CODE BEGIN */
void HomeActiveScreenPresenter::updateTime(int hour, int minute)
{
    view.updateTimeDisplay(hour, minute); // Forward time data to the view
}

void HomeActiveScreenPresenter::updateEnvironmentalData(float temp, int humi, int light)
{
    view.updateEnvDisplay(temp, humi, light);
}

void HomeActiveScreenPresenter::sendSecurityChangeCommand(bool active)
{
    model->sendHomeModeCmd(active); // Delegate to Model
}
/* USER CODE END */