/*
 * mpu6050_measurement.c
 *
 *  Created on: Sep 18, 2026
 *      Author: emreb
 */

#include "stdio.h"
#include "string.h"
#include "mpu6050.h"
#include<stdarg.h>
#include <math.h>
#include "kalman_filter.h"

void I2C1_init(void);
void i2cGpioInit(void);
void Timer4_init(void);
void tim4GpioInit(void);
void UART2_init(void);
void uart2GpioInit(void);
void Error_Handler(void);
void SystemClockConfig_HSE_50MHz (void);
uint32_t GetTick(void);
void SysTick_Init(void);
void printmsg(char *format, ...);

#define 	TRUE	1
#define 	FALSE	0

#define ANGLE_POS_MIN 0
#define ANGLE_POS_MAX 90

#define PWM_PULSE_MIN 0
#define PWM_PULSE_MAX 1000


I2C_Handle_t hi2c;
TIM_Handle_t htim4;
USART_Handle_t huart2;

uint32_t g_counter = 0;
volatile uint32_t g_channel_1_state = 800;
volatile uint32_t g_channel_2_state = 400;
int16_t roll_angle;
float kalman_angle;
mpu6050_accel_data_t acceleration_datas;

const mpu6050_accel_data_t error_data = {
		.x = 250,
		.y = -200,
		.z =156
};

volatile uint32_t uwTick = 0;


long map(float x,long in_min,long in_max,long out_min,long out_max){

	long result = ((x-in_min)*(out_max-out_min)/(in_max -in_min))+out_min;

	if(result > out_max){
		result=out_max;
	}
	else if(result <out_min){
		result=out_min;
	}

	return result;
}

void change_pwm_duty_cycle(uint32_t pulse,uint8_t channel){
	if (channel == TIM_CHANNEL_1) {
			htim4.pTIMx->CCR1 = pulse;
	    }
	    else if (channel == TIM_CHANNEL_2) {
	    	htim4.pTIMx->CCR2 = pulse;
	    }
	    else if (channel == TIM_CHANNEL_3) {
	    	htim4.pTIMx->CCR3 = pulse;
	    }
	    else if (channel == TIM_CHANNEL_4) {
	    	htim4.pTIMx->CCR4 = pulse;
	    }
}

