#include <Arduino.h>
#include <RotaryEncoder.h>

// Example for Arduino UNO with input signals on pin 2 and 3
#define PIN_IN1 7
#define PIN_IN2 6
#define PIN_IN3 4
#define PIN_IN4 5

// setup rotary encoders with 4 steps per latch for the 2 signal input pins
RotaryEncoder encoder1(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoder2(PIN_IN3, PIN_IN4, RotaryEncoder::LatchMode::FOUR3);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
//RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

#define PIN_BUTTON 8
int pButtonState = HIGH;
int buttonState = HIGH;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers

void setup()
{

  pinMode(PIN_BUTTON, INPUT_PULLUP);

  Serial.begin(115200);
  while (! Serial);
  Serial.println("SimplePollRotator example for the RotaryEncoder library.");
} // setup()


// Read the current position of the encoder and print out when changed.
void loop()
{

  // read button
  int buttonState = digitalRead(PIN_BUTTON);

  if (pButtonState != buttonState)
  {

    if ((millis() - lastDebounceTime) > debounceDelay) {

      lastDebounceTime = millis();

      if (buttonState == LOW)
      {
        // button has been pressed
        Serial.write(2);

      }
    }

    pButtonState = buttonState;

  } else {

  }

  static int pos1 = 0;
  encoder1.tick();
  int newPos1 = encoder1.getPosition();
  if (pos1 != newPos1) {
    if ((int)(encoder1.getDirection()) == 1) {
      //      Serial.println("1");
      Serial.write(1);
    } else {
      //      Serial.println("0");
      Serial.write(0);
    }
    pos1 = newPos1;
  } 



  static int pos2 = 0;
  encoder2.tick();
  int newPos2 = encoder2.getPosition();
  if (pos2 != newPos2) {
    if ((int)(encoder2.getDirection()) == 1) {
      Serial.write(3);
    } else {
      Serial.write(4);
    }
    pos2 = newPos2;
  } 

  
} // loop ()

