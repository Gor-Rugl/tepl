#define vl A0
#define sv A1
#define dth 2
#define pomp 6
#define vent 9

#define STRIP_PIN 7     // пин ленты
#define NUMLEDS 20      // кол-во светодиодов
#include <TroykaDHT.h>
#include <microLED.h>   // подключаем библу
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;
DHT dht(dth, DHT11);
void poko();
int vlvs,vlph,osvt;
float temp;
void setup() {
  Serial.begin(9600);
  dht.begin();
  strip.setBrightness(255);
  strip.clear();
  pinMode(vl,INPUT);
  pinMode(sv,INPUT);
  pinMode(pomp,OUTPUT);
  pinMode(vent,OUTPUT);
  strip.show(); // вывод изменений на ленту
}
//strip.fill(mRGB(255, 0, 0));  strip.show();  
void loop() {
  poko();
  if(vlph<300){digitalWrite(pomp,1);delay(2000);digitalWrite(pomp,0);}
  if(osvt>400){strip.fill(mRGB(255, 120, 10)); strip.show();} else {strip.fill(mRGB(0, 0, 0)); strip.show(); } 
  if(vlvs>40||temp>30.0){digitalWrite(vent,1);}else{digitalWrite(vent,0);}
  Serial.print("p");Serial.print(vlph);
  Serial.print("t");Serial.print(temp);
  Serial.print("v");Serial.print(vlvs);
}
void poko(){
  dht.read();
  temp = dht.getTemperatureC();
  vlvs= dht.getHumidity();
  vlph = analogRead(vl);
  osvt = analogRead(sv);
}
