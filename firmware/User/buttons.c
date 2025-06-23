#include "buttons.h"
#include "main.h"

#define DEBOUNCE_TICKS 5  // 5 ticks of the main loop (e.g., 5 * 10ms = 50ms)

static uint8_t press_counters[BTN_COUNT] = {0};
static uint8_t release_counters[BTN_COUNT] = {0};
static uint8_t button_held[BTN_COUNT] = {0};
static uint8_t button_just_pressed[BTN_COUNT] = {0};

// Maps Button_t enum to LL_GPIO_PIN
const static uint32_t button_pins[] = {
    [BTN_DOWN] = LL_GPIO_PIN_0,
    [BTN_UP] = LL_GPIO_PIN_1,
    [BTN_BACK] = LL_GPIO_PIN_5,
    [BTN_ENTER] = LL_GPIO_PIN_6,
};

void Buttons_Init(void) {
  // GPIOs are already configured in main.c's APP_GPIO_Config
}

void Buttons_Update(void) {
  for (int i = 0; i < BTN_COUNT; i++) {
    // Clear the "just pressed" flag after one cycle
    button_just_pressed[i] = 0;

    // Read raw pin state. Pins are pull-up, so low means pressed.
    uint8_t is_pressed_raw = !LL_GPIO_IsInputPinSet(GPIOA, button_pins[i]);

    if (is_pressed_raw) {
      release_counters[i] = 0;
      if (press_counters[i] < DEBOUNCE_TICKS) {
        press_counters[i]++;
      } else {
        if (!button_held[i]) {
          button_just_pressed[i] = 1;
        }
        button_held[i] = 1;
      }
    } else {
      press_counters[i] = 0;
      if (release_counters[i] < DEBOUNCE_TICKS) {
        release_counters[i]++;
      } else {
        button_held[i] = 0;
      }
    }
  }
}

uint8_t Button_IsJustPressed(Button_t btn) {
  if (btn < BTN_COUNT) {
    return button_just_pressed[btn];
  }
  return 0;
}