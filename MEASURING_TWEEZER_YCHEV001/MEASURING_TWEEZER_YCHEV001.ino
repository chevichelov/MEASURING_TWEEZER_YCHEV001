#include <Adafruit_SSD1306.h>                                          //Подключаем библиотеку
#include <avr/pgmspace.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include "BUTTON.h"
#include "DATA.h"                                                     //Конфигурационный файл, значения резисторов правятся в нём
#include "IMG.h"
#include "TO.h"  
#include <Wire.h>                                                      //Подключаем библиотеку

BUTTON BUTTON_PREV;
BUTTON BUTTON_NEXT;
BUTTON BUTTON_SETTING;
BUTTON BUTTON_HOLD;
DATA DATA;
TO TO;


Adafruit_SSD1306 TFT(DATA.TFT_WIDTH, DATA.TFT_HEIGHT, &Wire, DATA.TFT_RESET);        //Номер вывода сброса (или -1 при совместном использовании вывода сброса Arduino)

ISR (TIMER1_OVF_vect)
{ 
  TO.HZ++; 
}



//==========================================================================
void setup()
{
  pinMode(11, INPUT_PULLUP);        //Пин установлен на вход с подтяжкой на внутреннее сопротивление
  pinMode(12, INPUT_PULLUP);        //Пин установлен на вход с подтяжкой на внутреннее сопротивление
  pinMode(13, INPUT_PULLUP);        //Пин установлен на вход с подтяжкой на внутреннее сопротивление
  pinMode(A2, INPUT_PULLUP);        //Пин установлен на вход с подтяжкой на внутреннее сопротивление
  pinMode(A3, INPUT_PULLUP);        //Пин установлен на вход с подтяжкой на внутреннее сопротивление
  Wire.begin();
  Wire.setClock(400000L);

  TFT.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //Инициализируем экран
  TFT.setTextColor(WHITE);          //Устанавливаем цвет
  TFT.setRotation(2);
  TFT.clearDisplay(); 
  TFT.drawBitmap(0, 26, HEAD, 128, 11, WHITE);
  TFT.display();
  
  ADCSRB = 0;                         //источник опорного напряжения AVCC; несимметричный вход ADC0 (MUX4-0 = 0000); выравнивание результата по правому краю (ADLAR = 0) 
  ACSR =  _BV(ACI)                    //разрешаем прерывания компаратора, записав 1 в биты 3 и 4 (ACIE и ACI) ACSR |= _BV(ACIE) | _BV(ACI);
        | _BV(ACIE)            
        | _BV(ACIS0) | _BV(ACIS1);    //разрешаем прерывания при повышении и понижении входного напряжения по отношению к опорному, записав 0 в 7й бит(ADC). 
                                      //в 0 и 1 биты(ACIS0 и ACIS1)
  TCCR1A=0; TIMSK1 = 1<<TOIE1;        //прерывание по переполнению
  TCCR1A=1<<COM1A0;                   //подключить выход OC1A первого таймера
  TCCR1B=0;

  TFT.setTextColor(WHITE);
  TO.DEFAULT_VOLT = TO.GET_DEFAULT_VOLT();
}


void DISPLAY_DATA(String DATA)
{
  TFT.setCursor((TFT.width() - DATA.length() * 12)/2, 50);
  TFT.print(DATA);
  TFT.display();
}

void DISPLAY_DATA(uint32_t VALUE, String DATA)
{
  TFT.setCursor((TFT.width() - (TO.BREAK(VALUE == 0 ? 1 : VALUE) * 12 + DATA.length() * 12 + 12))/2, 50);
  TFT.print(VALUE);
  TFT.print(DATA);
  TFT.display();
}

void DISPLAY_DATA(float VALUE, String DATA)
{
  TFT.setCursor((TFT.width() - (TO.BREAK((uint32_t)(VALUE < 1 ? 100 : VALUE * (VALUE > 10 ? 10 : 100))) * 12 + DATA.length() * 12 + 24))/2, 50);
  TFT.print(VALUE, VALUE > 10 ? 1 : 2);
  TFT.print(DATA);
  TFT.display();
}


