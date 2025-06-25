#include <stdio.h>
#include <string.h>

#include "py32f0xx_ll_bus.h"
#include "py32f0xx_ll_gpio.h"
#include "py32f0xx_ll_usart.h"
#include "py32f0xx_bsp_printf.h"

#include "ascii_fonts.h"
#include "buttons.h"
#include "main.h"
#include "menu.h"
#include "ssd1306.h"

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
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4; // AF4 for USART2 on PA2/PA3

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

static void Action_Conectar(void) {
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

  while (1) {
    if (is_btn_back_pressed()) {
      printf("Button BACK pressed, stopping listener.\r\n");
      UART_Listener_DeInit(); // Clean up before exiting
      BSP_USART_Config(115200);
      printf("UART2 Received: %s\r\n", uart_rx_buf);
      break;
    }

    // Check if a complete message has been received via UART
    if (uart_rx_flag) {
      // A new message is available in uart_rx_buf.
      // We print it to the main debug port (USART1) for demonstration.
      printf("UART2 Received: %s\r\n", uart_rx_buf);

      // Clear the message area and display the received message on screen
      SSD1306_DrawFilledRectangle(0, 22, SSD1306_WIDTH, 10, SSD1306_COLOR_BLACK);
      SSD1306_GotoXY(3, 22);
      SSD1306_Puts("Message received", &Font_6x10, SSD1306_COLOR_WHITE);
      SSD1306_UpdateScreen();

      // Reset the flag and buffer position for the next message
      uart_rx_pos = 0;
      uart_rx_flag = 0;
    }

    LL_mDelay(20); // Small delay to yield CPU time
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

// --- UART Interrupt Service Routines ---

/**
 * @brief USART2 Interrupt Handler.
 * @note This is a global interrupt handler. It's placed in menu.c for simplicity
 *       in this example, but in a larger project, it might be better located
 *       in a dedicated interrupt handling file (e.g., main.c or py32f0xx_it.c).
 */
void USART2_IRQHandler(void)
{
  UART_Listener_IRQCallback();
}

/**
 * @brief Callback function for the USART2 interrupt.
 *        Handles character reception and error flags.
 */
static void UART_Listener_IRQCallback(void)
{
  // Check for Overrun, Framing, or Noise errors and clear them
  if (LL_USART_IsActiveFlag_ORE(USART2) || LL_USART_IsActiveFlag_FE(USART2) || LL_USART_IsActiveFlag_NE(USART2)) {
    LL_USART_ClearFlag_ORE(USART2);
    LL_USART_ClearFlag_FE(USART2);
    LL_USART_ClearFlag_NE(USART2);
    return;
  }

  // Check if the RXNE (Read Not Empty) flag is set and the interrupt is enabled
  if (LL_USART_IsActiveFlag_RXNE(USART2) && LL_USART_IsEnabledIT_RXNE(USART2))
  {
    // Read the received byte. This action also clears the RXNE flag.
    uint8_t ch = LL_USART_ReceiveData8(USART2);

    // Check for end-of-line characters, indicating a complete command
    if (ch == '\r' || ch == '\n')
    {
      // If there's data in the buffer, null-terminate it and set the flag
      if (uart_rx_pos > 0)
      {
        uart_rx_buf[uart_rx_pos] = '\0';
        uart_rx_flag = 1; // Signal to the main loop that a message is ready
      }
    }
    else
    {
      // Add the character to the buffer if there's space
      if (uart_rx_pos < (UART_BUF_SIZE - 1))
      {
        uart_rx_buf[uart_rx_pos++] = ch;
      }
      // If the buffer is full, we ignore further characters until the next newline.
    }
  }
}