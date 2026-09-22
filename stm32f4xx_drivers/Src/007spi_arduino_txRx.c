/*
 * 007spi_arduino_txRx.c
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
#include "stdio.h"
#include "string.h"

extern void initialise_monitor_handles() ;
//command codes
#define COMMAND_LED_CTRL	0x50
#define COMMAND_SENSOR_READ	0x51
#define COMMAND_LED_READ	0x52
#define COMMAND_PRINT		0x53
#define COMMAND_ID_READ 	0x54

#define LED_ON				1
#define LED_OFF				0

//arduino analog pins
#define ANALOG_PIN0			0
#define ANALOG_PIN1			1
#define ANALOG_PIN2			2
#define ANALOG_PIN3			3
#define ANALOG_PIN4			4
#define ANALOG_PIN5			5

//arduino LED
#define LED_PIN				9


//ACK and NACK
#define SLAVE_RESPONSE_ACK	0xF5
#define SLAVE_RESPONSE_NACK	0x05


uint8_t SPI_VerifyResponse(uint8_t ackByte){
	if(ackByte == SLAVE_RESPONSE_ACK)
		return 1;
	else
		return 0;
}

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
	SPI2Handle.SPIConfig.SPI_SclkSpeed=SPI_SPEED_PRESCALER_128;
	SPI2Handle.SPIConfig.SPI_DFF=SPI_DFF_8B;
	SPI2Handle.SPIConfig.SPI_CPOL=SPI_CLOCK_IDLE_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA=SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM=SPI_SSM_DI;


	SPI_Init(&SPI2Handle);
}

void delay(void){
	for(uint32_t i=0;i<2000000;i++);
}

int main(void)
{
	initialise_monitor_handles();

	printf("application is running\n");
	uint8_t dummy_write =0xff;
	uint8_t dummy_read;
	uint8_t ackByte;

	SPI_GPIOInits(GPIO_PIN_NUMBER_12);//nss
	SPI_GPIOInits(GPIO_PIN_NUMBER_13);//sclk
	SPI_GPIOInits(GPIO_PIN_NUMBER_14);//miso
	SPI_GPIOInits(GPIO_PIN_NUMBER_15);//mosi
	GPIO_ButtonInit(GPIO_PIN_NUMBER_0);//internal button

	SPI_Inits();
	SPI_SSOEConfig(SPI2, ENABLE);//this makes nss pin active

	printf("spi inits done\n");

	while(1){

	while(!GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NUMBER_0));

	delay();

	SPI_PeripheralControl(SPI2, ENABLE);//enable spi peripheral

	uint8_t args[2];
	//1. CMD_LED_CTRL <pin no(1)>  <value(1)>
	uint8_t commandCode=COMMAND_LED_CTRL;
	SPI_SendData(SPI2, &commandCode, 1);
	printf("COMMAND_LED_CTRL:%d sended\n",COMMAND_LED_CTRL);

	//dummy read to clear RXNE
	SPI_ReceiveData(SPI2, &dummy_read, 1);
	printf("dummy Response of sended command ctrl:%d \n",dummy_read);

	delay();

	//send some dummy bits (1byte) to fetch the response from the slave.
	SPI_SendData(SPI2, &dummy_write, 1);
	printf("dummy write:%d for reaching the response of sended COMMAND_LED_CTRL\n",dummy_write);

	SPI_ReceiveData(SPI2, &ackByte, 1);
	printf("expected response of COMMAND_LED_CTRL is ackByte:%d\n",ackByte);

	//send some dummy bits (1byte) to fetch the response from the slave.
	//SPI_SendData(SPI2, &dummy_write, 1);

	//SPI_ReceiveData(SPI2, &ackByte, 1);


	if(SPI_VerifyResponse(ackByte)){
		printf("ackbyte:%d is match the expected\n",ackByte);
		args[0]=LED_PIN;
		args[1]=LED_ON;
		//send arguments
		SPI_SendData(SPI2, args, 2);


		//dummy read to clear RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		delay();

		//send some dummy bits (1byte) to fetch the response from the slave.
		SPI_SendData(SPI2, &dummy_write, 1);

		SPI_ReceiveData(SPI2, &ackByte, 1);
	}



	//end of COOMAND_LED_CTRL

	//2. CMD_SENSOR_READ <analog pin number(1)>

	while(!GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NUMBER_0));

	delay();

	commandCode=COMMAND_SENSOR_READ;

	SPI_SendData(SPI2, &commandCode, 1);

	//dummy read to clear RXNE
	SPI_ReceiveData(SPI2, &dummy_read, 1);

	delay();

	//send some dummy bits (1byte) to fetch the response from the slave.
	SPI_SendData(SPI2, &dummy_write, 1);

	SPI_ReceiveData(SPI2, &ackByte, 1);

	delay();

	//send some dummy bits (1byte) to fetch the response from the slave.
	SPI_SendData(SPI2, &dummy_write, 1);

	SPI_ReceiveData(SPI2, &ackByte, 1);

	if(SPI_VerifyResponse(ackByte)){
		args[0]=ANALOG_PIN0;

		//send arguments
		SPI_SendData(SPI2, args, 1);

		//dummy read to clear RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		delay();


	}




	while(SPI_GetFlagStatus(SPI2,SPI_BSY_FLAG));

	SPI_PeripheralControl(SPI2, DISABLE);//disable spi peripheral
	}
}
