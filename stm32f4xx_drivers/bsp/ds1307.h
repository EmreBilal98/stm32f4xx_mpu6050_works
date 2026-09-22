/*
 * ds1307.h
 *
 *  Created on: Feb 26, 2026
 *      Author: emreb
 */

#ifndef DS1307_H_
#define DS1307_H_

#include "stm32f407xx.h"

/*Application configurable items*/
#define DS1307_I2C1			I2C1
#define DS1307_I2C1_PORT	GPIOB
#define DS1307_I2C1_SDA		GPIO_PIN_NUMBER_7
#define DS1307_I2C1_SCL		GPIO_PIN_NUMBER_6
#define DS1307_I2C1_SPEED	I2C_SCL_SPEED_SM
#define DS1307_I2C1_PUPD	GPIO_PIN_PU


/*Register Adresses*/
#define DS1307_ADD_SEC 		0x00
#define DS1307_ADD_MIN 		0x01
#define DS1307_ADD_HOUR 	0x02
#define DS1307_ADD_WDAY	 	0x03
#define DS1307_ADD_DATE 	0x04
#define DS1307_ADD_MONTH 	0x05
#define DS1307_ADD_YEAR		0x06

#define DS1307_I2C_ADD		0x68

#define TIME_FORMAT_12H_AM	0
#define TIME_FORMAT_12H_PM	1
#define TIME_FORMAT_24H		2

#define TIME_OSCILLATOR_EN	0
#define TIME_OSCILLATOR_DI	1

#define SUNDAY				1
#define MONDAY				2
#define TUESDAY				3
#define WEDNESDAY			4
#define THURSDAY			5
#define FRIDAY				6
#define SATURDAY			7

typedef struct{
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t day;

}RTC_date_t;

typedef struct{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t time_format;

}RTC_time_t;

/*function prototypes*/

uint8_t ds1307_init(void);

void ds1307_set_current_time(RTC_time_t *);
void ds1307_get_current_time(RTC_time_t *);

void ds1307_set_current_date(RTC_date_t *);
void ds1307_get_current_date(RTC_date_t *);


#endif /* DS1307_H_ */
