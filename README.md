# RadSens-SBM-20
открытый проект радиометра на счетчике СБМ-20
<br>
Радиометр на базе платы RadSens v.4 с пониженным энергопотреблением от ClimateGuard
<br>
https://climateguard.ru/radsens/
<br>
<img src="https://github.com/dimkra/RadSens-Blynk.tk/assets/37121139/c7f5860e-bdcb-4521-91ef-df862f12660b" width="500" >
<br>
Изменяемые параметры платы RadSens:<br>
<br>
Настройка чувствительности счетчика:
radSens.setSensitivity(105); //установить<br> 
radSens.getSensitivity(); //считать состояние<br>
значение 105 - по умолчанию для счетчика СБМ-20.<br>
<br>
Включение-выключение высоковольтного генератора:<br>
radSens.setHVGeneratorState(bool state); //установить <br>
radSens.getHVGeneratorState(); //считать состояние<br>
<br>
Включение-выключение светодиодного индикатора:<br>
radSens.setLedState(bool state); //установить <br>
radSens.getLedState(); //считать состояние<br>
<br>
Получение данных:<br>
radSens.getNumberOfPulses(); //количество зарегистрированных импульсов<br>
radSens.getRadIntensyDynamic(); //динамическое значение мощности дозы в мкР/ч<br>
radSens.getRadIntensyStatic(); //статическое значение мощности дозы в мкР/ч<br>
<br>
В проекте используется:<br>
<br>
ESP32-C3 supermini 
<br>
<img src="https://github.com/user-attachments/assets/6d28963c-b92d-44b3-90e4-519da4ca2fab" width="500" >
<br>
OLED дисплей 0,96" 128x64
<br>
<img src="https://github.com/user-attachments/assets/e3f0c5f6-6f3d-42f7-844d-ec651dce540a" width="250" >
<br>
Модуль заряда аккумуляторов TP4056
<br>
Резисторы: 220кОм, 100кОм
<br>
Литьевый аккумулятор
<br>
Кнопка -3шт
<br>
Выключатель
<br>
Пищалка<br>
<br>

Схема проекта:
![Схема типа Родник](https://github.com/user-attachments/assets/66348013-0c80-42ca-86b6-da3d2c07d48e)
