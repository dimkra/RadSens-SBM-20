// RadSens СБМ-20

/* 
>---------------------------------ПЕРЕМЕННЫЕ Blynk--------------------------------------------
  Blynk.virtualWrite(V0, cps1s); 		                      //Blynk CPS 4значения по 250мс = 1сек
  Blynk.virtualWrite(V1, din);  		                      //Blynk RadSens динамика
  Blynk.virtualWrite(V2, radSens.getRadIntensyStatic());  //Blynk RadSens статика
  +Blynk.virtualWrite(V3, light);  	                      //Blynk состояние света
  +Blynk.virtualWrite(V4, sound);  	                      //Blynk сотояние звука
  Blynk.virtualWrite(V5, ADC);  	 	                      //Blynk заряд аккумулятора
  Blynk.virtualWrite(V6, awarn); 		                      //Blynk режим поиска
  +V7					                                            //Blynk режим экрана
  Blynk.virtualWrite(V8, delta*100); 	                    //Blynk отклонение отностит в %
  Blynk.virtualWrite(V9, sr_a); 		                      //Blynk среднее setSensitivityарифм CPS за окно
  Blynk.virtualWrite(V10, Count); 	                      //Blynk CPS за 1сек;
  Blynk.virtualWrite(V11, MDozi); 	                      //Blynk мощность дозы
  Blynk.virtualWrite(V12, MDoziA); 	                      //Blynk мощность дозы усреднение 750сек
  +V13                                                    //Blynk экран включен-выключен
  +V14                                                    //Blynk чувствительность RadSens
  +V15                                                    //Blynk собственный фон счетчика
  +V16                                                    //Blynk мертвое время счетчика
  +V17                                                    //Blynk коэффициент счетчика
  <---------------------------------ПЕРЕМЕННЫЕ Blynk--------------------------------------------
*/

// инициализируем библиотеки
#include <Arduino.h>
#include <FileData.h>
#include <LittleFS.h>
#include <WiFi.h>

float k_count = 40;   // коэффициент счетчика (для СБТ10-а 6,15)
float s_f_c=0.1;     // собственный фон счетчика
float m_v_c=0;        // мертвое время
int okno=6;           // время окна
int Cycl = 100;       //начальное значение циклов
int tCylc=0;
int priz20=0;

struct Data {
  uint8_t wifi_e = 0;
  int sound_e = 0;
  int scr_e = 6;
  float s_f_c_e=s_f_c;
  float m_v_c_e=m_v_c;
  float k_c_e=k_count;
};
Data mydata;
FileData data(&LittleFS, "/data.dat", 'B', &mydata, sizeof(mydata));
#define BLYNK_PRINT Serial
#define BLYNK_AUTH_TOKEN ""
//#define OLED_SPI_SPEED 8000000ul
#include <Wire.h>
#include <CG_RadSens.h>
#include <GyverOLED.h>
#include <BlynkSimpleEsp32.h>
#define ADC_pin 0  // задаём значение пина АЦП
#define buz_pin 1  // Задаём значения пина для пищалки
#define OLED_NO_PRINT
//---OLED экраны--------------
GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;    // с буфером
//GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled; // без буфера
//GyverOLED<SSH1106_128x64> oled;              // только программный буфер
CG_RadSens radSens(RS_DEFAULT_I2C_ADDRESS);  // инициализируем RadSens
char ssid[] = "";
char pass[] = "";
char auth[] = BLYNK_AUTH_TOKEN;
char buf1[50];
char buf2[50];
char buf4[50];
char buf3[50];
char buf5[50];
char buf6[50];
//String tx;
uint16_t ADC;         // переменная для значений АЦП
uint32_t timer_cnt;   // таймер для измерений дозиметра
uint32_t timer_cps;   // таймер для измерений импульсов за 1 сек. дозиметра
uint32_t timer_bat;   // таймер для измерения заряда батареи
uint32_t timer_imp;   // таймер опроса импульсов для пьезоизлучателя
uint32_t pulsesPrev;  // число импульсов за предыдущую итерацию
float са;
int MDo=0;
//------------------------------------------------>
float Count = 0;    //счетчик импульсов
float prev_Count = 0;
float Counts[210];  //массив импульсов
float CountsA[760];  //массив импульсов
uint32_t Time = 0;  //счетчик массива счетчик времени в секундах
uint32_t TimeA = 0;  //счетчик всего времени в секундах 
float SUMofCounts;
float SUMofCountsA;  //усреднение за все время
float MDozi;
float prev_MDozi;
float MDoziA;       //усреднение за все время  
int priznC1 = 0;     //признак первого цикла окна
int priznC1000 = 0;  //признак 1000 импульсов

float cpulses;


//<------------------------------------------------

int prev_counter_cps = 0;  // предыдущее значение счетчика импульсов
int counter_cps = 0;       // счетчик импульсов
int cps;                   // импульсов в сек

uint32_t ic = 1;
float pP;
float pT;
float din;
const int buttonPin1 = 1;  //1 кнопка
const int buttonPin2 = 3;  //2 кнопка
const int buttonPin3 = 4;  //3 кнопка
boolean buttonState1 = false;
boolean buttonState2 = false;
boolean buttonState3 = false;
int light = 1;      //световая индикация вкл по умолчанию
int sound = 0;      //звуковая индикация выкл по умолчанию
int OLEDPower = 1;  //экран включен по уолчанию
uint8_t wifiOn = 0;
int Scr = 6;      //вид экрана по умолчанию
int clearSc = 0;  //признак очистки экрана
int puls = 0;
int Sensitivity;
float r;
float rr;
int i = 1;
int i24 = 1;
int cps250[8];
int cps24[210];
int cps1s;
float cpsMax;
float cpsK;
int warn = 0;
float maswarn[10];
float awarn;
boolean press_flag = false;       //признак нажатия кнопки
boolean long_press_flag = false;  //признак долгого нажатия кнопки
unsigned long last_press = 0;
boolean press_flag2 = false;       //признак нажатия кнопки
boolean long_press_flag2 = false;  //признак долгого нажатия кнопки
unsigned long last_press2 = 0;
boolean press_flag3 = false;       //признак нажатия кнопки
boolean long_press_flag3 = false;  //признак долгого нажатия кнопки
unsigned long last_press3 = 0;


