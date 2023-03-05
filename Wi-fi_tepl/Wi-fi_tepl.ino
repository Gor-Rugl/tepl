// Ссылка для менеджера плат:
// http://arduino.esp8266.com/stable/package_esp8266com_index.json
/*
Библиотека Firebase Arduino содержит ссылку на отпечаток пальца SSL-сертификата Firebase. Этот отпечаток пальца может не совпадать с текущим отпечатком пальца.
Этот отпечаток пальца находится в FirebaseHttpClient.h (обычно в C:\Users\<User>\Documents\Arduino\libraries\firebase-arduino-<version>\src\FirebaseHttpClient.h).
Чтобы найти и изменить текущий отпечаток пальца:
Перейти к https://www.grc.com/fingerprints.htm
Войти "test.firebaseio.com "
Запишите отпечаток пальца (например, в данный момент он 04:E0:BD:B0:F8:63:22:3C:3A:19:7D:92:B6:79:2A:44:BF:77:FC:DA
ОткрытьC:\Users\<User>\Documents\Arduino\libraries\firebase-arduino-<version>\src\FirebaseHttpClient.h
Заменить значение kFirebaseFingerprint с отпечатком пальца (без двоеточий)
Перекомпилировать

*/

#include <ESP8266WiFi.h>                            // esp8266 library
#include <FirebaseArduino.h>                        // Библиотека для работы с базой данных

#define  FIREBASE_HOST "smarthouse-43fc3-default-rtdb.firebaseio.com"    // адрес сайта firebase
#define  FIREBASE_AUTH "CLl9k32wxjrXJ7UlPj8uMp6puSFZbxeJaeW031Ah"               // ключ доступа
#define WIFI_SSID "Rugl2"                                                 //provide ssid (wifi name)
#define WIFI_PASSWORD "Wi-1234567890"                                           //wifi password


void setup() {
   Serial.begin(9600);//Общение с UNO
  Serial.setTimeout(50);
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);               //Производим подключение к WiFi сети
  delay(300);
      while (WiFi.status() != WL_CONNECTED) { 
        delay(300);//Вывод ошибок о подключении
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  Firebase.setInt("num", 40);
  // handle error
  if (Firebase.failed()) {
    while(true){}
  }
  Firebase.setInt("temmm",0);
  Firebase.setInt("vl_pochv",0);
  Firebase.setInt("vl_vosduch",0);
}

void loop() {
  if(Serial.available()>1){
char  key=Serial.read();
 int data=Serial.parseInt();
  switch(key){
    case 'p':
      Firebase.setInt("vl_pochv",data);
    break;
    case 't':
       Firebase.setInt("temmm",data);
    break;
    case 'v':
      Firebase.setInt("vl_vosduch",data);
    break;
  }
}}
