#pragma once
#include <Arduino.h>
class TO : public DATA
{
  protected:
    uint8_t  SELECT         = 0;
    uint32_t TIME_DELAY     = 0;
    uint32_t TIME_BUTTON    = 0;                                                //Объявляем переменную таймера кнопки
  public: 
    uint8_t LENGTH          = 102;                                              //Количество измерений
    uint16_t VOLTS[102];                                                        //Массив измерений напряжения
    uint16_t MAX_VOLT       = 0;
    uint16_t MIN_VOLT       = 0;
    float DISPLAY_MAX_VOLT  = 0;
    float DISPLAY_MIN_VOLT  = 0;
    uint32_t HZ             = 0; 
    float DEFAULT_VOLT      = 0;

    /*
     * Измеряем внутреннее напряжение Arduino
     */
    float GET_DEFAULT_VOLT() 
    {                                             
      float RESULT          = 0.0f;                                           //Определяем переменную для получения результата.
      uint8_t  COUNT_RESULT = 100;                                            //Определяем сколько значений АЦП требуется получить для усреднения результата.
      #if (defined( __AVR_ATmega640__ ) || defined( __AVR_ATmega1280__ ) || defined( __AVR_ATmega1281__ ) || defined( __AVR_ATmega2560__ ) || defined( __AVR_ATmega2561__ ) || defined( __AVR_ATmega32U4__ ) || defined( __AVR_ATmega32U4__ ))   //Устанавливаем Uвх АЦП с источника ИОН на 1,1В, а в качестве ИОН АЦП используем Vcc ИОН - источник опорного напряжения.
        ADCSRB &= ~(1<<MUX5);                                                 // У микроконтроллера «ATmega328» действительно нет такого бита, но он зарерервирован для совместимости
      #endif
                                                                              //Для Arduino Mega, Leonardo и Micro, сбрасываем бит «MUX5» регистра «ADCSRB», так как «MUX[5-0]» должно быть равно 011110 (см. регистр «ADMUX»).
      ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(1<<4)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(0<<MUX0);  //  Устанавливаем биты регистра «ADMUX»: «REFS»=01 (ИОН=VCC), «ADLAR»=0 (выравнивание результата по правому краю), «MUX[4-0]»=11110 или «MUX[3-0]»=1110 (источником сигнала для АЦП является напряжение ИОН на 1,1 В).                                                                   
      for(uint8_t i=0; i<COUNT_RESULT; ++i)                                   //Получаем несколько значений АЦП
      {
        ADCSRA |= (1<<ADEN )|(1<<ADSC );                                      //Запускаем преобразования АЦП:Устанавливаем биты регистра «ADCSRA»: «ADEN»=1  (вкл АЦП), «ADSC» =1 (запускаем новое преобразование).                                                                     
        while(bit_is_set(ADCSRA,ADSC)){};                                     //Получаем данные АЦП:
        RESULT += ADC;                                                        //Ждём завершения преобразования: о готовности результата свидетельствует сброс бита «ADSC» регистра «ADCSRA» и добавляем результат «ADC» к переменной «i».
      } 
      RESULT /= COUNT_RESULT;                                                 //Делим результат «i» на «j», так как мы получили его «j» раз.                                                            
      return (1.1f/RESULT) * 1024.0f;                                         //Рассчитываем напряжение питания:  //  АЦП = (Uвх/Vcc)*1023. Напряжение Uвх мы брали с внутреннего ИОН на 1.1 В, значение которого возвращает функция analogSave_1V1(0).
    }


    uint8_t BREAK(uint32_t DATA)
    {
      if (DATA < 10) 
        return 1;
      return 1 + BREAK(DATA / 10);
    }

    /*
     * Замеряем напряжение на пине
     */
    uint32_t GET_VOLT(uint8_t PIN)
    {
      int32_t VOLTS = 0;
      for (uint16_t i=0; i<=255; ++i) 
        VOLTS += analogRead(PIN);
      return VOLTS >>=4;                                                        //Сдвигаем на 4 бита вправо (делим на 2^4)
    }

    /*
     * Измеряем напряжение
     */
    float VOLT()
    {
      analogReference(INTERNAL);                                                //Встроенное опорное напряжение 1.1 В на микроконтроллерах ATmega168 и ATmega328
      pinMode(4, OUTPUT);                                                       //Пин на вывход
      digitalWrite(4,0);
      return this->GET_VOLT(A1) / 16383.0f * DATA::RESISTANCE;                  //Напряжение рассчитываем по формуле Значение / (14 бит = 16383) * ((R2 + R10) / R10)
    }

