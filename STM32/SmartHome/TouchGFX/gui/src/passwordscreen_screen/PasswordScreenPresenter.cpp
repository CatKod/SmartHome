#include <gui/passwordscreen_screen/PasswordScreenView.hpp>
#include <gui/passwordscreen_screen/PasswordScreenPresenter.hpp>

PasswordScreenPresenter::PasswordScreenPresenter(PasswordScreenView& v)
    : view(v)
{

}

void PasswordScreenPresenter::activate()
{

}

void PasswordScreenPresenter::deactivate()
{

}

/* USER CODE BEGIN */
void PasswordScreenPresenter::updateTime(int hour, int minute)
{
    view.updateTimeDisplay(hour, minute); // Forward time data to the view
}

void PasswordScreenPresenter::onAuthorizationSuccess()
{
    // Notify the view to navigate to the HomeActive screen when auth succeeds from H7
    view.navigateToHomeActive();
}

void PasswordScreenPresenter::onAuthorizationFailed()
{
    // Forward PIN failure event to update txtNoti
    view.showAuthFailedMessage();
}
/* USER CODE END */