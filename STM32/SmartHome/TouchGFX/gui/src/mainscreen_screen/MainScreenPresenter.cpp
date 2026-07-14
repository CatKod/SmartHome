#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

MainScreenPresenter::MainScreenPresenter(MainScreenView& v)
    : view(v)
{

}

void MainScreenPresenter::activate()
{

}

void MainScreenPresenter::deactivate()
{

}

/* USER CODE BEGIN */
void MainScreenPresenter::updateEnvironmentalData(float temp, int humi, int light)
{
    view.updateEnvDisplay(temp, humi, light);
}

void MainScreenPresenter::onAuthorizationSuccess()
{
    view.navigateToHomeActive();
}

void MainScreenPresenter::updateTime(int hour, int minute)
{
    view.updateTimeDisplay(hour, minute); // Pass clock tick directly to view
}
/* USER CODE END */