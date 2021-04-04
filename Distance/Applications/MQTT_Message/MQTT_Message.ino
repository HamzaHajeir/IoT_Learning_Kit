#include <Wire.h>
#include<H4Plugins.h>
H4_USE_PLUGINS(115200, 10, true) // Serial baud rate, Q size, SerialCmd autostop
//
//    Configuration
//
const char* WIFI_SSID = "XXXXXXXX";
const char* WIFI_PASS = "XXXXXXXX";
const char* MQTT_SERVER = "192.168.1.4";
const int   MQTT_PORT = 1883;

H4P_WiFi h4wifi(WIFI_SSID,WIFI_PASS,"MySensor");
H4P_AsyncMQTT h4mqtt(MQTT_SERVER,MQTT_PORT);

#include <VL53L0X.h>
VL53L0X sensor;

void h4setup()
{

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }

  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  sensor.startContinuous(20);


  h4.every(250, //call below void function every 500 ms
  []() {


    int distance = sensor.readRangeContinuousMillimeters();
    Serial.print(distance);
    if (sensor.timeoutOccurred()) {
      Serial.print(" TIMEOUT");
    }

    Serial.println();
    h4mqtt.publishDevice("distance",distance);
  }

          );
}
