#include <stdio.h>

#include "menu.h"
#include "buttons.h"
#include "ascii_fonts.h"
#include "main.h"
#include "ssd1306.h"

// Menu actions
static void Action_Conectar(void);
static void Action_Creditos(void);

// Menu item definition
typedef struct {
  const char* name;
  void (*action)(void);
} MenuItem_t;

// Menu definitions
static const MenuItem_t mainMenuItems[] = {
    {"Conectar", Action_Conectar},
    {"Creditos", Action_Creditos},
};

#define MAIN_MENU_ITEM_COUNT (sizeof(mainMenuItems) / sizeof(MenuItem_t))

// Menu state
static int8_t selected_index = 0;
static uint8_t menu_active = 1;
static uint8_t needs_render = 1;

void Menu_Init(void) {
  selected_index = 0;
  menu_active = 1;
  needs_render = 1;
}

uint8_t Menu_IsActive(void) {
  return menu_active;
}

void Menu_Exit(void) {
  printf("Exiting menu\r\n");
  menu_active = 0;
}

static void Menu_Render(void) {
  printf("Rendering menu, selected index: %d\r\n", selected_index);
  SSD1306_Fill(SSD1306_COLOR_BLACK);
  SSD1306_UpdateScreen();
  SSD1306_GotoXY(26, 0);  // Centered title
  SSD1306_Puts("BSides CDMX", &Font_6x10, SSD1306_COLOR_WHITE);

  for (uint8_t i = 0; i < MAIN_MENU_ITEM_COUNT; i++) {
    uint8_t y_pos = 12 + i * 10;
    SSD1306_GotoXY(10, y_pos);

    if (i == selected_index) {
      // Draw inverted for selection
      SSD1306_DrawFilledRectangle(0, y_pos - 1, SSD1306_WIDTH, 10, SSD1306_COLOR_WHITE);
      SSD1306_Puts((char*)mainMenuItems[i].name, &Font_6x10, SSD1306_COLOR_BLACK);
    } else {
      SSD1306_Puts((char*)mainMenuItems[i].name, &Font_6x10, SSD1306_COLOR_WHITE);
    }
  }
  printf("Selected item: %s\r\n", mainMenuItems[selected_index].name);
  SSD1306_UpdateScreen();
}

void Menu_UpdateAndRender(void) {
  if (is_btn_up_pressed()) {
    printf("Button UP pressed\r\n");
    selected_index--;
    if (selected_index < 0) {
      selected_index = MAIN_MENU_ITEM_COUNT - 1;
    }
    needs_render = 1;
  } else if (is_btn_down_pressed()) {
    printf("Button DOWN pressed\r\n");
    selected_index++;
    if (selected_index >= MAIN_MENU_ITEM_COUNT) {
      selected_index = 0;
    }
    needs_render = 1;
  } else if (is_btn_enter_pressed()) {
    printf("Button ENTER pressed\r\n");
    if (mainMenuItems[selected_index].action) {
      mainMenuItems[selected_index].action();
      needs_render = 1;  // Rerender after action returns
    }
  }

  if (needs_render) {
    Menu_Render();
    needs_render = 0;
  }
}

// --- Menu Action Implementations ---

static void Action_Conectar(void) {
  SSD1306_Fill(SSD1306_COLOR_BLACK);
  SSD1306_UpdateScreen();
  SSD1306_GotoXY(3, 2);
  SSD1306_Puts("Esperando conexion", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_GotoXY(3, 12);
  SSD1306_Puts("con badges...", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_UpdateScreen();

  uint8_t dot_count = 0;
  uint32_t last_dot_time = 0;
  uint32_t current_time = 0;

  // Wait for back button with animated dots
  while (1) {
    if (is_btn_back_pressed()) {
      printf("Button BACK pressed\r\n");
      break;
    }
    LL_mDelay(10);
  }
}

static void Action_Creditos(void) {
  SSD1306_Fill(SSD1306_COLOR_BLACK);
  SSD1306_UpdateScreen();
  SSD1306_GotoXY(3, 2);
  SSD1306_Puts("Firmware by:", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_GotoXY(3, 12);
  SSD1306_Puts("Electronic Cats", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_GotoXY(15, 22);
  SSD1306_Puts("Press BACK", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_UpdateScreen();

  // Wait for back button
  while (1) {
    if (is_btn_back_pressed()) {
      printf("Button BACK pressed\r\n");
      break;
    }
    LL_mDelay(10);
  }
}