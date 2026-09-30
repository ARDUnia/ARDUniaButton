#include <ARDUniaButton.h>

ARDUniaButton up(12);
ARDUniaButton down(13);

int volume = 50;

void setup() {
  Serial.begin(115200);
  up.begin();
  down.begin();
  up.setRepeat(500, 100);
  down.setRepeat(500, 100);
}

void loop() {
  up.update();
  down.update();

  if (up.wasClicked() || up.wasRepeated()) {
    if (volume < 100) volume++;
  }

  if (down.wasClicked() || down.wasRepeated()) {
    if (volume > 0) volume--;
  }

  if (up.wasLongPressed()) Serial.println("UP long press");
  if (down.wasLongPressed()) Serial.println("DOWN long press");
}
