/*
 * stm32f407xx_timers_driver.h
 *
 *  Created on: Aug 18, 2026
 *      Author: emreb
 */

#ifndef INC_STM32F407XX_TIMERS_DRIVER_H_
#define INC_STM32F407XX_TIMERS_DRIVER_H_

#include "stm32f407xx.h"

typedef struct {
    uint8_t  Channel;  /*!< possible values from @TIM_CHANNEL_VALUES>*/
    uint32_t Pulse;     /* This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF.  */
    uint8_t  Mode;       /*!< possible values from @TIM_MODE_VALUES>*/
    uint8_t  Polarity;   /*!< possible values from @TIM_POLARITY_VALUES>*/
} Timer_PWM_Config_t;

typedef struct{
	uint32_t prescaler;	/* This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF.  */
	uint32_t period;  /* This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF.  */

}TIM_Config_t;


typedef struct{
	TIM_RegDef_t *pTIMx;
	TIM_Config_t TIMConfig;

}TIM_Handle_t;

/*
 *	@TIM_CHANNEL_VALUES
 *	select channel
*/
#define TIM_CHANNEL_1	0
#define TIM_CHANNEL_2	1
#define TIM_CHANNEL_3	2
#define TIM_CHANNEL_4	3

/*
 *	@TIM_MODE_VALUES
 *	select mode
*/
#define TIM_OCMODE_PWM1                     6
#define TIM_OCMODE_PWM2                     7

/*
 *	@TIM_POLARITY_VALUES
 *	select polarity
*/
#define TIM_OCPOLARITY_HIGH                0
#define TIM_OCPOLARITY_LOW                 1

/* TIM Application Events */
#define TIM_EVENT_UPDATE        0  // timer period done
#define TIM_EVENT_CC1           1  // PWM ch1 reach the duty
#define TIM_EVENT_CC2           2  // PWM ch2 reach the duty
#define TIM_EVENT_CC3           3  // PWM ch3 reach the duty
#define TIM_EVENT_CC4           4  // PWM ch4 reach the duty


/************************************************************************************************************************************
 	 	 	 	 	 MCU specific peripheral register data bitfield macros
 ***********************************************************************************************************************************/

/*
 * Bit position definitions of TIMx_CR1
 */
#define TIM_CR1_CEN		0
#define TIM_CR1_UDIS	1
#define TIM_CR1_URS		2
#define TIM_CR1_OPM		3
#define TIM_CR1_DIR		4
#define TIM_CR1_CMS		5
#define TIM_CR1_ARPE	7
#define TIM_CR1_CKD		8



/********************************************************************************************************************
 	 	 	 	 	 	 	 	 	 * APIS supported by this driver
 	 	 	 	 	 	 * For more information about the APIS check the function definitions
********************************************************************************************************************/

/*
 * peripheral clock control
 */
void TIM_PeripheralClockControl(TIM_RegDef_t *pTIMx,uint8_t EnorDi);
void TIM_PeripheralControl(TIM_RegDef_t *pTIMx,uint8_t EnorDi);

/*
 * Init's
 */
void TIM_Init(TIM_Handle_t *pTIMHandle);
void TIM_PWM_ConfigChannel(TIM_Handle_t *pTIMHandle, Timer_PWM_Config_t *pPWMConfig);

/*
 * IRQ configuration and ISR handling
 */
void TIM_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void TIM_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void TIM_IRQHandling(TIM_Handle_t *pTIMHandle);
void TIM_InterruptControl(TIM_Handle_t *pTIMHandle, uint8_t Event, uint8_t EnorDi);

/*Application Callback*/
void TIM_ApplicationCallback(TIM_Handle_t *pTIMHandle, uint8_t AppEv);
#endif /* INC_STM32F407XX_TIMERS_DRIVER_H_ */