int main(void){

	KalmanFilter kf;
	float dt;
	kalman_filter_init(&kf);

	SystemClockConfig_HSE_50MHz ();
	SysTick_Init();
	i2cGpioInit();
	I2C1_init();
	I2C_PeripheralControl(hi2c.pI2Cx, ENABLE);
	hi2c.pI2Cx->CR1 |= (1 << 10);
	Timer4_init();
	tim4GpioInit();
	UART2_init();
	uart2GpioInit();

	TIM_InterruptControl(&htim4, TIM_EVENT_UPDATE, ENABLE);//updateevent interrupt enable ediliyor


	// Kanal 1 ve Kanal 2'nin pin çıkışlarını (CCxE) aç
	htim4.pTIMx->CCER |= (1 << (4*TIM_CHANNEL_1)); // CH1 Enable
	htim4.pTIMx->CCER |= (1 << (4*TIM_CHANNEL_2)); // CH2 Enable

	TIM_InterruptControl(&htim4, TIM_EVENT_CC1, ENABLE); // CH1 Pulse kesmesini aç
	TIM_InterruptControl(&htim4, TIM_EVENT_CC2, ENABLE); // CH2 Pulse kesmesini aç

	//clear the old flags
	htim4.pTIMx->SR = 0;

	TIM_IRQInterruptConfig(30, ENABLE);
	TIM_PeripheralControl(htim4.pTIMx, ENABLE);

	if(mpu6050_init(&hi2c)!= MPU6050_OK){
		//printmsg("error occured when MPU6050 init\r\n");
	}
	else{
		//printmsg("mpu6050 initializition done succesfully\r\n");
	}

	if(mpu6050_Set_LPF(&hi2c, MPU6050_BANDWITH_20)!=MPU6050_OK){
		Error_Handler();
	}


	uint32_t previous_tick= GetTick();
	while(1){

		uint32_t current_tick = GetTick();

		// En az 1 ms geçmesini bekle (dt'nin 0 olmasını kesinlikle engeller)
				if ((current_tick - previous_tick) == 0) {
					continue;
				}

		dt = (current_tick-previous_tick)/1000.0f;
		previous_tick=current_tick;

#ifdef PWM_DEBUG
		g_counter = __HAL_TIM_GET_COUNTER(&htim4);
#endif
		if(mpu6050_read_accelerometer_data(&hi2c, &acceleration_datas)!= MPU6050_OK){
			Error_Handler();
		}
		mpu6050_calibrate_accelerometer_data(&acceleration_datas, &error_data);
		//printmsg("x:%d\r\n",acceleration_datas.x);
		//printmsg("y:%d\r\n",acceleration_datas.y);
		//printmsg("z:%d\r\n",acceleration_datas.z);

		roll_angle = atan2(acceleration_datas.y,acceleration_datas.z)*(180/M_PI);
		kalman_angle = kalman_filter_get_angle(&kf, roll_angle, dt);


		uint8_t channel = (kalman_angle < 0) ? TIM_CHANNEL_1 : TIM_CHANNEL_2;
		float abs_kalman_angle = (kalman_angle < 0) ? -kalman_angle : kalman_angle;
		        uint32_t pwm_pulse = map(abs_kalman_angle, ANGLE_POS_MIN, ANGLE_POS_MAX, PWM_PULSE_MIN, PWM_PULSE_MAX);
		        // Eğer HAL'de "hayalet LED" sorunu yaşamıyorsan, doğrudan senin yazım şeklini kullanıyoruz:
		                change_pwm_duty_cycle(pwm_pulse, channel);

		                // (Opsiyonel Güvenlik: Eğer HAL'de LED'lerden biri takılı kalmıyorsa bu satırı silebilirsin.
		                // Takılı kalıyorsa diğer kanalı sıfırlamak için bunu kullan)
		                change_pwm_duty_cycle(0, (channel == TIM_CHANNEL_1) ? TIM_CHANNEL_2 : TIM_CHANNEL_1);

//		uint32_t wait_tick = GetTick();
//		while((GetTick() - wait_tick) < 10);
	}


	return 0;
}


void I2C1_init(void){


	//pb8->i2c1_scl
	//pb9->i2c1_sda

	hi2c.pI2Cx=I2C1;
	hi2c.I2CConfig.I2C_ACKControl=I2C_ACK_ENABLE;
	hi2c.I2CConfig.I2C_SCLSpeed=I2C_SCL_SPEED_SM;
	hi2c.I2CConfig.I2C_DeviceAddress=0;

	I2C_Init(&hi2c);



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
	i2c1_pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_8;
	GPIO_Init(&i2c1_pins);

	//sda
	i2c1_pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_9;
	GPIO_Init(&i2c1_pins);

}





void Timer4_init(void){

	htim4.pTIMx = TIM4;
	htim4.TIMConfig.period=1000-1;
	htim4.TIMConfig.prescaler=50000-1;

	TIM_Init(&htim4);

	Timer_PWM_Config_t tim4PWM;

	memset(&tim4PWM,0,sizeof(tim4PWM));
	tim4PWM.Pulse = ((htim4.TIMConfig.period+1) * 0)/10;//%0duty cycle
	tim4PWM.Polarity = TIM_OCPOLARITY_HIGH;
	tim4PWM.Mode = TIM_OCMODE_PWM1;
	tim4PWM.Channel =TIM_CHANNEL_1;

	TIM_PWM_ConfigChannel(&htim4, &tim4PWM);


	tim4PWM.Channel =TIM_CHANNEL_2;
	tim4PWM.Pulse = ((htim4.TIMConfig.period+1) * 0)/10;//%0duty cycle

	TIM_PWM_ConfigChannel(&htim4, &tim4PWM);


}