void DISPLAY_GRAPH() 
{                                                                       //Рисуем график напряжения
  uint16_t Y1, Y2; 
  uint8_t LENGTH = TO.LENGTH - 1;
  for (uint8_t X = 0; X < LENGTH; ++X) {                                //Определяем длину графика
    Y1 = map(TO.VOLTS[X], TO.MIN_VOLT, TO.MAX_VOLT, 63, 9);             //Определяем значение точки Y1
    Y2 = map(TO.VOLTS[X + 1], TO.MIN_VOLT, TO.MAX_VOLT, 63, 9);         //Определяем значение точки Y2
    TFT.drawLine(X + 27, Y1, X + 28, Y2, WHITE);                        //Рисуем линии
  }
                                                                        //Рисуем слева числа для сетки напряжения
  TFT.setCursor(0, 9);                                                  //Устанавливаем позицию текста
  TFT.print(TO.DISPLAY_MAX_VOLT, 2); //Выводим максимальное значение напряжения
  TFT.drawLine(1, 20, 23, 20, WHITE);

  TFT.setCursor(0, 33);                                                 //Устанавливаем позицию текста
  TFT.print(TO.DISPLAY_MIN_VOLT + ((TO.DISPLAY_MAX_VOLT - TO.DISPLAY_MIN_VOLT) / 2), 2);   //Выводим среднее значение напряжения

  TFT.setCursor(0, 57);                                                 //Устанавливаем позицию текста
  TFT.print(TO.DISPLAY_MIN_VOLT, 2);                                                     //Выводим 0


  
  uint32_t HZ = TO.HERTZ();
  if (HZ < 1000000000)
    DATA.HZ = HZ;

  if (DATA.HZ < 1000)
  {
    TFT.setCursor(92, 0);                                                 //Устанавливаем позицию текста
    TFT.print(DATA.HZ);                                                   //Выводим данные на экран  
    TFT.print(F("Hz"));
  }
  else if (DATA.HZ >= 1000 && DATA.HZ < 1000000)
  {
    TFT.setCursor(80, 0);
    TFT.print(DATA.HZ / 1000.0f, 1);                                      //Выводим данные на экран  
    TFT.print(F("kHz"));
  }
  else if (DATA.HZ >= 1000000 && DATA.HZ < 1000000000)
  {
    TFT.setCursor(80, 0);
    TFT.print(DATA.HZ / 1000000.0f, 1);                                   //Выводим данные на экран  
    TFT.print(F("MHz"));
  }

  TFT.setCursor(26, 0);                                                 //Устанавливаем позицию текста
  const char TIME_LINE[8][6] = {"200us", "500us", "  1ms", "  2ms", "  5ms", " 10ms"};  //Режимы измерения
  TFT.print(TIME_LINE[DATA.SETTING_MODE]);                              //Выводим данные на экран
}


void BACKGROUND() 
{                                                                       //Функция рисует сетку для осциллографа
  for (uint16_t X = 26; X <= DATA.TFT_WIDTH; X += 5) {                  //Определяем шаг в 5 пикселей между горизонтальными линиями
    TFT.drawFastHLine(X, 36, 2, WHITE);
    if ((X - 1) % 25 == 0)                                              //Рисуем чёрточки раз в 25 пикселей (для красоты)
    {
      TFT.drawFastHLine((X - 1 == 25 ? X : (X - 1 == 125 ? X - 2 : X - 1)), 9, 3, WHITE);  //Для красоты добавляем чёрточки сверху
      TFT.drawFastHLine((X - 1 == 25 ? X : (X - 1 == 125 ? X - 2 : X - 1)), 63, 3, WHITE); //Для красоты добавляем чёрточки снизу
    }
  }
  for (uint16_t X = 26; X <= DATA.TFT_WIDTH; X += 25) {                 //Определяем шаг в 25 пикселей между вертикальными линиями
    for (uint16_t Y = 10; Y < 63; Y += 5) {                             //Определяем значение координаты Y
      TFT.drawFastVLine(X, Y, 2, WHITE);                                //Рисуем вертикальные линии
    }
  }
}


