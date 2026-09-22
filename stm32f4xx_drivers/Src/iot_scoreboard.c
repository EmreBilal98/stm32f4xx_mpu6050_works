/*
 * 014rtc_lcd.c
 *
 *  Created on: Feb 26, 2026
 *      Author: emreb
 */

#include <stdio.h>
#include "ds1307.h"
#include "lcd.h"
#include "stm32f407xx.h"
#include "stdint.h"
#include "string.h"
#include <stdlib.h>





#define SYSTICK_TIM_CLK 	16000000UL
#define MESSAGE_COUNT		5
#define SCREEN_CHANGE_COUNT 30

#define HOST 				"192.168.1.24"
#define COMPANY_ID			2
#define PITCH_ID			1

#define TEAM_1				1
#define TEAM_2				2

USART_Handle_t USART2Handle;

RTC_time_t current_time;
RTC_date_t current_date;

uint8_t single_byte;
uint16_t rx_idx = 0;
uint32_t wait_timer = 0; // Non-blocking bekleme için

uint8_t screen_change=0;//30 saniyede bir ekran score ile saat arası geçiş yapması için
uint8_t screen_change_flag=0;

uint8_t scores[2]={0,0};//team1 score:scores[0] ,team2 score:scores[1]


uint8_t day,date,month,year,hours,minutes,seconds;

void parse_and_update_rtc(char* str);
uint8_t get_month_num(char *month_str);
uint8_t get_day_num(char *day_str);

char *msg[5] = {"ATE0\r\n","AT+CWMODE=1\r\n", "AT+CWJAP=\"Vodafonenet_Wifi_0930\",\"enes1234\"\r\n", "AT+CIPSNTPCFG=1,3,\"pool.ntp.org\"\r\n","AT+CIPSNTPTIME?\r\n"};

typedef enum {
    STATE_ATE0,
    STATE_CWMODE,
    STATE_CWJAP,
    STATE_SNTP_CFG,
    STATE_WAIT_SYNC, // Saatin internetten gelmesi için bekleme
    STATE_GET_TIME,
	STATE_DONE,
    STATE_ERROR
} ESP_State_t;

typedef enum {
    STATE_CONNECTION,
    STATE_ACK,
    STATE_SEND,
	STATE_FAILURE
} TIMESTAMP_Send_t;

ESP_State_t current_state = STATE_ATE0;
TIMESTAMP_Send_t current_send_state = STATE_CONNECTION;

//reply from arduino will be stored here
char rx_buf[1024];

//This flag indicates reception completion
uint8_t rxCmplt = RESET;

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

void GPIO_ScoreButtonInits(void){

	GPIO_Handle_t GpioIButton;

	GpioIButton.pGPIOx=GPIOA;
	GpioIButton.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_5;
	GpioIButton.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_IT_RT;
	GpioIButton.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PD;
	GpioIButton.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;

	GPIO_Init(&GpioIButton);

	GpioIButton.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_0;
	GpioIButton.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_NONE;
	GPIO_Init(&GpioIButton);



}

void delay(void){
	for(uint32_t i=0;i<500000;i++);
}

void init_systick_timer(uint32_t tick_hz)
{
	uint32_t *pSRVR = (uint32_t*)0xE000E014;
	uint32_t *pSCSR = (uint32_t*)0xE000E010;

    /* calculation of reload value */
    uint32_t count_value = (SYSTICK_TIM_CLK/tick_hz)-1;

    //Clear the value of SVR
    *pSRVR &= ~(0x00FFFFFFFF);

    //load the value in to SVR
    *pSRVR |= count_value;

    //do some settings
    *pSCSR |= ( 1 << 1); //Enables SysTick exception request:
    *pSCSR |= ( 1 << 2);  //Indicates the clock source, processor clock source

    //enable the systick
    *pSCSR |= ( 1 << 0); //enables the counter

}

