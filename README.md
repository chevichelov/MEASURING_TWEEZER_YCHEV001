# MEASURING_TWEEZER_YCHEV001
Схема измерительного пинцета невероятно проста, она состоит из 
Arduino Nano, обратите внимание, на плате требуется выпаять резистор светодиода L иначе кнопка SB3 не будет работать, дисплея SSD1306, одного конденсатора на 100 nF и 11 резисторов.
Модуль TP4056 DIY Kit я вынес отдельно за схему, так как для контроля заряда-разряда аккумулятора Вы можете использовать любой контроллер, однако данная плата способна выдавать 5 вольт, а АЦП Arduino Nano не может измерять напряжение выше напряжения АЦП, поэтому 5 вольт на входе Arduino Nano желательно.
И, как всегда, я для Вас подготовил плату в Sprint-Layout и конвертировал её в PDF, что бы Вы смогли сразу распечатать шаблон и изготовить плату без дополнительных программ.
С прошивкой всё ещё проще, в менеджере плат выбираем Arduino Nano, затем соответствующий Вашей плате процессор и Порт.

<h2 align="center">Главная страница</h2>
<img src="https://github.com/chevichelov/MEASURING_TWEEZER_YCHEV001/blob/main/IMG/MAIN.jpg" alt="Измерительный пинцет DIY YCHEV001 на Arduino" />

<h2 align="center">Схема</h2>
<img src="https://github.com/chevichelov/MEASURING_TWEEZER_YCHEV001/blob/main/IMG/SCHEME.jpg" alt="Схема измерительного пинцета DIY YCHEV001 на Arduino" />

<h2 align="center">Видео инструкция</h2>
<div align="center">
  <a href="https://youtu.be/co7q47WMKG8"><img src="https://github.com/chevichelov/MEASURING_TWEEZER_YCHEV001/blob/main/IMG/MAIN.jpg" alt="Измерительный пинцет DIY YCHEV001 на Arduino">
  <div>Измерительный пинцет DIY YCHEV001 на Arduino</div>
  </a>
</div>