void OHMMETER_DISPLAY(uint32_t RESULT)
{
  TFT.drawBitmap(0, 0, HEAD_R, 80, 22, WHITE);
  if (RESULT < 1000)
  {
    DISPLAY_DATA(RESULT, F("Om"));
    return;
  }
  else if (RESULT >= 1000 && RESULT < 1000000)
  {
    DISPLAY_DATA(RESULT / 1000.0f, F("kOm"));
    return;
  }
  else if (RESULT >= 1000000 && RESULT < 3000000)
  {
    DISPLAY_DATA(RESULT / 1000000.0f, F("MOm"));
    return;
  }
  DISPLAY_DATA(F("OPEN"));
}


void CAPACITOR_DISPLAY(float RESULT)
{
  TFT.drawBitmap(0, 0, HEAD_C, 80, 22, WHITE);
  if (RESULT > 0 && RESULT < 1000)
  {
    DISPLAY_DATA(RESULT, F("nF"));
    return;
  }
  else if (RESULT >= 1000 && RESULT < 1000000)
  {
    DISPLAY_DATA(RESULT / 1000.0f, F("uF"));
    return;
  }
  DISPLAY_DATA(F("OPEN"));
}


void HERTZ_DISPLAY(uint32_t RESULT)
{
  TFT.drawBitmap(0, 0, HEAD_HZ_IN, 80, 22, WHITE);
  if (RESULT < 1000000000)
    DATA.HZ = RESULT;
  if (DATA.HZ < 1000)
  {
    DISPLAY_DATA(DATA.HZ, F("Hz"));
    return;
  }
  else if (DATA.HZ >= 1000 && DATA.HZ < 1000000)
  {
    DISPLAY_DATA(DATA.HZ / 1000.0f, F("kHz"));
    return;
  }
  else if (DATA.HZ >= 1000000 && DATA.HZ < 1000000000)
  {
    DISPLAY_DATA(DATA.HZ / 1000000.0f, F("MHz"));
    return;
  }
  DISPLAY_DATA(F("OPEN"));
}


void VOLT_DISPLAY(float RESULT)
{
    TFT.drawBitmap(0, 0, HEAD_V, 80, 22, WHITE);
    if (RESULT > 0)
    {
      DISPLAY_DATA(RESULT, F("V"));
      return;
    }
    DISPLAY_DATA(F("OPEN"));
}



