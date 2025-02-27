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
int analogPin = 34, light = 0;
SemaphoreHandle_t xMutex;

int lightCount = 0, airCount = 0, ctpowerCount = 0, PowFleeHrCount = 0;
float lightUnit, powerFlee, lightPower = 46, CheckDelay = 22500;
float h, t, hic, airUnit, usageUnit, airFlee, airPower = 782.81;
float CTPower, CTPowerUnit, CTPowerFlee;
float PowFleePerHr, PowFleePerHr_Temp, PowFleePerDay;
String lightStatus, heatStatus;

EnergyMonitor emon1;
EnergyMonitor emon2;

String airSliderValue = "OFF";
String lightSliderValue = "OFF";

const char* PARAM_INPUT = "value";

const char* WIFI_NAME = "WIFI_NAME";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";
AsyncWebServer server(80);

float UnitCalculate(float power){
  usageUnit = power*(CheckDelay/3600000)/1000; //22.5 sec/1hr
  return usageUnit;
}

void lightTask(void *param){
  while(1){
    xSemaphoreTake( xMutex,portMAX_DELAY);
    light = analogRead(analogPin);
    Serial.println(light);
    if(light < 3800){
      lightStatus = "Bright!";
      Serial.println(lightStatus);
    }
    else{
      if(lightSliderValue == "OFF")
      lightStatus = "Dark! turn on the light?";
      Serial.println(lightStatus);
    }
    //light turn on
    if(lightSliderValue == "ON"){
      Serial.printf("\n\t\tlight is %s", lightSliderValue);
      if(lightCount > 0){
        lightUnit += UnitCalculate(lightPower);
        powerFlee = lightUnit * 3.96;
      }
      Serial.printf("%.4f\n",lightUnit);
      Serial.printf("%.4f\n",powerFlee);
      lightCount++;
    }
    //light turn off
    else if(lightSliderValue == "OFF"){
      Serial.printf("\n\t\tlight is %s\n", lightSliderValue);
      lightCount = 0;
    }

    xSemaphoreGive( xMutex ); //release key(xMutex)
    vTaskDelay(pdMS_TO_TICKS(CheckDelay));
  }
}

void TempTask(void *param){
  while(1){
    xSemaphoreTake( xMutex,portMAX_DELAY);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
    }
    
    // Compute heat index in Celsius (isFahreheit = false)
    hic = dht.computeHeatIndex(t, h, false);
    
    Serial.print(F("\t\t\tHumidity: "));
    Serial.print(h);
    Serial.print(F("% Temperature: "));
    Serial.print(t);
    Serial.print(F(" C "));
    Serial.print(F(" Heat index: "));
    Serial.print(hic);
    Serial.print(F(" C "));

    if(hic >= 32){
      if(airSliderValue == "OFF")
      heatStatus = "so HOT! turn on the cooler?";
      else heatStatus = " ";
      Serial.println(heatStatus);
    }
    else if(hic < 17){
      if(airSliderValue == "ON")
      heatStatus = "so COLD! turn off the cooler?";
      else heatStatus = " ";
      Serial.println(heatStatus);
    }
    else{
      heatStatus = "COOL!";
      Serial.println(heatStatus);
    }

    //Aircon turn on
    if(airSliderValue == "ON"){
      Serial.printf("\n\t\tair is %s", airSliderValue);
      if(airCount > 0){
        airUnit += UnitCalculate(airPower);
        airFlee = airUnit * 3.96;
      }
      Serial.printf("AU:%.4f\n",airUnit);
      Serial.printf("AF:%.4f\n",airFlee);
      airCount++;
    }
    //Aircon turn off
    else if(airSliderValue == "OFF"){
      Serial.printf("\n\t\tair is %s\n", airSliderValue);
      airCount = 0;
    }

    xSemaphoreGive( xMutex ); //release key(xMutex)
    vTaskDelay(pdMS_TO_TICKS(CheckDelay));
  }
}


void Power(void *parameter){
  while(1){
    xSemaphoreTake( xMutex,portMAX_DELAY);
    //CT sensor from PC
    double Irms = emon1.calcIrms(1480);  // Calculate Irms only
    Serial.print(Irms*230.0);	       // Apparent power
    Serial.print(" ");
    Serial.println(Irms);		       // Irms

    //CT sensor from repeater
    double Irmss = emon2.calcIrms(1480);  // Calculate Irms only
    Serial.print(Irmss*230.0);	       // Apparent power
    Serial.print(" ");
    Serial.println(Irmss);		       // Irms

    CTPower = (Irms + Irmss)*230;

    if(ctpowerCount > 3){
      CTPowerUnit += UnitCalculate(CTPower);
      CTPowerFlee = CTPowerUnit * 3.96;
    }
    Serial.printf("PU:%.4f\n",CTPowerUnit);
    Serial.printf("PF:%.4f\n",CTPowerFlee);
    ctpowerCount++;

    Serial.println(uxTaskGetStackHighWaterMark(NULL));
    xSemaphoreGive( xMutex ); //release key(xMutex)
    vTaskDelay(pdMS_TO_TICKS(CheckDelay));
  }
}

