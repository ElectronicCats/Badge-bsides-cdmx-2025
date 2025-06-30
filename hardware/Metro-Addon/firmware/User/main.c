/**
 ******************************************************************************
 * @file    main.c
 * @author  MCU Application Team
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
 * All rights reserved.</center></h2>
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 * J-Link Connections: https://private-user-images.githubusercontent.com/44976441/277049150-2c44cddc-163c-4924-840f-2b99760bbca3.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MDc5NTA5NTYsIm5iZiI6MTcwNzk1MDY1NiwicGF0aCI6Ii80NDk3NjQ0MS8yNzcwNDkxNTAtMmM0NGNkZGMtMTYzYy00OTI0LTg0MGYtMmI5OTc2MGJiY2EzLnBuZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNDAyMTQlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjQwMjE0VDIyNDQxNlomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPWYyODFkMTNjN2YyMDgyZTJhZmExYzc3MWI0M2YwYzBjZmRiZWYxYmJiMTY3NjE1ZDU5YmU3Njc3NThhMjAwOTUmWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0JmFjdG9yX2lkPTAma2V5X2lkPTAmcmVwb19pZD0wIn0.hkoYOHl2-wbGiy8-bgevjSikxN3sZxpCxZn3IUqJlPI
 * PUYA instructions: https://github.com/ElectronicCats/puya-projects
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "py32f0xx_bsp_printf.h"
#include <stdint.h>

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_LedConfig(void);
uint16_t delay1 = 2000, delay3 = 4000;
uint8_t delay = 200, delay2 = 250, delay4 = 50;


// Sequence 1
void lightSequence()
{
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay1);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay);

  //Bink
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay);
}

// Sequence 2
void windowsSequence()
{
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay1);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay);

  //Bink
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1);  
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay);

  //One window
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay1);

  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);

  //Off
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
}

// Sequence 3
void rouletteSequence()
{
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay2);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay2);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay2);

  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay2);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(delay2);
  HAL_GPIO_OffPin(GPIOF, GPIO_PIN_3);
  HAL_Delay(delay2);
}

// Sequence 4
void allSequence()
{
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);  
  HAL_Delay(delay3);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4); 
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1); 
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3); 
  HAL_Delay(delay);

  //Blink
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);  
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4); 
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1); 
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);  
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);  
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4); 
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1); 
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);  
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);  
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4); 
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1); 
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);  
  HAL_Delay(delay);
}

// Seqeuence 5
void slowrouletteSequence()
{
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3); 
  HAL_Delay(delay2);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3); 
  HAL_Delay(delay2);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay2);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay2);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay2);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay2);

  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3); 
  HAL_Delay(delay2);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3); 
  HAL_Delay(delay2);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay2);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay2);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay2);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay2);
}

// Sequence 6
void allblinkSequence()
{
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);  
  HAL_Delay(delay3);
  
  //light
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3); 
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3); 
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay);

  //windows
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay);

  //window
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay3);

  //off
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4); 
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1); 
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3); 
  HAL_Delay(delay);
}

// Sequence 7
void fastrouletteSequence()
{
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay4);

  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay4);

  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay4);

  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay4);

  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay4);

  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay4);

  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay4);

  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_3);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_1); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(delay4);
  HAL_GPIO_OnPin(GPIOA, GPIO_PIN_4); 
  HAL_Delay(delay4);
  HAL_GPIO_OffPin(GPIOA, GPIO_PIN_4);
  HAL_Delay(delay4);
}

void sequences(int sequence)
{
  switch (sequence)
  {
  case 1:
    lightSequence();
    break;
  case 2:
    windowsSequence();
    break;
  case 3:
    rouletteSequence();
    break;
  case 4:
    allSequence();
    break;
  case 5:
    slowrouletteSequence();
    break;
  case 6:
    allblinkSequence();
    break;
    case 7:
    fastrouletteSequence();
    break;
  default:
    ledsOn();
    HAL_Delay(1000);
    break;
  }
}

int main(void)
{
  HAL_Init();

  APP_LedConfig();

  //BSP_USART_Config();
  
  while (1)
  {
    for (uint8_t c = 1; c < 8; c++)
    {
      sequences(c);
    }
  }
}

static void APP_LedConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void APP_ErrorHandler(void)
{
  while (1)
    ;
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
