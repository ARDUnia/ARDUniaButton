#include "Arduino.h"
#include "ARDUniaButton.h"
#include <cassert>
#include <iostream>

static uint32_t g_now = 0;
static int g_level = HIGH;

uint32_t millis() { return g_now; }
void pinMode(uint8_t, uint8_t) {}
int digitalRead(uint8_t) { return g_level; }

static void advance(ARDUniaButton& b, uint32_t ms) {
  g_now += ms;
  b.update();
}

static void press(ARDUniaButton& b) {
  g_level = LOW;
  advance(b, 1);
  advance(b, 30);
}

static void release(ARDUniaButton& b) {
  g_level = HIGH;
  advance(b, 1);
  advance(b, 30);
}

int main() {
  ARDUniaButton b(7);
  assert(b.isValid());
  assert(b.begin());

  press(b);
  assert(b.wasPressed());
  assert(!b.wasReleased());
  release(b);
  assert(b.wasReleased());

  advance(b, 399);
  assert(!b.wasClicked());
  advance(b, 1);
  assert(b.wasClicked());

  press(b); release(b);
  advance(b, 100);
  press(b); release(b);
  advance(b, 400);
  assert(b.wasDoubleClicked());

  press(b);
  advance(b, 1000);
  assert(b.wasLongPressed());
  release(b);
  advance(b, 400);
  assert(!b.wasClicked());

  b.setRepeat(500, 100);
  press(b);
  advance(b, 500);
  assert(b.wasRepeated());
  advance(b, 500);
  assert(b.wasLongPressed());
  assert(b.wasRepeated());
  release(b);
  advance(b, 400);
  assert(!b.wasClicked());

  // Three short presses must produce a multi-click event with the full count.
  press(b); release(b);
  advance(b, 100);
  press(b); release(b);
  advance(b, 100);
  press(b); release(b);
  advance(b, 400);
  assert(b.wasMultiClicked());
  assert(b.clickCount() == 3);
  assert(!b.wasClicked());
  assert(!b.wasDoubleClicked());

  // Reject timing values outside the documented ranges.
  assert(!b.setDebounce(0));
  assert(!b.setLongPressTime(50));
  assert(!b.setDoubleClickTime(10));
  assert(!b.setRepeat(10, 0));

  // Active-HIGH mode should report a press when the input becomes HIGH.
  g_level = LOW;
  ARDUniaButton activeHigh(8, INPUT, false);
  assert(activeHigh.isValid());
  assert(activeHigh.begin());
  g_level = HIGH;
  advance(activeHigh, 1);
  advance(activeHigh, 30);
  assert(activeHigh.wasPressed());
  g_level = LOW;
  advance(activeHigh, 1);
  advance(activeHigh, 30);
  assert(activeHigh.wasReleased());

  std::cout << "ARDUniaButton host tests: PASS\n";
  return 0;
}
