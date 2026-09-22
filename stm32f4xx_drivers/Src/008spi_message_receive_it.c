/*
 * 008spi_message_receive_it.c
 *
 *  Created on: Jan 30, 2026
 *      Author: emreb
 */

/*
 * PB14 --> SPI2_MISO
 * PB15 --> SPI2_MOSI
 * PB13 --> SPI2_SCLK
 * PB12 --> SPI2_NSS
 * ALT function mode : 5
 * Note:datasheet(stm32f407vg.pdf) alternate function mapden bakılarak bu pinlere ve alternate function moda karar verildi
 */

#include "stm32f407xx.h"
#include "stdio.h"
#include "string.h"


SPI_Handle_t SPI2Handle;

#define MAX_LEN 500

char RcvBuff[MAX_LEN];

volatile uint8_t rcvStop;

volatile char ReadByte;

//this flag will be set when the arduino interrupt GPIO
volatile uint8_t dataAvailable=0;

void delay(void){
	for(uint32_t i=0;i<500000/2;i++);
}

void SPI_GPIOInits(uint8_t pinNumber){
	GPIO_Handle_t SPIPins;
	SPIPins.pGPIOx=GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinNumber=pinNumber;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode=GPIO_ALFC_AF5;

	GPIO_Init(&SPIPins);
}


void SPI_Inits(void){

	SPI2Handle.pSPIx=SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig=SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_DeviceMode=SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_SclkSpeed=SPI_SPEED_PRESCALER_128;
	SPI2Handle.SPIConfig.SPI_DFF=SPI_DFF_8B;
	SPI2Handle.SPIConfig.SPI_CPOL=SPI_CLOCK_IDLE_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA=SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM=SPI_SSM_DI;


	SPI_Init(&SPI2Handle);
}

void Slave_GPIO_InterrruptPınInit(void){
	GPIO_Handle_t sipIntPin;
	memset(&sipIntPin,0,sizeof(sipIntPin));

	sipIntPin.pGPIOx=GPIOD;
	sipIntPin.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_IT_FT;
	sipIntPin.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_6;
	sipIntPin.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_LS;
	sipIntPin.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_NONE;
	sipIntPin.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;

	GPIO_Init(&sipIntPin);

	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, 15);
	GPIO_IRQConfig(IRQ_NO_EXTI9_5, ENABLE);


}

int main(void){

	uint8_t dummy=0xff;

	Slave_GPIO_InterrruptPınInit();
	SPI_GPIOInits(GPIO_PIN_NUMBER_12);//nss
	SPI_GPIOInits(GPIO_PIN_NUMBER_13);//sclk
	SPI_GPIOInits(GPIO_PIN_NUMBER_14);//miso
	SPI_GPIOInits(GPIO_PIN_NUMBER_15);//mosi

	SPI_Inits();
	SPI_SSOEConfig(SPI2, ENABLE);//this makes nss pin active

	SPI_IRQConfig(IRQ_NO_SPI2, ENABLE);
	while(1){
		rcvStop=0;

		while(!dataAvailable);//wait till data available interrupt  from transmitter device(slave)

		GPIO_IRQConfig(IRQ_NO_EXTI9_5, DISABLE);


		SPI_PeripheralControl(SPI2, ENABLE);
		while(!rcvStop){
			/*fetch the data from the SPI peripheral byte by byte in interrupt mode*/
			while(SPI_SendDataIT(&SPI2Handle,&dummy,1)==SPI_BUSY_IN_TX);
			while(SPI_ReceiveDataIT(&SPI2Handle,&ReadByte,1)==SPI_BUSY_IN_RX);
		}

		while(SPI_GetFlagStatus(SPI2,SPI_BSY_FLAG));

		SPI_PeripheralControl(SPI2, DISABLE);//disable spi peripheral

		printf("RCVd data=%s\n",RcvBuff);

		dataAvailable = 0;

		GPIO_IRQConfig(IRQ_NO_EXTI9_5, ENABLE);
	}



	return 0;
}

void SPI2_IRQHandler(void){
	SPI_IRQHandling(&SPI2Handle);
}

void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t Event){
	static uint32_t i=0;

	if(Event == SPI_EVENT_RX_CMPLT)
	{
		RcvBuff[i++]=ReadByte;
		if(ReadByte == '\0'  || (i==MAX_LEN)){
			rcvStop=1;
			RcvBuff[i-1]='\0';
			i=0;
		}
	}
}

void EXTI9_5_IRQHandler(void){
	GPIO_IRQHandling(GPIO_PIN_NUMBER_6);
	dataAvailable =1;
}
