#pragma once
#include <Arduino.h>

class DATA 
{
  protected:
    float RESISTANCE = 5.3084;                                              //Значение считаем по формуле ((R2 + R10) / R10) 
    float R2 = 17980.0f;                                                    //Сопротивление в Омах резистора R2
    float R10 = 4843.0f;                                                    //Сопротивление в Омах резистора R10
    float R11 = 469.0f;                                                     //Сопротивление в Омах резистора R11
  public:
    uint8_t TFT_WIDTH = 128;                                                //Ширина Экрана
    uint8_t TFT_HEIGHT = 64;                                                //Высота Экрана
    uint8_t TFT_RESET = -1;

    int8_t MODE = 0;
    int8_t TEST_MODE = 0;
    int8_t SETTING_MODE = 0;
        
    uint32_t RESULT_INT = 0;
    float RESULT_FLOAT = 0;

    bool FIRST_LOAD_FONT = true;
    bool HOLD = false;
    uint8_t IS_BUTTON_LONG = 0;

    uint32_t HZ = 0;
    int8_t DATA_HZ_MODE = 0;
};