char * get_day_of_week(uint8_t i){
	char * days[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

	return days[i-1];

}

void number_to_string(uint8_t num,char* buf){

	if(num <10){
		buf[0] = '0';
		buf[1] = num+48;//get the ascii of the number
	}
	else if(num>=10 && num <99){
		buf[0] = (num/10)+48;
		buf[1] = (num%10)+48;
	}
}

//hh:mm:ss
char * time_to_string(RTC_time_t *time){

	static char buf[9];
	buf[2]=':';
	buf[5]=':';

	number_to_string(time->hours,buf);
	number_to_string(time->minutes,&buf[3]);
	number_to_string(time->seconds,&buf[6]);

	buf[8]='\0';

	return buf;

}


//dd/mm/yy
char * date_to_string(RTC_date_t *date){
	static char buf[9];
	buf[2]='/';
	buf[5]='/';

	number_to_string(date->date,buf);
	number_to_string(date->month,&buf[3]);
	number_to_string(date->year,&buf[6]);

	buf[8]='\0';

	return buf;

}


int main (void){

	GPIO_PeripheralClockControl(GPIOA, ENABLE);
	GPIO_ScoreButtonInits();
	USART2_GPIOInits();
	USART2_Inits();
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, 15);
	GPIO_IRQConfig(IRQ_NO_EXTI9_5, ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, 15);
	GPIO_IRQConfig(IRQ_NO_EXTI0, ENABLE);
	USART_IRQInterruptConfig(IRQ_NO_USART2, ENABLE);
	USART_PeripheralControl(USART2, ENABLE);

	uint8_t cnt =0;

	printf("RTC test\n");


	if(ds1307_init()){

		printf("RTC init has failed\n");
		while(1);

	}

	lcd_init();

	lcd_print_string("RTC test...");

	mdelay(2000);

	lcd_display_clear();
	lcd_display_return_home();

	if (current_state == STATE_ATE0) {
	    rx_idx = 0; memset(rx_buf, 0, 1024); rxCmplt = DISABLE;
	    USART_ReceiveDataIT(&USART2Handle, &single_byte, 1);

	    USART_SendData(&USART2Handle, (uint8_t *)msg[cnt], strlen(msg[cnt]));
	    printf("Transmitted: %s", msg[cnt]);

	    uint32_t timeout = 30000000;
	    while(rxCmplt != ENABLE && --timeout);

	    if(rxCmplt == ENABLE) {
	        current_state = STATE_CWMODE; // Başarılı, sıradakine geç
	        cnt++;
	    } else {
	    	printf("State %d Timeout!\n", current_state);
	    	current_state = STATE_ERROR;
	    }
	}

	if (current_state == STATE_CWMODE) {
	    rx_idx = 0; memset(rx_buf, 0, 1024); rxCmplt = DISABLE;
	    USART_ReceiveDataIT(&USART2Handle, &single_byte, 1);

	    USART_SendData(&USART2Handle, (uint8_t *)msg[cnt], strlen(msg[cnt]));
	    printf("Transmitted: %s", msg[cnt]);

	    uint32_t timeout = 30000000;
	    while(rxCmplt != ENABLE && --timeout);

	    if(rxCmplt == ENABLE) {
	        current_state = STATE_CWJAP; // Başarılı, sıradakine geç
	        cnt++;
	    } else {
	    	printf("State %d Timeout!\n", current_state);
	    	current_state = STATE_ERROR;
	    }
	}

	if (current_state == STATE_CWJAP) {
	    rx_idx = 0; memset(rx_buf, 0, 1024); rxCmplt = DISABLE;
	    USART_ReceiveDataIT(&USART2Handle, &single_byte, 1);

	    USART_SendData(&USART2Handle, (uint8_t *)msg[cnt], strlen(msg[cnt]));
	    printf("Transmitted: %s", msg[cnt]);

	    // Wi-Fi için 15 saniye sabret
	    uint32_t timeout = 150000000;
	    while(rxCmplt != ENABLE && --timeout);

	    if(rxCmplt == ENABLE) {
	        current_state = STATE_SNTP_CFG; // Başarılı, sıradakine geç
	        cnt++;
	    } else {
	    	printf("State %d Timeout!\n", current_state);
	    	current_state = STATE_ERROR;
	    }
	}


	if (current_state == STATE_SNTP_CFG) {
	    rx_idx = 0; memset(rx_buf, 0, 1024); rxCmplt = DISABLE;
	    USART_ReceiveDataIT(&USART2Handle, &single_byte, 1);

	    USART_SendData(&USART2Handle, (uint8_t *)msg[cnt], strlen(msg[cnt]));
	    printf("Transmitted: %s", msg[cnt]);

	    uint32_t timeout = 30000000;
	    while(rxCmplt != ENABLE && --timeout);

	    if(rxCmplt == ENABLE) {
	        current_state = STATE_WAIT_SYNC; // Başarılı, sıradakine geç
	        cnt++;
	    } else {
	    	printf("State %d Timeout!\n", current_state);
	    	current_state = STATE_ERROR;
	    }
	}

	if (current_state == STATE_WAIT_SYNC) {

		printf("NTP Senkronizasyonu icin 5 saniye bekleniyor...\n");

		    // İşlemci burada 5 saniye boyunca taş kesilir, başka iş yapmaz.
		    mdelay(5000);

		    printf("Sure doldu, saati isteme asamasina geciliyor.\n");

		    // Süre biter bitmez durumu değiştiriyoruz
		    current_state = STATE_GET_TIME;
	}

	if (current_state == STATE_GET_TIME) {
	    rx_idx = 0; memset(rx_buf, 0, 1024); rxCmplt = DISABLE;
	    USART_ReceiveDataIT(&USART2Handle, &single_byte, 1);

	    USART_SendData(&USART2Handle, (uint8_t *)msg[cnt], strlen(msg[cnt]));
	    printf("Transmitted: %s", msg[cnt]);

	    uint32_t timeout = 30000000;
	    while(rxCmplt != ENABLE && --timeout);

	    if(rxCmplt == ENABLE) {
	    	parse_and_update_rtc((char*)rx_buf);
	        current_state = STATE_DONE; // Başarılı, sıradakine geç
	    } else {
	    	printf("State %d Timeout!\n", current_state);
	    	current_state = STATE_ERROR;
	    }
	}

	init_systick_timer(1);

	ds1307_set_current_time(&current_time);
	ds1307_set_current_date(&current_date);


	//ekleme
	lcd_display_clear();
	lcd_display_return_home();

	//ekleme son

 	while(1);

	return 0;
}

