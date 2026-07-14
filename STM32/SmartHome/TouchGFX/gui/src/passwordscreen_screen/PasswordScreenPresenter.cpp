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
void PasswordScreenPresenter::onAuthorizationSuccess()
{
    // Notify the view to navigate to the HomeActive screen when auth succeeds from H7
    view.navigateToHomeActive();
}
/* USER CODE END */