// функция аудиоприветствия
void hello() {
  for (int i = 1; i < 5; i++) {
    //tone(buz_pin, i * 1000);
    delay(100);
  }
  //tone(buz_pin, 0);
  delay(100);
  oled.setScale(2);
  oled.setCursor(11, 3);
  oled.print("RadSens");
  oled.update();
  delay(3000);
  oled.clear();
}

// функция, которая создаёт "трески" пьезоизлучателя при появлении импульсов
void beep() {
  tone(buz_pin, 3500);
  delay(13);
  tone(buz_pin, 0);
  delay(40);
}

// функция предупреждения при превышении порога излучения
void warning() {
  for (int i = 0; i < 3; i++) {
    tone(buz_pin, 1500);
    delay(250);
    tone(buz_pin, 0);
    delay(250);
  }
}

// рисует значек динамика
void dinamik(uint8_t d) {
  oled.rect(93, 4, 96, 9, d);
  oled.line(95, 5, 95, 8, 0);
  oled.line(97, 3, 99, 1, d);
  oled.line(97, 10, 99, 12, d);
  oled.line(100, 1, 100, 12, d);
}

// горизонтальная линейка
void drawRule() {
  oled.line(0, 63, 100, 63);
  oled.line(0, 58, 0, 63);
  oled.line(10, 60, 10, 63);
  oled.line(20, 60, 20, 63);
  oled.line(30, 60, 30, 63);
  oled.line(40, 60, 40, 63);
  oled.line(50, 58, 50, 63);
  oled.line(60, 60, 60, 63);
  oled.line(70, 60, 70, 63);
  oled.line(80, 60, 80, 63);
  oled.line(90, 60, 90, 63);
  oled.line(100, 58, 100, 63);
}

// горизонтальная сетка
void drawGrid() {
  oled.line(0, 35, 0, 57, 0);
  oled.line(10, 35, 10, 57, 0);
  oled.line(20, 35, 20, 57, 0);
  oled.line(30, 35, 30, 57, 0);
  oled.line(40, 35, 40, 57, 0);
  oled.line(50, 35, 50, 57, 0);
  oled.line(60, 35, 60, 57, 0);
  oled.line(70, 35, 70, 57, 0);
  oled.line(80, 35, 80, 57, 0);
  oled.line(90, 35, 90, 57, 0);
  oled.line(100, 35, 100, 57, 0);
}

// вертикальная сетка
void drawGridV() {

  oled.line(108, 23, 123, 23, 0);
  oled.line(108, 27, 123, 27, 0);
  oled.line(108, 31, 123, 31, 0);
  oled.line(108, 35, 123, 35, 0);
  oled.line(108, 39, 123, 39, 0);
  oled.line(108, 43, 123, 43, 0);
  oled.line(108, 47, 123, 47, 0);
  oled.line(108, 51, 123, 51, 0);
  oled.line(108, 55, 123, 55, 0);
  oled.line(108, 59, 123, 59, 0);
  oled.line(108, 63, 123, 63, 0);
}

// вертикальная линейка
void drawRuleV() {
  oled.line(127, 23, 127, 63);
  oled.line(124, 23, 126, 23);
  oled.line(125, 27, 126, 27);
  oled.line(125, 31, 126, 31);
  oled.line(125, 35, 126, 35);
  oled.line(125, 39, 126, 39);
  oled.line(124, 43, 126, 43);
  oled.line(125, 47, 126, 47);
  oled.line(125, 51, 126, 51);
  oled.line(125, 55, 126, 55);
  oled.line(125, 59, 126, 59);
  oled.line(124, 63, 126, 63);

  oled.line(104, 23, 104, 63);
  oled.line(105, 23, 107, 23);
  oled.line(105, 27, 106, 27);
  oled.line(105, 31, 106, 31);
  oled.line(105, 35, 106, 35);
  oled.line(105, 39, 106, 39);
  oled.line(105, 43, 107, 43);
  oled.line(105, 47, 106, 47);
  oled.line(105, 51, 106, 51);
  oled.line(105, 55, 106, 55);
  oled.line(105, 59, 106, 59);
  oled.line(105, 63, 107, 63);
}

//вертикальная шкала поиска экран6
void drawRule6() {
  //oled.line (4,46,85,46,1);
  oled.line (4,18,85,18,1);
  oled.line(105,19,105,59,1);
  oled.line(106,19,109,19,1);
  oled.line(106,23,107,23,1);
  oled.line(106,27,107,27,1);
  oled.line(106,31,107,31,1);
  oled.line(106,35,107,35,1);
  oled.line(106,39,109,39,1);
  oled.line(106,43,107,43,1);
  oled.line(106,47,107,47,1);
  oled.line(106,51,107,51,1);
  oled.line(106,55,107,55,1);
  oled.line(106,59,109,59,1);
}

void drawPoint(int n, uint8_t d) {
  oled.rect(114,59,124,59-(n)*4,d);
  oled.line(114,19,127,19,0);
  oled.line(114,23,127,23,0);
  oled.line(114,27,127,27,0);
  oled.line(114,31,127,31,0);
  oled.line(114,35,127,35,0);
  oled.line(114,39,127,39,0);
  oled.line(114,43,127,43,0);
  oled.line(114,47,127,47,0);
  oled.line(114,51,127,51,0);
  oled.line(114,55,127,55,0);
}

void drawDown(uint8_t d) {
  oled.line(3,33,3,41,d);
  oled.line(0,38,2,40,d);
  oled.line(4,40,6,38,d);
}

void drawUp(uint8_t d) {
  oled.line(3,33,3,41,d);
  oled.line(0,36,2,34,d);
  oled.line(4,34,6,36,d);
}

void sWiFi(uint8_t d) {
  if (Scr != 4) {
    int s = 8;
    oled.line(72 - s, 1, 73 - s, 1, d);
    oled.line(70 - s, 2, 71 - s, 2, d);
    oled.line(74 - s, 2, 75 - s, 2, d);
    oled.line(68 - s, 3, 69 - s, 3, d);
    oled.line(76 - s, 3, 77 - s, 3, d);
    oled.dot(67 - s, 4, d);
    oled.dot(78 - s, 4, d);
    oled.line(72 - s, 4, 73 - s, 4, d);
    oled.line(70 - s, 5, 71 - s, 5, d);
    oled.line(74 - s, 5, 75 - s, 5, d);
    oled.line(68 - s, 6, 69 - s, 6, d);
    oled.line(76 - s, 6, 77 - s, 6, d);
    oled.line(72 - s, 7, 73 - s, 7, d);
    oled.line(70 - s, 8, 71 - s, 8, d);
    oled.line(74 - s, 8, 75 - s, 8, d);
    oled.rect(72 - s, 10, 73 - s, 12, d);
  }
}

