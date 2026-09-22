/*
 * it.c
 *
 *  Created on: Jul 28, 2026
 *      Author: emreb
 */

#include "main.h"

extern TIM_HandleTypeDef htim4;;

void SysTick_Handler(void){

	HAL_IncTick();

	HAL_SYSTICK_IRQHandler();

}

void TIM4_IRQHandler(void){

	HAL_TIM_IRQHandler(&htim4);
}
