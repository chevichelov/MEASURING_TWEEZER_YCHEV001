#pragma once
#include <Arduino.h>

class BUTTON 
{
  protected:
    uint32_t TIME_BUTTON = 0;                                   //Объявляем переменную таймера кнопки
    bool IS_FIRST = true;
  public:                                      
     /*
     * Определяет нажата ли кнопка 
     */
    uint8_t iS_BUTTON(uint8_t PIN) 
    {
      if (!digitalRead(PIN))                                  
      {
        if (IS_FIRST)
        {
          TIME_BUTTON = millis();                               //Сохраняем время последнего нажатия
          IS_FIRST = false;
        }
      }
      if (digitalRead(PIN) && !IS_FIRST)
      {
        IS_FIRST = true;
        if (millis() - TIME_BUTTON >= 400)
          return 2;
        else
          return 1;
      }
      return 0;                                                   //Кнопка отжата
    }
};