void loop(){
  analogReference(DEFAULT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT); 
  pinMode(10, INPUT);

  DATA.IS_BUTTON_LONG = BUTTON_HOLD.iS_BUTTON(A2);
  if (DATA.IS_BUTTON_LONG == 1)
    DATA.HOLD = !DATA.HOLD;
  else if (DATA.IS_BUTTON_LONG == 2)
    TFT.setRotation(TFT.getRotation() == 2 ? 0 : 2);

  if (DATA.HOLD)
    return;

  if (BUTTON_NEXT.iS_BUTTON(11) == 1)                                 //Нажатием кнопки переключаем режимы исследования сигнала
  {
      --DATA.MODE;                                                    //Переключаем на следующий режим
      if (DATA.MODE < 0)                                              //После режима 6 снова переходим на режим 0
        DATA.MODE = 7;
      DATA.FIRST_LOAD_FONT = true;
  }
  if (BUTTON_PREV.iS_BUTTON(12) == 1)                                 //Нажатием кнопки переключаем режимы исследования сигнала
  {
      ++DATA.MODE;                                                    //Переключаем на следующий режим
      if (DATA.MODE > 7)                                              //После режима 6 снова переходим на режим 0
        DATA.MODE = 0;
      DATA.FIRST_LOAD_FONT = true; 
      TO.DEFAULT_VOLT = TO.GET_DEFAULT_VOLT(); 
  }
  
  TFT.clearDisplay();
  if (DATA.FIRST_LOAD_FONT)
  {
    if (DATA.MODE != 5)
      TFT.setFont(&FreeMonoBold12pt7b);
    else
      TFT.setFont();
    DATA.FIRST_LOAD_FONT = false;
  }
  if (DATA.MODE != 5)
    TFT.drawBitmap(95, 0, TO.DEFAULT_VOLT >= 4 ? BATTERY_FULL : TO.DEFAULT_VOLT < 4 && TO.DEFAULT_VOLT >= 3 ? BATTERY_MIDDLE : TO.DEFAULT_VOLT < 3 && TO.DEFAULT_VOLT >= 2.5 ? BATTERY_LOW : BATTERY_EMPTY, 32, 22, WHITE);
  if (DATA.MODE == 0)
  {
    if (DATA.TEST_MODE == 1)
    {
      DATA.RESULT_INT = TO.OHMMETER();
      if (DATA.RESULT_INT < 3000000)
      {
        OHMMETER_DISPLAY(DATA.RESULT_INT);
        DATA.TEST_MODE = 1;
      }
      else
      {
        TFT.drawBitmap(0, 0, HEAD_AUTO, 80, 22, WHITE);
        DISPLAY_DATA(F("OPEN"));
        DATA.TEST_MODE = 0;
      }
    }
    if (DATA.TEST_MODE == 2)
    {
      DATA.RESULT_FLOAT = TO.CAPACITOR();
      if (DATA.RESULT_FLOAT > 0 && DATA.RESULT_FLOAT < 3000000000)
      {
        CAPACITOR_DISPLAY(DATA.RESULT_FLOAT); 
        DATA.TEST_MODE = 2;
        return;
      }
    }
    if (TO.TEST())
      DATA.TEST_MODE = 2;
    else 
      DATA.TEST_MODE = 1; 
  }
  else if (DATA.MODE == 1)
  {
    OHMMETER_DISPLAY(TO.OHMMETER());
    return;
  }
  else if (DATA.MODE == 2)
  {
    CAPACITOR_DISPLAY(TO.CAPACITOR());
    return;
  }
  else if (DATA.MODE == 3)
  {
    VOLT_DISPLAY(TO.VOLT());
    return;
  }
  else if (DATA.MODE == 4)
  {
    TFT.drawBitmap(0, 0, HEAD_D, 80, 22, WHITE);
    DATA.RESULT_FLOAT = TO.DIODE();
    if (DATA.RESULT_FLOAT <= floor(TO.DEFAULT_VOLT))
    {
      DISPLAY_DATA(DATA.RESULT_FLOAT, F("V"));
      return;
    }
    else
     DISPLAY_DATA(F("OPEN"));
  }
  else if (DATA.MODE == 5)
  {
    if (BUTTON_SETTING.iS_BUTTON(13))                                        //Нажатием кнопки переключаем режимы исследования сигнала
    {
        ++DATA.SETTING_MODE;                                                  //Переключаем на следующий режим
        if (DATA.SETTING_MODE > 5)                                            //После режима 6 снова переходим на режим 0
          DATA.SETTING_MODE = 0;
        TO.DEFAULT_VOLT = TO.GET_DEFAULT_VOLT();
    }
    BACKGROUND();                                                             //Рисуем сетку 
    TO.OSCILLOSCOPE(DATA.SETTING_MODE);                                       //Выбираем время иследования сигнала и рисуем график                       
    DISPLAY_GRAPH();                                                          //Выводим данные на экран*/
    TFT.display();
  }
  else if (DATA.MODE == 6)
  {
    HERTZ_DISPLAY(TO.HERTZ());
    return;
  }
  else if (DATA.MODE == 7)
  {
    if (BUTTON_SETTING.iS_BUTTON(13))                                          //Нажатием кнопки переключаем режимы исследования сигнала
    {
        ++DATA.DATA_HZ_MODE;
        if (DATA.DATA_HZ_MODE > 10)
          DATA.DATA_HZ_MODE = 0;
    }
    uint32_t DATA_HZ[11] = {30, 61, 122, 244, 488, 976, 4000, 7800, 31400, 62500, 1000000};
    TFT.drawBitmap(0, 0, HEAD_HZ_OUT, 80, 22, WHITE);
    TO.GENERATOR(DATA_HZ[DATA.DATA_HZ_MODE], 1, 0);
    DISPLAY_DATA(DATA_HZ[DATA.DATA_HZ_MODE], F("Hz"));
  }
}
