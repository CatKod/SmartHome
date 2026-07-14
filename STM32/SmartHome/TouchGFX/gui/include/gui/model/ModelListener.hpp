#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    /* USER CODE BEGIN */
    virtual void updateEnvironmentalData(float temp, int humi, int light) {}
    virtual void onAuthorizationSuccess() {}
    virtual void onSecurityAlert(bool isDanger) {}
    /* USER CODE END */
protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
