/*
 * 002led_buttonControl.c
 *
 *  Created on: Jan 22, 2026
 *      Author: emreb
 */

#include "stm32f407xx.h"

void delay(void){
	for(uint32_t i=0;i<500000;i++);
}

int main(void)
{

	GPIO_Handle_t GpioLed,GpioIButton;

	GpioLed.pGPIOx=GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_NONE;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;


	GpioIButton.pGPIOx=GPIOA;
	GpioIButton.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_0;
	GpioIButton.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_IN;
	GpioIButton.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_NONE;
	GpioIButton.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;

	GPIO_PeripheralClockControl(GPIOA, ENABLE);
	GPIO_PeripheralClockControl(GPIOD, ENABLE);
	GPIO_Init(&GpioIButton);
	GPIO_Init(&GpioLed);



	for(;;){
		if(GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NUMBER_0)==ENABLE){
			GPIO_ToggleOutputPin(GPIOD,GPIO_PIN_NUMBER_12);
			delay();
		}
	}


}

