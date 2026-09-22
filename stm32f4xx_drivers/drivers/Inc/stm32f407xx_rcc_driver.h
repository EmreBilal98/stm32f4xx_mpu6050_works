/*
 * stm32f407xx_rcc_driver.h
 *
 *  Created on: Feb 25, 2026
 *      Author: emreb
 */

#ifndef INC_STM32F407XX_RCC_DRIVER_H_
#define INC_STM32F407XX_RCC_DRIVER_H_

#include "stm32f407xx.h"

//pll configuration
typedef struct {
    uint32_t PLL_M;
    uint32_t PLL_N;
    uint32_t PLL_P;
    uint32_t PLL_Q;
} RCC_PLL_Config_t;

//APB1 clock
uint32_t RCC_GetPCLK1Value(void);

//APB2 clock
uint32_t RCC_GetPCLK2Value(void);

//AHB1 clock
uint32_t RCC_GetHCLKValue(void) ;

//PLL settings which is using HSE
void SystemClock_Config_HSE(RCC_PLL_Config_t *pll_config);


#endif /* INC_STM32F407XX_RCC_DRIVER_H_ */
