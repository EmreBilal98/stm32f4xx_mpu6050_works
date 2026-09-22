/*
 * msp.c
 *
 *  Created on: Jul 28, 2026
 *      Author: emreb
 */


#include "main.h"

GPIO_InitTypeDef i2cGpio;
GPIO_InitTypeDef tim4Ch1Gpio;


void HAL_MspInit(void)
{
	//Here will do low level processor specific inits.
		//1. Set up the priority grouping of the arm cortex mx processor
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

		//2. Enable the required system exceptions of the arm cortex mx processor
		SCB->SHCSR |= (0x7 << 16);//usg fault,memory fault,bus fault enabled

		//3. configure the priority for the system exceptions
		HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
		HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
		HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);

}


void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	//pb8->i2c1_scl
	//pb9->i2c1_sda

	//clock settings
	__HAL_RCC_I2C1_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	//i2c1 pin configurations
	i2cGpio.Alternate = GPIO_AF4_I2C1;
	i2cGpio.Mode = GPIO_MODE_AF_OD;
	i2cGpio.Pin= GPIO_PIN_8 | GPIO_PIN_9;
	i2cGpio.Pull = GPIO_PULLUP;
	i2cGpio.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &i2cGpio);

}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{

	//tim4 ch1 -> pd12

	//clock settings
	__HAL_RCC_TIM4_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	//tim4 pin configurations
	tim4Ch1Gpio.Alternate = GPIO_AF2_TIM4;
	tim4Ch1Gpio.Mode = GPIO_MODE_AF_PP;
	tim4Ch1Gpio.Pin= GPIO_PIN_12 | GPIO_PIN_13 ;
	tim4Ch1Gpio.Pull = GPIO_NOPULL;
	tim4Ch1Gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD, &tim4Ch1Gpio);


	//interrupt config
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
	HAL_NVIC_SetPriority(TIM4_IRQn, 15, 0);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart){

	GPIO_InitTypeDef gpio_uart;

	//USART2 low level initialization
	//1. enable the clock for the USART2 peripheral
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//2. Do the pin muxing configurations
	gpio_uart.Pin=GPIO_PIN_2;
	gpio_uart.Mode=GPIO_MODE_AF_PP;
	gpio_uart.Alternate=GPIO_AF7_USART2;
	gpio_uart.Speed=GPIO_SPEED_FREQ_LOW;
	gpio_uart.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &gpio_uart);//TX

	gpio_uart.Pin=GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &gpio_uart);//RX


}
