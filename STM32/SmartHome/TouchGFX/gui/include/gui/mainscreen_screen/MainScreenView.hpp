#ifndef MAINSCREENVIEW_HPP
#define MAINSCREENVIEW_HPP

#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

class MainScreenView : public MainScreenViewBase
{
public:
    MainScreenView();
    virtual ~MainScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    /* USER CODE BEGIN */
    void updateTimeDisplay(int hour, int minute);
    void updateEnvDisplay(float temp, int humi, int light);
    void navigateToHomeActive();
    /* USER CODE END */

protected:
    /* USER CODE BEGIN */
    static const uint16_t TXTTEMP_SIZE = 10;
    static const uint16_t TXTHUMI_SIZE = 10;
    static const uint16_t TXTBRIGHT_SIZE = 10;
    /* USER CODE END */
};

#endif // MAINSCREENVIEW_HPP
