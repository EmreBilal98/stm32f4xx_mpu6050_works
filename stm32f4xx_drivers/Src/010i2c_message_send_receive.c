/*
 * 009i2c_message_send.c
 *
 *  Created on: Feb 12, 2026
 *      Author: emreb
 */

#include "stm32f407xx.h"
#include <stdio.h>
#include <string.h>

#define MY_ADDR 0x61
#define SLAVE_ADDR 0x68
#define FETCH_LENGHT 0x51
#define FETCH_DATA 0x52

I2C_Handle_t  i2c1;

void delay(void){
	for(uint32_t i=0;i<500000;i++);
}

void i2cGpioInit(void){
	GPIO_Handle_t i2c1_pins;
	i2c1_pins.pGPIOx=GPIOB;
	i2c1_pins.GPIO_PinConfig.GPIO_PinAltFunMode=GPIO_ALFC_AF4;
	i2c1_pins.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	i2c1_pins.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_OD;
	i2c1_pins.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;
	i2c1_pins.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;

	//scl
	i2c1_pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_6;
	GPIO_Init(&i2c1_pins);

	//sda
	i2c1_pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_9;
	GPIO_Init(&i2c1_pins);

}

void i2cInit(void){

	i2c1.pI2Cx=I2C1;
	i2c1.I2CConfig.I2C_ACKControl=I2C_ACK_ENABLE;
	i2c1.I2CConfig.I2C_SCLSpeed=I2C_SCL_SPEED_SM;
	i2c1.I2CConfig.I2C_DeviceAddress=MY_ADDR;

	I2C_Init(&i2c1);

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

int main(void) {

	uint8_t sizeofData;
	uint8_t commandcode;
	uint8_t data[32];

	i2cGpioInit();
	i2cInit();
	I2C_PeripheralControl(I2C1,ENABLE);
	I2C_ManageAcking(i2c1.pI2Cx,ENABLE);//ack bit can setted when PE=1
	GPIO_ButtonInit(GPIO_PIN_NUMBER_0);//internal button;



	while(1){
		while(!GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NUMBER_0));

		delay();

		commandcode=0x51;
		I2C_MasterSendData(&i2c1,&commandcode, 1, SLAVE_ADDR,ENABLE);

		I2C_MasterReceiveData(&i2c1, &sizeofData, 1, SLAVE_ADDR,ENABLE);

		commandcode=0x52;
		I2C_MasterSendData(&i2c1,&commandcode, 1, SLAVE_ADDR,ENABLE);

		I2C_MasterReceiveData(&i2c1,data, sizeofData, SLAVE_ADDR,DISABLE);

		data[sizeofData+1]='\0';

		printf("rcv data:%s\n",data);
	}

	return 0;
}