void nWiFi(uint8_t d) {
  if (Scr != 4) {
    int s = 8;
    oled.line(72 - s, 1, 73 - s, 1, d);
    oled.line(70 - s, 2, 71 - s, 2, 0);
    oled.line(74 - s, 2, 75 - s, 2, 0);
    oled.line(68 - s, 3, 69 - s, 3, 0);
    oled.line(76 - s, 3, 77 - s, 3, 0);
    oled.dot(67 - s, 4, 0);
    oled.dot(78 - s, 4, 0);
    oled.line(72 - s, 4, 73 - s, 4, d);
    oled.line(70 - s, 5, 71 - s, 5, 0);
    oled.line(74 - s, 5, 75 - s, 5, 0);
    oled.line(68 - s, 6, 69 - s, 6, 0);
    oled.line(76 - s, 6, 77 - s, 6, 0);
    oled.line(72 - s, 7, 73 - s, 7, d);
    oled.line(70 - s, 8, 71 - s, 8, 0);
    oled.line(74 - s, 8, 75 - s, 8, 0);
    oled.rect(72 - s, 10, 73 - s, 12, d);
  }
}

// рисует знак световой сигнализации
void drawlight(uint16_t d) {
  if (Scr != 4) {
    oled.rect(82, 1, 83, 2, d);
    oled.rect(82, 11, 83, 12, d);
    oled.rect(82, 5, 83, 8, d);
    oled.line(81, 6, 81, 7, d);
    oled.line(84, 6, 84, 7, d);
    oled.rect(78, 4, 79, 5, d);
    oled.rect(86, 4, 87, 5, d);
    oled.rect(78, 8, 79, 9, d);
    oled.rect(86, 8, 87, 9, d);
    oled.dot(79, 3, d);
    oled.dot(86, 3, d);
    oled.dot(79, 10, d);
    oled.dot(86, 10, d);
  }
  radSens.setLedState(d);
}

void rad_warning(uint8_t d) {
  if (Scr==6) {
    oled.clear(3, 0, 48, 13);
    if (d <= 30) {
      oled.setScale(1);
      oled.setCursorXY(0, 3);
      oled.print("нормально ");
    } 
    if (d>30 && d <= 100) {
      oled.setScale(1);
      oled.setCursorXY(0, 3);
      oled.print("внимание ");
    }
    if (d > 100) {
      oled.setScale(1);
      oled.setCursorXY(0, 3);
      oled.print("опасно   ");
    }
  }
}

// рисует знак радиации
void rad_sign(int is, uint8_t d) {
  int dY=0;
  if (Scr==6) {
    int dY=26;
    is=-3;
  } 
  if (Scr != 4 && Scr != 5 && Scr != 6) {
    oled.line(6 + is, 0 +dY , 13 + is, 0 +dY , d);
    oled.line(7 + is, 1 +dY , 12 + is, 1 +dY , d);
    oled.line(8 + is, 2 +dY , 11 + is, 2 +dY , d);
    oled.line(9 + is, 3 +dY , 10 + is, 3 +dY , d);
    oled.line(9 + is, 5 +dY , 10 + is, 5 +dY , d);
    oled.line(8 + is, 6 +dY , 11 + is, 6 +dY , d);
    oled.line(8 + is, 7 +dY , 11 + is, 7 +dY , d);
    oled.line(9 + is, 8 +dY , 10 + is, 8 +dY , d);
    oled.line(3 + is, 9 +dY , 7 + is, 9 +dY , d);
    oled.line(12 + is, 9 +dY , 16 + is, 9 +dY , d);
    oled.line(4 + is, 10 +dY , 7 + is, 10 +dY , d);
    oled.line(12 + is, 10 +dY , 15 + is, 10 +dY , d);
    oled.line(5 + is, 11 +dY , 7 + is, 11 +dY , d);
    oled.line(12 + is, 11 +dY , 14 + is, 11 +dY , d);
    oled.line(6 + is, 12 +dY , 7 + is, 12 +dY , d);
    oled.line(12 + is, 12 +dY , 13 + is, 12 +dY , d);
    oled.dot(7 + is, 12 +dY , d);
    oled.dot(12 + is, 12 +dY , d);
  }
}

void Rule5() {
  oled.line(1, 23, 1, 63);
  oled.line(127, 23, 127, 63);
  oled.line(2, 23, 3, 23);
  oled.line(125, 23, 126, 23);
  oled.line(2, 63, 3, 63);
  oled.line(125, 63, 126, 63);
  oled.dot(2, 27);
  oled.dot(2, 31);
  oled.dot(2, 35);
  oled.dot(2, 39);
  oled.dot(2, 43);
  oled.dot(2, 47);
  oled.dot(2, 51);
  oled.dot(2, 55);
  oled.dot(2, 59);
  oled.dot(126, 27);
  oled.dot(126, 31);
  oled.dot(126, 35);
  oled.dot(126, 39);
  oled.dot(126, 43);
  oled.dot(126, 47);
  oled.dot(126, 51);
  oled.dot(126, 55);
  oled.dot(126, 59);
}

void ResetDoze() {
  Time = 0;
  memset(Counts, 0, 110);
}

void RuleDelta() {
  int dX=1;
  oled.line(5-dX ,63,83-dX ,63,1);
  oled.line(5-dX ,62,5-dX ,61,1);
  oled.line(18-dX ,62,18-dX ,61,1);
  oled.line(31-dX ,62,31-dX ,61,1);
  oled.line(44-dX ,62,44-dX ,61,1);
  oled.line(57-dX ,62,57-dX ,61,1);
  oled.line(70-dX ,62,70-dX ,61,1);
  oled.line(83-dX ,62,83-dX ,61,1);

}

