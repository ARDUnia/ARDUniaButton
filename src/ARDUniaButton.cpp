#include "ARDUniaButton.h"

ARDUniaButton::ARDUniaButton(uint8_t pin, uint8_t mode, bool activeLow)
  : _pin(pin), _mode(mode), _activeLow(activeLow), _valid(true),
    _rawPressed(false), _pressed(false), _state(STATE_IDLE),
    _lastRawChange(0), _pressStarted(0), _lastRepeat(0), _lastRelease(0),
    _clicks(0), _repeatCount(0),
    _debounceMs(30), _longPressMs(1000), _doubleClickMs(400),
    _repeatInitialMs(500), _repeatIntervalMs(100),
    _event(EVENT_NONE), _eventClicks(0),
    _pressedEvent(false), _releasedEvent(false), _clickedEvent(false),
    _doubleClickedEvent(false), _multiClickedEvent(false),
    _longPressedEvent(false), _repeatedEvent(false),
    _longPressReported(false),
    _onPress(nullptr), _onRelease(nullptr), _onClick(nullptr),
    _onDoubleClick(nullptr), _onMultiClick(nullptr),
    _onLongPress(nullptr), _onRepeat(nullptr) {
  if (pin == 255) {
    _valid = false;
  }

  // INPUT_PULLDOWN is not defined by classic AVR Arduino cores such as the
  // Arduino UNO core. Only validate it when the selected core exposes it.
  if (_mode != INPUT && _mode != INPUT_PULLUP) {
#ifdef INPUT_PULLDOWN
    if (_mode != INPUT_PULLDOWN) {
      _valid = false;
    }
#else
    _valid = false;
#endif
  }
}

bool ARDUniaButton::begin() {
  if (!_valid) return false;

  pinMode(_pin, _mode);

  const uint32_t now = millis();
  _rawPressed = readPressed();
  _pressed = _rawPressed;
  _lastRawChange = now;
  _pressStarted = _pressed ? now : 0;
  _lastRelease = now;
  _lastRepeat = now;
  _state = _pressed ? STATE_PRESSED : STATE_IDLE;
  _clicks = 0;
  _repeatCount = 0;
  _longPressReported = false;
  clearEvents();
  return true;
}

bool ARDUniaButton::readPressed() const {
  const bool level = digitalRead(_pin) == HIGH;
  return _activeLow ? !level : level;
}

void ARDUniaButton::update() {
  if (!_valid) return;

  const uint32_t now = millis();
  clearEvents();

  const bool raw = readPressed();
  if (raw != _rawPressed) {
    _rawPressed = raw;
    _lastRawChange = now;
  }

  if ((uint32_t)(now - _lastRawChange) >= _debounceMs &&
      _pressed != _rawPressed) {
    _pressed = _rawPressed;

    if (_pressed) {
      _state = STATE_PRESSED;
      _pressStarted = now;
      _lastRepeat = now;
      _repeatCount = 0;
      _longPressReported = false;
      emit(EVENT_PRESS);
      if (_onPress) _onPress();
    } else {
      _state = STATE_IDLE;
      _lastRelease = now;
      emit(EVENT_RELEASE);
      if (_onRelease) _onRelease();

      if (!_longPressReported && _clicks < 255) {
        ++_clicks;
      }
    }
  }

  if (_pressed) {
    const uint32_t heldFor = now - _pressStarted;

    if (!_longPressReported && heldFor >= _longPressMs) {
      _longPressReported = true;
      _state = STATE_HELD;
      emit(EVENT_LONG_PRESS);
      if (_onLongPress) _onLongPress();
    }

    if (_repeatIntervalMs > 0 && heldFor >= _repeatInitialMs &&
        (uint32_t)(now - _lastRepeat) >= _repeatIntervalMs) {
      _lastRepeat = now;
      if (_repeatCount < 255) ++_repeatCount;
      emit(EVENT_REPEAT);
      if (_onRepeat) _onRepeat(_repeatCount);
    }
  } else if (_clicks > 0 &&
             (uint32_t)(now - _lastRelease) >= _doubleClickMs) {
    finalizeClicks();
  }
}

