#include <Arduino.h>
#include <RotaryEncoder.h>

// Example for Arduino UNO with input signals on pin 2 and 3
#define PIN_IN1 7
#define PIN_IN2 6

// Setup a RotaryEncoder with 4 steps per latch for the 2 signal input pins:
// RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

void setup()
{
  Serial.begin(115200);
  while (! Serial);
  Serial.println("SimplePollRotator example for the RotaryEncoder library.");
} // setup()


// Read the current position of the encoder and print out when changed.
void loop()
{
  static int pos = 0;
  encoder.tick();

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    if ((int)(encoder.getDirection()) == 1) {
//      Serial.println("1");
      Serial.write(1);
    } else {
//      Serial.println("0");
      Serial.write(0);
    }
    pos = newPos;
  } // if
} // loop ()

