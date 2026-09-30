# ARDUniaButton

A lightweight, non-blocking Arduino library for physical push buttons.

Designed in the same philosophy as **TTP223Row**: simple API, `begin()`, non-blocking `update()`, deterministic timing, no third-party dependencies, and compatibility with Arduino UNO, ESP8266 and ESP32.

## Features

- Non-blocking debounce using `millis()`
- Default wiring: digital pin -> button -> GND
- Internal `INPUT_PULLUP` by default
- Active-LOW or Active-HIGH logic
- `INPUT`, `INPUT_PULLUP`, and `INPUT_PULLDOWN` where the selected Arduino core defines `INPUT_PULLDOWN`
- Press / Release
- Single click
- Double click
- Multi click
- Long press
- Hold state
- Repeat while held
- Configurable timing
- Polling API
- Callback API
- No dynamic allocation
- No third-party dependencies
- Safe `millis()` elapsed-time comparisons

## Basic wiring

The default configuration is intended for a button connected between a digital pin and GND:

```text
Arduino GPIO ----[ BUTTON ]---- GND
```

No external pull-up resistor is required when using the default constructor because the internal pull-up is enabled.

## Basic usage

```cpp
#include <ARDUniaButton.h>

ARDUniaButton button(7); // INPUT_PULLUP, active LOW

void setup() {
  Serial.begin(115200);
  button.begin();
}

void loop() {
  button.update();

  if (button.wasPressed()) Serial.println("Pressed");
  if (button.wasReleased()) Serial.println("Released");
  if (button.wasClicked()) Serial.println("Click");
  if (button.wasDoubleClicked()) Serial.println("Double Click");
  if (button.wasMultiClicked()) {
    Serial.print("Multi Click: ");
    Serial.println(button.clickCount());
  }
  if (button.wasLongPressed()) Serial.println("Long Press");
  if (button.wasRepeated()) Serial.println("Repeat");
}
```

## Timing

Defaults:

| Setting | Default |
|---|---:|
| Debounce | 30 ms |
| Long press | 1000 ms |
| Double click window | 400 ms |
| Repeat initial delay | 500 ms |
| Repeat interval | 100 ms |

Configure before or after `begin()`:

```cpp
button.setDebounce(30);
button.setLongPressTime(1000);
button.setDoubleClickTime(400);
button.setRepeat(500, 100);
```

## Event semantics

`update()` must be called regularly from `loop()`.

Immediate events:

- `wasPressed()` is generated after a debounced press.
- `wasReleased()` is generated after a debounced release.
- `wasLongPressed()` is generated once when the long-press threshold is reached.
- `wasRepeated()` is generated at the configured repeat interval while the button remains pressed.

Click events are intentionally delayed until the double-click window expires. This is required to distinguish a single click from a double click.

A long press is not counted as a click.

The `wasXxx()` methods use independent one-shot flags. Therefore, if more than one event is generated during the same `update()`, querying the individual methods does not lose an event. `event()` returns the last event generated during that update.

## Repeat

Repeat is independent from the long-press event. By default it starts after 500 ms of continuous pressing and then repeats every 100 ms.

```cpp
button.setRepeat(500, 100);
```

The repeat callback receives an incrementing repeat count:

```cpp
button.onRepeat([](uint8_t count) {
  Serial.println(count);
});
```

## Callbacks

```cpp
void setup() {
  button.onClick([]() {
    Serial.println("Click");
  });

  button.onLongPress([]() {
    Serial.println("Long press");
  });

  button.onRepeat([](uint8_t count) {
    Serial.println(count);
  });

  button.begin();
}
```

## Active-HIGH

Active-HIGH input is also supported when the hardware provides an appropriate pull-down or external resistor:

```cpp
ARDUniaButton button(7, INPUT, false);
```

On Arduino UNO, the classic AVR core does not define `INPUT_PULLDOWN`. The library therefore does not reference that symbol unless the selected core provides it, so the library compiles correctly on UNO.

## API

### State

```cpp
button.isPressed();
button.isReleased();
button.isHeld();
button.state();
```

### Events

```cpp
button.wasPressed();
button.wasReleased();
button.wasClicked();
button.wasDoubleClicked();
button.wasMultiClicked();
button.wasLongPressed();
button.wasRepeated();
button.event();
button.clickCount();
```

### Configuration

```cpp
button.setDebounce(30);
button.setLongPressTime(1000);
button.setDoubleClickTime(400);
button.setRepeat(500, 100);
```

### Callbacks

```cpp
button.onPress(callback);
button.onRelease(callback);
button.onClick(callback);
button.onDoubleClick(callback);
button.onMultiClick(callback);
button.onLongPress(callback);
button.onRepeat(callback);
```

## Examples

- `BasicUsage` — polling API and all major events
- `Callbacks` — callback API
- `VolumeControl` — practical repeat use for up/down controls

## Compatibility

The library uses only standard Arduino APIs and has no third-party dependencies.

The default configuration is directly compatible with Arduino UNO and other boards that provide `INPUT_PULLUP`.

`INPUT_PULLDOWN` is accepted only when it is defined by the selected Arduino core.

## License

MIT
