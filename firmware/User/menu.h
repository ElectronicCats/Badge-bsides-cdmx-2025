#ifndef MENU_H
#define MENU_H

#include <stdint.h>

void Menu_Init(void);
void Menu_UpdateAndRender(void);
uint8_t Menu_IsActive(void);
void Menu_Exit(void);

#endif  // MENU_H