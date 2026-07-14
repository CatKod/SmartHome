#include <gui/securityscreen_screen/SecurityScreenView.hpp>
#include <gui/securityscreen_screen/SecurityScreenPresenter.hpp>

SecurityScreenPresenter::SecurityScreenPresenter(SecurityScreenView& v)
    : view(v)
{

}

void SecurityScreenPresenter::activate()
{

}

void SecurityScreenPresenter::deactivate()
{

}

/* USER CODE BEGIN */
void SecurityScreenPresenter::updateTime(int hour, int minute)
{
    view.updateTimeDisplay(hour, minute); // Forward time data to the view
}
/* USER CODE END */