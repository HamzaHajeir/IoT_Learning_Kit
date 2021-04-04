/* This example shows how to use continuous mode to take
  range measurements with the VL53L0X. It is based on
  vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

  The range readings are in units of mm. */

#include <Wire.h>
#include <VL53L0X.h>

#include <H4Plugins.h>

H4_USE_PLUGINS(115200, 20, false);

H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;


VL53L0X sensor;

void h4setup()
{
  Serial.begin(115200);
  Wire.begin();

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
  pinMode(D5, OUTPUT);

  h4.every(1000,
  []() {
    int distance = sensor.readRangeContinuousMillimeters();
    Serial.print(distance);
    if (sensor.timeoutOccurred()) {
      Serial.println(" TIMEOUT");
    }
    else {
      Serial.println(" mm");
    }
    int time_period = -1;
    if (50 < distance && distance < 600 )
    {
      static int previous_time_period;
      time_period = ::map(distance, 50, 600, 75, 500);
      Serial.print("new time_period = ");
      Serial.println(time_period);
      if (abs(time_period - previous_time_period) > 50)
      {
        h4fc.flashPattern("101000", time_period, D5, ACTIVE_HIGH);
        previous_time_period = time_period;
      }
      return;
    }
    h4fc.stopLED(D5);

  });
}
