#include <ARDUniaButton.h>

ARDUniaButton button(12);

void setup() {
  Serial.begin(115200);

  button.onPress([]() {
    Serial.println("PRESS");
  });

  button.onClick([]() {
    Serial.println("CLICK");
  });

  button.onDoubleClick([]() {
    Serial.println("DOUBLE CLICK");
  });

  button.onMultiClick([](uint8_t count) {
    Serial.print("MULTI CLICK: ");
    Serial.println(count);
  });

  button.onLongPress([]() {
    Serial.println("LONG PRESS");
  });

  button.onRepeat([](uint8_t count) {
    Serial.print("REPEAT: ");
    Serial.println(count);
  });

  button.begin();
}

void loop() {
  button.update();
}
