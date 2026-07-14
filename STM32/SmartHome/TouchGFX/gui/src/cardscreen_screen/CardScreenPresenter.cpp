#include <gui/cardscreen_screen/CardScreenView.hpp>
#include <gui/cardscreen_screen/CardScreenPresenter.hpp>

CardScreenPresenter::CardScreenPresenter(CardScreenView& v)
    : view(v)
{

}

void CardScreenPresenter::activate()
{

}

void CardScreenPresenter::deactivate()
{

}

/* USER CODE BEGIN */
void CardScreenPresenter::onAuthorizationSuccess()
{
    // Notify the view to navigate to the HomeActive screen when auth succeeds from H7
    view.navigateToHomeActive();
}
/* USER CODE END */