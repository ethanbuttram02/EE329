/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 * PROJECT         : Parallel LCD
 * Author(s)       : Seth Saxena & Ethan Buttram
 * Date			: 2023/04/19
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

void SystemClock_Config(void);
int Countdown(int count);
int Key_Map(int key_val);

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
	int STATE = STATE_WAIT;
	// write "EE 329 A3 TIMER "
	char str[] = "EE 329 A3 TIMER ";
	char str2[] = "*=SET #=GO 00:00";
	LCD_write_string(str);
	LCD_command(0xC0); // move cursor to next line
	LCD_write_string(str2);
	int count = 0;																  // seconds needed to count
	char timec[5] = "00:00";													  // stored time
	int time1[5] = {0, 0, 10, 0, 0};											  // array of zeros, 10 in the middle cuz 10+48 is 58, ascii code for ':'
	int key_map[17] = {1, 2, 3, 10, 4, 5, 6, 11, 7, 8, 9, 12, -6, 0, -13, 14, 0}; // real values of keypress
	int key_idx = 0;															  // tracking index into key_map
	while (1)
	{
		switch (STATE)
		{
		case STATE_WAIT: // waits for '*' or '#' press
			while (!(Keypad_IsAnyKeyPressed()))
			{
				;
			}
			key_idx = Keypad_WhichKeyIsPressed();
			if (key_idx == 12)
			{
				STATE = STATE_ENTER; // go to state enter if '*' press
			}
			else if (key_idx == 14)
			{ // go to state count if '#' press
				STATE = STATE_COUNT;
			}
			break;
		// State that takes care of keypad entry for countdown time
		case STATE_ENTER:
			LCD_command(0xCB);		 // moves cursor to tens position of the minutes
			LCD_write_string(timec); // writes stored time in the location
			LCD_command(0xCB);		 // resets cursor
			for (int num_press = 0; num_press < 5; num_press++)
			{
				if (num_press != 2)
				{ // keeps ':'
					while (!(Keypad_IsAnyKeyPressed()))
					{				   // wait for key to be pressed
						delay_us(100); // delay for consistency
					}
					key_idx = Keypad_WhichKeyIsPressed();
					// prevent entries greater than 59:59
					if (key_map[key_idx] >= 0 && key_map[key_idx] < 10)
					{ // if 0-9 entered
						time1[num_press] = key_map[key_idx];
						// if entry for tens place of minute or second is > 5
						if ((num_press == 0 || num_press == 3) && (time1[num_press] > 5))
						{
							// "this whole chuck just prints 59... thats all it does" - seth before filming
							time1[num_press] = 5;
							time1[num_press + 1] = 9;
							count = ((time1[0]) * 60 * 10) + ((time1[1]) * 60) +
									((time1[3]) * 10) + (time1[4]);
							LCD_command(0xCB + num_press);
							LCD_write_char(time1[num_press] + 48);
							LCD_command(0xCC + num_press);
							LCD_write_char(time1[num_press + 1] + 48);
							num_press++;
						}
						else
						{
							count = ((time1[0]) * 60 * 10) + ((time1[1]) * 60) +
									((time1[3]) * 10) + (time1[4]);
							LCD_command(0xCB + num_press);
							LCD_write_char(time1[num_press] + 48);
						}
					}
					else if (key_idx == 13)
					{
						time1[num_press] = key_map[key_idx];
						count = ((time1[0]) * 60 * 10) + ((time1[1]) * 60) +
								((time1[3]) * 10) + (time1[4]);
						LCD_command(0xCB + num_press);
						LCD_write_char(time1[num_press] + 48);
					}
					else if (key_idx == 12)
					{
						STATE = STATE_ENTER;
						break;
					}
				}
				else
				{
					LCD_command(0xCE); // moves colon over to prevent a display error
				}
				delay_us(500000);
			}
			STATE = STATE_WAIT;
			break;
		case STATE_COUNT:
			int i = Countdown(count);
			if (i == 0)
			{
				STATE = STATE_END;
			}
			else
			{
				STATE = STATE_ENTER;
			}
			break;
		case STATE_END:
			GPIOA->BSRR = GPIO_PIN_3; // LED on for finish
			while (!(Keypad_IsAnyKeyPressed()))
			{
				delay_us(100);
			}
			key_idx = Keypad_WhichKeyIsPressed();
			delay_us(500000);
			if (key_idx == 14)
			{
				GPIOA->BRR = GPIO_PIN_3; //	led off for pound press
				STATE = STATE_WAIT;
			}
			else
				STATE = STATE_WAIT;
			break;
		default:
			STATE = STATE_WAIT;
		}
	}
}

/*
 * Countdowm function:
 * 	->Parameter: int array that stores acii values of each char in MM:SS time
 * 	-the function converts the time from MM:SS to seconds and then counts down
 * 	 to zero in real time, while updating the the LCD to dispay the MM:SS time
 * 	 every time it changes (once every second).
 * 	-> this function has no return
 */
int Countdown(int count)
{
	int time[5] = {0, 0, 10, 0, 0}; // initialize to "00:00"
	int val;
	uint32_t sistick1; // track start time of each cycle in the loop
	uint32_t sistick2; // track end time of each cycle in the loop
	while (count > 0)
	{
		sistick1 = (SysTick->VAL) / 4000000; // start time
		count -= 1;
		time[0] = ((count / 60)) / 10; // stores 10s place of minutes
		time[1] = ((count / 60)) % 10; // store 1s place of minutes
		time[3] = ((count % 60)) / 10; // stores 10s place of seconds
		time[4] = ((count % 60)) % 10; // stores 1s place of seconds
		for (int i = 0; i < 5; i++)
		{
			if (i != 2)
			{
				LCD_command(0xCB + i); // moves cursor to correct spot
				LCD_write_char(time[i] + 48);
			}
		}
		if (Keypad_WhichKeyIsPressed() == 12)
		{
			break;
		}
		sistick2 = (SysTick->VAL) / 4000000; // end time
		// delay to maintain one loop cycle per second
		delay_us(970000 - (sistick2 - sistick1));
	}
	if (count == 0)
	{
		GPIOA->BSRR = (GPIO_PIN_3); // count over so turn on LED
	}
	return val = count;
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
