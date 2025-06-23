#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

typedef enum {
  BTN_DOWN,
  BTN_UP,
  BTN_BACK,
  BTN_ENTER,
  BTN_COUNT
} Button_t;

void Buttons_Init(void);
void Buttons_Update(void);
uint8_t Button_IsJustPressed(Button_t btn);

#endif  // BUTTONS_H