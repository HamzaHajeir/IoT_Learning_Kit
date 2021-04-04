#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <string>

#include "index.h" //Our HTML webpage contents with javascripts

#define LED 2 //On board LED


Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();


//SSID and Password of your WiFi router
const char* ssid = "ORANGE_28E7";
const char* password = "abu@123456789";

ESP8266WebServer server(80); //Server on port 80

void handleRoot() {
  Serial.println("HandlingRoot");
  server.send(200, "text/html", MAIN_page); //Send web page
}

float humidity, temperature;

void handleADC() {

  sensors_event_t temp_event, pressure_event;
  bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);
  
  Serial.print(F("Temperature = "));
  Serial.print(temp_event.temperature);
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(pressure_event.pressure);
  Serial.println(" hPa");
  
  //Create JSON data
  String data = "{\"Pressure\":\"" + String(pressure_event.pressure) + "\", \"Temperature\":\"" + String(temp_event.temperature) + "\"}";

  digitalWrite(LED, !digitalRead(LED)); //Toggle LED on data request ajax
  server.send(200, "text/plane", data); //Send ADC value, temperature and humidity JSON to client ajax request
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  if (!bmp.begin(BMP280_ADDRESS_ALT,0x60)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1) delay(10);
  }
  
  WiFi.begin(ssid, password); //Connect to your WiFi router
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //IP address assigned to your ESP

  server.on("/", handleRoot); //Which routine to handle at root location. This is display page
  server.on("/readADC", handleADC); //This page is called by java Script AJAX

  server.begin(); //Start server
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient(); //Handle client requests
}
