/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 * PROJECT         : Parallel LCD
 * Author(s)       : Seth Saxena & Ethan
 * 				  stsaxena@calpoly.edu  ssrini22@calpoly.edu
 * Date			: 2023/04/15
 * Compiler		: STM32CubeIDE v.1.15.0
 * Target			: Nucleo-L496ZG
 * Clock			: 4 MGHz MSI to AHB2
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "LCD.h"
#include "keypad.h"
#include "delay.h"
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define CLEAR_SCREEN 0x80
#define RETURN_HOME 0x40
#define WRITE_2ND_LINE 0XC0

void SystemClock_Config(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  // configure LED
  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN); // enables GPIO clock
  // clears bits in pin PA0
  GPIOA->MODER &= ~(GPIO_MODER_MODE3);
  // sets PA0 to general-purpose output mode
  GPIOA->MODER |= (GPIO_MODER_MODE3_0);
  // sets pin to push-pull
  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT3);
  // disables PU and PD resistors connected to PA0
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD3);
  // sets pin speed to high for pin 0
  GPIOA->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED3_Pos));
  // preset PA0 to 0
  GPIOA->BRR = (GPIO_PIN_3);

  // initialize keypad
  Keypad_Config();
  // Initialize LCD
  LCD_config();
  LCD_init();
  // write to lCD

  delay_us(200000); // delay for LCD initialization

  LCD_command(CLEAR_SCREEN); // clear screen

  // LCD start message
  LCD_write_string("EE 329 A3 Timer");

  LCD_command(WRITE_2ND_LINE);
  LCD_write_string("*=Set #=Go ");
  int time_arr[] = {1,2,5,5};

  int num_presses = 0;

  while(1) {
    if (Keypad_IsAnyKeyPressed()) {
      int button = Keypad_WhichKeyIsPressed();
      switch(button) {
        case 12:
        // clear and set the countdown
          Countdown(time_arr);
        break;
        case 14:
        // start the countdown
        break;
      }
    }
  }
}

void Countdown(int time[]) {
    if (time[0] > 5) time[0] = 5;
    if (time[2] > 5) time[2] = 5;
    int count = ((time[0])*60*10) + ((time[1])*60) + ((time[2])*10) + (time[3] + 1); // converts time to seconds
    uint32_t sistick1 = (SysTick->VAL)/4000000;
    while(count > 0){
      
        if (Keypad_WhichKeyIsPressed() == 12) {
          return;
        }
      
        count -= 1;
        time[0] = (int)((count/60))/10; //stores 10s place of minutes
        time[1] = (int)((count/60))%10; //store 1s place of minutes
        time[3] = (int)((count%60))/10; //stores 10s place of seconds
        time[4] = (int)((count%60))%10; //stores 1s place of seconds
        for (int i = 0; i < 6; i++){
            if(i==2){
                LCD_write_char(58);
            }
            else{
                LCD_command(0xCB+i);
                LCD_write_char(time[i]+48);
            }
        }
        uint32_t sistick2 = (SysTick->VAL)/4000000;
        delay_us(1000000-(sistick2-sistick1));
    }
    if(count==0){
        GPIOA->BSRR = (GPIO_PIN_3);
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
// static void MX_GPIO_Init(void)
//{
//   GPIO_InitTypeDef GPIO_InitStruct = {0};
///* USER CODE BEGIN MX_GPIO_Init_1 */
///* USER CODE END MX_GPIO_Init_1 */
//
//  /* GPIO Ports Clock Enable */
//  __HAL_RCC_GPIOC_CLK_ENABLE();
//  __HAL_RCC_GPIOH_CLK_ENABLE();
//  __HAL_RCC_GPIOB_CLK_ENABLE();
//  __HAL_RCC_GPIOG_CLK_ENABLE();
//  HAL_PWREx_EnableVddIO2();
//  __HAL_RCC_GPIOA_CLK_ENABLE();
//
//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin, GPIO_PIN_RESET);
//
//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);
//
//  /*Configure GPIO pin : B1_Pin */
//  GPIO_InitStruct.Pin = B1_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
//
//  /*Configure GPIO pins : LD3_Pin LD2_Pin */
//  GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//  /*Configure GPIO pin : USB_OverCurrent_Pin */
//  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);
//
//  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
//  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);
//
//  /*Configure GPIO pins : STLK_RX_Pin STLK_TX_Pin */
//  GPIO_InitStruct.Pin = STLK_RX_Pin|STLK_TX_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//  GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
//  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
//
//  /*Configure GPIO pins : USB_SOF_Pin USB_ID_Pin USB_DM_Pin USB_DP_Pin */
//  GPIO_InitStruct.Pin = USB_SOF_Pin|USB_ID_Pin|USB_DM_Pin|USB_DP_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
///* USER CODE BEGIN MX_GPIO_Init_2 */
///* USER CODE END MX_GPIO_Init_2 */
//}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
