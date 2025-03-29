#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_NeoPixel.h>
#include <TroykaDHT.h>
// создаём объект класса DHT
// передаём номер пина к которому подключён датчик и тип датчика
// типы сенсоров: DHT11, DHT21, DHT22


//  ---  Определения пинов  ---
const int DHT_PIN = 2;  //  Пин датчика температуры и влажности (DHT)
const int SOIL_MOISTURE_PIN = A0; //  Аналоговый пин датчика влажности почвы
const int LIGHT_SENSOR_PIN = A1; //  Аналоговый пин датчика освещенности

const int POT_PUMP_TIME_PIN = A3; //  Аналоговый пин потенциометра времени полива
const int POT_LIGHT_LEVEL_PIN = A4; //  Аналоговый пин потенциометра уровня освещенности
const int POT_LIGHT_DURATION_PIN = A5; //  Аналоговый пин потенциометра времени работы подсветки

const int PUMP_PIN = 12; //  Пин управления помпой
const int LED_STRIP_PIN = 7; //  Пин управления светодиодной лентой
const int LED_COUNT = 11; //  Количество светодиодов в ленте

//  ---  Параметры  ---
//  Диапазоны потенциометров (можно менять)
const int POT_MIN = 1023;
const int POT_MAX = 0;

//  Диапазоны времени полива (в секундах)
const int PUMP_TIME_MIN = 1;
const int PUMP_TIME_MAX = 60;

//  Диапазон уровня освещенности (значение с датчика)
const int LIGHT_LEVEL_MIN = 0;
const int LIGHT_LEVEL_MAX = 100;

//  Диапазон времени работы подсветки (в часах)
const int LIGHT_DURATION_MIN = 1;
const int LIGHT_DURATION_MAX = 12;

//  Пороговые значения (настраиваются)
int pumplevlSetting = 9;     //  Время полива (в секундах)
int lightLevelSetting = 400;    //  Уровень освещенности
int lightDurationSetting = 4;     //  Время работы подсветки (в часах)

//  Таймеры (в миллисекундах)
const unsigned long SOIL_CHECK_INTERVAL = 60000; //  Проверка влажности почвы раз в минуту
const unsigned long LIGHT_CHECK_INTERVAL = 120000; //  Проверка освещенности раз в 2 минуты

unsigned long lastSoilCheckTime = 0;
unsigned long lastLightCheckTime = 0;
unsigned long lightStartTime = 0;
bool isLightOn = false;

//  ---  Объекты  ---
DHT dht(DHT_PIN, DHT11);
#define COLOR_DEBTH 3
#include <microLED.h>   // подключаем библу
microLED<LED_COUNT, LED_STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;
//  ---  Функции  ---
void setup() {
  Serial.begin(9600);
  dht.begin();
  //  Инициализация пинов
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); //  Помпа выключена

  strip.setBrightness(250);
  strip.fill(mRGB(0, 0, 0));
  strip.show();   // вывод
  //  Вывод на дисплей приветственного сообщения
  delay(1000);
}

void SendData(String dev, String dat) {

  Serial.print(dev);                                // Отправляем данные dev(номер экрана, название переменной) на Nextion
  Serial.print("=");                                // Отправляем данные =(знак равно, далее передаем сами данные) на Nextion
  Serial.print(dat);                               // Отправляем данные data(данные) на Nextion

  comandEnd();
  dev = "";                                         // Очищаем переменную
  dat = "";                                        // Очищаем переменную
}

void comandEnd() {
  for (int i = 0; i < 3; i++) {
    Serial.write(0xff);
  }                            //Завершение команд дисплею
}





// Функция для преобразования значения потенциометра в значение для времени полива
int mapPotToPumpTime(int potValue) {
  return map(potValue, POT_MIN, POT_MAX, PUMP_TIME_MIN, PUMP_TIME_MAX);
}

// Функция для преобразования значения потенциометра в значение для уровня освещенности
int mapPotToLightLevel(int potValue) {
  return map(potValue, POT_MIN, POT_MAX, LIGHT_LEVEL_MIN, LIGHT_LEVEL_MAX);
}