void setup() {
  Serial.begin(115200);
  oled.init();  // Инициализируем OLED в коде
  Wire.setClock(800000L);
  oled.clear();
  oled.update();
  pinMode(ADC_pin, OUTPUT);                  // Инициализируем АЦП как получатель данных
  pulsesPrev = radSens.getNumberOfPulses();  // Записываем значение для предотвращения серии тресков на старте
  prev_counter_cps = pulsesPrev;
  pinMode(buttonPin1, INPUT_PULLUP);  //Вход кнопки1
  pinMode(buttonPin2, INPUT_PULLUP);  //Вход кнопки2
  pinMode(buttonPin3, INPUT_PULLUP);  //Вход кнопки3
  oled.setContrast(254);        //Яркость дисплея
  oled.invertDisplay(0);
  oled.flipV(1);         // Я перевернул экран для удобства
  oled.flipH(1);
  oled.textMode(BUF_REPLACE);
  delay(1000);
  hello();  
  light = radSens.getLedState();
  Wire.begin();
  oled.update();  // Обновляем экран
  LittleFS.begin();
  FDstat_t stat = data.read();
  switch (stat) {
    case FD_FS_ERR:
      Serial.println("FS Error");
      break;
    case FD_FILE_ERR:
      Serial.println("Error");
      break;
    case FD_WRITE:
      Serial.println("Data Write");
      break;
    case FD_ADD:
      Serial.println("Data Add");
      break;
    case FD_READ:
      Serial.println("Data Read");
      break;
    default:
      break;
  }

  wifiOn = (mydata.wifi_e);
  sound = (mydata.sound_e);
  Scr = (mydata.scr_e);
  Serial.println(mydata.scr_e);
  Sensitivity = radSens.getSensitivity();
  s_f_c=mydata.s_f_c_e;
  m_v_c=mydata.m_v_c_e;
  k_count=mydata.k_c_e;
  Serial.print("коэф.счетчика=");
  Serial.println(k_count);
  radSens.setSensitivity(105);  //Установка чувствительности ссчетчика (105 для SBM20) первая проба 580

  dinamik(sound);
  light = radSens.getLedState();
  drawlight(light);

  if (wifiOn == 0) {WiFi.mode(WIFI_OFF);} 
  else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    Blynk.config(auth, "blynk.tk", 8080);
    delay(1000);
    Blynk.virtualWrite(V15, s_f_c );
    Blynk.virtualWrite(V16, m_v_c );
    Blynk.virtualWrite(V17, k_count);
  }
}

// управление световой индикацией с приложения
BLYNK_WRITE(V3) {
  int pinValue = param.asInt();
  light = pinValue;
}

// управление звуком с приложения
BLYNK_WRITE(V4) {
  int pinValue = param.asInt();
  sound = pinValue;
}

// управление видом экрана с приложения
BLYNK_WRITE(V7) {
  int pinValue = param.asInt();  
  Scr = pinValue;
  oled.clear();
  oled.update();
  Serial.println(pinValue);
}

// вкл/выкл экрана
BLYNK_WRITE(V13) {
  int pinValue = param.asInt();  
  OLEDPower = pinValue;
  oled.setPower(OLEDPower);
}

// чувствительность счетчика для RadSens
BLYNK_WRITE(V14) {
  int pinValue = param.asInt();  
  radSens.setSensitivity(pinValue);
  Serial.println(pinValue);
  Serial.println(radSens.getSensitivity());
}

//коэффициент счетчика для СБТ10а 6.15 сек (за это время кол.импульсов = мкР/ч)
BLYNK_WRITE(V17) {
  float pinValue = param.asFloat();  
  k_count=pinValue;
  mydata.k_c_e=pinValue;
  //data.update();
  data.updateNow();
  Serial.print("коэф.счетчика=");
  Serial.println(k_count);
}

//собственный фон счетчика
BLYNK_WRITE(V15) {
  float pinValue = param.asFloat();  
  s_f_c=pinValue;
  mydata.s_f_c_e=pinValue;
  //data.update();
  data.updateNow();
  Serial.print("собственный фон счетчика=");
  Serial.println(s_f_c);
}

// яркость экрана
BLYNK_WRITE(V18) {
  int pinValue = param.asInt();  
  oled.setContrast(pinValue);
}

//логарифмическая шкала мкР/ч
void log_shkala(float r) {


  if (r<=10) {
    oled.rect(4, 59, 4+r*1.3, 60, 1); 
    oled.rect(4+r*1.3+1, 59, 83, 60, 0);
  }
 
  if (r>10 && r<=100) {
    oled.rect(4, 59, 16, 60, 1);
    oled.rect(17, 59, 17+r*1.3/10, 60, 1);
    oled.rect(17+r*1.3/10, 59, 83, 60, 0);
  }
 
  if (r>100 && r<=1000) {
    oled.rect(5, 59, 16, 60, 1);
    oled.rect(17, 59, 30, 60, 1);
    oled.rect(30, 59, 30+r*1.3/100, 60, 1);
    oled.rect(30+r*1.3/100+1, 59, 83, 60, 0);
  }
  
  if (r>1000 && r<=10000) {
    oled.rect(5, 59, 16, 60, 1);
    oled.rect(17, 59, 30, 60, 1);
    oled.rect(31, 59, 43, 60, 1);
    oled.rect(43, 59, 43+r*1.3/1000, 60, 1);
    oled.rect(43+r*1.3/1000+1, 59, 83, 60, 0);
  }

  if (r>10000 && r<=100000) {
    oled.rect(5, 59, 16, 60, 1);
    oled.rect(17, 59, 30, 60, 1);
    oled.rect(31, 59, 43, 60, 1);
    oled.rect(44, 59, 56, 60, 1);
    oled.rect(56, 59, 56+r*1.3/10000, 60, 1);
    oled.rect(56+r*1.3/10000+1, 59, 83, 60, 0);
  }

  if (r>10000 && r<=1000000) {
    oled.rect(5, 59, 16, 60, 1);
    oled.rect(17, 59, 30, 60, 1);
    oled.rect(31, 59, 43, 60, 1);
    oled.rect(44, 59, 56, 60, 1);
    oled.rect(57, 59, 69, 60, 1);
    oled.rect(69, 59, 69+r*1.3/100000, 60, 1);
    oled.rect(69+r*1.3/100000+1, 59, 83, 60, 0);
  }

}

