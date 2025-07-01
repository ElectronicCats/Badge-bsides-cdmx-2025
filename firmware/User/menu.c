#include <stdio.h>
#include <string.h>

#include "py32f0xx_bsp_printf.h"
#include "py32f0xx_ll_bus.h"
#include "py32f0xx_ll_gpio.h"
#include "py32f0xx_ll_usart.h"

#include "ascii_fonts.h"
#include "buttons.h"
#include "main.h"
#include "menu.h"
#include "ssd1306.h"
#include "ws2812_spi.h"

static const char* credits_lines[] = {
    "Gracias a los",
    "patrocinadores HSBC",
    "Gracias a Electronic",
    "Cats Team",
    "Hardware: Uri,",
    "Capuchino, Lizeth,",
    "Yoshio y Carlos",
    "Firmware: Francisco",
    " - @deimoshall",
    "",
    "Hecho con amor",
    "desde Mexico"};
#define CREDITS_LINE_COUNT (sizeof(credits_lines) / sizeof(credits_lines[0]))
#define CREDITS_FONT_HEIGHT 10
#define CREDITS_VISIBLE_LINES 3

// Buffer and flags for UART communication
#define UART_BUF_SIZE 128
static uint8_t uart_rx_buf[UART_BUF_SIZE];
static volatile uint8_t uart_rx_flag = 0;
static volatile uint8_t uart_rx_pos = 0;

// Forward declarations for UART listener functions
static void UART_Listener_Init(void);
static void UART_Listener_DeInit(void);
static void UART_Listener_IRQCallback(void);

// Menu actions
static void connect_action();
static void credits_action();
static void settings_action();
static void leds_brightness_action();

typedef struct {
  uint8_t r, g, b;
} RGBColor;

static volatile RGBColor current_color = {0, 0x60, 0xC0};

// Menu item definition
typedef struct {
  const char* name;
  void (*action)(void);
} MenuItem_t;

// Menu definitions
static const MenuItem_t mainMenuItems[] = {
    {"Conectar", connect_action},
    {"Ajustes", settings_action},
    {"Creditos", credits_action},
};

#define MAIN_MENU_ITEM_COUNT (sizeof(mainMenuItems) / sizeof(MenuItem_t))
#define MAX_VISIBLE_MENU_ITEMS 2

// Menu state
static int8_t selected_index = 0;
static int8_t menu_offset = 0;
static uint8_t menu_active = 1;
static uint8_t needs_render = 1;

uint8_t i = 0, r = 0, g = 0x60, b = 0xC0;

