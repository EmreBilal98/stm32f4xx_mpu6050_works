/*
 * 004led_buttonControl_ext_interrupt.c
 *
 *  Created on: Jan 23, 2026
 *      Author: emreb
 */

#include "stm32f407xx.h"

void delay(void){
	for(uint32_t i=0;i<500000;i++);
}

int main(void)
{

	GPIO_Handle_t GpioLed,GpioIButton;
	memset(&GpioLed,0,sizeof(GpioLed));
	memset(&GpioIButton,0,sizeof(GpioIButton));

	GpioLed.pGPIOx=GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_4;
	GpioLed.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_NONE;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;


	GpioIButton.pGPIOx=GPIOA;
	GpioIButton.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_5;
	GpioIButton.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_IT_FT;
	GpioIButton.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PD;
	GpioIButton.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;

	GPIO_PeripheralClockControl(GPIOA, ENABLE);
	GPIO_PeripheralClockControl(GPIOD, ENABLE);
	GPIO_Init(&GpioIButton);
	GPIO_Init(&GpioLed);

	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, 15);
	GPIO_IRQConfig(IRQ_NO_EXTI9_5, ENABLE);



	while(1){}


}

void EXTI9_5_IRQHandler(void){

	GPIO_ToggleOutputPin(GPIOD,GPIO_PIN_NUMBER_4);
	GPIO_IRQHandling(GPIO_PIN_NUMBER_5);
	delay();


}