void loop() {
  
  //data.tick();
  if (data.tick() == FD_WRITE) Serial.println("Data updated!");

  bool net = WiFi.isConnected();
  if (net) {
    Blynk.run();
    sWiFi(wifiOn);
    Blynk.virtualWrite(V15, s_f_c );
    Blynk.virtualWrite(V16, m_v_c );
    Blynk.virtualWrite(V17, k_count);

  } else {
    nWiFi(wifiOn);
  }


  // раз в 250 мс происходит опрос счётчика импульсов для создания тресков, если число импульсов за 250 мс превысит 5, раздастся предупреждение
  if (millis() - timer_imp > 750) {  //250
    timer_imp = millis();
    int pulses = radSens.getNumberOfPulses();
    puls = pulses - pulsesPrev;
    tCylc++;
    Blynk.virtualWrite(V22, pulses);
    Blynk.virtualWrite(V23, tCylc);
    //звук.сигнал. предупреждение
    if (pulses - pulsesPrev > 5) {
      if (sound == 1) { warning(); }
      rad_sign(0, 1);  //рисует знак радиации
    } else {rad_sign(0, 0);}

    if (puls < 1) {
      warn--;
      if (warn == -1) { warn = 0; }
    }
    if (puls > 1) { warn = 1; }
    if (puls > 2) { warn = 2; }
    if (puls > 3) { warn = 3; }
    if (puls > 4) { warn = 4; }
    if (puls > 5) { warn = 5; }
    if (puls > 6) { warn = 6; }
    if (puls > 7) { warn = 7; }
    if (puls > 8) { warn = 8; }
    if (puls > 9) { warn = 9; }
    if (puls > 10) { warn = 10; }
    maswarn[i] = warn;
    cps250[i] = puls;
    i++;
    if (i == 5) { i = 1; }
    awarn = (maswarn[1] + maswarn[2] + maswarn[3] + maswarn[4]) / 4;
    cps1s = cps250[1] + cps250[2] + cps250[3] + cps250[4];  //сумма 4х замеров по 250мс
    //звук щелчков
    if (pulses > pulsesPrev) {
      for (int i = 0; i < (puls); i++) {
        if (sound == 1) { beep(); }
      }
    }
    pulsesPrev = pulses;
    // передача данных на blynk.tk
    if (net) {
      Blynk.virtualWrite(V0, cps1s); //Blynk CPS 4значения по 250мс = 1сек
      Blynk.virtualWrite(V6, awarn); //Blynk режим поиска
    }
    // экран №2
    if (Scr == 2) {
      oled.setScale(2);
      if (cps1s > 0) {
        int dec1 = (cps1s % 10) * 10;
        int dec2 = ((cps1s % 100) - (cps1s % 10)) / 1;
        int dec3 = ((cps1s % 1000) - (cps1s % 100)) / 10;
        oled.setCursorXY(0, 18);
        sprintf(buf4, "%d cps ", cps1s);
        oled.print(buf4);
        oled.rect(0, 35, dec1, 38, OLED_FILL);
        oled.rect(dec1 + 1, 35, 127, 38, 0);
        oled.rect(0, 43, dec2, 46, OLED_FILL);
        oled.rect(dec2 + 1, 43, 127, 46, 0);
        oled.rect(0, 51, dec3, 54, OLED_FILL);
        oled.rect(dec3 + 1, 51, 127, 54, 0);
        drawGrid();
        oled.setScale(1);
        oled.setCursorXY(105, 32);
        oled.print("x1");
        oled.setCursorXY(105, 41);
        oled.print("x10");
        oled.setCursorXY(105, 50);
        oled.print("x100");
        oled.setCursorXY(0, 50);
        oled.print("0");
        oled.setCursorXY(48, 50);
        oled.print("5");
        oled.setCursorXY(90, 50);
        oled.print("10");
      }
      drawRule();
    }
    // экран №3
    if (Scr == 3) {
      drawRuleV();
      oled.setCursorXY(0, 28);
      oled.setScale(3);
      oled.print(buf5);
      oled.setCursorXY(5, 56);
      oled.setScale(1);
      oled.print(buf6);
      if (cps1s > 0) {
        int dec1 = 4 * (cps1s % 10) * 1;
        int dec2 = 4 * ((cps1s % 100) - (cps1s % 10)) / 10;
        int dec3 = 4 * ((cps1s % 1000) - (cps1s % 100)) / 100;
        oled.rect(109, 63, 112, 63 - dec1, OLED_FILL);
        oled.rect(109, 63 - dec1 - 1, 112, 23, 0);
        oled.rect(114, 63, 117, 63 - dec2, OLED_FILL);
        oled.rect(114, 63 - dec1 - 1, 117, 23, 0);
        oled.rect(119, 63, 122, 63 - dec3, OLED_FILL);
        oled.rect(119, 63 - dec1 - 1, 122, 23, 0);
        drawGridV();
      }
    }
    // экран №4
    if (Scr == 4) {
      r = awarn;
      int dY = -5;
      int dX = -2;
      oled.setScale(1);
      oled.setCursorXY(11 + dX, 53 + dY);
      oled.print("0");
      oled.setCursorXY(18 + dX, 43 + dY);
      oled.print("1");
      oled.setCursorXY(28 + dX, 34 + dY);
      oled.print("2");
      oled.setCursorXY(39 + dX, 28 + dY);
      oled.print("3");
      oled.setCursorXY(51 + dX, 24 + dY);
      oled.print("4");
      oled.setCursorXY(63 + dX, 23 + dY);
      oled.print("5");
      oled.setCursorXY(75 + dX, 24 + dY);
      oled.print("6");
      oled.setCursorXY(87 + dX, 28 + dY);
      oled.print("7");
      oled.setCursorXY(98 + dX, 34 + dY);
      oled.print("8");
      oled.setCursorXY(108 + dX, 43 + dY);
      oled.print("9");
      oled.setCursorXY(110 + dX, 53 + dY);
      oled.print("10");
      int cX = 63;
      int cY = 83;
      int cX0 = 63;
      int cY0 = 83;
      int rad1 = 36;
      int rad2 = 70;
      oled.setCursorXY(37, 34);
      oled.setScale(1);
      oled.print(buf1);
      oled.circle(cX, cY, 76, OLED_STROKE);
      oled.circle(cX, cY, rad2, OLED_STROKE);
      oled.circle(cX, cY, rad1, 1);
      oled.line(7, 32, 11, 36);
      oled.line(18, 22, 22, 26);
      oled.line(32, 14, 35, 19);
      oled.line(47, 9, 48, 15);
      oled.line(63, 7, 63, 13);
      oled.line(79, 9, 78, 15);
      oled.line(94, 14, 91, 19);
      oled.line(108, 22, 104, 26);
      oled.line(119, 32, 115, 36);
      oled.line(0, 63, 127, 63);
      oled.line(0, 42, 0, 63);
      oled.line(127, 43, 127, 63);
      //-----Движение стрелки-----
      cX = 62;
      cY = 84;
      oled.line((cX + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad1), (cY - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad1), (cX0 + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad2), (cY0 - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad2), 0);
      cX = 64;
      cY = 82;
      oled.line((cX + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad1), (cY - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad1), (cX0 + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad2), (cY0 - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad2), 0);
      cX = 62;
      cY = 82;
      oled.line((cX + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad1), (cY - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad1), (cX0 + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad2), (cY0 - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad2), 0);
      cX = 64;
      cY = 84;
      oled.line((cX + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad1), (cY - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad1), (cX0 + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad2), (cY0 - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad2), 0);
      rr = r;
      cX = 62;
      cY = 84;
      oled.line((cX + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad1), (cY - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad1), (cX0 + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad2), (cY0 - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad2));
      cX = 64;
      cY = 82;
      oled.line((cX + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad1), (cY - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad1), (cX0 + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad2), (cY0 - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad2));
      cX = 62;
      cY = 82;
      oled.line((cX + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad1), (cY - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad1), (cX0 + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad2), (cY0 - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad2));
      cX = 64;
      cY = 84;
      oled.line((cX + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad1), (cY - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad1), (cX0 + cos(PI - (30 * PI / 180 + ((rr * 120 * PI / 180) / 10))) * rad2), (cY0 - sin(30 * PI / 180 + ((rr * 120 * PI / 180) / 10)) * rad2));
      //--------------------------
      oled.setScale(1);
      oled.setCursorXY(59, 54);
      oled.textMode(BUF_SUBTRACT);
      oled.print(warn);
      oled.textMode(BUF_REPLACE);
    }
    // экран №6
    if (Scr == 6) {
      drawPoint(warn, 1);
    }
  }



  //Считаем импульсы за окно 
  if (millis() - timer_cps > (okno*1000)) {  //1000
    timer_cps = millis();
    
    //>------------- окно --------------------
    //получение импульсов за окно
    counter_cps = radSens.getNumberOfPulses();
    prev_Count = Count;
    Count = (counter_cps - prev_counter_cps);
    prev_counter_cps = counter_cps;
    //<---------------------------------------------

    //>---------основной цикл------------------------
    Time=Time+1;
    TimeA=TimeA+1;
    if (Time > Cycl) { Time = Cycl; }
    if (TimeA > 750) { TimeA = 750; }
    SUMofCounts = 0; 
    SUMofCountsA = 0;

    //наполение массива
    if (priznC1000 == 1) { //признак достаточного набора
      for (int i1 = Cycl; i1 > Time; i1--) { Counts[i1] = 0; }
    }

    for (int i1 = 1; i1 < Time + 1; i1++) {
      if (i1 < Time) {
        Counts[Time + 1 - i1] = Counts[Time + 1 - i1 - 1];
      } else {
        Counts[Time + 1 - i1] = Count;
      }
    }

    for (int i1 = 1; i1 < TimeA + 1; i1++) {
      if (i1 < TimeA) {
        CountsA[TimeA + 1 - i1] = CountsA[TimeA + 1 - i1 - 1];
      } else {
        CountsA[TimeA + 1 - i1] = Count;
      }
    }

    //>---Сумма импульсов за основной цикл 10,5мин
    priznC1000 = 0;
    for (int i1 = 1; i1 < Time + 1; i1++) {
      SUMofCounts = SUMofCounts + Counts[i1];
      //>---Уменьшение основного цикла при наборе достаточного кол. импульсов за цикл
      if (SUMofCounts > 500) {
        priznC1000 = 1;
        Time = i1;
        break;
      }
      //<---
    }
    //<------------------------------------------------

    //>---Сумма импульсов в цикле 37,5мин.
    for (int i1 = 1; i1 < TimeA + 1; i1++) {
      SUMofCountsA = SUMofCountsA + CountsA[i1];
    }
    //<-----------------------------------------

    float allSUMofCounts=SUMofCounts;
    SUMofCounts=SUMofCounts-s_f_c*Time*okno ;
    SUMofCountsA=SUMofCountsA-s_f_c*TimeA*okno;
    if (SUMofCounts<0) {SUMofCounts=0;}
    if (SUMofCountsA<0) {SUMofCountsA=0;}
    prev_MDozi=MDozi;
    MDozi = (k_count / Time) * SUMofCounts;
    MDoziA = (k_count / TimeA) * SUMofCountsA;
     
    //if ((MDozi*1.2)<MDoziA) {Serial.println("меньше среднего");}
    //if (MDozi>(MDoziA*1.2)) {Serial.println("больше среднего");}
    //-----> сумма квадратов
    float sr_a = SUMofCounts / Time;
    float sum_kv = 0;
    for (int i1 = 1; i1 < Time + 1; i1++) {
      sum_kv = sum_kv + sq(Counts[i1] - sr_a);
    }
    //<----- сумма квадратов
    float st_otkl = sqrt(sum_kv / (Time - 1));            //стандартное отклонение
    float delta = ((2 * st_otkl / (sqrt(Time))) / sr_a);  //относительное отклонение

    if (sr_a * 7 < Count) {
      Serial.println("Скачек вверх");
      ResetDoze();
      if (Scr == 6) { //drawDown(0*); //drawUp(1);
      }
    }
    if ((Count + 1) * 6 < sr_a) {
      Serial.println("Скачек вниз");
      ResetDoze();
      if (Scr == 6) { //drawUp(0); //drawDown(1);
      }
    }

    if (net) {
      Blynk.virtualWrite(V8, delta*100); //Blynk отклонение отностит в %
      Blynk.virtualWrite(V9, sr_a); //Blynk среднее арифм CPS за окно
      Blynk.virtualWrite(V10, Count/okno); //Blynk  CPS за 1сек;
      Blynk.virtualWrite(V11, MDozi/okno); //Blynk мощность дозы
      Blynk.virtualWrite(V12, MDoziA/okno); //Blynk мощность дозы усреднение 750сек
      Blynk.virtualWrite(V20, Time*okno); 
      Blynk.virtualWrite(V21, allSUMofCounts); 
    }
    
    Serial.print(Time*okno);
    Serial.println(" секундный цикл  ");
    Serial.print(sr_a/okno);
    Serial.println(" сред.арифм.CPS");
    Serial.print(Count);
    Serial.println(" CPS за окно");
    Serial.print("сумма CPS за цикл ");
    Serial.println(allSUMofCounts);
    Serial.print("собств. фон за цикл ");
    Serial.println(s_f_c*Time*okno);
    Serial.print("           мощность дозы -");
    Serial.print(MDozi/okno);
    Serial.println("мкР/ч");
    Serial.print("предыдущая мощность дозы -");
    Serial.print(prev_MDozi/okno);
    Serial.println("мкР/ч");
    Serial.print("мощность дозы стат 37,5мин ");
    Serial.print(MDoziA/okno);
    Serial.println("мкР/ч");
    Serial.print("дельта ");
    Serial.print(delta * 100);
    Serial.println("% ");
    Serial.print(buf1);
    Serial.println(" мкР/ч динам. radsens");
    Serial.print("поиск - ");
    Serial.println(puls);
    if ((delta*100)<21 || priz20==0) {
      Serial.println(Time*okno);
      priz20=1;
    }
    Serial.println("____________________");

    // экран №5 Obsidin
    if (Scr == 5) {
      Rule5();

      for (int ic = 1; ic < 24; ic++) {
        cps24[25 - ic] = cps24[25 - ic - 1];
        if (ic == 1) {
          cpsMax = cps24[25 - ic];
        } else {
          if (cpsMax < cps24[25 - ic]) { cpsMax = cps24[25 - ic]; }
        }
        oled.rect(5 * ic, 63, 5 * ic + 2, 62 - cps24[25 - ic] * cpsK, OLED_FILL);

        oled.rect(5 * ic, 62 - cps24[25 - ic] * cpsK,5 * ic+2, 23, 0);
        Serial.print("IC=");
        Serial.print(25-ic);
        Serial.print("  cps=");
        Serial.print(cps24[25 - ic]);
        Serial.print("   Y=");
        Serial.println(62 - cps24[25 - ic] * cpsK);
      }
      Serial.println("____________________");
      
      if (Count > 0) {
        cps24[1] = Count; //cps1s;
      } else {
        if ((cps24[2] - 1) > 0) {
          cps24[1] = cps24[2] - 1;
        } else {
          cps24[1] = 0;
        }
      }
      if (cpsMax < cps24[1]) { cpsMax = cps24[1]; }
      if (cpsMax == 0) {
        cpsK = 40 / 8;
      } else {
        cpsK = 40 / cpsMax;
      }
      oled.rect(120, 63, 122, 62 - cps24[1] * cpsK, OLED_FILL);
      oled.rect(120, 62 - cps24[1] * cpsK, 122, 23, 0);
      Serial.println(cpsK);
      oled.setScale(2);
      oled.setCursor(0, 0);
      oled.print("      ");
      oled.setCursor(0, 0);
      oled.print("CPS ");
      oled.setCursor(45, 0);
      int Countint=Count;
      oled.print(Countint);
      oled.update();
      Serial.print("CPS Max=");
      Serial.println(cpsMax);

    }   

    // экран №6
    if (Scr == 6) {
      char MDozi_txt[50];
      char Delta_txt[50];
      if (delta*100>99) {delta=0.99;}
      sprintf(Delta_txt, "%.0f", delta * 100);
      strcat(Delta_txt, "%  ");
      if ((MDozi/okno) < 100) {
        sprintf(MDozi_txt, "%.2f  ", (MDozi/okno));
        
      }
      else {
        if ((MDozi/okno) < 1000) {
          sprintf(MDozi_txt, "%.1f  ", (MDozi/okno));
          
        } else {
          sprintf(MDozi_txt, "%.2f  ", MDozi / 3000);
        }
      }
      if ((MDozi/okno) < 10) {
        sprintf(MDozi_txt, "%.2f  ", (MDozi/okno));
        oled.setCursorXY(19, 26);
      } else { oled.setCursorXY(19, 26); }
      
      oled.setScale(2);
      oled.print(MDozi_txt);
      oled.setCursorXY(15, 48);
      oled.setScale(1);
      oled.print(Delta_txt);
      oled.setCursorXY(51, 48);
      oled.setScale(1);
      if (((MDozi/okno) < 1000)) {oled.print("мкР/ч ");} else {oled.print("мР/ч  ");}
      //знак +-
      oled.line(6,50,10,50,1);
      oled.line(6,54,10,54,1);
      oled.line(8,48,8,52,1);
      //<---- +-

      drawRule6();
      oled.rect(111,12,127,63,0);
      RuleDelta();
      int deltarX = 4+delta*80;
      if (deltarX>84) {deltarX=84;}
      log_shkala((MDozi/okno));
    }
  }

  // снимаем показания с дозиметра и выводим их на экран
  if (millis() - timer_cnt > 2000) {
    timer_cnt = millis();
    pT = radSens.getNumberOfPulses();
    cpulses = (pT - pP) / 2;
    pP = pT;
    din = radSens.getRadIntensyDynamic();

    rad_warning(MDozi/okno);

    if (din < 1000) {
      sprintf(buf1, "%.1fмкР/ч ", din);  // Собираем строку с показаниями динамической интенсивности
      sprintf(buf5, "%.1f", din);        // Собираем строку с показаниями динамической интенсивности
      sprintf(buf6, "%.1f мкР/ч ", radSens.getRadIntensyStatic());
    } else {
      sprintf(buf1, "%.3fмР/ч ", din / 1000);  // Собираем строку с показаниями динамической интенсивности
      sprintf(buf5, "%.2f", din / 1000);       // Собираем строку с показаниями динамической интенсивности
      sprintf(buf6, "%.1f мР/ч ", radSens.getRadIntensyStatic() / 1000);
    }
    sprintf(buf2, "ст: %.1fмкР/ч ", radSens.getRadIntensyStatic());  // Собираем строку с показаниями средней интенсивности за период работы
    sprintf(buf3, "%.1fcps ", cpulses);                              //Импульсы в секунду
    if (net) {
      Blynk.virtualWrite(V2, radSens.getRadIntensyStatic());  //Blynk RadSens статика
      Blynk.virtualWrite(V1, din);  //Blynk RadSens динамика
      Blynk.virtualWrite(V3, light);  //Blynk состояние света
      Blynk.virtualWrite(V4, sound);  //Blynk сотояние звука
    }
    // экран №1
    if (Scr == 1) {
      //---CPS---
      oled.setCursorXY(0, 18);
      oled.setScale(2);
      if (cpulses > 0) { oled.print(buf3); }
      //---Дин---
      oled.setCursorXY(0, 37);
      oled.setScale(2);
      oled.print(buf1);
      //---Стат---
      oled.setCursor(0, 7);
      oled.setScale(1);
      oled.print(buf2);
    }
  }

  // cчитываем показание с АЦП, рисуем батарею и создаём индикацию заряда, показания АЦП вы можете подстроить под своё удобство
  if (millis() - timer_bat > 5000) {
    timer_bat = millis();
    ADC = analogRead(ADC_pin);
    oled.rect(110, 1, 124, 11, OLED_STROKE);
    oled.rect(111, 2, 123, 10, 0);
    oled.rect(125, 4, 126, 8, OLED_FILL);
    if (ADC >= 320) {
      oled.rect(111, 2, 123, 10, 0);         //контур
      oled.rect(112, 3, 114, 9, OLED_FILL);  //1
      oled.rect(116, 3, 118, 9, OLED_FILL);  //2
      oled.rect(120, 3, 122, 9, OLED_FILL);  //3
    }
    if (ADC < 320 && ADC >= 309) {
      oled.rect(112, 3, 114, 9, OLED_FILL);  //1
      oled.rect(116, 3, 118, 9, OLED_FILL);  //2
      oled.rect(120, 3, 122, 9, 0);          //3
    }
    if (ADC < 309 && ADC >= 290) {
      oled.rect(112, 3, 114, 9, OLED_FILL);  //1
      oled.rect(116, 3, 118, 9, 0);          //2
      oled.rect(120, 3, 122, 9, 0);          //3
    }
    if (ADC < 290) {
      oled.rect(112, 3, 114, 9, 0);  //1
      oled.rect(116, 3, 118, 9, 0);  //2
      oled.rect(120, 3, 122, 9, 0);  //3
    }
    if (net) { Blynk.virtualWrite(V5, ADC); }  //Blynk заряд аккумулятора
  }

  // кнопка 1
  //корткое вкл/выкл свет, длинное вкл/выкл звук
  buttonState1 = !digitalRead(buttonPin1);
  if (buttonState1 == true && press_flag == false && millis() - last_press > 100) {
    press_flag = !press_flag;
    last_press = millis();
  }
  if (buttonState1 == true && press_flag == true && millis() - last_press > 1000) {
    long_press_flag = !long_press_flag;
    last_press = millis();
    Serial.println("долгое нажатие 1");
    if (sound == 0) {
      sound = 1;
    } else {
      sound = 0;
    }
    dinamik(sound);
    mydata.sound_e = sound;
    data.updateNow();
  }
  if (buttonState1 == false && press_flag == true && long_press_flag == true) {
    press_flag = !press_flag;
    long_press_flag = !long_press_flag;
  }
  if (buttonState1 == false && press_flag == true && long_press_flag == false) {
    press_flag = !press_flag;
    Serial.println("короткое нажатие 1");
    if (Scr == 10) {
      Sensitivity++;
    } else {
      light = !(light);
      drawlight(light);
    }
  }

  // кнопка 2
  // переключение видов экрана
  buttonState2 = !digitalRead(buttonPin2);
  if (buttonState2 == true && press_flag2 == false && millis() - last_press2 > 100) {
    press_flag2 = !press_flag2;
    last_press2 = millis();
  }
  if (buttonState2 == true && press_flag2 == true && millis() - last_press2 > 1000) {
    long_press_flag2 = !long_press_flag2;
    last_press2 = millis();
    Serial.println("долгое нажатие 2");
    if (OLEDPower == 0) {
      OLEDPower = 1;
      oled.setPower(1);
      ;
    } else {
      OLEDPower = 0;
      oled.setPower(0);
      ;
    }
  }
  if (buttonState2 == false && press_flag2 == true && long_press_flag2 == true) {
    press_flag2 = !press_flag2;
    long_press_flag2 = !long_press_flag2;
  }
  if (buttonState2 == false && press_flag2 == true && long_press_flag2 == false) {
    press_flag2 = !press_flag2;
    Serial.println("короткое нажатие 2");
    if (Scr == 10) {
      Sensitivity--;
    } else {
      oled.clear();
      clearSc = 0;
      Scr++;
      if (Scr == 7) { Scr = 1; }
      mydata.scr_e = Scr;
      data.update();
      data.updateNow();
      Serial.print("SCR=");
      Serial.print(Scr);
      Serial.print("  SCR в памяти=");
      Serial.print(mydata.scr_e);
    }
  }

  // кнопка 3
  // откл-вкл WiFi
  buttonState3 = !digitalRead(buttonPin3);
  if (buttonState3 == true && press_flag3 == false && millis() - last_press3 > 100) {
    press_flag3 = !press_flag3;
    last_press3 = millis();
  }
  if (buttonState3 == true && press_flag3 == true && millis() - last_press3 > 1000) {
    long_press_flag3 = !long_press_flag3;
    last_press3 = millis();
    Serial.println("долгое нажатие 3");
    //----->экран настроек
    if (Scr == 10) {
      oled.clear();
      Scr = mydata.scr_e;
        radSens.setSensitivity(Sensitivity);
      //Scr=1;
    } else {
      Scr = 10;
    }

    //<-----экран настроек
  }
  if (buttonState3 == false && press_flag3 == true && long_press_flag3 == true) {
    press_flag3 = !press_flag3;
    long_press_flag3 = !long_press_flag3;
  }
  if (buttonState3 == false && press_flag3 == true && long_press_flag3 == false) {
    press_flag3 = !press_flag3;
    Serial.println("короткое нажатие 3");

    if (Scr != 10) {
      if (wifiOn == 1) {
        wifiOn = 0;
        WiFi.mode(WIFI_OFF);
        Serial.println(wifiOn);
        Serial.println(WiFi.getMode());
      } else {
        wifiOn = 1;
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, pass);
        Blynk.config(auth, "blynk.tk", 8080);
        Serial.println(wifiOn);
        Serial.println(WiFi.getMode());
      }
      mydata.wifi_e = wifiOn;
      data.updateNow();
    } 
  }

  if (Scr == 10) {
    oled.clear();
    oled.setScale(1);
    oled.setCursor(0, 3);
    oled.print("Чувствительность");
    oled.setCursor(0, 4);
    oled.print("счетчика имп/мкР:");
    oled.setScale(2);
    oled.setCursor(0, 6);
    oled.print("     ");
    oled.setCursor(0, 6);
    oled.print(Sensitivity);
  }
  //------------------------------------------------------------------------

  drawlight(light);
  dinamik(sound);
  oled.update();  // Обновляем экран в конце цикла
}
