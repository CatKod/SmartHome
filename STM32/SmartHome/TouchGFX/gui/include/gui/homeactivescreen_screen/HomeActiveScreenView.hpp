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
protected:
};

#endif // HOMEACTIVESCREENVIEW_HPP
