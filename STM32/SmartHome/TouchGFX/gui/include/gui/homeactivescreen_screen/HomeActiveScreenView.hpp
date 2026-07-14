#ifndef HOMEACTIVESCREENVIEW_HPP
#define HOMEACTIVESCREENVIEW_HPP

#include <gui_generated/homeactivescreen_screen/HomeActiveScreenViewBase.hpp>
#include <gui/homeactivescreen_screen/HomeActiveScreenPresenter.hpp>

class HomeActiveScreenView : public HomeActiveScreenViewBase
{
public:
    HomeActiveScreenView();
    virtual ~HomeActiveScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    /* USER CODE BEGIN */
    virtual void activateSecurityMode() override;
    void updateEnvDisplay(float temp, int humi, int light);
    /* USER CODE END */

protected:
    /* USER CODE BEGIN */
    static const uint16_t TXTTEMP_SIZE = 10;
    static const uint16_t TXTHUMI_SIZE = 10;
    static const uint16_t TXTBRIGHT_SIZE = 10;
    /* USER CODE END */
};

#endif // HOMEACTIVESCREENVIEW_HPP
