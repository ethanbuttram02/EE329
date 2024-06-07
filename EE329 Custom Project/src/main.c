/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include <main.h>
#include <ADC.h>
#include <LCD.h>
#include <button.h>
#include <stdio.h>
#include <stdlib.h>

void SystemClock_Config(void);

// global variable definitions
uint16_t samples[BUFFER_LEN];
uint8_t unitFlag = 0; // imperial by default
uint16_t time1 = 0;
uint16_t time2 =0;
uint16_t translationalDistance = 125 * WHEEL_SIZE;  // 1/8 * 1000 * 26
uint8_t numArray[4];
uint16_t samples[BUFFER_LEN];
uint32_t sum;
uint16_t avg = 0;
uint8_t STATE = 0;

int main(void)
{
  
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  SysTick_Init();
  button_init();
  ADC_init();
  LCD_init();

  // initialize screen

  LCD_command(CLEAR);
  LCD_set_cursor(1, 0);
  LCD_write_string("MPH: 0");
  LCD_command(0xC0);  // move cursor to next line
  LCD_write_string("GEAR: 0");

  while (1) {
    delay_us(250000); // quarter second delay for LCD response time and stability
    ADC1->CR |= ADC_CR_ADSTART; // start conversion 
    

    for (int i = 0; i < BUFFER_LEN; i++) {  // iterate over the sample array
      ADC1->CR |= ADC_CR_ADSTART; // start conversion again

      samples[i] = adcResult; // once ready, assign position in array to that value
      sum += adcResult; // add the result to the sum for averaging

      avg = sum / BUFFER_LEN;
    }
    sum = 0;

    LCD_set_cursor(0,5);
    
    while(1) {
    
    switch(STATE) {
      case OPEN:
        ADC1->CR |= ADC_CR_ADSTART; // start conversion
        if (time1 == 0) {
          time1 = (SysTick -> VAL) / 4000000;
        }
        else if (time2 == 0) {
          time2 = (SysTick -> VAL) / 4000000;
        }
        if (adcResult > 2000) {
          STATE = CLOSED;
          LCD_write_string("open");
          break;
        }

      break;

      case CLOSED:
        ADC1->CR |= ADC_CR_ADSTART; // start conversion
        if (time2 - time1 != 0) {
              // sets to 0 mph
        }
        if (adcResult > 2000) {
          STATE = OPEN;
        }
        else if (adcResult < 2000 || (time2 - time1 <= 1)) {
          time2 = (SysTick -> VAL) / 4000000;
          STATE = CLOSED;
        }
        else if (time2 - time1 >= 1) {
          STATE = STOP;
        }
      break;

      case STOP:
        ADC1->CR |= ADC_CR_ADSTART; // start conversion
        LCD_set_cursor(0, 5);     // sets cursor to overwrite speed
        LCD_write_string("    "); // clears speed
        LCD_set_cursor(0, 5);     // resets cursor
        LCD_write_string("0000");    // sets to 0 mph

        if (adcResult > 2000) {
          STATE = OPEN;
        }
      break;

      default: STATE = STOP; break;
    }
  }
  }
}


char* unitConvert(uint16_t value) {
	switch(unitFlag) {
		case 0: // mph
			return intToString(make_Counts(value / 63360));
		case 1: // kph
			return intToString(make_Counts(value / 39370));
		default: unitFlag = 0; break;
	}
}

uint8_t* make_Counts(uint16_t num) {
   numArray[0] = num / 1000;  //take thousands place
   numArray[1] = (num / 100) % 10;  //take hundreds place
   numArray[2] = (num / 10) % 10;  //take tens place
   numArray[3] = num % 10;  //take ones place
   return numArray;
}

char* intToString(uint8_t *digits) {
	// Allocate memory for the resulting char array (5 characters: 4 digits + 1 null terminator)
  char *charArray = (char*) malloc(5 * sizeof(char));
  if (charArray == NULL)
  		return NULL;  // Check for allocation failure

   	// Convert each digit to the corresponding character
   	for (uint8_t i = 0; i < 4; i++) {
  		charArray[i] = digits[i] + '0';  // Convert digit to character, adding '0' converts to ASCII
   	}
  	// Null-terminate the string
   	charArray[4] = '\0';

   return charArray;
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

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

#ifdef  USE_FULL_ASSERT
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
