/*
 * stm32f407xx_timers_driver.c
 *
 *  Created on: Aug 18, 2026
 *      Author: emreb
 */

#include "stm32f407xx_timers_driver.h"
#include "stm32f407xx_rcc_driver.h"


/*************************************************************************************************************
* @fn 										-TIM_PeripheralClockControl
* @brief									-This function enables or disables peripheral clock for the given TIM port
*
* @param[in]								-base address of the TIMx peripheral
* @param[in]								-ENABLE or DISABLE macros
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void TIM_PeripheralClockControl(TIM_RegDef_t *pTIMx,uint8_t EnorDi){

	if(EnorDi == ENABLE){
		if(pTIMx == TIM1)
			TIM1_PCLK_EN();
		else if(pTIMx == TIM2)
			TIM2_PCLK_EN();
		else if(pTIMx == TIM3)
			TIM3_PCLK_EN();
		else if(pTIMx == TIM4)
			TIM4_PCLK_EN();
		else if(pTIMx == TIM5)
			TIM5_PCLK_EN();
		else if(pTIMx == TIM6)
			TIM6_PCLK_EN();
		else if(pTIMx == TIM7)
			TIM7_PCLK_EN();
		else if(pTIMx == TIM8)
			TIM8_PCLK_EN();
		else if(pTIMx == TIM9)
			TIM9_PCLK_EN();
		else if(pTIMx == TIM10)
			TIM10_PCLK_EN();
		else if(pTIMx == TIM11)
			TIM11_PCLK_EN();
		else if(pTIMx == TIM12)
			TIM12_PCLK_EN();
		else if(pTIMx == TIM13)
			TIM13_PCLK_EN();
		else if(pTIMx == TIM14)
			TIM14_PCLK_EN();

	}
	else{
		if(pTIMx == TIM1)
			TIM1_PCLK_DI();
		else if(pTIMx == TIM2)
			TIM2_PCLK_DI();
		else if(pTIMx == TIM3)
			TIM3_PCLK_DI();
		else if(pTIMx == TIM4)
			TIM4_PCLK_DI();
		else if(pTIMx == TIM5)
			TIM5_PCLK_DI();
		else if(pTIMx == TIM6)
			TIM6_PCLK_DI();
		else if(pTIMx == TIM7)
			TIM7_PCLK_DI();
		else if(pTIMx == TIM8)
			TIM8_PCLK_DI();
		else if(pTIMx == TIM9)
			TIM9_PCLK_DI();
		else if(pTIMx == TIM10)
			TIM10_PCLK_DI();
		else if(pTIMx == TIM11)
			TIM11_PCLK_DI();
		else if(pTIMx == TIM12)
			TIM12_PCLK_DI();
		else if(pTIMx == TIM13)
			TIM13_PCLK_DI();
		else if(pTIMx == TIM14)
			TIM14_PCLK_DI();
	}

}

/*************************************************************************************************************
* @fn 										-TIM_PeripheralControl
* @brief									-This function enables or disables peripheral for the given TIM port
*
* @param[in]								-base address of the TIMx peripheral
* @param[in]								-ENABLE or DISABLE macros
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void TIM_PeripheralControl(TIM_RegDef_t *pTIMx,uint8_t EnorDi){

	if(EnorDi == ENABLE){
		pTIMx->CR1 |= (1<<TIM_CR1_CEN);
	}
	else{
		pTIMx->CR1 &= ~(1<<TIM_CR1_CEN);
	}

}


/*************************************************************************************************************
* @fn 										-TIM_Init
* @brief			-This function make the init configuration of selected TIM peripheral
*
* @param[in]		-Hold the base address and values of the registers of TIM peripheral
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void TIM_Init(TIM_Handle_t *pTIMHandle) {
    // 1. İlgili Timer'ın saat hattını aktif et
    TIM_PeripheralClockControl(pTIMHandle->pTIMx, ENABLE);

    // 2. Timer'ı durdur ve CR1'i varsayılan  duruma getir
    pTIMHandle->pTIMx->CR1 = 0;

    // 3. ARR (Period) ve PSC (Prescaler) değerlerini yaz
    pTIMHandle->pTIMx->ARR = pTIMHandle->TIMConfig.period;
    pTIMHandle->pTIMx->PSC = pTIMHandle->TIMConfig.prescaler;

    // 4. Update Generation (UG) bitini set et
    // Bu işlem, PSC ve ARR değerlerinin shadow register'lardan
    // anında aktif register'lara aktarılmasını sağlar.
    pTIMHandle->pTIMx->EGR |= (1 << 0); // TIM_EGR_UG biti (Bit 0)
}



void TIM_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi) {
    uint8_t tempReg = IRQNumber / 32;
    uint8_t tempBit = IRQNumber % 32;

    if (EnorDi == ENABLE) {
        *(NVIC_ISER + tempReg) |= (1 << tempBit);
    } else if (EnorDi == DISABLE) {
        *(NVIC_ICER + tempReg) |= (1 << tempBit);
    }
}

void TIM_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority) {
    uint8_t tempReg = IRQNumber / 4;
    uint8_t tempBit = IRQNumber % 4;

    // NO_PR_BITS_IMPLEMENTED genellikle STM32'de 4'tür (Alt 4 bit kullanılmaz)
    *(NVIC_IPR + tempReg) |= (IRQPriority << ((tempBit * 8) + NO_PR_BITS_IMPLEMENTED));
}


/*************************************************************************************************************
* @fn 										-TIM_PWM_ConfigChannel
* @brief			-This function make the pwm speciality enable and configure the features
*
* @param[in]		-Hold the base address and values of the registers of TIM peripheral
* @param[in]		-Hold the pwm features
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void TIM_PWM_ConfigChannel(TIM_Handle_t *pTIMHandle, Timer_PWM_Config_t *pPWMConfig) {
    uint8_t ch = pPWMConfig->Channel;

    // 1. Mod (PWM1/PWM2) ve Preload (Ön Yükleme) Ayarları (CCMRx)
    if (ch == TIM_CHANNEL_1) {
        // İlgili 8 biti temizle (Reset state)
        pTIMHandle->pTIMx->CCMR1 &= ~(0xFF << 0);
        // OC1M (Bit 4, 5, 6): PWM Modunu yaz
        pTIMHandle->pTIMx->CCMR1 |= (pPWMConfig->Mode << 4);
        // OC1PE (Bit 3): Preload (Ön Yükleme) aktif et
        pTIMHandle->pTIMx->CCMR1 |= (1 << 3);
        // Başlangıç Duty Cycle değerini doğrudan CCR1'e yaz
        pTIMHandle->pTIMx->CCR1 = pPWMConfig->Pulse;
    }
    else if (ch == TIM_CHANNEL_2) {
        pTIMHandle->pTIMx->CCMR1 &= ~(0xFF << 8);
        // OC2M (Bit 12, 13, 14): PWM Modunu yaz
        pTIMHandle->pTIMx->CCMR1 |= (pPWMConfig->Mode << 12);
        // OC2PE (Bit 11): Preload (Ön Yükleme) aktif et
        pTIMHandle->pTIMx->CCMR1 |= (1 << 11);
        pTIMHandle->pTIMx->CCR2 = pPWMConfig->Pulse;
    }
    else if (ch == TIM_CHANNEL_3) {
        pTIMHandle->pTIMx->CCMR2 &= ~(0xFF << 0);
        // OC3M (Bit 4, 5, 6)
        pTIMHandle->pTIMx->CCMR2 |= (pPWMConfig->Mode << 4);
        // OC3PE (Bit 3)
        pTIMHandle->pTIMx->CCMR2 |= (1 << 3);
        pTIMHandle->pTIMx->CCR3 = pPWMConfig->Pulse;
    }
    else if (ch == TIM_CHANNEL_4) {
        pTIMHandle->pTIMx->CCMR2 &= ~(0xFF << 8);
        // OC4M (Bit 12, 13, 14)
        pTIMHandle->pTIMx->CCMR2 |= (pPWMConfig->Mode << 12);
        // OC4PE (Bit 11)
        pTIMHandle->pTIMx->CCMR2 |= (1 << 11);
        pTIMHandle->pTIMx->CCR4 = pPWMConfig->Pulse;
    }

    // Çıkış Polaritesi Ayarı (CCER Register)
    // CCER yazmacında her kanal 4 bitlik yer kaplar.
    // Polarite biti (CCxP) her bloğun 1. bitine denk gelir (1, 5, 9, 13. bitler)
    uint8_t shift_val = (ch * 4) + 1;

    // Önce ilgili polarite bitini temizle, sonra struct'tan gelen değeri yaz
    pTIMHandle->pTIMx->CCER &= ~(1 << shift_val);
    pTIMHandle->pTIMx->CCER |= (pPWMConfig->Polarity << shift_val);
}

void TIM_InterruptControl(TIM_Handle_t *pTIMHandle, uint8_t Event, uint8_t EnorDi) {

    if (EnorDi == ENABLE) {
        pTIMHandle->pTIMx->DIER |= (1 << Event);
    } else {
        pTIMHandle->pTIMx->DIER &= ~(1 << Event);
    }
}


void TIM_IRQHandling(TIM_Handle_t *pTIMHandle) {

    // UIF (Update Interrupt Flag) biti SR'nin 0. bitidir. UIE biti de DIER'in 0. bitidir.
    uint32_t sr = pTIMHandle->pTIMx->SR;
    uint32_t dier = pTIMHandle->pTIMx->DIER;

    //timer done period callback
    if ((sr & (1 << 0)) && (dier & (1 << 0))) {
        // clear flag
        pTIMHandle->pTIMx->SR &= ~(1 << 0);

        //send user the information
        TIM_ApplicationCallback(pTIMHandle, TIM_EVENT_UPDATE);
    }

    //pwm ch1 reach the duty
    if ((sr & (1 << 1)) && (dier & (1 << 1))) {
        // clear flag (Clear CC1IF)
        pTIMHandle->pTIMx->SR &= ~(1 << 1);
        TIM_ApplicationCallback(pTIMHandle, TIM_EVENT_CC1);
    }

    //pwm ch2 reach the duty
    if ((sr & (1 << 2)) && (dier & (1 << 2))) {
        //clear flag(Clear CC2IF)
        pTIMHandle->pTIMx->SR &= ~(1 << 2);
        TIM_ApplicationCallback(pTIMHandle, TIM_EVENT_CC2);
    }

    //pwm ch3 reach the duty
    if ((sr & (1 << 3)) && (dier & (1 << 3))) {
        //clear flag(Clear CC3IF)
        pTIMHandle->pTIMx->SR &= ~(1 << 3);
        TIM_ApplicationCallback(pTIMHandle, TIM_EVENT_CC3);
    }

    //pwm ch4 reach the duty
    if ((sr & (1 << 4)) && (dier & (1 << 4))) {
        //clear flag(Clear CC4IF)
        pTIMHandle->pTIMx->SR &= ~(1 << 4);
        TIM_ApplicationCallback(pTIMHandle, TIM_EVENT_CC4);
    }

}

__attribute__((weak)) void TIM_ApplicationCallback(TIM_Handle_t *pTIMHandle, uint8_t AppEv) {

}
