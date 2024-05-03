/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 *
 * author: Eugenio Pasos
 *
 * File for a game played by iniializing with a button press
 * Waits for a random amount of time and then turns on an LED
 * user responds with a button press and LCD displays recation
 * time in seconds.
 *
 ******************************************************************************
 */

#include "main.h"

uint32_t ms = 0;								//Initialize Global variable
char timeout_flag = 0;

int main(void) {
	HAL_Init();									//initializing peripherals
	SystemClock_Config();
	LCD_Config();
	LCD_init();
	Button_Config();
	BLED_Config();
	setup_MCO_CLK();

	while (1) {

		LCD_write_string("EE 329 A4 REACT"); 		//Format display
		LCD_command(0xC0);
		LCD_write_string("Push SW to trig ");

		while (!(GPIOC->IDR & GPIO_PIN_13)) {		//Waits for initial press
			;
		}
		LCD_command(0xC0);							//Update Display
		LCD_write_string("Are you ready?! ");
		while (GPIOC->IDR & GPIO_PIN_13) {			//De-bounce button
			;
		}

		delay_us(6000000 + (rand() % (6000001)));   //Delays random time
		setup_TIM2(DUTYCYCLE, 40000000);			//Start Timer
		GPIOB->ODR |= GPIO_PIN_7;					//Sets LED high

		while (!(GPIOC->IDR & GPIO_PIN_13)) {		//Wait for reaction press
			if (timeout_flag == 1)					//Checks for timeout
				break;
		}
		if (timeout_flag == 1) {					//Displays timeout text
			GPIOB->BRR = GPIO_PIN_7;
			LCD_command(0xC0);
			LCD_write_string("TIME'S UP       ");
			delay_us(2000000);

			LCD_command(0xC0);
			LCD_write_string("Press SW to RST ");
		}

		else {
			__disable_irq();							//Disable interrupts
			GPIOB->BRR = GPIO_PIN_7;
			LCD_command(0xC0);							//Write Reaction Time
			LCD_write_string("Time = ");
			LCD_write_char(48 + (ms / 1000));
			LCD_write_char('.');
			LCD_write_char(48 + (ms % 1000) / 100);
			LCD_write_char(48 + ((ms % 1000) % 100) / 10);
			LCD_write_char(48 + ((ms % 1000) % 100) % 10);
			LCD_write_string(" s  ");
			delay_us(4000000);
			LCD_command(0xC0);
			LCD_write_string("Press SW to RST ");
		}
		while (!(GPIOC->IDR & GPIO_PIN_13)) {		//Wait for RST press
			;
		}

		ms = 0;										//Resets variables
		timeout_flag = 0;
		while ((GPIOC->IDR & GPIO_PIN_13)) {		//De-bounce
			;
		}
	}
}

void Button_Config(void) {
	//Sets up on board Button Connected to PC13
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN);
	GPIOC->MODER &= ~(GPIO_MODER_MODE13);
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13_Msk);
	GPIOC->PUPDR |= (GPIO_PUPDR_PUPD13_1);
}

void BLED_Config(void) {
	//Sets up On Board Blue LED Connected to PB7
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);
	GPIOB->MODER &= ~(GPIO_MODER_MODE7);
	GPIOB->MODER |= (GPIO_MODER_MODE7_0);
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT7);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD7);
	GPIOB->OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED7_Pos);
	GPIOB->BRR = (GPIO_PIN_7);
}

void setup_TIM2(int iDutyCycle, int iPeriod) {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;          // enable clock for TIM2
	TIM2->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE); // enable event gen, rcv CCR1
	TIM2->ARR = iPeriod;                           // ARR = T = counts @4MHz
	TIM2->CCR1 = iDutyCycle;                       // ticks for duty cycle
	TIM2->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);      // clr IRQ flag in status reg
	NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F));    // set NVIC interrupt: 0x1F
	__enable_irq();                                // global IRQ enable
	TIM2->CR1 |= TIM_CR1_CEN;                      // start TIM2 CR1
	TIM2->CNT = 0;
}

void TIM2_IRQHandler(void) {

	if (TIM2->SR & TIM_SR_CC1IF) {      // triggered by CCR1 event ...
		TIM2->SR &= ~(TIM_SR_CC1IF);    // manage the flag
		TIM2->CCR1 += DUTYCYCLE;		//Sets next CRR value
		ms += 1;						//Counts each millisecond
	}
	if (TIM2->SR & TIM_SR_UIF) {        // triggered by ARR event ...
		TIM2->SR &= ~(TIM_SR_UIF);      // manage the flag
		timeout_flag = 1;
	}
}

void setup_MCO_CLK(void) {
	// Enable MCO, select MSI (4 MHz source)
	RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_MCOSEL)) | (RCC_CFGR_MCOSEL_0));
	// Configure MCO output on PA8
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
	GPIOA->MODER &= ~(GPIO_MODER_MODE8);    	// clear MODER bits
	GPIOA->MODER |= (GPIO_MODER_MODE8_1);		//set alternate function mode
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

