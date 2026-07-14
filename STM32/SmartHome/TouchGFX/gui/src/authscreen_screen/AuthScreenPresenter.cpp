#include <gui/authscreen_screen/AuthScreenView.hpp>
#include <gui/authscreen_screen/AuthScreenPresenter.hpp>

AuthScreenPresenter::AuthScreenPresenter(AuthScreenView& v)
    : view(v)
{

}

void AuthScreenPresenter::activate()
{

}

void AuthScreenPresenter::deactivate()
{

}

/* USER CODE BEGIN */
void AuthScreenPresenter::updateTime(int hour, int minute)
{
    view.updateTimeDisplay(hour, minute); // Forward time data to the view
}
/* USER CODE END */