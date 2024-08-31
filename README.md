# RadSens-SBM-20
открытый проект радиометра на счетчике СБМ-20
Радиометр RadSens 7.2 на базе платы RadSens от ClimateGuard
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

![Схема типа Родник](https://github.com/user-attachments/assets/66348013-0c80-42ca-86b6-da3d2c07d48e)