// Функция для преобразования значения потенциометра в значение для времени работы подсветки
int mapPotToLightDuration(int potValue) {
  return map(potValue, POT_MIN, POT_MAX, LIGHT_DURATION_MIN, LIGHT_DURATION_MAX);
}

//  Функция для чтения и обработки данных с датчика влажности почвы
int readSoilMoisture() {
  int sensorValue = analogRead(SOIL_MOISTURE_PIN);
  //  Датчик влажности почвы выдает значения наоборот:
  //  Чем больше значение, тем меньше влажность
  //  Поэтому инвертируем значения для удобства
  return map(sensorValue, 0, 800, 0, 100); //  Масштабируем до диапазона 0-100%
}

//  Функция для чтения данных с датчика освещенности
int readLightLevel() {
  int sensorValue = analogRead(LIGHT_SENSOR_PIN);
  return map(sensorValue, 1023, 0, 0, 100); //  Масштабируем до диапазона 0-100%
}

//  Функция для полива
void waterPlant(int duration) {

  digitalWrite(PUMP_PIN, HIGH); //  Включить помпу
  delay(duration * 1000);        //  Полив
  digitalWrite(PUMP_PIN, LOW);  //  Выключить помпу

}

//  Функция для включения подсветки
void turnOnLights() {
  strip.fill(mRGB(255, 255, 255));
  strip.show();   
  isLightOn = true;
  lightStartTime = millis();
}

//  Функция для выключения подсветки
void turnOffLights() {
  Serial.println("Turning off lights.");
  strip.fill(mRGB(0, 0, 0));
  strip.show();   
  isLightOn = false;
}

void loop() {
  // Считываем значения с потенциометров
  int potPumpTimeValue = analogRead(POT_PUMP_TIME_PIN);
  int potLightLevelValue = analogRead(POT_LIGHT_LEVEL_PIN);
  int potLightDurationValue = analogRead(POT_LIGHT_DURATION_PIN);

  // Преобразуем значения потенциометров в необходимые диапазоны
  pumplevlSetting = mapPotToPumpTime(potPumpTimeValue);
  lightLevelSetting = mapPotToLightLevel(potLightLevelValue);
  lightDurationSetting = mapPotToLightDuration(potLightDurationValue);

  //  Чтение данных с датчиков
  int soilMoisture = readSoilMoisture();
  int lightLevel = readLightLevel();
  SendData("vlp.txt", "\"" + String(soilMoisture) + "%" + "\"");
  SendData("sv.txt", "\"" + String(lightLevel) + "%" + "\"");
  //  --- Проверка влажности почвы ---
  if (millis() - lastSoilCheckTime >= SOIL_CHECK_INTERVAL) {
    lastSoilCheckTime = millis();

    if (soilMoisture < pumplevlSetting) { //  Например, если влажность меньше 30%
      waterPlant(5); //  Полив в течение pumplevlSetting секунд
    }
  }

  //  --- Проверка освещенности ---
  if (millis() - lastLightCheckTime >= LIGHT_CHECK_INTERVAL) {
    lastLightCheckTime = millis();


    if (lightLevel < lightLevelSetting && !isLightOn) { //  Если освещенность ниже установленного уровня и подсветка выключена
      turnOnLights();
    }

  }


  //  --- Выключение подсветки по таймеру ---
  if (isLightOn && (millis() - lightStartTime >= (unsigned long)lightDurationSetting * 3600000)) { //  Прошло lightDurationSetting часов
    turnOffLights();
  }


  //  --- Вывод данных на дисплей ---
  dht.read();

  switch (dht.getState()) {
    // всё OK
    case DHT_OK:
      // выводим показания влажности и температуры
      SendData("tem.txt", "\"" + String(dht.getTemperatureC()) + "%" + "" + "\"");

      SendData("vl.txt", "\"" + String(dht.getHumidity()) + "%" + "" + "\"");
      break;
  }
  SendData("vrn.txt", "\"" + String(pumplevlSetting) + "%" "" + "\"");
  SendData("svn.txt", "\"" + String(lightLevelSetting) + "%" + "" + "\"");
  SendData("vrs.txt", "\"" + String(lightDurationSetting) + " h" + "" + "\"");

  delay(500); //  Небольшая задержка
}