    /*
     * Измеряем падение напряжения на диоде
     */
    float DIODE()
    {
      analogReference(INTERNAL);                                              //Встроенное опорное напряжение 1.1 В на микроконтроллерах ATmega168 и ATmega328
      pinMode(2, OUTPUT);                                                     //Пин на вывход
      digitalWrite(2,1);
      pinMode(4, OUTPUT);                                                     //Пин на вывход
      digitalWrite(4,0);
      return this->GET_VOLT(A1) / 16383.0f * DATA::RESISTANCE;                //Напряжение рассчитываем по формуле Значение / (14 бит = 16383) * ((R2 + R10) / R10)
    }

    /*
     * Проверяем что перед нами, конденсатор или резистор
     */
    bool TEST()
    {
      pinMode(10, OUTPUT);
      pinMode(8, INPUT);
      digitalWrite(10, 1);
    
      delay(100);
      digitalWrite(10, 0);
      delay(100);
      
      if (analogRead(A0) > 100)
        return true;
      else
        return false;
    }

    /*
     * Измеряем сопротивление
     */
    uint32_t OHMMETER()
    {                                                                       //Измеряем напряжение на резисторе в 20кОм
      pinMode(4, OUTPUT);                                                   //Пин на выход
      digitalWrite(4, 1);
      float U   = this->GET_VOLT(A1);
      float U1  = this->GET_VOLT(A7);
      if (U1 > 4000 && U - U1 > 0)
        return U1 * DATA::R2 /float(U - U1);                                //Сопротивление рассчитываем по формуле R = U1 * R2 / (U - U1)
    
                                                                            //Измеряем напряжение на резисторе в R2
      pinMode(4, INPUT);                                                    //Пин на вход
      pinMode(3, OUTPUT);                                                   //Пин на выход 
      digitalWrite(3, 1);
      U   = this->GET_VOLT(A1);
      U1  = this->GET_VOLT(A6);
      if (U1 > 4000 && U1 - U > 0)
        return U * DATA::R10 /float(U1 - U);                                //Сопротивление рассчитываем по формуле R = U * R10 / (U1 - U)
    
                                                                            //Измеряем напряжение на резисторе в R11
      pinMode(3, INPUT);                                                    //Пин на вход
      pinMode(2, OUTPUT);                                                   //Пин на выход 
      digitalWrite(2, 1); 
      U = this->GET_VOLT(A1);
      U1  = this->GET_VOLT(A7);
      if (U1 > 4000 && U1 - U > 0)
        return U * DATA::R11 /float(U1 - U);                                //Сопротивление рассчитываем по формуле R = U * R11 / (U1 - U)
    
                                                                            //Изменяем опорное напряжение
      U1 = this->GET_VOLT(A7) / 16383.0f * this->GET_DEFAULT_VOLT();
      analogReference(INTERNAL);                                            //Встроенное опорное напряжение 1.1 В на микроконтроллерах ATmega168 и ATmega328
      U = this->GET_VOLT(A1) / 16383.0f * 1100;
      if (U1 - U > 0)
        return U * DATA::R11 /float(U1 - U);                                //Сопротивление рассчитываем по формуле R = U * R11 / (U1 - U)
    }
    
    /*
     * Измеряем ёмкость конденсатора
     */
    float CAPACITOR()
    {
      while(analogRead(A0) > 0)
      { 
        pinMode(8, OUTPUT); 
        pinMode(10, OUTPUT); 
        digitalWrite(8, 0); 
        digitalWrite(10, 0);
      }
      
      if(SELECT == 0)
      {
        pinMode(10, OUTPUT);
        pinMode(8, INPUT);
        digitalWrite(10, 1);
      }
    
      if(SELECT == 1)
      {
        pinMode(8, OUTPUT);
        pinMode(10, INPUT);
        digitalWrite(8, 1);
      }
    
      uint32_t TIME_ZERO = micros(), TIME_1 = 0, TIME_2 = 0;
      
      while(analogRead(A0) < 644)
      {
        TIME_2      = micros() - TIME_ZERO;
        if(TIME_2 >= 1000000 && SELECT == 0)
        {
          SELECT    = 1;
          TIME_ZERO = 100000000;
          break;
        }
      }
    
      TIME_1 = micros() - TIME_ZERO; 
    
      while(analogRead(A0) > 0)
      { 
        pinMode(8, OUTPUT); 
        pinMode(10, OUTPUT); 
        digitalWrite(8, 0); 
        digitalWrite(10, 0);
      }
    
      if(SELECT == 1 && TIME_1 < 1000)
        SELECT = 0;
    
      delay(100); 
      if (TIME_1 < 300)
        TIME_1 = 0;
      return  abs(TIME_1 / 1000.0f * (SELECT == 0 ? 1 : 1000));
    }

