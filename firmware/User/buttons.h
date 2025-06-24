#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#include <stdbool.h>

#define BUTTON_BACK_PIN LL_GPIO_PIN_5
#define BUTTON_ENTER_PIN LL_GPIO_PIN_6
#define BUTTON_DOWN_PIN LL_GPIO_PIN_0
#define BUTTON_UP_PIN LL_GPIO_PIN_1

/**
 * @brief Check if button back is pressed.
 * 
 * @return true if button back is pressed, false otherwise.
 */
bool is_btn_back_pressed();

/**
 * @brief Check if button enter is pressed.
 * 
 * @return true if button enter is pressed, false otherwise.
 */
bool is_btn_enter_pressed();

/**
 * @brief Check if button down is pressed.
 * 
 * @return true if button down is pressed, false otherwise.
 */
bool is_btn_down_pressed();

/**
 * @brief Check if button up is pressed.
 * 
 * @return true if button up is pressed, false otherwise.
 */
bool is_btn_up_pressed();

#endif  // BUTTONS_H