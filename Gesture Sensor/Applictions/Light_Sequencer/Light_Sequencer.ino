#include <H4Plugins.h>
H4_USE_PLUGINS(115200, 20, false);
#define USER_BTN D3
#define UB_ACTIVE ACTIVE_LOW

#define ENCODER_A D7
#define ENCODER_B D6

#include <Wire.h>
#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;

H4_TIMER ROW_TIMER;
bool direction; // Driven by APDS
bool anOrBlink; //analog or row blink
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

long myMap(long x, long in_min, long in_max, long out_min, long out_max)
{
  const long dividend = out_max - out_min;
  const long divisor = in_max - in_min;
  const long delta = x - in_min;

  return (delta * dividend + (divisor / 2)) / divisor + out_min;
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

    if (anOrBlink)
    {
      int value = myMap(pin->autoValue, 30, 500, 0, 1023);
      analogValue = value;
      Serial.print("new analog ");
      Serial.println(analogValue);
    }
    else
    {
      ledStripRun();
      ROW_TIMER = h4.every(pin->autoValue, //call below void function every 250 ms
                           ledStripRun, nullptr, 10U, true);
      Serial.print("resetting timer ");
      Serial.println(pin->autoValue);
    }
  }
  //H4GM_FN_EVENT or int& variable
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

  h4.every(500, //call below void function every 500 ms
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
  });
}
