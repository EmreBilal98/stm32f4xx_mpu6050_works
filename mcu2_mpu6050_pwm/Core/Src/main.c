/*
 *  main.c
 *
 *  Created on: Jul 28, 2026
 *      Author: emreb
 */

#include "main.h"
#include "stdio.h"
#include "string.h"
#include "mpu6050.h"
#include<stdarg.h>
#include <math.h>
#include "kalman_filter.h"

void I2C1_init(void);
void Timer4_init(void);
void UART2_init(void);
void Error_Handler(void);
void SystemClockConfig_HSE (uint8_t);

#define 	TRUE	1
#define 	FALSE	0

#define		SYS_CLK_FREQ_50 	50
#define		SYS_CLK_FREQ_84 	84
#define		SYS_CLK_FREQ_120 	120
#define		SYS_CLK_FREQ_168 	168

#define ANGLE_POS_MIN 0
#define ANGLE_POS_MAX 90

#define PWM_PULSE_MIN 0
#define PWM_PULSE_MAX 1000


I2C_HandleTypeDef hi2c;
TIM_HandleTypeDef htim4;
UART_HandleTypeDef huart2;

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


long map(long x,long in_min,long in_max,long out_min,long out_max){

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
	__HAL_TIM_SET_COMPARE(&htim4,channel,pulse);
}

int main(void){

	KalmanFilter kf;
	float dt;
	kalman_filter_init(&kf);

	SystemClockConfig_HSE(SYS_CLK_FREQ_50);
	I2C1_init();
	Timer4_init();
	UART2_init();

	__HAL_TIM_ENABLE_IT(&htim4,TIM_IT_UPDATE);//updateevent interrupt enable ediliyor

	HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_2);

	if(mpu6050_init(&hi2c)!= MPU6050_OK){
		printmsg("error occured when MPU6050 init\r\n");
	}
	else{
		printmsg("mpu6050 initializition done succesfully\r\n");
	}

	if(mpu6050_Set_LPF(&hi2c, MPU6050_BANDWITH_20)!=MPU6050_OK){
		Error_Handler();
	}


	uint32_t previous_tick= HAL_GetTick();
	while(1){

		uint32_t current_tick = HAL_GetTick();
		dt = (current_tick-previous_tick)/1000.0f;
		previous_tick=HAL_GetTick();

#ifdef PWM_DEBUG
		g_counter = __HAL_TIM_GET_COUNTER(&htim4);
#endif
		if(mpu6050_read_accelerometer_data(&hi2c, &acceleration_datas)!= MPU6050_OK){
			Error_Handler();
		}
		mpu6050_calibrate_accelerometer_data(&acceleration_datas, &error_data);
		printmsg("x:%d\r\n",acceleration_datas.x);
		printmsg("y:%d\r\n",acceleration_datas.y);
		printmsg("z:%d\r\n",acceleration_datas.z);

		roll_angle = atan2(acceleration_datas.y,acceleration_datas.z)*(180/M_PI);
		kalman_angle = kalman_filter_get_angle(&kf, roll_angle, dt);
		uint8_t channel= (kalman_angle < 0) ? TIM_CHANNEL_1 : TIM_CHANNEL_2;
		kalman_angle = (kalman_angle < 0) ? -kalman_angle : kalman_angle;
		uint32_t pwm_pulse = map(kalman_angle, ANGLE_POS_MIN, ANGLE_POS_MAX, PWM_PULSE_MIN, PWM_PULSE_MAX);
		change_pwm_duty_cycle(pwm_pulse, channel);

		//HAL_Delay(500);
	}


	return 0;
}


void I2C1_init(void){


	hi2c.Instance = I2C1;
	hi2c.Init.OwnAddress1 = 0;
	hi2c.Init.OwnAddress2 = 0;
	hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c.Init.ClockSpeed=400000;
	hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	if(HAL_I2C_Init(&hi2c) != HAL_OK){
		Error_Handler();
	}



}

