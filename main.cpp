#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define DHTPIN 16 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
int analogPin = 4, light = 0;

void lightTask(void *parameters){
  while(1){
  light = analogRead(analogPin);
  Serial.println(light);
  String lightStatus = (light < 3000) ? "Light is on" : "Light is off";
  Serial.println(lightStatus);
  vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void TempTask(void *parameters){
  while(1){
    // Wait a few seconds between measurements.
  delay(2000);
  
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

void setup(){
Serial.begin(9600);

pinMode(analogPin, INPUT);
xTaskCreate(lightTask, "lightTask", 2048, NULL, 1, NULL); 
xTaskCreate(TempTask, "TempTask", 2048, NULL, 1, NULL);
dht.begin();
}

void loop(){

}