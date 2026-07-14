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
void HomeActiveScreenPresenter::updateEnvironmentalData(float temp, int humi, int light)
{
    view.updateEnvDisplay(temp, humi, light);
}
/* USER CODE END */

/* USER CODE BEGIN */
void HomeActiveScreenPresenter::sendSecurityChangeCommand(bool active)
{
    model->sendHomeModeCmd(active); // Delegate to Model
}
/* USER CODE END */