void SysTick_Handler(void){

	ds1307_get_current_time(&current_time);

	lcd_display_clear();
	lcd_display_return_home();

	screen_change++;
	if(!screen_change_flag){
	char *am_pm;

	if(current_time.time_format != TIME_FORMAT_24H){
		am_pm = (current_time.time_format) ? "PM" : "AM";
		printf("Current time: %s %s\n",time_to_string(&current_time),am_pm);//04:25:41 PM
		lcd_print_string(time_to_string(&current_time));
		lcd_print_string(am_pm);
	}else{
		printf("Current time: %s\n",time_to_string(&current_time));//04:25:41 PM
		lcd_print_string(time_to_string(&current_time));
	}

	ds1307_get_current_date(&current_date);

	printf("Current date: %s <%s>\n",date_to_string(&current_date),get_day_of_week(current_date.day));
	lcd_set_cursor(2, 1);
	lcd_print_string(date_to_string(&current_date));
	lcd_send_char('<');
	lcd_print_string(get_day_of_week(current_date.day));
	lcd_send_char('>');
	}
	else{
		//scoreboard

		char buffer[20];
		lcd_print_string("TEAM1 ");
		sprintf(buffer, "%d", scores[0]);
		lcd_print_string(buffer);
		lcd_send_char('-');
		sprintf(buffer, "%d", scores[1]);
		lcd_print_string(buffer);
		lcd_print_string(" TEAM2");

		lcd_set_cursor(2, 6);
		sprintf(buffer, "%d", current_time.minutes);
		lcd_print_string(buffer);
		lcd_send_char(':');
		sprintf(buffer, "%d", current_time.seconds);
		lcd_print_string(buffer);
	}

	if(screen_change >= SCREEN_CHANGE_COUNT){
		screen_change = 0;
		screen_change_flag = screen_change_flag ^ 1;
	}


}

void USART2_IRQHandler(void){
	USART_IRQHandling(&USART2Handle);
}



void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle, uint8_t AppEv) {

    if(AppEv == USART_EV_RX_CMPLT) {
            if (rx_idx < sizeof(rx_buf) - 1) {
                rx_buf[rx_idx++] = single_byte;
                rx_buf[rx_idx] = '\0';
            }

            // DURMA ŞARTLARI: Duruma göre ne arıyoruz?
            if (strstr((char*)rx_buf, "OK\r\n") || strstr((char*)rx_buf, "ERROR\r\n")) {
                rxCmplt = 1;
            }
            else if (current_state == STATE_GET_TIME && strlen((char*)rx_buf)==37) {
                rxCmplt = 1; // Zaman bilgisi gelmeye başladı, durabiliriz
            }
            else if (current_send_state == STATE_ACK && strstr((char*)rx_buf,">")){
            	rxCmplt = 1; // ack_geldi
            }
            else {
                // Hiçbir anahtar kelime yoksa dinlemeye devam et
                USART_ReceiveDataIT(pUSARTHandle, &single_byte, 1);
            }
        }

    if(AppEv == USART_ERR_ORE) {

    	uint8_t dummy = pUSARTHandle->pUSARTx->DR;
    	(void)dummy; // Derleyici uyarısı vermemesi için

    	// ÇOK KRİTİK: Hata sonrası kesme mekanizması durmuş olabilir.
    	    // Alımı 1 byte olarak tekrar başlatmalısın:
    	USART_ReceiveDataIT(pUSARTHandle, &single_byte, 1);

    }
}


