#ifndef ARDUNIA_BUTTON_H
#define ARDUNIA_BUTTON_H

#include <Arduino.h>

class ARDUniaButton {
public:
  enum Event : uint8_t {
    EVENT_NONE = 0,
    EVENT_PRESS,
    EVENT_RELEASE,
    EVENT_CLICK,
    EVENT_DOUBLE_CLICK,
    EVENT_MULTI_CLICK,
    EVENT_LONG_PRESS,
    EVENT_REPEAT
  };

  enum State : uint8_t {
    STATE_IDLE = 0,
    STATE_PRESSED,
    STATE_HELD
  };

  // Default wiring: GPIO -> button -> GND, using the MCU's internal pull-up.
  explicit ARDUniaButton(uint8_t pin, uint8_t mode = INPUT_PULLUP,
                         bool activeLow = true);

  bool begin();
  void update();

  bool isPressed() const;
  bool isReleased() const;
  bool isHeld() const;

  bool wasPressed();
  bool wasReleased();
  bool wasClicked();
  bool wasDoubleClicked();
  bool wasMultiClicked();
  bool wasLongPressed();
  bool wasRepeated();

  Event event();
  uint8_t clickCount() const;
  State state() const;

  bool setDebounce(uint16_t ms);
  bool setLongPressTime(uint16_t ms);
  bool setDoubleClickTime(uint16_t ms);
  bool setRepeat(uint16_t initialDelayMs, uint16_t intervalMs);

  uint16_t debounceTime() const;
  uint16_t longPressTime() const;
  uint16_t doubleClickTime() const;
  uint16_t repeatInitialDelay() const;
  uint16_t repeatInterval() const;

  void onPress(void (*callback)());
  void onRelease(void (*callback)());
  void onClick(void (*callback)());
  void onDoubleClick(void (*callback)());
  void onMultiClick(void (*callback)(uint8_t count));
  void onLongPress(void (*callback)());
  void onRepeat(void (*callback)(uint8_t count));

  bool isValid() const;

private:
  uint8_t _pin;
  uint8_t _mode;
  bool _activeLow;
  bool _valid;
  bool _rawPressed;
  bool _pressed;
  State _state;

  uint32_t _lastRawChange;
  uint32_t _pressStarted;
  uint32_t _lastRepeat;
  uint32_t _lastRelease;
  uint8_t _clicks;
  uint8_t _repeatCount;

  uint16_t _debounceMs;
  uint16_t _longPressMs;
  uint16_t _doubleClickMs;
  uint16_t _repeatInitialMs;
  uint16_t _repeatIntervalMs;

  // One-shot flags are kept independently so events generated in one update()
  // cannot overwrite each other. event() remains the primary single-event view.
  uint8_t _event;
  uint8_t _eventClicks;
  bool _pressedEvent;
  bool _releasedEvent;
  bool _clickedEvent;
  bool _doubleClickedEvent;
  bool _multiClickedEvent;
  bool _longPressedEvent;
  bool _repeatedEvent;

  bool _longPressReported;

  void (*_onPress)();
  void (*_onRelease)();
  void (*_onClick)();
  void (*_onDoubleClick)();
  void (*_onMultiClick)(uint8_t);
  void (*_onLongPress)();
  void (*_onRepeat)(uint8_t);

  bool readPressed() const;
  void emit(Event e);
  void finalizeClicks();
  void clearEvents();
};

#endif
