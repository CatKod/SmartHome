#ifndef MODEL_HPP
#define MODEL_HPP

class ModelListener;

class Model
{
public:
    Model();
    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }
    void tick();

    /* USER CODE BEGIN */
    void sendHomeModeCmd(bool active);
    /* USER CODE END */
protected:
    ModelListener* modelListener;

    /* USER CODE BEGIN */
    float currentTemp;
    int currentHumi;
    int currentLight;
    bool lastLoginState;
    bool lastWarningState;
    /* USER CODE END */
};

#endif // MODEL_HPP
