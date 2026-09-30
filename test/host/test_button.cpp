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

  std::cout << "ARDUniaButton host tests: PASS\n";
  return 0;
}