    uint32_t HERTZ()
    {
      pinMode(4, OUTPUT);
      digitalWrite(4, 0);
      uint32_t RESULT = 0;
      TCCR1B  = (1<<CS10)|(1<<CS11)|(1<<CS12);//тактировани от входа Т1
      if (millis() - TIME_DELAY >= 1000)
      {  
        TCCR1B      = 0;
        TIME_DELAY  = millis();
        RESULT      = ((uint32_t)HZ<<16) | TCNT1; //сложить что натикало
        HZ          = 0; 
        TCNT1       = 0; 
        return RESULT;
      }
    }
    
    /*
     * Генератор частоты
     */
    void GENERATOR(uint32_t HZ, uint16_t DIVIDER, uint8_t LVL) 
    {
      pinMode(9, OUTPUT);
      uint32_t RESULT = (F_CPU / HZ / 2 / DIVIDER);
      if (RESULT > 65536 && DIVIDER <= 1024)
      { 
        DIVIDER = DIVIDER * (DIVIDER >= 64 ? 4 : 8);
        ++LVL;
        this->GENERATOR(HZ, DIVIDER, LVL);
        return;
      }
    
      OCR1A  = RESULT - 1; 
      TCCR1B = (LVL + 1)|(1<<WGM12);
    }

    /*
     * Функция получения данных для графика
     */
    void OSCILLOSCOPE(int8_t MODE) 
    { 
      pinMode(4, OUTPUT);                                                 //Пин на вывход
      digitalWrite(4,0); 
      MAX_VOLT      = 0;                                                  //Обнуляем максимальное значение напряжения                                           
      MIN_VOLT      = 100;
      ADCSRA = ADCSRA & 0xf8;                                             //очищает младшие 3 бита перед операцией
      if (MODE == 0)
        ADCSRA = ADCSRA | 0x02;
      else if (MODE == 1)
        ADCSRA = ADCSRA | 0x04;
      else if (MODE == 2)
        ADCSRA = ADCSRA | 0x05;
      else if (MODE == 3)
        ADCSRA = ADCSRA | 0x06;
      else
        ADCSRA = ADCSRA | 0x07;

      for (uint8_t i = 0; i < LENGTH; ++i)                                  //Делаем несколько измерений напряжения
      {
        VOLTS[i]  = analogRead(A1);                                         //Получаем значения
        MAX_VOLT  = max(MAX_VOLT,  VOLTS[i]);                               //Находим максимальное значение
        MIN_VOLT  = min(MIN_VOLT,  VOLTS[i]);
        if (MODE == 0)
        {
          asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); //Обеспечивает микрозадержку
          asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
          asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
        }
        else if (MODE == 1)
        {
          delayMicroseconds(4);                                             //Задержка в 4 микросекунды
          asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); //Обеспечивает микрозадержку
          asm("nop"); asm("nop"); asm("nop");
        }
        else if (MODE == 2)
          delayMicroseconds(12);                                            //Задержка в 12 микросекунд
        else if (MODE == 3)
          delayMicroseconds(24);                                            //Задержка в 24 микросекунды
        else if (MODE == 4)
          delayMicroseconds(88);                                            //Задержка в 88 микросекунд
        else
          delayMicroseconds(288);                                           //Задержка в 288 микросекунд  
      }                        
      DISPLAY_MAX_VOLT = MAX_VOLT * DEFAULT_VOLT / 1024 * 1.1f;             //Получаем максимальное напряжение
      DISPLAY_MIN_VOLT = MIN_VOLT * DEFAULT_VOLT / 1024 * 1.1f;
    } 
};
