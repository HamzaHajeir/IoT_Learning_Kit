/* This example shows how to use continuous mode to take
  range measurements with the VL53L0X. It is based on
  vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

  The range readings are in units of mm. */

#include <Wire.h>
#include <VL53L0X.h>
#include <BlynkSimpleEsp8266.h>


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "YourAuthToken";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

VL53L0X sensor;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  Blynk.begin(auth, ssid, pass);

  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  sensor.startContinuous(20);
}

void loop()
{
  Blynk.run();

  int distance = sensor.readRangeContinuousMillimeters();
  Serial.print(distance);
  if (sensor.timeoutOccurred()) {
    Serial.println(" TIMEOUT");
  }
  else {
    Serial.println(" mm");
    Blynk.virtualWrite(V1, distance);
  }

}