void Timer4_init(void){

	htim4.Instance = TIM4;
	htim4.Init.Period = 1000-1;
	htim4.Init.Prescaler = 50000-1;

	if(HAL_TIM_PWM_Init(&htim4) != HAL_OK){
		Error_Handler();
	}

	TIM_OC_InitTypeDef tim4PWM;

	memset(&tim4PWM,0,sizeof(tim4PWM));
	tim4PWM.Pulse = ((htim4.Init.Period+1) * 0)/10;//%0duty cycle
	tim4PWM.OCPolarity = TIM_OCPOLARITY_HIGH;
	tim4PWM.OCMode = TIM_OCMODE_PWM1;
	if(HAL_TIM_PWM_ConfigChannel(&htim4, &tim4PWM, TIM_CHANNEL_1)!= HAL_OK){
		Error_Handler();
	}

	tim4PWM.Pulse = ((htim4.Init.Period+1) * 0)/10;//%0duty cycle
	if(HAL_TIM_PWM_ConfigChannel(&htim4, &tim4PWM, TIM_CHANNEL_2)!= HAL_OK){
		Error_Handler();
	}


}

void UART2_init(void){

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength =UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	if (HAL_UART_Init(&huart2) != HAL_OK){
		//problem
		Error_Handler();
	}

}

void SystemClockConfig_HSE (uint8_t clock_freq){

	RCC_OscInitTypeDef  osc_init = {0};
	RCC_ClkInitTypeDef clk_init = {0};

	osc_init.OscillatorType=RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState=RCC_HSE_BYPASS;
	osc_init.PLL.PLLState=RCC_PLL_ON;
	osc_init.PLL.PLLSource=RCC_PLLSOURCE_HSE;

	clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK ;

	uint32_t latency=0;


	switch(clock_freq){
		case SYS_CLK_FREQ_50:
			osc_init.PLL.PLLM=8;
			osc_init.PLL.PLLN=100;
			osc_init.PLL.PLLP=RCC_PLLP_DIV2;
			osc_init.PLL.PLLQ = 2;

			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV1;
			latency = FLASH_ACR_LATENCY_1WS;
			break;
		case SYS_CLK_FREQ_84:
			osc_init.PLL.PLLM=8;
			osc_init.PLL.PLLN=168;
			osc_init.PLL.PLLP=RCC_PLLP_DIV2;
			osc_init.PLL.PLLQ = 2;


			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV1;
			latency = FLASH_ACR_LATENCY_2WS;
			break;
		case SYS_CLK_FREQ_120:
			osc_init.PLL.PLLM=8;
			osc_init.PLL.PLLN=240;
			osc_init.PLL.PLLP=RCC_PLLP_DIV2;
			osc_init.PLL.PLLQ = 2;

			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
			latency = FLASH_ACR_LATENCY_3WS;
			break;
		case SYS_CLK_FREQ_168:
			//enable the clock for the power controller
			__HAL_RCC_PWR_CLK_ENABLE();

			//set regulator voltage scale as 1
			__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

			osc_init.PLL.PLLM=8;
			osc_init.PLL.PLLN=336;
			osc_init.PLL.PLLP=RCC_PLLP_DIV2;
			osc_init.PLL.PLLQ = 2;

			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
			latency = FLASH_ACR_LATENCY_5WS;
			break;
		default:
			break;

	}

	if (HAL_RCC_OscConfig(&osc_init) != HAL_OK) {
	        Error_Handler();
	}
	if (HAL_RCC_ClockConfig(&clk_init, latency) != HAL_OK) {
	        Error_Handler();
	}

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	__HAL_RCC_HSI_DISABLE();

}

//timer period end interrupt
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	g_counter = 0;
	g_channel_1_state = 800;
	g_channel_2_state = 400;

}

//counter reach the pwm value interrupt
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{

	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	g_channel_1_state = 0;

	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	g_channel_2_state = 0;
}


void Error_Handler(void){
	while(1);
}

void printmsg(char *format,...)
 {

	char str[80];

	/*Extract the the argument list using VA apis */
	va_list args;
	va_start(args, format);
	vsprintf(str, format,args);
	HAL_UART_Transmit(&huart2,(uint8_t *)str, strlen(str),HAL_MAX_DELAY);
	va_end(args);

 }
