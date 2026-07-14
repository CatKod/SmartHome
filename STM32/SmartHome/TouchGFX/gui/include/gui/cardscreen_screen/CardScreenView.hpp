#ifndef CARDSCREENVIEW_HPP
#define CARDSCREENVIEW_HPP

#include <gui_generated/cardscreen_screen/CardScreenViewBase.hpp>
#include <gui/cardscreen_screen/CardScreenPresenter.hpp>

class CardScreenView : public CardScreenViewBase
{
public:
    CardScreenView();
    virtual ~CardScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    /* USER CODE BEGIN */
    void navigateToHomeActive();
    /* USER CODE END */
protected:
};

#endif // CARDSCREENVIEW_HPP