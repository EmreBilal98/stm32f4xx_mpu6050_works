/*
 * lcd.c
 *
 *  Created on: Feb 26, 2026
 *      Author: emreb
 */

#include "lcd.h"

static void write_4_bits(uint8_t value);
static void lcd_enable(void);

void lcd_send_command(uint8_t cmd){

	/*RS=0,for LCD command*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, DISABLE);

	/*RnW=0,writing to LCD*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, DISABLE);

	write_4_bits((cmd >> 4) & 0x0F);//higher nibble

	write_4_bits(cmd & 0x0F);//lower nibble


}

void lcd_send_char(uint8_t cmd){

	/*RS=1,for LCD user data*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, ENABLE);

	/*RnW=0,writing to LCD*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, DISABLE);

	write_4_bits((cmd >> 4) & 0x0F);//higher nibble

	write_4_bits(cmd & 0x0F);//lower nibble


}

void lcd_print_string(char *message){

	do{
		lcd_send_char((uint8_t)*message++);
	}
	while(*message != '\0');

}

void lcd_set_cursor(uint8_t row,uint8_t column){

	column--;
	switch(row){
	case 1:
		lcd_send_command(column |= 0x80);
		break;
	case 2:
		lcd_send_command(column |= 0xC0);
		break;
	default:
		break;
	}
}

void lcd_init(void){

	//1.configure the gpio pins which are used for lcd connections

	GPIO_Handle_t lcd_signal;

	lcd_signal.pGPIOx=LCD_GPIO_PORT;
	lcd_signal.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_OUT;
	lcd_signal.GPIO_PinConfig.GPIO_PinNumber=LCD_GPIO_RS;
	lcd_signal.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	lcd_signal.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_NONE;
	lcd_signal.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber=LCD_GPIO_RW;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber=LCD_GPIO_EN;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber=LCD_GPIO_D4;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber=LCD_GPIO_D5;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber=LCD_GPIO_D6;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber=LCD_GPIO_D7;
	GPIO_Init(&lcd_signal);

	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, DISABLE);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, DISABLE);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, DISABLE);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, DISABLE);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, DISABLE);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, DISABLE);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, DISABLE);

	//Do the LCD initialization

	mdelay(40);

	/*RS=0,for LCD command*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, DISABLE);

	/*RnW=0,writing to LCD*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, DISABLE);

	write_4_bits(0x03);

	mdelay(5);

	write_4_bits(0x03);

	udelay(150);

	write_4_bits(0x03);
	write_4_bits(0x02);

	//function set command
	lcd_send_command(LCD_CMD_4DL_2N_5x8F);

	//display on and cursor on
	lcd_send_command(LCD_CMD_DON_CURON);

	lcd_display_clear();

	//entry mode set
	lcd_send_command(LCD_CMD_INCADD);

}

static void write_4_bits(uint8_t value){
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, (value & 0x01) );
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, ((value >> 1 ) & 0x01));
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, ((value >> 2 ) & 0x01));
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, ((value >> 3 ) & 0x01));

	lcd_enable();
}

static void lcd_enable(void){
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, ENABLE);
	udelay(10);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, DISABLE);
	udelay(100);// execution time must be greater than 37 micro seconds.
}


void lcd_display_clear(void){
	//display clear
	lcd_send_command(LCD_CMD_DIS_CLEAR);
	mdelay(2);
}

void lcd_display_return_home(void){
	//display clear
	lcd_send_command(LCD_CMD_DIS_RETURN_HOME);
	mdelay(2);
}

void mdelay(uint32_t cnt){

	for(uint32_t i=0 ; i<(cnt * 1000) ;i++);
}

void udelay(uint32_t cnt){

	for(uint32_t i=0 ; i<(cnt * 1) ;i++);
}
