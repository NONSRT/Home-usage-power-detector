#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include "EmonLib.h"

#define DHTPIN 16 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
int analogPin = 4, light = 0, lightTemp;

#define LIGHT_SWITCH GPIO_NUM_5 //Pin 5 for input button
int state = 0;
int LtaskPrio = 1, TtaskPrio = 1, count = 0;
double lightUnit, powerFlee, lightPower = 46, lightCheckDelay = 45000;

EnergyMonitor emon1;
EnergyMonitor emon2;

void lightTask(void *param){
  while(1){
  light = analogRead(analogPin);
  Serial.println(light);
  if(light < 3300){
    Serial.println("Light is on");
    if(count > 0){
    
    lightUnit  += lightPower*(lightCheckDelay/3600000)/1000;
    powerFlee = lightUnit * 3.96;
    }
    Serial.printf("%.4f\n",lightUnit);
    Serial.printf("%.4f\n",powerFlee);
    count++;
  }
  else{
    Serial.println("Light is off");
    count = 0;
  }

  vTaskDelay(pdMS_TO_TICKS(lightCheckDelay));
  }
}

void TempTask(void *param){
  while(1){
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
  Serial.println(F("Failed to read from DHT sensor!"));
  return;
  }
  
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("% Temperature: "));
  Serial.print(t);
  Serial.print(F(" C "));
  Serial.print(f);
  Serial.print(F(" F Heat index: "));
  Serial.print(hic);
  Serial.print(F(" C "));
  Serial.print(hif);
  Serial.println(F(" F"));

  vTaskDelay(pdMS_TO_TICKS(60000));
  }
}

void Power(void *param){
  while(1){
    double Irms = emon1.calcIrms(1480);  // Calculate Irms only
    Serial.print(Irms*230.0);	       // Apparent power
    Serial.print(" ");
    Serial.println(Irms);		       // Irms
  
    double Irmss = emon2.calcIrms(1480);  // Calculate Irms only
    Serial.print(Irmss*230.0);	       // Apparent power
    Serial.print(" ");
    Serial.println(Irmss);		       // Irms

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void setup(){
Serial.begin(9600);
pinMode(analogPin, INPUT);
emon1.current(13, 111.1);
emon2.current(12, 111.1);
dht.begin();
xTaskCreate(lightTask, "lightTask", 2048, NULL, LtaskPrio, NULL); 
xTaskCreate(TempTask, "TempTask", 2048, NULL, TtaskPrio, NULL);
xTaskCreate(Power, "Power", 2048, NULL, TtaskPrio, NULL);
}



void loop(){

}
