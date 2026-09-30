#include <ARDUniaButton.h>

ARDUniaButton button(12);

void setup() {
  Serial.begin(115200);
  button.begin();
}

void loop() {
  button.update();

  if (button.wasPressed()) Serial.println("PRESS");
  if (button.wasReleased()) Serial.println("RELEASE");
  if (button.wasClicked()) Serial.println("CLICK");
  if (button.wasDoubleClicked()) Serial.println("DOUBLE CLICK");
  if (button.wasLongPressed()) Serial.println("LONG PRESS");
  if (button.wasRepeated()) Serial.println("REPEAT");
}
