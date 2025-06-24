/***
 * Demo: I2C - SSD1306 OLED
 *
 * PY32          SSD1306
 *  PF1/PA9       SCL
 *  PF0/PA10      SDA
 */
#include "main.h"
#include <string.h>
#include "bitmaps.h"
#include "buttons.h"
#include "menu.h"
#include "py32f0xx_bsp_clock.h"
#include "py32f0xx_bsp_printf.h"
#include "ssd1306.h"

#define I2C_ADDRESS 0xA0 /* host/client address */
#define I2C_STATE_READY 0
#define I2C_STATE_BUSY_TX 1
#define I2C_STATE_BUSY_RX 2

__IO uint32_t i2cState = I2C_STATE_READY;

static void APP_I2C_Config(void);
static void APP_GPIO_Config(void);
static void Start_Interactive_App(void);

int main(void) {
  BSP_RCC_HSI_24MConfig();

  BSP_USART_Config(115200);
  printf("I2C Demo: \r\nClock: %ld\r\n", SystemCoreClock);

  APP_I2C_Config();
  APP_GPIO_Config();

  uint8_t res = SSD1306_Init();
  printf("OLED init: %s\n", res == 0 ? "ERROR" : "OK");

  SSD1306_UpdateScreen();
  SSD1306_DrawBitmap(epd_bitmap_bsides_logo, 0, 0, 128, 32);
  SSD1306_UpdateScreen();
  LL_mDelay(2000);

  while (1) {
    Menu_UpdateAndRender();
    LL_mDelay(10);
  }
}

static void Start_Interactive_App(void) {
  // Placeholder for the "Interactive App"
  SSD1306_Fill(SSD1306_COLOR_BLACK);
  SSD1306_GotoXY(10, 2);
  SSD1306_Puts("Interactive App", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_GotoXY(10, 12);
  SSD1306_Puts("is now running.", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_GotoXY(15, 22);
  SSD1306_Puts("Press BACK", &Font_6x10, SSD1306_COLOR_WHITE);
  SSD1306_UpdateScreen();

  // Wait for BACK button to return to menu
  while (1) {
    if (is_btn_back_pressed()) {
      return;  // Return to main loop, which will re-init the menu
    }
    LL_mDelay(10);
  }
}

static void APP_GPIO_Config(void) {
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  /*
   * Buttons:
   * DOWN:  PA0
   * UP:    PA1
   * BACK:  PA5
   * ENTER: PA6
   */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void APP_I2C_Config(void) {
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOF);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /**
   * SCL: PF1 & AF_12, PA9 & AF_6
   * SDA: PF0 & AF_12, PA10 & AF_6
   *
   * Change pins to PF1 / PF0 for parts have no PA9 / PA10
   */
  // PF1 SCL
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_12;
  LL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  // PF0 SDA
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_12;
  LL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C1);
  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C1);

  LL_I2C_InitTypeDef I2C_InitStruct;
  /*
   * Clock speed:
   * - standard = 100khz, if PLL is on, set system clock <= 16MHz, or i2c cannot work normally
   * - fast     = 400khz
   */
  I2C_InitStruct.ClockSpeed = LL_I2C_MAX_SPEED_FAST;
  I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_16_9;
  I2C_InitStruct.OwnAddress1 = I2C_ADDRESS;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_NACK;
  LL_I2C_Init(I2C1, &I2C_InitStruct);

  /* Enale clock stretch (reset default: on) */
  // LL_I2C_EnableClockStretching(I2C1);

  /* Enable general call (reset default: off) */
  // LL_I2C_EnableGeneralCall(I2C1);
}

void APP_I2C_Transmit(uint8_t devAddress, uint8_t memAddress, uint8_t* pData, uint16_t len) {
  while (i2cState == I2C_STATE_BUSY_TX)
    ;

  LL_I2C_DisableBitPOS(I2C1);

  i2cState = I2C_STATE_BUSY_TX;

  /* Start */
  LL_I2C_GenerateStartCondition(I2C1);
  while (LL_I2C_IsActiveFlag_SB(I2C1) != 1)
    ;
  /* Send slave address */
  LL_I2C_TransmitData8(I2C1, (devAddress & (uint8_t)(~0x01)));
  while (LL_I2C_IsActiveFlag_ADDR(I2C1) != 1)
    ;
  LL_I2C_ClearFlag_ADDR(I2C1);

  /* Send memory address */
  LL_I2C_TransmitData8(I2C1, memAddress);
  while (LL_I2C_IsActiveFlag_BTF(I2C1) != 1)
    ;

  /* Transfer data */
  while (len > 0) {
    while (LL_I2C_IsActiveFlag_TXE(I2C1) != 1)
      ;
    LL_I2C_TransmitData8(I2C1, *pData++);
    len--;

    if ((LL_I2C_IsActiveFlag_BTF(I2C1) == 1) && (len != 0U)) {
      LL_I2C_TransmitData8(I2C1, *pData++);
      len--;
    }

    while (LL_I2C_IsActiveFlag_BTF(I2C1) != 1)
      ;
  }

  /* Stop */
  LL_I2C_GenerateStopCondition(I2C1);
  i2cState = I2C_STATE_READY;
}

void APP_ErrorHandler(void) {
  while (1)
    ;
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
  while (1)
    ;
}
#endif /* USE_FULL_ASSERT */
