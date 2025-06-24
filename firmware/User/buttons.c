#include <stdio.h>

#include "buttons.h"
#include "main.h"

#define DEBOUNCE_DELAY_MS 100

bool is_btn_back_pressed() {
  uint8_t is_pressed_now = !LL_GPIO_IsInputPinSet(GPIOA, BUTTON_BACK_PIN);
  if (is_pressed_now) {
    LL_mDelay(DEBOUNCE_DELAY_MS);
  }
  return is_pressed_now && !LL_GPIO_IsInputPinSet(GPIOA, BUTTON_BACK_PIN);
}

bool is_btn_enter_pressed() {
  uint8_t is_pressed_now = !LL_GPIO_IsInputPinSet(GPIOA, BUTTON_ENTER_PIN);
  if (is_pressed_now) {
    LL_mDelay(DEBOUNCE_DELAY_MS);
  }
  return is_pressed_now && !LL_GPIO_IsInputPinSet(GPIOA, BUTTON_ENTER_PIN);
}

bool is_btn_down_pressed() {
  uint8_t is_pressed_now = !LL_GPIO_IsInputPinSet(GPIOA, BUTTON_DOWN_PIN);
  if (is_pressed_now) {
    LL_mDelay(DEBOUNCE_DELAY_MS);
  }
  return is_pressed_now && !LL_GPIO_IsInputPinSet(GPIOA, BUTTON_DOWN_PIN);
}

bool is_btn_up_pressed() {
  uint8_t is_pressed_now = !LL_GPIO_IsInputPinSet(GPIOA, BUTTON_UP_PIN);
  if (is_pressed_now) {
    LL_mDelay(DEBOUNCE_DELAY_MS);
  }
  return is_pressed_now && !LL_GPIO_IsInputPinSet(GPIOA, BUTTON_UP_PIN);
}
