/*
 * 006spi_arduino_txOnly.c
 *
 *  Created on: Jan 27, 2026
 *      Author: emreb
 */

/*
 * PA2 --> USART2_TX
 * PA3 --> USART2_RX
 * ALT function mode : 7
 * Note:datasheet(stm32f407vg.pdf) alternate function mapden bakılarak bu pinlere ve alternate function moda karar verildi
 */

#include "stm32f407xx.h"
#include "stdint.h"
#include "string.h"

USART_Handle_t USART2Handle;

//we have 3 different messages that we transmit to arduino
//you can by all means add more messages
char *msg[3] = {"hihihihihihi123", "Hello How are you ?", "Today is Monday !"};

//reply from arduino will be stored here
char rx_buf[1024];

//This flag indicates reception completion
uint8_t rxCmplt = RESET;


void GPIO_ButtonInit(){
	GPIO_Handle_t GPIO_Button;
	GPIO_Button.pGPIOx=GPIOA;
	GPIO_Button.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_IN;
	GPIO_Button.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_0;
	GPIO_Button.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;
	GPIO_Button.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_NONE;

	GPIO_Init(&GPIO_Button);
}

void USART2_GPIOInits(void){
	GPIO_Handle_t USART2Pins;
	USART2Pins.pGPIOx=GPIOA;
	USART2Pins.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	USART2Pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_2;
	USART2Pins.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;
	USART2Pins.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;
	USART2Pins.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	USART2Pins.GPIO_PinConfig.GPIO_PinAltFunMode=GPIO_ALFC_AF7;

	GPIO_Init(&USART2Pins);//TX PA2

	USART2Pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_3;

	GPIO_Init(&USART2Pins);//RX PA3
}


void USART2_Inits(void){
	USART2Handle.pUSARTx=USART2;
	USART2Handle.USART_Config.USART_Baud=USART_STD_BAUD_115200;
	USART2Handle.USART_Config.USART_Mode=USART_MODE_TXRX;
	USART2Handle.USART_Config.USART_ParityControl=USART_PARITY_DISABLE;
	USART2Handle.USART_Config.USART_HWFlowControl=USART_HW_FLOW_CTRL_NONE;
	USART2Handle.USART_Config.USART_NoOfStopBits=USART_STOPBITS_1;
	USART2Handle.USART_Config.USART_WordLength=USART_WORDLEN_8BITS;


	USART_Init(&USART2Handle);
}

void delay(void){
	for(uint32_t i=0;i<500000;i++);
}

int main(void)
{
	uint32_t cnt =0;


	USART2_GPIOInits();
	USART2_Inits();
	GPIO_ButtonInit();//internal button
	USART_IRQInterruptConfig(IRQ_NO_USART2, ENABLE);
	USART_PeripheralControl(USART2, ENABLE);


	while(1){
		while(!GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NUMBER_0));

		delay();

		//Next message index ; make sure that cnt value doesn t cross 2
		cnt = cnt % 3;

		//First lets enable the reception in interrupt mode
		//this code enables thereceive interrupt
		while ( USART_ReceiveDataIT(&USART2Handle,rx_buf, strlen(msg[cnt])) != USART_READY );

		USART_SendData(&USART2Handle,(uint8_t *)msg[cnt], strlen(msg[cnt]));

		printf("Transmitted : %s\n",msg[cnt]);

		//Now lets wait until all the bytes are received from the arduino
		//when all the bytes are received rCmplt will be SET in application callback
		while(rxCmplt != ENABLE);

		rx_buf[strlen(msg[cnt])+1]= '\0';

		printf("Received : %s\n",rx_buf);

		rxCmplt=DISABLE;

		cnt++;
	}

	return 0;
}

void USART2_IRQHandler(void){
	USART_IRQHandling(&USART2Handle);
}

void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t AppEv){
	if(AppEv == USART_EV_RX_CMPLT){
		rxCmplt =ENABLE;
	}
	else if(AppEv == USART_EV_RX_CMPLT){
		;
	}
}
