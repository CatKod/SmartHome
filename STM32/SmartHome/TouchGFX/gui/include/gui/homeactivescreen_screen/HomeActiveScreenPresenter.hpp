#ifndef HOMEACTIVESCREENPRESENTER_HPP
#define HOMEACTIVESCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class HomeActiveScreenView;

class HomeActiveScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    HomeActiveScreenPresenter(HomeActiveScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~HomeActiveScreenPresenter() {}

    virtual void updateEnvironmentalData(float temp, int humi, int light) override;

    /* USER CODE BEGIN */
    void sendSecurityChangeCommand(bool active);
    /* USER CODE END */
private:
    HomeActiveScreenPresenter();

    HomeActiveScreenView& view;
};

#endif // HOMEACTIVESCREENPRESENTER_HPP