void parse_and_update_rtc(char* str) {

    char parca[10];
    char *p;
    uint8_t hours;
    // Şimdilik boş kalsın, sadece derleme hatasını çözmek için.
    printf("Gelen Zaman Verisi: %s\n", str);

    str=strstr((char*)str, ":");
    str++;

    printf("Gelen Zaman Verisi: %s\n", str);

    p = strchr(str, ' '); // İlk boşluk karakterini bul

    if (p != NULL) {
        int uzunluk = p - str; // Karakterin konumunu hesapla
        strncpy(parca, str, uzunluk);
        parca[uzunluk] = '\0'; // String sonlandırıcıyı unutma!
        printf("weekday: %s\n", parca);
        current_date.day =get_day_num((char *)parca);

    }

    str=strstr((char*)str, " ");
    str++;

    p = strchr(str, ' '); // İlk boşluk karakterini bul

    if (p != NULL) {
        int uzunluk = p - str; // Karakterin konumunu hesapla
        strncpy(parca, str, uzunluk);
        parca[uzunluk] = '\0'; // String sonlandırıcıyı unutma!
        printf("month: %s\n", parca);
        current_date.month =get_month_num((char *)parca);
    }

    str=strstr((char*)str, " ");
    str++;

    p = strchr(str, ' '); // İlk boşluk karakterini bul

    if (p != NULL) {
        int uzunluk = p - str; // Karakterin konumunu hesapla
        strncpy(parca, str, uzunluk);
        parca[uzunluk] = '\0'; // String sonlandırıcıyı unutma!
        printf("date: %s\n", parca);
        current_date.date =atoi((char*)parca);

    }

    str=strstr((char*)str, " ");
    str++;

    p = strchr(str, ':'); // İlk boşluk karakterini bul

    if (p != NULL) {
        int uzunluk = p - str; // Karakterin konumunu hesapla
        strncpy(parca, str, uzunluk);
        parca[uzunluk] = '\0'; // String sonlandırıcıyı unutma!
        printf("hours: %s\n", parca);
        hours=atoi((char*)parca);

        if(hours==12){
        	current_time.hours=12;
        }else{
        	current_time.hours=hours-((hours/12)*12);
        }



        current_time.time_format=(hours/12)?TIME_FORMAT_12H_PM:TIME_FORMAT_12H_AM;
    }

    str=strstr((char*)str, ":");
    str++;

    p = strchr(str, ':'); // İlk boşluk karakterini bul

    if (p != NULL) {
        int uzunluk = p - str; // Karakterin konumunu hesapla
        strncpy(parca, str, uzunluk);
        parca[uzunluk] = '\0'; // String sonlandırıcıyı unutma!
        printf("minutes: %s\n", parca);
        current_time.minutes =atoi((char*)parca);
    }

    str=strstr((char*)str, ":");
    str++;

    p = strchr(str, ' '); // İlk boşluk karakterini bul

    if (p != NULL) {
        int uzunluk = p - str; // Karakterin konumunu hesapla
        strncpy(parca, str, uzunluk);
        parca[uzunluk] = '\0'; // String sonlandırıcıyı unutma!
        printf("seconds: %s\n", parca);
        current_time.seconds =atoi((char*)parca);
    }

    str=strstr((char*)str, " ");
    str++;

    printf("year: %s\n", str);
    current_date.year = atoi(str)-2000;

	current_time.time_format=TIME_FORMAT_12H_PM;
}

// AY İSMİNİ SAYIYA ÇEVİRİR (1-12)
uint8_t get_month_num(char *month_str) {
    if (strcmp(month_str, "Jan") == 0) return 1;
    if (strcmp(month_str, "Feb") == 0) return 2;
    if (strcmp(month_str, "Mar") == 0) return 3;
    if (strcmp(month_str, "Apr") == 0) return 4;
    if (strcmp(month_str, "May") == 0) return 5;
    if (strcmp(month_str, "Jun") == 0) return 6;
    if (strcmp(month_str, "Jul") == 0) return 7;
    if (strcmp(month_str, "Aug") == 0) return 8;
    if (strcmp(month_str, "Sep") == 0) return 9;
    if (strcmp(month_str, "Oct") == 0) return 10;
    if (strcmp(month_str, "Nov") == 0) return 11;
    if (strcmp(month_str, "Dec") == 0) return 12;
    return 0; // Hata durumu
}

