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

![Схема](https://github.com/user-attachments/assets/032ff4c3-46fd-400b-a261-69386662177b)

<br><br>
В проекте используются следующие библиотеки:<br>
<br>

GyverOLED от AlexGyver <br>
https://github.com/GyverLibs/GyverOLED <br>
<br>

FileDat от AlexGyver<br>
https://github.com/GyverLibs/FileData<br>
<br>

Official RadSens library by ClimateGuard<br>
https://github.com/climateguard/RadSens<br>
<br>

Blynk от Volodymyr Shymanskyy Внимание!!! есть ограничение на версию - не старше 1.1.0<br>
BlynkSimpleEsp32
Blynk сервер - blynk.tk<br>
Руководство по подключению https://wiki.blynk.tk/<br>
<br>

Дополнительные ссылки для менеджера плат: https://espressif.github.io/arduino-esp32/package_esp32_index.json <br>
<br>
Работа кнопок:
<br>
1 кнопка - короткое нажатие - вкл., откл. световых сигналов<br>
долгое нажатие (1сек.) - вкл., откл. звукового сигнала<br>
<br>

2 кнопка - короткое нажатие - переключение экранов (поиск-стрелочный индикатор, поиск по CPS, гафик мощности дозы)<br>
долгое нажатие (1сек.) - вкл., откл. экрана<br>
<br>

3 кнопка - короткое нажатие - вкл., откл. wi-fi<br>
долгое нажатие (1сек.) - сброс измерения<br>
<br>

На экране отображаются следующие данные:
<br>
-Текущая мощность дозы;<br>
-погрешность <br>
Содержит динамическое значение интенсивности ионизирующего гамма-
излучения. При детектировании резкого изменения интенсивности излучения
(как в большую, так и в меньшую сторону) динамически регулирует период счета
скользящего окна, чтобы диапазон охватывал временной промежуток,
содержащий только актуальные данные. Позволяет использовать устройство в
режиме поиска локальных загрязнений. Частота обновления – 1 сек.
<br>
-Статическое значение мощности дозы в мкР или мР
Содержит статистическое значение интенсивности ионизирующего гамма-
излучения. Период счета скользящего окна составляет 500 сек. Позволяет
производить точные измерения постоянного радиационного фона. Частота
обновления – 1 сек.<br>
<br>
пример отображения данных в мобильном приложении Blynk
<br>
<img src="https://github.com/dimkra/RadSens-Blynk.tk/assets/37121139/c710e1c0-8256-446f-a18e-244fac3d791f" height="300" >
<br><br>
Управление через API:
<br>
Пример: https://api.blynk.tk/{TOKEN}/update/V0?value=0
<br>
![2024-09-01 17-13-34](https://github.com/user-attachments/assets/77881c1f-b1bc-46e9-addf-e510f3448367)

<br>
