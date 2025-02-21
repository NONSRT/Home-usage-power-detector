#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include "EmonLib.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "index.h"

#define DHTPIN 16 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
int analogPin = 34, light = 0, lightTemp;

int state = 0;

int LtaskPrio = 1, TtaskPrio = 1, lightCount = 0, airCount = 0;
double lightUnit, powerFlee, lightPower = 46, lightCheckDelay = 22500;
double airUnit, airFlee, airPower = 782.81, airCheckDelay = 22500;
String lightStatus;

float h, t, f;

EnergyMonitor emon1;
EnergyMonitor emon2;

String sliderValue = "OFF";

const char* PARAM_INPUT = "value";

const char* WIFI_NAME = "Homewifi_2.4G";
const char* WIFI_PASSWORD = "No0955653261";
AsyncWebServer server(80);

void lightTask(void *param){
  while(1){
  light = analogRead(analogPin);
  Serial.println(light);
  if(light < 3300){
    lightStatus = "Light is on";
    Serial.println(lightStatus);
    if(lightCount > 0){
    
    lightUnit  += lightPower*(lightCheckDelay/3600000)/1000;
    powerFlee = lightUnit * 3.96;
    }
    Serial.printf("%.4f\n",lightUnit);
    Serial.printf("%.4f\n",powerFlee);
    lightCount++;
  }
  else{
    lightStatus = "Light is off";
    Serial.println(lightStatus);
    lightCount = 0;
  }

  vTaskDelay(pdMS_TO_TICKS(lightCheckDelay));
  }
}

void TempTask(void *param){
  while(1){
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
  Serial.println(F("Failed to read from DHT sensor!"));
  return;
  }
  
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  
  Serial.print(F("\t\t\tHumidity: "));
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

  if(sliderValue == "ON"){
    Serial.printf("air is %s", sliderValue);
    if(airCount > 0){
      airUnit  += airPower*(airCheckDelay/3600000)/1000;
      airFlee = airUnit * 3.96;
    }
    Serial.printf("AU:%.4f\n",airUnit);
    Serial.printf("AF:%.4f\n",airFlee);
    airCount++;
  }
  else if(sliderValue == "OFF"){
    Serial.printf("air is %s", sliderValue);
    airCount = 0;
  }

  vTaskDelay(pdMS_TO_TICKS(22500));
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

// Connect to Wi-Fi
WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.println("Connecting to WiFi...");
}
Serial.println("Connected to WiFi");

// Print the ESP32's IP address
Serial.print("ESP32 Web Server's IP address: ");
Serial.println(WiFi.localIP());

// Serve the HTML page from the file
server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /");                                    // for debugging

  request->send(200, "text/html", webpage);
});

// Define a route to get the light data
server.on("/light", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /light");                         // for debugging
  request->send(200, "text/plain", lightStatus);
});

server.on("/lightFlee", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /lightFlee");                         // for debugging
  String lightFlee = String(powerFlee, 4);
  request->send(200, "text/plain", lightFlee);
});

server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /temperature");                         // for debugging
  float temperature = t;
  String temperatureStr = String(temperature, 2);
  request->send(200, "text/plain", temperatureStr);
});

server.on("/airStatus", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /airStatus");                         // for debugging
  request->send(200, "text/plain", sliderValue);
});

server.on("/airFlee", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /airFlee");                         // for debugging
  float af = airFlee;
  String AirFlee = String(af, 4);
  request->send(200, "text/plain", AirFlee);
});

server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      sliderValue = inputMessage;
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

// Start the server
server.begin();

xTaskCreate(lightTask, "lightTask", 2048, NULL, LtaskPrio, NULL); 
xTaskCreate(TempTask, "TempTask", 2048, NULL, TtaskPrio, NULL);
xTaskCreate(Power, "Power", 2048, NULL, TtaskPrio, NULL);
}

void loop(){
  
}