/*
 * 006spi_arduino_txOnly.c
 *
 *  Created on: Jan 27, 2026
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
#include "stdint.h"
#include "string.h"

void SPI_GPIOInits(uint8_t pinNumber){
	GPIO_Handle_t SPIPins;
	SPIPins.pGPIOx=GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinNumber=pinNumber;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_NONE;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode=GPIO_ALFC_AF5;

	GPIO_Init(&SPIPins);
}

void GPIO_ButtonInit(uint8_t pinNumber){
	GPIO_Handle_t GPIO_Button;
	GPIO_Button.pGPIOx=GPIOA;
	GPIO_Button.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_IN;
	GPIO_Button.GPIO_PinConfig.GPIO_PinNumber=pinNumber;
	GPIO_Button.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;
	GPIO_Button.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_NONE;

	GPIO_Init(&GPIO_Button);
}


void SPI_Inits(void){
	SPI_Handle_t SPI2Handle;
	SPI2Handle.pSPIx=SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig=SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_DeviceMode=SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_SclkSpeed=SPI_SPEED_PRESCALER_64;
	SPI2Handle.SPIConfig.SPI_DFF=SPI_DFF_8B;
	SPI2Handle.SPIConfig.SPI_CPOL=SPI_CLOCK_IDLE_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA=SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM=SPI_SSM_DI;


	SPI_Init(&SPI2Handle);
}

void delay(void){
	for(uint32_t i=0;i<500000;i++);
}

int main(void)
{
	char user_data[] = "Hello world";

	SPI_GPIOInits(GPIO_PIN_NUMBER_12);//nss
	SPI_GPIOInits(GPIO_PIN_NUMBER_13);//sclk
	//SPI_GPIOInits(GPIO_PIN_NUMBER_14);//miso
	SPI_GPIOInits(GPIO_PIN_NUMBER_15);//mosi
	GPIO_ButtonInit(GPIO_PIN_NUMBER_0);//internal button

	SPI_Inits();
	SPI_SSOEConfig(SPI2, ENABLE);//this makes nss pin active

	while(1){

	while(!GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NUMBER_0));

	delay();

	SPI_PeripheralControl(SPI2, ENABLE);//enable spi peripheral

	uint8_t sizeofData=strlen(user_data);
	SPI_SendData(SPI2,&sizeofData, 1);

	SPI_SendData(SPI2,(uint8_t *) user_data, strlen(user_data));

	while(SPI_GetFlagStatus(SPI2,SPI_BSY_FLAG));

	SPI_PeripheralControl(SPI2, DISABLE);//disable spi peripheral
	}
}