void ARDUniaButton::finalizeClicks() {
  if (_clicks == 1) {
    emit(EVENT_CLICK);
    if (_onClick) _onClick();
  } else if (_clicks == 2) {
    emit(EVENT_DOUBLE_CLICK);
    if (_onDoubleClick) _onDoubleClick();
  } else if (_clicks > 2) {
    _eventClicks = _clicks;
    emit(EVENT_MULTI_CLICK);
    if (_onMultiClick) _onMultiClick(_clicks);
  }
  _clicks = 0;
}

void ARDUniaButton::clearEvents() {
  _event = EVENT_NONE;
  _eventClicks = 0;
  _pressedEvent = false;
  _releasedEvent = false;
  _clickedEvent = false;
  _doubleClickedEvent = false;
  _multiClickedEvent = false;
  _longPressedEvent = false;
  _repeatedEvent = false;
}

void ARDUniaButton::emit(Event e) {
  _event = (uint8_t)e;
  switch (e) {
    case EVENT_PRESS:         _pressedEvent = true; break;
    case EVENT_RELEASE:       _releasedEvent = true; break;
    case EVENT_CLICK:         _clickedEvent = true; break;
    case EVENT_DOUBLE_CLICK:  _doubleClickedEvent = true; break;
    case EVENT_MULTI_CLICK:   _multiClickedEvent = true; break;
    case EVENT_LONG_PRESS:    _longPressedEvent = true; break;
    case EVENT_REPEAT:        _repeatedEvent = true; break;
    default: break;
  }
}

bool ARDUniaButton::isPressed() const { return _pressed; }
bool ARDUniaButton::isReleased() const { return !_pressed; }
bool ARDUniaButton::isHeld() const { return _pressed && _state == STATE_HELD; }

bool ARDUniaButton::wasPressed() { return _pressedEvent; }
bool ARDUniaButton::wasReleased() { return _releasedEvent; }
bool ARDUniaButton::wasClicked() { return _clickedEvent; }
bool ARDUniaButton::wasDoubleClicked() { return _doubleClickedEvent; }
bool ARDUniaButton::wasMultiClicked() { return _multiClickedEvent; }
bool ARDUniaButton::wasLongPressed() { return _longPressedEvent; }
bool ARDUniaButton::wasRepeated() { return _repeatedEvent; }

ARDUniaButton::Event ARDUniaButton::event() {
  return (Event)_event;
}

uint8_t ARDUniaButton::clickCount() const { return _eventClicks; }
ARDUniaButton::State ARDUniaButton::state() const { return _state; }

bool ARDUniaButton::setDebounce(uint16_t ms) {
  if (ms == 0 || ms > 2000) return false;
  _debounceMs = ms;
  return true;
}

bool ARDUniaButton::setLongPressTime(uint16_t ms) {
  if (ms < 100 || ms > 60000) return false;
  _longPressMs = ms;
  return true;
}

bool ARDUniaButton::setDoubleClickTime(uint16_t ms) {
  if (ms < 50 || ms > 5000) return false;
  _doubleClickMs = ms;
  return true;
}

bool ARDUniaButton::setRepeat(uint16_t initialDelayMs, uint16_t intervalMs) {
  if (initialDelayMs < 50 || intervalMs == 0) return false;
  _repeatInitialMs = initialDelayMs;
  _repeatIntervalMs = intervalMs;
  return true;
}

uint16_t ARDUniaButton::debounceTime() const { return _debounceMs; }
uint16_t ARDUniaButton::longPressTime() const { return _longPressMs; }
uint16_t ARDUniaButton::doubleClickTime() const { return _doubleClickMs; }
uint16_t ARDUniaButton::repeatInitialDelay() const { return _repeatInitialMs; }
uint16_t ARDUniaButton::repeatInterval() const { return _repeatIntervalMs; }

void ARDUniaButton::onPress(void (*callback)()) { _onPress = callback; }
void ARDUniaButton::onRelease(void (*callback)()) { _onRelease = callback; }
void ARDUniaButton::onClick(void (*callback)()) { _onClick = callback; }
void ARDUniaButton::onDoubleClick(void (*callback)()) { _onDoubleClick = callback; }
void ARDUniaButton::onMultiClick(void (*callback)(uint8_t)) { _onMultiClick = callback; }
void ARDUniaButton::onLongPress(void (*callback)()) { _onLongPress = callback; }
void ARDUniaButton::onRepeat(void (*callback)(uint8_t)) { _onRepeat = callback; }

bool ARDUniaButton::isValid() const { return _valid; }
