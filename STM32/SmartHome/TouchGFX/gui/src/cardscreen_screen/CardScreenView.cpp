#include <gui/cardscreen_screen/CardScreenView.hpp>

CardScreenView::CardScreenView()
{
}

void CardScreenView::setupScreen()
{
    CardScreenViewBase::setupScreen();
}

void CardScreenView::tearDownScreen()
{
    CardScreenViewBase::tearDownScreen();
}

/* USER CODE BEGIN */
void CardScreenView::navigateToHomeActive()
{
    // Execute screen transition ONLY when the presenter receives successful auth signal from H7
    application().gotoHomeActiveScreenScreenNoTransition();
}
/* USER CODE END */