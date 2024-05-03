/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 *
 * author: Eugenio Pasos
 *
 *
 * Creates square wave at 5kHz
 * Times function output
 *
 *
 ******************************************************************************
 */

#include "main.h"
int main(void) {

	HAL_Init();								//initializing peripherals
	SystemClock_Config();
	LED_Config();
	LCD_Config();
	LCD_init();
	setup_MCO_CLK();
	setup_TIM2(DUTYCYCLE, 0xFFFFFFFF);

	while (1) {								//Forever Loop
		;
	}
}

void setup_TIM2(int iDutyCycle, int iPeriod) {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;           // enable clock for TIM2
	TIM2->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE);  // enable event gen, rcv CCR1
	TIM2->ARR = iPeriod;                            // ARR = T = counts @4MHz
	TIM2->CCR1 = iDutyCycle;                        // ticks for duty cycle
	TIM2->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);       // clr IRQ flag in status reg
	NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F));     // set NVIC interrupt: 0x1F
	__enable_irq();                                 // global IRQ enable
	TIM2->CR1 |= TIM_CR1_CEN;                       // start TIM2 CR1
}

void TIM2_IRQHandler(void) {
	GPIOC->BSRR = 2;					//Output to time interrupt
	if (TIM2->SR & TIM_SR_CC1IF) {      // triggered by CCR1 event ...
		TIM2->SR &= ~(TIM_SR_CC1IF);    // manage the flag
		TIM2->CCR1 += DUTYCYCLE;
		if (GPIOC->ODR &= 1) {			//Toggle Output to create square wave
			GPIOC->BRR |= 1;
		} else {
			GPIOC->ODR |= 1;
		}
	}
	if (TIM2->SR & TIM_SR_UIF) {        // triggered by ARR event ...
		TIM2->SR &= ~(TIM_SR_UIF);       // manage the flag

	}
	GPIOC->BRR |= 2;
}

void setup_MCO_CLK(void) {
	// Enable MCO, select MSI (4 MHz source)
	RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_MCOSEL)) | (RCC_CFGR_MCOSEL_0));
	// Configure MCO output on PA8
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
	GPIOA->MODER &= ~(GPIO_MODER_MODE8);    	// clear MODER bits
	GPIOA->MODER |= (GPIO_MODER_MODE8_1);	// set alternate function mode
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT8);     	// Push-pull output
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD8);    	// no resistor
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED8);   // high speed
	GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL8);    	// select MCO function
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
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
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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

