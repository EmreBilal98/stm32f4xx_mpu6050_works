/*
 * stm32f407xx_rcc_driver.c
 *
 *  Created on: Feb 25, 2026
 *      Author: emreb
 */

#include "stm32f407xx_rcc_driver.h"

/*************************************************************************************************************
* @fn 										-RCC_GetPCLK1Value
* @brief									-This function find PLL clock frequency
*
* @return									-PLL clock frequency
* @Note										-none
***************************************************************************************************************/
uint32_t RCC_GetPLLOutputClock(void){
	uint32_t vco_input, vco_output, pll_output;
	    uint32_t pllsource, pllm, plln, pllp;

	    // 1. PLL Kaynağını bul (PLLCFGR Bit 22)
	    // 0: HSI, 1: HSE
	    if ((RCC->PLLCFGR >> 22) & 1) {
	        pllsource = 8000000; // HSE değeri (Discovery kartındaki harici kristal genelde 8 MHz'dir)
	    } else {
	        pllsource = 16000000; // HSI değeri (Dahili osilatör 16 MHz'dir)
	    }

	    // 2. PLL_M değerini oku (Bit 5:0)
	    pllm = RCC->PLLCFGR & 0x3F;

	    // 3. PLL_N değerini oku (Bit 14:6)
	    plln = (RCC->PLLCFGR >> 6) & 0x1FF;

	    // 4. PLL_P değerini oku (Bit 17:16)
	    // Referans kılavuzuna göre: 00 -> 2, 01 -> 4, 10 -> 6, 11 -> 8
	    pllp = (((RCC->PLLCFGR >> 16) & 0x3) + 1) * 2;

	    // 5. Formülü uygula
	    vco_input = pllsource / pllm;
	    vco_output = vco_input * plln;
	    pll_output = vco_output / pllp;

	    return pll_output;
}
/*************************************************************************************************************
* @fn 										-RCC_GetPCLK1Value
* @brief									-This function find APB1 clock frequency
*
* @return									-APB1 clock frequency
* @Note										-none
***************************************************************************************************************/
uint32_t RCC_GetPCLK1Value(void){

	uint32_t pclk1,SystemClk;

	uint8_t clksrc,AHBprescaler,AHBprescalerValue,APBprescaler,APBprescalerValue;

	clksrc=((RCC->CFGR >> 2) & 0x3);

	if(clksrc == 0){
		//HSI
		SystemClk=16000000;

	}
	if(clksrc==1){
		//HSE
		SystemClk=8000000;
	}
	if(clksrc==2){
		//PLL
		SystemClk=RCC_GetPLLOutputClock();
	}

	//AHB
	AHBprescaler=(RCC->CFGR >> 4) & 0xF;

	if(AHBprescaler < 8){
		AHBprescalerValue = 1;
	}
	else if(AHBprescaler >= 8){
		AHBprescalerValue=(1<<(AHBprescaler-7));
	}


	//APB
	APBprescaler=(RCC->CFGR >> 10) & 0x7;

	if(APBprescaler < 4){
		APBprescalerValue = 1;
	}
	else if(APBprescaler >= 4){
		APBprescalerValue=(1<<(APBprescaler-3));
	}


	pclk1=((SystemClk/AHBprescalerValue)/APBprescalerValue);

	return pclk1;

}
/*************************************************************************************************************
* @fn 										-RCC_GetHCLKValue
* @brief									-This function find AHB1 clock frequency
*
* @return									-AHB1 clock frequency
* @Note										-none
***************************************************************************************************************/
uint32_t RCC_GetHCLKValue(void) {

	uint32_t hclk1,SystemClk;

	uint8_t clksrc,AHBprescaler,AHBprescalerValue;

	clksrc=((RCC->CFGR >> 2) & 0x3);

	if(clksrc == 0){
		//HSI
		SystemClk=16000000;

	}
	if(clksrc==1){
		//HSE
		SystemClk=8000000;
	}
	if(clksrc==2){
		//PLL
		SystemClk=RCC_GetPLLOutputClock();
	}

	//AHB
	AHBprescaler=(RCC->CFGR >> 4) & 0xF;

	if(AHBprescaler < 8){
		AHBprescalerValue = 1;
	}
	else if(AHBprescaler >= 8){
		AHBprescalerValue=(1<<(AHBprescaler-7));
	}


	hclk1=(SystemClk/AHBprescalerValue);

	return hclk1;

}
/*************************************************************************************************************
* @fn 										-RCC_GetPCLK1Value
* @brief									-This function find APB1 clock frequency
*
* @return									-APB1 clock frequency
* @Note										-none
***************************************************************************************************************/
uint32_t RCC_GetPCLK2Value(void){

	uint32_t pclk2,SystemClk;

	uint8_t clksrc,AHBprescaler,AHBprescalerValue,APBprescaler,APBprescalerValue;

	clksrc=((RCC->CFGR >> 2) & 0x3);

	if(clksrc == 0){
		//HSI
		SystemClk=16000000;

	}
	if(clksrc==1){
		//HSE
		SystemClk=8000000;
	}
	if(clksrc==2){
		//PLL
		SystemClk=RCC_GetPLLOutputClock();
	}

	//AHB
	AHBprescaler=(RCC->CFGR >> 4) & 0xF;

	if(AHBprescaler < 8){
		AHBprescalerValue = 1;
	}
	else if(AHBprescaler >= 8){
		AHBprescalerValue=(1<<(AHBprescaler-7));
	}


	//APB
	APBprescaler=(RCC->CFGR >> 13) & 0x7;

	if(APBprescaler < 4){
		APBprescalerValue = 1;
	}
	else if(APBprescaler >= 4){
		APBprescalerValue=(1<<(APBprescaler-3));
	}


	pclk2=((SystemClk/AHBprescalerValue)/APBprescalerValue);

	return pclk2;

}

