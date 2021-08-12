const int numPots = 3;
const int potPins[numPots] = {0, 1, 2};
int potVals[numPots];

char outBuffer[64];

void setup() {
  Serial.begin(9600);
}

void loop() {

  // loop and assign pot values
  for (int i = 0; i < numPots; i++) {
    potVals[i] = analogRead(potPins[i]);
  }
  sprintf(outBuffer, "%u,%u,%u", potVals[0], potVals[1], potVals[2]);
  Serial.println(outBuffer);

  delay(2);
}