// GÜN İSMİNİ SAYIYA ÇEVİRİR (1-7)
uint8_t get_day_num(char *day_str) {
    if (strcmp(day_str, "Mon") == 0) return MONDAY;
    if (strcmp(day_str, "Tue") == 0) return TUESDAY;
    if (strcmp(day_str, "Wed") == 0) return WEDNESDAY;
    if (strcmp(day_str, "Thu") == 0) return THURSDAY;
    if (strcmp(day_str, "Fri") == 0) return FRIDAY;
    if (strcmp(day_str, "Sat") == 0) return SATURDAY;
    if (strcmp(day_str, "Sun") == 0) return SUNDAY;
    return 0; // Hata durumu

}

void timestamp_send_api(uint8_t team_id){
	//{"pitch_id": 1, "datetime_from_st": "%/%/% %:%:%", "user_id": 1}
	char con_server[128],msg_pck[128],http_request[512];

	int con_len=sprintf(con_server,"AT+CIPSTART=\"TCP\",\"%s\",8000\r\n",HOST);

	char ack[32];

	int msg_len=sprintf(msg_pck,"{\"pitch_id\": %d,\"team_id\": %d,\"datetime_from_st\":\"%d/%d/%d %d:%d:%d\",\"user_id\":%d}",PITCH_ID,team_id,current_date.date,current_date.month,current_date.year,
																						(current_time.time_format & (current_time.hours!=12))?current_time.hours+12:current_time.hours,
																						current_time.minutes,current_time.seconds,COMPANY_ID);
	int request_len = sprintf(http_request,
	        "POST /api/records HTTP/1.1\r\n"
	        "Host: %s:8000\r\n"
	        "Content-Type: application/json\r\n"
	        "Content-Length: %d\r\n"
			"Connection: close\r\n"
	        "\r\n"
	        "%s",
	        HOST, msg_len, msg_pck);

	int ack_len=sprintf(ack, "AT+CIPSEND=%d\r\n", request_len);


	if (current_send_state == STATE_CONNECTION) {
		    rx_idx = 0; memset(rx_buf, 0, 1024); rxCmplt = DISABLE;
		    USART_ReceiveDataIT(&USART2Handle, &single_byte, 1);

		    USART_SendData(&USART2Handle, (uint8_t *)con_server, con_len);
		    printf("Transmitted: %s", con_server);
		    printf("Transmitted: %s", ack);
		    printf("Transmitted: %s", http_request);

		    uint32_t timeout = 30000000;
		    while(rxCmplt != ENABLE && --timeout);

		    if(rxCmplt == ENABLE) {
		    	current_send_state = STATE_ACK; // Başarılı, sıradakine geç
		    } else {
		    	printf("State %d Timeout!\n", current_send_state);
		    	current_send_state = STATE_FAILURE;
		    }
		}

		if (current_send_state == STATE_ACK) {
		    rx_idx = 0; memset(rx_buf, 0, 1024); rxCmplt = DISABLE;
		    USART_ReceiveDataIT(&USART2Handle, &single_byte, 1);

		    USART_SendData(&USART2Handle, (uint8_t *)ack, ack_len);
		    printf("Transmitted: %s", ack);

		    uint32_t timeout = 30000000;
		    while(rxCmplt != ENABLE && --timeout);

		    if(rxCmplt == ENABLE) {
		    	current_send_state = STATE_SEND; // Başarılı, sıradakine geç
		    } else {
		    	printf("State %d Timeout!\n", current_send_state);
		    	current_send_state = STATE_FAILURE;
		    }
		}

		if (current_send_state == STATE_SEND) {
		    rx_idx = 0; memset(rx_buf, 0, 1024); rxCmplt = DISABLE;
		    USART_ReceiveDataIT(&USART2Handle, &single_byte, 1);

		    USART_SendData(&USART2Handle, (uint8_t *)http_request, request_len);
		    printf("Transmitted: %s", http_request);

		    uint32_t timeout = 30000000;
		    while(rxCmplt != ENABLE && --timeout);

		    if(rxCmplt == ENABLE) {
		    	current_send_state = STATE_CONNECTION; // Başarılı, sıradakine geç
		    } else {
		    	printf("State %d Timeout!\n", current_send_state);
		    	current_send_state = STATE_FAILURE;
		    }
		}


}

void EXTI9_5_IRQHandler(void){

	scores[0]++;
	timestamp_send_api(TEAM_1);
	GPIO_IRQHandling(GPIO_PIN_NUMBER_5);

}

void EXTI0_IRQHandler(void){

	scores[1]++;
	timestamp_send_api(TEAM_2);
	GPIO_IRQHandling(GPIO_PIN_NUMBER_0);

}