void PowSum(void *param){
  while(1){
    xSemaphoreTake( xMutex,portMAX_DELAY);
    //Power flee / Hour
    if(ctpowerCount % 161 == 0){
      PowFleePerHr = powerFlee + airFlee + CTPowerFlee;
      PowFleePerHr_Temp += PowFleePerHr;
      PowFleeHrCount++;
    }
    //Power flee / Day
    if(PowFleeHrCount% 24 == 0){
      PowFleePerDay = PowFleePerHr_Temp;
    }
    xSemaphoreGive( xMutex ); //release key(xMutex)
    vTaskDelay(pdMS_TO_TICKS(CheckDelay));
  }
}

void setup(){
Serial.begin(9600);
pinMode(analogPin, INPUT); //light sensor setup
emon1.current(35, 111.1); //CT sensor from PC
emon2.current(39, 111.1); //CT sensor from repeater
dht.begin(); //start detect temperature

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
//-------------------------------------------------------------------------------------
// Define a route to get the light data
server.on("/light", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /light");                         // for debugging
  request->send(200, "text/plain", lightStatus);
});

server.on("/LightSwitchStatus", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /LightSwitchStatus");                         // for debugging
  request->send(200, "text/plain", lightSliderValue);
});

server.on("/lightFlee", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /lightFlee");                         // for debugging
  String lightFlee = String(powerFlee, 4);
  request->send(200, "text/plain", lightFlee);
});
//-------------------------------------------------------------------------------------
// Define a route to get the Temperature data
server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /temperature");                         // for debugging
  float temperature = t;
  String temperatureStr = String(temperature, 2);
  request->send(200, "text/plain", temperatureStr);
});

server.on("/heatIndex", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /heatIndex");                         // for debugging
  String heatIndecStr = String(hic, 2);
  request->send(200, "text/plain", heatIndecStr);
});

server.on("/heatStatus", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /heatStatus");                         // for debugging
  request->send(200, "text/plain", heatStatus);
});

server.on("/airStatus", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /airStatus");                         // for debugging
  request->send(200, "text/plain", airSliderValue);
});

server.on("/airFlee", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /airFlee");                         // for debugging
  float af = airFlee;
  String AirFlee = String(af, 4);
  request->send(200, "text/plain", AirFlee);
});
//-------------------------------------------------------------------------------------
// Define a route to get the Current power data
server.on("/CTPowerFlee", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /CTPowerFlee");                         // for debugging
  float CTPF = CTPowerFlee;
  String CTPFlee = String(CTPF, 4);
  request->send(200, "text/plain", CTPFlee);
});
//-------------------------------------------------------------------------------------
//// Define a route to get the sum of power flee data
server.on("/PowerFleePerHr", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /PowerFleePerHr");                         // for debugging
  String PFleePerHr = String(PowFleePerHr, 4);
  request->send(200, "text/plain", PFleePerHr);
});

server.on("/PowerFleePerDay", HTTP_GET, [](AsyncWebServerRequest* request) {
  Serial.println("ESP32 Web Server: New request received:");  // for debugging
  Serial.println("GET /PowerFleePerHr");                         // for debugging
  String PFleePerDay = String(PowFleePerDay, 4);
  request->send(200, "text/plain", PFleePerDay);
});
//-------------------------------------------------------------------------------------
//get value from Aircon switch
server.on("/airSlider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String AirInputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      AirInputMessage = request->getParam(PARAM_INPUT)->value();
      airSliderValue = AirInputMessage;
    }
    else {
      AirInputMessage = "No message sent";
    }
    Serial.println(AirInputMessage);
    request->send(200, "text/plain", "OK");
  });
//-------------------------------------------------------------------------------------
  //get value from Light switch
  server.on("/lightSlider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String LightInputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      LightInputMessage = request->getParam(PARAM_INPUT)->value();
      lightSliderValue = LightInputMessage;
    }
    else {
      LightInputMessage = "No message sent";
    }
    Serial.println(LightInputMessage);
    request->send(200, "text/plain", "OK");
  });

// Start the server
server.begin();

xMutex = xSemaphoreCreateMutex(); //craete Mutex key to control task memory usage

//create task
xTaskCreate(lightTask, "lightTask", 2048, NULL, 1, NULL); 
xTaskCreate(TempTask, "TempTask", 4096, NULL, 1, NULL);
xTaskCreate(Power, "Power", 8192, NULL, 1, NULL);
xTaskCreate(PowSum, "PowSum", 4096, NULL, 1, NULL);

}

void loop(){
  
}