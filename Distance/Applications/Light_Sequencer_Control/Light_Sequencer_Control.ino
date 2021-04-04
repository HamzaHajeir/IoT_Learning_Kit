#include <Wire.h>
#include <H4Plugins.h>

H4_USE_PLUGINS(115200, 20, false);

H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;

#define USER_BTN D3
#define UB_ACTIVE ACTIVE_LOW

#define ENCODER_A D7
#define ENCODER_B D6

#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

#include <VL53L0X.h>
VL53L0X sensor;



H4_TIMER ROW_TIMER;
bool direction; // Driven by APDS
bool anOrBlink; //analog control or row blink
int analogValue = 512;

void ledStripRun()
{
  static int count;

  if (direction)
  {
    count--;
    if (count < 0)
      count = 3;
  }
  else
  {
    count++;
    count %= 4;
  }
  switch (count)
  {
    case 0:
      {

        analogWrite(D0, analogValue);
        analogWrite(D5, 0);
        analogWrite(D8, 0);
        analogWrite(D4, 0);
      }
      break;
    case 1:
      {
        analogWrite(D0, 0);
        analogWrite(D5, analogValue);
        analogWrite(D8, 0);
        analogWrite(D4, 0);
      }
      break;
    case 2:
      {
        analogWrite(D0, 0);
        analogWrite(D5, 0);
        analogWrite(D8, analogValue);
        analogWrite(D4, 0);
      }
      break;
    case 3:
      {
        analogWrite(D0, 0);
        analogWrite(D5, 0);
        analogWrite(D8, 0);
        analogWrite(D4, analogValue);
      }
      break;
  }
}

void h4setup()
{

  pinMode(D0, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(D4, OUTPUT);

  h4gm.Filtered(D3,
                INPUT,
                ACTIVE_LOW,
                ON, //Pass ON transition
  [](H4GPIOPin * ptr) {
    H4GM_PIN(Filtered); //Create the currect gpio function
    Serial.println("button clicked!");

    anOrBlink = !anOrBlink;
  });
  h4gm.EncoderAuto(ENCODER_A,
                   ENCODER_B,
                   INPUT,
                   ACTIVE_LOW,
                   30,  //Minimum value
                   500, //Maximum value
                   250, //Initial value,
                   25,  //Increment/decrement step
  [](H4GPIOPin * ptr) {
    H4GM_PIN(EncoderAuto); //Create the currect gpio function
    Serial.print("ENCODER AUTO value ");
    Serial.println(pin->autoValue);

    if (anOrBlink == false)
    {
      ledStripRun();
      ROW_TIMER = h4.every(pin->autoValue, //call below void function every 250 ms
                           ledStripRun, nullptr, 10U, true);
      Serial.print("resetting timer ");
      Serial.println(pin->autoValue);
    }
  }
                  );

  ROW_TIMER = h4.every(250, //call below void function every 250 ms
                       ledStripRun, nullptr, 10U, true);

  if (!apds.begin())
  {
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else
    Serial.println("Device initialized!");

  //gesture mode will be entered once proximity mode senses something close
  apds.enableProximity(true);
  apds.enableGesture(true);


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
    uint8_t gesture = apds.readGesture();
    if (gesture == APDS9960_DOWN)
    {
      Serial.println("v");
      direction = true;
    }
    if (gesture == APDS9960_LEFT)
    {
      Serial.println("<");
      direction = true;
    }
    if (gesture == APDS9960_UP)
    {
      Serial.println("^");
      direction = false;
    }
    if (gesture == APDS9960_RIGHT)
    {
      Serial.println(">");
      direction = false;
    }

    if (anOrBlink)
    {
      int distance = sensor.readRangeContinuousMillimeters();
      Serial.print(distance);
      if (sensor.timeoutOccurred()) {
        Serial.print(" TIMEOUT");
      }

      Serial.println();
      analogValue = 0;
      if (50 < distance && distance <= 500)
      {
        analogValue = ::map(distance, 50, 500, 1023, 0);
      }

      Serial.print("new analog ");
      Serial.println(analogValue);
    }
  }
          );
}
