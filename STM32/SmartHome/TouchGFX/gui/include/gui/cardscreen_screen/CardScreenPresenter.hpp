#ifndef CARDSCREENPRESENTER_HPP
#define CARDSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class CardScreenView;

class CardScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    CardScreenPresenter(CardScreenView& v);

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

    virtual ~CardScreenPresenter() {}

private:
    CardScreenPresenter();

    CardScreenView& view;
};

#endif // CARDSCREENPRESENTER_HPP
