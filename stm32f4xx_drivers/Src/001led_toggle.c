/*
 * 001led_toggle.c
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

	GPIO_Handle_t GpioLed12,GpioLed13,GpioLed14,GpioLed15;

	GpioLed12.pGPIOx=GPIOD;
	GpioLed12.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_12;
	GpioLed12.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_OUT;
	GpioLed12.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	GpioLed12.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_NONE;
	GpioLed12.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;

	//ı want pd13,pd14 as open drain.if you select open drain there must be pull up resistor.the internal pull up is work but it resistor value is 40kohm
	//so that you can obreve the LED light very tiny.for this reason the best choice is external pullup resistor.I connect pd13 with od and internall pull up and
	//pd14 od and externall pull up with 1kohm resistor.
	GpioLed13=GpioLed12;
	GpioLed13.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_13;
	GpioLed13.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_OD;
	GpioLed13.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;


	GpioLed14=GpioLed12;
	GpioLed14.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_14;
	GpioLed14.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_OD;

	GpioLed15=GpioLed12;
	GpioLed15.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_15;

	GPIO_PeripheralClockControl(GPIOD, ENABLE);
	GPIO_Init(&GpioLed12);
	GPIO_Init(&GpioLed13);
	GPIO_Init(&GpioLed14);
	GPIO_Init(&GpioLed15);



	for(;;){
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NUMBER_12);
		delay();
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NUMBER_13);
		delay();
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NUMBER_14);
		delay();
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NUMBER_15);
		delay();
	}

}