void tim4GpioInit(void){
	GPIO_Handle_t tim4_pins;
	tim4_pins.pGPIOx=GPIOD;
	tim4_pins.GPIO_PinConfig.GPIO_PinAltFunMode=GPIO_ALFC_AF2;
	tim4_pins.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	tim4_pins.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	tim4_pins.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_NONE;
	tim4_pins.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HS;

	//ch1
	tim4_pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_12;
	GPIO_Init(&tim4_pins);

	//ch2
	tim4_pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_13;
	GPIO_Init(&tim4_pins);

}



void UART2_init(void){

	huart2.pUSARTx = USART2;
	huart2.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	huart2.USART_Config.USART_WordLength =USART_WORDLEN_8BITS;
	huart2.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	huart2.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	huart2.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	huart2.USART_Config.USART_Mode= USART_MODE_TXRX;
	USART_Init(&huart2);

}

void uart2GpioInit(void){
	GPIO_Handle_t uart2_pins;
	uart2_pins.pGPIOx=GPIOA;
	uart2_pins.GPIO_PinConfig.GPIO_PinAltFunMode=GPIO_ALFC_AF7;
	uart2_pins.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	uart2_pins.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	uart2_pins.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;
	uart2_pins.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_LS;

	//tx
	uart2_pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_2;
	GPIO_Init(&uart2_pins);

	//rx
	uart2_pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NUMBER_3;
	GPIO_Init(&uart2_pins);



}



void SystemClockConfig_HSE_50MHz (){

	RCC_PLL_Config_t pll_config= {
	        .PLL_M = 8,
	        .PLL_N = 200,
	        .PLL_P = 4,
	        .PLL_Q = 7
	    };

	SystemClock_Config_HSE(&pll_config);
}




void Error_Handler(void){
	while(1);
}

void TIM_ApplicationCallback(TIM_Handle_t *pTIMHandle, uint8_t AppEv) {

	//counter reach the pwm value interrupt(CH1)
    if (AppEv == TIM_EVENT_CC1) {
    	g_channel_1_state = 0;
    }
    //counter reach the pwm value interrupt(CH2)
    else if (AppEv == TIM_EVENT_CC2) {
    	g_channel_2_state = 0;
    }
    //timer period end interrupt
    else if (AppEv == TIM_EVENT_UPDATE) {
    	g_counter = 0;
    	g_channel_1_state = 800;
    	g_channel_2_state = 400;
    }
}

void printmsg(char *format,...)
 {

	char str[80];

	/*Extract the the argument list using VA apis */
	va_list args;
	va_start(args, format);
	vsprintf(str, format,args);
	USART_SendData(&huart2,(uint8_t *)str, strlen(str));
	va_end(args);

 }

uint32_t GetTick(void) {
    return uwTick;
}

void SysTick_Init(void) {
    // 1. Reload (Yükleme) Değerini Ayarla
    // Adres: 0xE000E014 (SYSTICK_LOAD)
    // 50 MHz'de 1 ms elde etmek için (50.000 - 1) = 49999 yazılır.
    SysTick->LOAD = 49999;

    // 2. Sayacın Mevcut Değerini (Current Value) Sıfırla
    // Adres: 0xE000E018 (SYSTICK_VAL)
    // Bu yazmaca herhangi bir değer yazmak sayacı ve bayrakları anında sıfırlar.
    SysTick->VAL = 0;

    // 3. Kontrol ve Durum Yazmacını (CTRL) Ayarla ve Sayacı Başlat
    // Adres: 0xE000E010 (SYSTICK_CTRL)
    // Bit 2 (CLKSOURCE) = 1 : Saat kaynağı olarak doğrudan işlemci saatini (50 MHz) kullan.
    // Bit 1 (TICKINT)   = 1 : Sayıcı 0'a ulaştığında SysTick istisnasını (kesmesini) tetikle.
    // Bit 0 (ENABLE)    = 1 : SysTick sayacını başlat.
    SysTick->CTRL = (1 << 2) | (1 << 1) | (1 << 0);
}


// İşlemci her 1 ms'de bir bu fonksiyonu otomatik çağırır
void SysTick_Handler(void) {
    uwTick++; // Global tick değişkenini artır
}

void TIM4_IRQHandler(void) {
    // Donanımdan gelen kesmeyi, senin yazdığın sürücü işleyicisine yönlendir
    TIM_IRQHandling(&htim4);
}