void SystemClock_Config_HSE(RCC_PLL_Config_t *pll_config) {
    //  ENABLE HSE
    RCC->CR |= (1 << 16);
    while (!(RCC->CR & (1 << 17)));

    RCC->APB1ENR |= (1 << 28);

    uint8_t cr_offset= 0x00;
    volatile uint32_t *PWR_CR_reg = (volatile uint32_t *)(PWR_BASEADDR+cr_offset);
    *PWR_CR_reg |= (3 << 14);
    //PWR->CR |= (3 << 14);

    // 3. Flash Bellek Gecikmesi (Latency) Ayarı
    // İşlemci 50 MHz'e çıkacağı için bellek okuma hızı işlemciye yetişemez.
    // 3.3V beslemede 50 MHz için 1 Wait State (WS) gereklidir.


    uint8_t flash_offset= 0x00;
    volatile uint32_t *FLASH_CR_reg = (volatile uint32_t *)(FLSH_BASEADDR+flash_offset);

    *FLASH_CR_reg = (1 << 0)  | // LATENCY = 1 WS
                 (1 << 8)  | // PRFTEN (Prefetch Enable)
                 (1 << 9)  | // ICEN (Instruction Cache Enable)
                 (1 << 10);  // DCEN (Data Cache Enable)

    RCC->CFGR &= ~(0xF << 4);  // AHB Prescaler = /1
    RCC->CFGR |=  (4 << 10);   // APB1 Prescaler = /2
    RCC->CFGR &= ~(0x7 << 13); // APB2 Prescaler = /1

    uint32_t pllp_reg = (pll_config->PLL_P / 2) - 1;

    RCC->PLLCFGR = (pll_config->PLL_M << 0)   |  // PLLM
                   (pll_config->PLL_N << 6)   |  // PLLN
                   (pllp_reg << 16)           |  // PLLP
                   (pll_config->PLL_Q << 24)  |  // PLLQ
                   (1 << 22);                    // select HSE

    // start pll
    RCC->CR |= (1 << 24);
    while (!(RCC->CR & (1 << 25)));

    // choose pll as clock
    RCC->CFGR &= ~(3 << 0);
    RCC->CFGR |= (2 << 0);

    // control that system work on pll
    while ((RCC->CFGR & (3 << 2)) != (2 << 2));
}