static void leds_on_action(void) {
  ws2812_set_brightness(DEFAULT_BRIGHTNESS);

  SSD1306_Fill(SSD1306_COLOR_BLACK);
  SSD1306_UpdateScreen();
  SSD1306_DrawFilledRectangle(0, 22, SSD1306_WIDTH, 10, SSD1306_COLOR_BLACK);
  SSD1306_GotoXY(20, 16);
  SSD1306_Puts("LEDs Encendidos", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_UpdateScreen();
  LL_mDelay(1000);
}

static void leds_off_action(void) {
  ws2812_set_brightness(0);

  SSD1306_Fill(SSD1306_COLOR_BLACK);
  SSD1306_UpdateScreen();
  SSD1306_DrawFilledRectangle(0, 22, SSD1306_WIDTH, 10, SSD1306_COLOR_BLACK);
  SSD1306_GotoXY(20, 16);
  SSD1306_Puts("LEDs Apagados", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_UpdateScreen();
  LL_mDelay(1000);
}

static const MenuItem_t ledsMenuItems[] = {
    {"Encender", leds_on_action},
    {"Apagar", leds_off_action},
    {"Brillo", leds_brightness_action},
};
#define LEDS_MENU_ITEM_COUNT (sizeof(ledsMenuItems) / sizeof(MenuItem_t))

static void leds_submenu_action(void) {
  int8_t leds_selected_index = 0;
  int8_t leds_menu_offset = 0;
  uint8_t leds_needs_render = 1;

  while (1) {
    i = (i + 1) % WS2812_NUM_LEDS;
    ws2812_pixel(i, r++, g++, b++);

    if (is_btn_up_pressed()) {
      leds_selected_index--;
      if (leds_selected_index < 0) {
        leds_selected_index = LEDS_MENU_ITEM_COUNT - 1;
      }
      // Update offset if selection is out of view
      if (leds_selected_index < leds_menu_offset) {
        leds_menu_offset = leds_selected_index;
      } else if (leds_selected_index == LEDS_MENU_ITEM_COUNT - 1) {
        leds_menu_offset = LEDS_MENU_ITEM_COUNT > MAX_VISIBLE_MENU_ITEMS ? LEDS_MENU_ITEM_COUNT - MAX_VISIBLE_MENU_ITEMS : 0;
      }
      leds_needs_render = 1;
    } else if (is_btn_down_pressed()) {
      leds_selected_index++;
      if (leds_selected_index >= LEDS_MENU_ITEM_COUNT) {
        leds_selected_index = 0;
      }
      // Update offset if selection is out of view
      if (leds_selected_index >= leds_menu_offset + MAX_VISIBLE_MENU_ITEMS) {
        leds_menu_offset = leds_selected_index - MAX_VISIBLE_MENU_ITEMS + 1;
      } else if (leds_selected_index == 0) {
        leds_menu_offset = 0;
      }
      leds_needs_render = 1;
    } else if (is_btn_enter_pressed()) {
      if (ledsMenuItems[leds_selected_index].action) {
        ledsMenuItems[leds_selected_index].action();
      }
      leds_needs_render = 1;  // Rerender after action
    } else if (is_btn_back_pressed()) {
      break;  // Exit to parent menu
    }

    if (leds_needs_render) {
      SSD1306_Fill(SSD1306_COLOR_BLACK);
      SSD1306_UpdateScreen();
      SSD1306_GotoXY(45, 0);
      SSD1306_Puts("LEDs", &Font_6x10, SSD1306_COLOR_WHITE);

      for (uint8_t i = 0; i < MAX_VISIBLE_MENU_ITEMS; i++) {
        uint8_t item_index = leds_menu_offset + i;
        if (item_index >= LEDS_MENU_ITEM_COUNT) {
          break;
        }
        uint8_t y_pos = 12 + i * 10;
        SSD1306_GotoXY(10, y_pos);
        if (item_index == leds_selected_index) {
          SSD1306_DrawFilledRectangle(0, y_pos - 1, SSD1306_WIDTH, 10, SSD1306_COLOR_WHITE);
          SSD1306_Puts((char*)ledsMenuItems[item_index].name, &Font_6x10, SSD1306_COLOR_BLACK);
        } else {
          SSD1306_Puts((char*)ledsMenuItems[item_index].name, &Font_6x10, SSD1306_COLOR_WHITE);
        }
      }
      SSD1306_UpdateScreen();
      leds_needs_render = 0;
    }
    LL_mDelay(20);
  }
}

static const MenuItem_t settingsMenuItems[] = {
    {"LEDs", leds_submenu_action},
};

#define SETTINGS_MENU_ITEM_COUNT (sizeof(settingsMenuItems) / sizeof(MenuItem_t))

static void settings_action(void) {
  int8_t settings_selected_index = 0;
  uint8_t settings_needs_render = 1;

  while (1) {
    i = (i + 1) % WS2812_NUM_LEDS;
    ws2812_pixel(i, r++, g++, b++);

    if (is_btn_up_pressed()) {
      settings_selected_index--;
      if (settings_selected_index < 0) {
        settings_selected_index = SETTINGS_MENU_ITEM_COUNT - 1;
      }
      settings_needs_render = 1;
    } else if (is_btn_down_pressed()) {
      settings_selected_index++;
      if (settings_selected_index >= SETTINGS_MENU_ITEM_COUNT) {
        settings_selected_index = 0;
      }
      settings_needs_render = 1;
    } else if (is_btn_enter_pressed()) {
      if (settingsMenuItems[settings_selected_index].action) {
        settingsMenuItems[settings_selected_index].action();
      }
      settings_needs_render = 1;  // Rerender after action
    } else if (is_btn_back_pressed()) {
      break;  // Exit to main menu
    }

    if (settings_needs_render) {
      SSD1306_Fill(SSD1306_COLOR_BLACK);
      SSD1306_UpdateScreen();
      SSD1306_GotoXY(35, 0);
      SSD1306_Puts("Ajustes", &Font_6x10, SSD1306_COLOR_WHITE);

      for (uint8_t i = 0; i < SETTINGS_MENU_ITEM_COUNT; i++) {
        uint8_t y_pos = 12 + i * 10;
        SSD1306_GotoXY(10, y_pos);
        if (i == settings_selected_index) {
          SSD1306_DrawFilledRectangle(0, y_pos - 1, SSD1306_WIDTH, 10, SSD1306_COLOR_WHITE);
          SSD1306_Puts((char*)settingsMenuItems[i].name, &Font_6x10, SSD1306_COLOR_BLACK);
        } else {
          SSD1306_Puts((char*)settingsMenuItems[i].name, &Font_6x10, SSD1306_COLOR_WHITE);
        }
      }
      SSD1306_UpdateScreen();
      settings_needs_render = 0;
    }
    LL_mDelay(20);
  }
}

static void leds_brightness_action(void) {
  uint8_t percent = 100;
  uint8_t needs_render = 1;

  // Get current brightness as percent
  extern uint8_t _brightness;
  percent = (_brightness * 100 + 127) / 255;  // Round to nearest percent

  // Snap to nearest 10%
  percent = (percent / 10) * 10;

  while (1) {
    if (is_btn_up_pressed()) {
      if (percent < 100) {
        percent += 10;
        needs_render = 1;
      }
    } else if (is_btn_down_pressed()) {
      if (percent > 0) {
        percent -= 10;
        needs_render = 1;
      }
    } else if (is_btn_back_pressed()) {
      break;
    }

    if (needs_render) {
      SSD1306_Fill(SSD1306_COLOR_BLACK);
      SSD1306_UpdateScreen();
      SSD1306_GotoXY(15, 8);
      SSD1306_Puts("Brillo NeoPixels", &Font_6x10, SSD1306_COLOR_WHITE);

      char buf[20];
      snprintf(buf, sizeof(buf), "%3d%%", percent);
      SSD1306_GotoXY(50, 20);
      SSD1306_Puts(buf, &Font_6x10, SSD1306_COLOR_WHITE);
      SSD1306_UpdateScreen();

      // Set brightness
      uint8_t value = (percent * 255) / 100;
      ws2812_set_brightness(value);

      needs_render = 0;
    }
    LL_mDelay(100);
  }
}

void Menu_Init(void) {
  selected_index = 0;
  menu_offset = 0;
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
  SSD1306_GotoXY(26, 0);
  SSD1306_Puts("BSides CDMX", &Font_6x10, SSD1306_COLOR_WHITE);

  for (uint8_t i = 0; i < MAX_VISIBLE_MENU_ITEMS; i++) {
    uint8_t item_index = menu_offset + i;
    if (item_index >= MAIN_MENU_ITEM_COUNT) {
      break;
    }

    uint8_t y_pos = 12 + i * 10;
    SSD1306_GotoXY(10, y_pos);

    if (item_index == selected_index) {
      // Draw inverted for selection
      SSD1306_DrawFilledRectangle(0, y_pos - 1, SSD1306_WIDTH, 10, SSD1306_COLOR_WHITE);
      SSD1306_Puts((char*)mainMenuItems[item_index].name, &Font_6x10, SSD1306_COLOR_BLACK);
    } else {
      SSD1306_Puts((char*)mainMenuItems[item_index].name, &Font_6x10, SSD1306_COLOR_WHITE);
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
    // Update offset if selection is out of view
    if (selected_index < menu_offset) {
      menu_offset = selected_index;
    } else if (selected_index == MAIN_MENU_ITEM_COUNT - 1) {
      // Handle wrap-around
      menu_offset = MAIN_MENU_ITEM_COUNT > MAX_VISIBLE_MENU_ITEMS ? MAIN_MENU_ITEM_COUNT - MAX_VISIBLE_MENU_ITEMS : 0;
    }
    needs_render = 1;
  } else if (is_btn_down_pressed()) {
    printf("Button DOWN pressed\r\n");
    selected_index++;
    if (selected_index >= MAIN_MENU_ITEM_COUNT) {
      selected_index = 0;
    }
    // Update offset if selection is out of view
    if (selected_index >= menu_offset + MAX_VISIBLE_MENU_ITEMS) {
      menu_offset = selected_index - MAX_VISIBLE_MENU_ITEMS + 1;
    } else if (selected_index == 0) {
      // Handle wrap-around
      menu_offset = 0;
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

/**
 * @brief Initializes USART2 for listening to incoming data.
 *        - Configures PA2 (TX) and PA3 (RX).
 *        - Sets up USART2 with 115200 baud, 8-N-1.
 *        - Enables RXNE interrupt for receiving data.
 */
static void UART_Listener_Init(void) {
  // Enable clocks for USART2 and GPIOA
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  // Configure GPIO pins (PA2 TX, PA3 RX) for USART2
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;  // AF4 for USART2 on PA2/PA3

  // PA2 (TX)
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  // PA3 (RX)
  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configure USART2 parameters
  LL_USART_SetBaudRate(USART2, SystemCoreClock, LL_USART_OVERSAMPLING_16, 115200);
  LL_USART_SetDataWidth(USART2, LL_USART_DATAWIDTH_8B);
  LL_USART_SetStopBitsLength(USART2, LL_USART_STOPBITS_1);
  LL_USART_SetParity(USART2, LL_USART_PARITY_NONE);
  LL_USART_SetHWFlowCtrl(USART2, LL_USART_HWCONTROL_NONE);
  LL_USART_SetTransferDirection(USART2, LL_USART_DIRECTION_TX_RX);

  // Enable USART2 and its interrupts
  LL_USART_Enable(USART2);
  LL_USART_EnableIT_RXNE(USART2);
  LL_USART_EnableIT_ERROR(USART2);

  // Configure and enable NVIC for USART2 interrupts
  NVIC_SetPriority(USART2_IRQn, 1);
  NVIC_EnableIRQ(USART2_IRQn);
}

/**
 * @brief De-initializes USART2 to stop listening and save power.
 */
static void UART_Listener_DeInit(void) {
  // Disable USART2 and its interrupt
  LL_USART_Disable(USART2);
  NVIC_DisableIRQ(USART2_IRQn);
  // Disable peripheral clock to save power
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART2);
}

/**
 * Send one char to UART port
 */
void APP_UART_TxChar(char ch) {
  LL_USART_TransmitData8(USART2, ch);
  while (!LL_USART_IsActiveFlag_TC(USART2))
    ;
  LL_USART_ClearFlag_TC(USART2);
}

/**
 * Send one string (a serial of chars end with '\0') to UART port
 */
void APP_UART_TxString(char* str) {
  while (*str)
    APP_UART_TxChar(*str++);
}

static void connect_action(void) {
  char tx_buf[32];
  const uint32_t tx_interval = 100;  // Transmit every 100ms

  SSD1306_Fill(SSD1306_COLOR_BLACK);
  SSD1306_UpdateScreen();
  SSD1306_GotoXY(3, 2);
  SSD1306_Puts("Esperando conexion", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_GotoXY(3, 12);
  SSD1306_Puts("con badges...", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_UpdateScreen();

  // Initialize and start the UART listener
  UART_Listener_Init();
  printf("UART listener started on USART2.\r\n");
  bool syncronized = false;

  while (1) {
    // Animate local LEDs with the current (potentially synchronized) color
    i = (i + 1) % WS2812_NUM_LEDS;
    if (!syncronized) {
      ws2812_pixel(i, current_color.r++, current_color.g++, current_color.b++);
    } else {
      ws2812_pixel(i, current_color.r, current_color.g, current_color.b);
    }

    if (is_btn_back_pressed()) {
      UART_Listener_DeInit();  // Clean up before exiting
      BSP_USART_Config(115200);
      printf("Button BACK pressed, stopping listener.\r\n");
      break;
    }

    // Check if a complete message has been received via UART
    if (uart_rx_flag) {
      syncronized = true;
      // APP_UART_TxString("Message received: ");
      // APP_UART_TxString((char*)uart_rx_buf);
      // APP_UART_TxChar('\n');
      // Check if it's a valid RGB command
      int r_val, g_val, b_val;
      if (sscanf((char*)uart_rx_buf, "rgb:%d,%d,%d.", &r_val, &g_val, &b_val) == 3) {
        // Valid command received, update local color
        current_color.r = (uint8_t)r_val;
        current_color.g = (uint8_t)g_val;
        current_color.b = (uint8_t)b_val;

        SSD1306_Fill(SSD1306_COLOR_BLACK);
        SSD1306_UpdateScreen();
        // SSD1306_DrawFilledRectangle(0, 22, SSD1306_WIDTH, 10, SSD1306_COLOR_BLACK);
        SSD1306_GotoXY(19, 0);
        SSD1306_Puts("Las rutas estan", &Font_6x10, SSD1306_COLOR_WHITE);
        SSD1306_GotoXY(43, 10);
        SSD1306_Puts("unidas,", &Font_6x10, SSD1306_COLOR_WHITE);
        SSD1306_GotoXY(28, 20);
        SSD1306_Puts("cuantos mas?", &Font_6x10, SSD1306_COLOR_WHITE);
        SSD1306_UpdateScreen();
      }

      // Reset the flag and buffer position for the next message
      uart_rx_pos = 0;
      uart_rx_flag = 0;
    }

    snprintf(tx_buf, sizeof(tx_buf), "rgb:%d,%d,%d.", current_color.r, current_color.g, current_color.b);
    APP_UART_TxString(tx_buf);

    LL_mDelay(20);  // Small delay to yield CPU time
  }
}

static void credits_action(void) {
  int8_t scroll_offset = 0;
  uint8_t needs_render = 1;

  while (1) {
    i = (i + 1) % WS2812_NUM_LEDS;
    ws2812_pixel(i, r++, g++, b++);

    if (is_btn_up_pressed()) {
      if (scroll_offset > 0) {
        scroll_offset--;
        needs_render = 1;
      }
    } else if (is_btn_down_pressed()) {
      if (scroll_offset < (CREDITS_LINE_COUNT - CREDITS_VISIBLE_LINES)) {
        scroll_offset++;
        needs_render = 1;
      }
    } else if (is_btn_back_pressed()) {
      printf("Button BACK pressed\r\n");
      break;
    }

    if (needs_render) {
      SSD1306_Fill(SSD1306_COLOR_BLACK);
      SSD1306_UpdateScreen();
      for (uint8_t line = 0; line < CREDITS_VISIBLE_LINES; ++line) {
        uint8_t idx = scroll_offset + line;
        if (idx >= CREDITS_LINE_COUNT)
          break;
        SSD1306_GotoXY(0, 2 + line * CREDITS_FONT_HEIGHT);
        SSD1306_Puts((char*)credits_lines[idx], &Font_6x10, SSD1306_COLOR_WHITE);
      }
      SSD1306_UpdateScreen();
      needs_render = 0;
    }
    LL_mDelay(20);
  }
}

// --- UART Interrupt Service Routines ---

/**
 * @brief USART2 Interrupt Handler.
 * @note This is a global interrupt handler. It's placed in menu.c for simplicity
 *       in this example, but in a larger project, it might be better located
 *       in a dedicated interrupt handling file (e.g., main.c or py32f0xx_it.c).
 */
void USART2_IRQHandler(void) {
  UART_Listener_IRQCallback();
}

/**
 * @brief Callback function for the USART2 interrupt.
 *        Handles character reception and error flags.
 */
static void UART_Listener_IRQCallback(void) {
  // Check for Overrun, Framing, or Noise errors and clear them
  if (LL_USART_IsActiveFlag_ORE(USART2) || LL_USART_IsActiveFlag_FE(USART2) || LL_USART_IsActiveFlag_NE(USART2)) {
    LL_USART_ClearFlag_ORE(USART2);
    LL_USART_ClearFlag_FE(USART2);
    LL_USART_ClearFlag_NE(USART2);
    uart_rx_pos = 0;  // Discard partial data on error
    return;
  }

  // Check if the RXNE (Read Not Empty) flag is set and the interrupt is enabled
  if (LL_USART_IsActiveFlag_RXNE(USART2) && LL_USART_IsEnabledIT_RXNE(USART2)) {
    // Read the received byte. This action also clears the RXNE flag.
    uint8_t ch = LL_USART_ReceiveData8(USART2);

    // Check for newline character, indicating a complete command
    if (ch == '.') {
      // If there's data in the buffer, null-terminate it and set the flag
      if (uart_rx_pos > 0) {
        uart_rx_buf[uart_rx_pos] = '\0';
        uart_rx_flag = 1;  // Signal to the main loop that a message is ready
      }
    } else {
      // Add the character to the buffer if there's space
      if (uart_rx_pos < (UART_BUF_SIZE - 1)) {
        uart_rx_buf[uart_rx_pos++] = ch;
      }
      // If the buffer is full, we ignore further characters until the next newline.
    }
  }
}