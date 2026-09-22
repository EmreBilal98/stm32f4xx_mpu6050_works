/*
 * ds1307.c
 *
 *  Created on: Feb 26, 2026
 *      Author: emreb
 */
#include "ds1307.h"
#include "stdint.h"
#include "string.h"


static void ds1307_i2c_pin_config(void);
static void ds1307_i2c_peripheral_config(void);
static void ds1307_write(uint8_t value,uint8_t regAddr);
static uint8_t ds1307_read(uint8_t regAddr);
static uint8_t bcd_to_binary(uint8_t value);
static uint8_t binary_to_bcd(uint8_t value);

I2C_Handle_t g_ds1307I2cHandle;

//CH=return of this function,if CH=0 init success ,CH=1 init failed
uint8_t ds1307_init(void){
	//1. init the i2c pins
	ds1307_i2c_pin_config();
	//2.initialize the i2c peripheral
	ds1307_i2c_peripheral_config();
	//Enable ı2c peripheral
	I2C_PeripheralControl(g_ds1307I2cHandle.pI2Cx, ENABLE);
	//Make clock halt(CH)=0 reg addrerss=0x00
	ds1307_write(0x00,DS1307_ADD_SEC);
	//read clack halt bit
	uint8_t clock_state=ds1307_read(DS1307_ADD_SEC);
	//retun the clock state
	return ((clock_state>>7) & 0x1);
}

void ds1307_set_current_time(RTC_time_t * rtc_time){

	uint8_t seconds,hours;
	seconds=binary_to_bcd(rtc_time->seconds);
	seconds &= ~(1 << 7);
	ds1307_write(seconds, DS1307_ADD_SEC);
	ds1307_write(binary_to_bcd(rtc_time->minutes), DS1307_ADD_MIN);
	hours=binary_to_bcd(rtc_time->hours);
	if(rtc_time->time_format == TIME_FORMAT_24H){
		hours &= ~(1 << 6);
	}
	else{
		hours |= (1 << 6);
		if(rtc_time->time_format ==TIME_FORMAT_12H_AM){
			hours &= ~(1 << 5);
		}
		else if(rtc_time->time_format ==TIME_FORMAT_12H_PM){
			hours |= (1 << 5);
		}
	}
	ds1307_write(hours, DS1307_ADD_HOUR);
}
void ds1307_get_current_time(RTC_time_t * rtc_time){

	uint8_t seconds,hours;

	seconds=ds1307_read(DS1307_ADD_SEC);

	seconds &=~(1<<7);

	rtc_time->seconds = bcd_to_binary(seconds);

	rtc_time->minutes = bcd_to_binary(ds1307_read(DS1307_ADD_MIN));

	hours=ds1307_read(DS1307_ADD_HOUR);

	if(!((hours >> 6) & 0x1)){
		rtc_time->time_format = TIME_FORMAT_24H;

		hours = (hours & 0x3F);

		rtc_time->hours = bcd_to_binary(hours);
	}
	else{
		if(((hours >> 5) & 0x1)){
			rtc_time->time_format = TIME_FORMAT_12H_PM;
		}
		else{
			rtc_time->time_format = TIME_FORMAT_12H_AM;
		}

		hours = (hours & 0x1F);

		rtc_time->hours = bcd_to_binary(hours);
	}

}

void ds1307_set_current_date(RTC_date_t * rtc_date){

	ds1307_write(binary_to_bcd(rtc_date->date), DS1307_ADD_DATE);

	ds1307_write(binary_to_bcd(rtc_date->month), DS1307_ADD_MONTH);

	ds1307_write(binary_to_bcd(rtc_date->year), DS1307_ADD_YEAR);

	ds1307_write(binary_to_bcd(rtc_date->day), DS1307_ADD_WDAY);

}
void ds1307_get_current_date(RTC_date_t * rtc_date){

	rtc_date->date = bcd_to_binary(ds1307_read(DS1307_ADD_DATE));

	rtc_date->month = bcd_to_binary(ds1307_read(DS1307_ADD_MONTH));

	rtc_date->year = bcd_to_binary(ds1307_read(DS1307_ADD_YEAR));

	rtc_date->day = bcd_to_binary(ds1307_read(DS1307_ADD_WDAY));

}

static void ds1307_i2c_pin_config(void){

	GPIO_Handle_t i2c_sda,i2c_scl;

	memset(&i2c_sda,0,sizeof(i2c_sda));
	memset(&i2c_scl,0,sizeof(i2c_scl));

	i2c_sda.pGPIOx=DS1307_I2C1_PORT;
	i2c_sda.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	i2c_sda.GPIO_PinConfig.GPIO_PinAltFunMode=GPIO_ALFC_AF4;
	i2c_sda.GPIO_PinConfig.GPIO_PinNumber=DS1307_I2C1_SDA;
	i2c_sda.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;
	i2c_sda.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_OD;
	i2c_sda.GPIO_PinConfig.GPIO_PinPuPdControl=DS1307_I2C1_PUPD;

	GPIO_Init(&i2c_sda);

	i2c_scl.pGPIOx=DS1307_I2C1_PORT;
	i2c_scl.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	i2c_scl.GPIO_PinConfig.GPIO_PinAltFunMode=GPIO_ALFC_AF4;
	i2c_scl.GPIO_PinConfig.GPIO_PinNumber=DS1307_I2C1_SCL;
	i2c_scl.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;
	i2c_scl.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_OD;
	i2c_scl.GPIO_PinConfig.GPIO_PinPuPdControl=DS1307_I2C1_PUPD;


	GPIO_Init(&i2c_scl);



}

static void ds1307_i2c_peripheral_config(void){


	g_ds1307I2cHandle.pI2Cx=DS1307_I2C1;
	g_ds1307I2cHandle.I2CConfig.I2C_ACKControl=I2C_ACK_ENABLE;
	g_ds1307I2cHandle.I2CConfig.I2C_FMDutyCycle=I2C_FM_DUTY_2;
	g_ds1307I2cHandle.I2CConfig.I2C_SCLSpeed=DS1307_I2C1_SPEED;

	I2C_Init(&g_ds1307I2cHandle);
}

static void ds1307_write(uint8_t value,uint8_t regAddr){

	uint8_t tx[2];
	tx[0]=regAddr;
	tx[1]=value;
	I2C_MasterSendData(&g_ds1307I2cHandle, tx, 2, DS1307_I2C_ADD, 0);

}

static uint8_t ds1307_read(uint8_t regAddr){



	I2C_MasterSendData(&g_ds1307I2cHandle, &regAddr, 1, DS1307_I2C_ADD, 1);

	uint8_t rx;
	I2C_MasterReceiveData(&g_ds1307I2cHandle, &rx,1, DS1307_I2C_ADD, 0);

	return rx;


}

static uint8_t bcd_to_binary(uint8_t value){

	return (((value >> 4) & 0xF)*10)+(value & 0xF);
}

static uint8_t binary_to_bcd(uint8_t value){

	uint8_t value_units,value_tenth;

	value_tenth= value/10;
	value_units=value-(value_tenth*10);

	return value_units + (value_tenth << 4);

}
