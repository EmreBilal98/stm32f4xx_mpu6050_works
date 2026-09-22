/*
 * stm32f407xx_gpio_driver.c
 *
 *  Created on: Jan 22, 2026
 *      Author: emreb
 */

#include "stdint.h"
#include "stm32f407xx_gpio_driver.h"

/*************************************************************************************************************
* @fn 										-GPIO_PeripheralClockControl
* @brief									-This function enables or disables peripheral clock for the given GPIO port
*
* @param[in]								-base address of the gpıo peripheral
* @param[in]								-ENABLE or DISABLE macros
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void GPIO_PeripheralClockControl(GPIO_RegDef_t *pGPIOx,uint8_t EnorDi){

	if(EnorDi == ENABLE){
		if(pGPIOx == GPIOA)
			GPIOA_PCLK_EN();
		else if(pGPIOx == GPIOB)
			GPIOB_PCLK_EN();
		else if(pGPIOx == GPIOC)
			GPIOC_PCLK_EN();
		else if(pGPIOx == GPIOD)
			GPIOD_PCLK_EN();
		else if(pGPIOx == GPIOE)
			GPIOE_PCLK_EN();
		else if(pGPIOx == GPIOF)
			GPIOF_PCLK_EN();
		else if(pGPIOx == GPIOG)
			GPIOG_PCLK_EN();
		else if(pGPIOx == GPIOH)
			GPIOH_PCLK_EN();
		else if(pGPIOx == GPIOI)
			GPIOI_PCLK_EN();
	}
	else{
		if(pGPIOx == GPIOA)
			GPIOA_PCLK_DI();
		else if(pGPIOx == GPIOB)
			GPIOB_PCLK_DI();
		else if(pGPIOx == GPIOC)
			GPIOC_PCLK_DI();
		else if(pGPIOx == GPIOD)
			GPIOD_PCLK_DI();
		else if(pGPIOx == GPIOE)
			GPIOE_PCLK_DI();
		else if(pGPIOx == GPIOF)
			GPIOF_PCLK_DI();
		else if(pGPIOx == GPIOG)
			GPIOG_PCLK_DI();
		else if(pGPIOx == GPIOH)
			GPIOH_PCLK_DI();
		else if(pGPIOx == GPIOI)
			GPIOI_PCLK_DI();
	}


}


/*************************************************************************************************************
* @fn 										-GPIO_Init
* @brief			-This function make the init configuration of selected gpio peripheral
*
* @param[in]		-Hold the base address and values of the registers
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void GPIO_Init(GPIO_Handle_t *pGPIOHandle){

	//periperal clock enable
	GPIO_PeripheralClockControl(pGPIOHandle->pGPIOx, ENABLE);

	uint32_t temp=0;
	//1 configure the mode of gpio pin
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG){
		//the none interrupt modes
		temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));//multiplied by 2 for holding in two bit fields
		pGPIOHandle->pGPIOx->MODER &= ~(0x03 << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );//clearing
		pGPIOHandle->pGPIOx->MODER |= temp;//setting
	}else{
	//the interrupt modes
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT){

			//1.configure the FTSR and Clear the corresponding FTSR
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT){

			//1.configure the RTSR and Clear the corresponding FTSR
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT){

			//1.configure the FTSR and RTSR
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}

		//2. configure the GPIO port selection in SYSCFG_EXTICR
		SYSCFG_PCLK_EN();

		uint8_t tempBitExti=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber%4;
		uint8_t tempRegExti=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/4;
		uint8_t tempPortExti=GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);

		SYSCFG->EXTICR[tempRegExti] &= ~(0x0F << 4*tempBitExti);//clearing
		SYSCFG->EXTICR[tempRegExti] |= (tempPortExti << 4*tempBitExti);//setting



		//3. enable the exti interrupt delivery using IMR
		EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);



	}

	//2. configure the speed
	temp=0;
	temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));//multiplied by 2 for holding in two bit fields
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x03 << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );//clearing
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;//setting

	//3. configure the pupd settings
	temp=0;
	temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));//multiplied by 2 for holding in two bit fields
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x03 << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );//clearing
	pGPIOHandle->pGPIOx->PUPDR |= temp;//setting

	//4. configure the optype
	temp=0;
	temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));//multiplied by 1 for holding in one bit field
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x01 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber );//clearing
	pGPIOHandle->pGPIOx->OTYPER |= temp;//setting

	//5. configure the alt functionality
	uint8_t tempReg=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/8;//pinNumber divided by 8 because each reg hold 8 pin's value
	uint8_t tempBitBase=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber%8;//get pinNumber mode 8 because each reg hold 8 pin's value
	pGPIOHandle->pGPIOx->AFR[tempReg] &= ~(0x0F << (4*tempBitBase) );//clearing
	pGPIOHandle->pGPIOx->AFR[tempReg] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << 4*tempBitBase); //multiplied by 4 for holding in four bit fields
}

/*************************************************************************************************************
* @fn 										-GPIO_DeInit
* @brief			-this function reset the GPIOx peripheral
*
* @param[in]		-Hold peripheral base addresses
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx){
	if(pGPIOx == GPIOA)
				GPIOA_REG_RESET();
			else if(pGPIOx == GPIOB)
				GPIOB_REG_RESET();
			else if(pGPIOx == GPIOC)
				GPIOC_REG_RESET();
			else if(pGPIOx == GPIOD)
				GPIOD_REG_RESET();
			else if(pGPIOx == GPIOE)
				GPIOE_REG_RESET();
			else if(pGPIOx == GPIOF)
				GPIOF_REG_RESET();
			else if(pGPIOx == GPIOG)
				GPIOG_REG_RESET();
			else if(pGPIOx == GPIOH)
				GPIOH_REG_RESET();
			else if(pGPIOx == GPIOI)
				GPIOI_REG_RESET();

}

/*************************************************************************************************************
* @fn 										-GPIO_ReadFromInputPin
* @brief			-this function read GPIOx peripheral specific pin value
*
* @param[in]		-hold GPIOx peripheral adresses
* @param[in]		-reading pin number
*
* @return			-data at this pin number
* @Note				-none
***************************************************************************************************************/
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber){
	uint8_t value;
	value=(uint8_t)((pGPIOx->IDR  >> PinNumber) & 0x00000001);
	return value;
}

/*************************************************************************************************************
* @fn 										-GPIO_ReadFromInputPort
* @brief			-this function read GPIOx peripheral whole value
*
* @param[in]		-hold GPIOx peripheral adresses
*
* @return			-whole data at the port
* @Note				-none
***************************************************************************************************************/
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx){
	uint16_t value;
	value=(uint16_t)pGPIOx->IDR;
	return value;
}

/*************************************************************************************************************
* @fn 										-GPIO_WriteToOutputPin
* @brief			-this function write Value  to GPIOx peripheral specific pin
*
* @param[in]		-hold GPIOx peripheral adresses
* @param[in]		-reading pin number
* @param[in]		-written data
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber,uint8_t Value){
	if(Value == SET){
		pGPIOx->ODR |= (1 << PinNumber);
	}else{
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
}

/*************************************************************************************************************
* @fn 										-GPIO_WriteToOutputPort
* @brief			-this function write Value  to GPIOx peripheral whole port
*
* @param[in]		-hold GPIOx peripheral adresses
* @param[in]		-written data
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx,uint16_t Value){
	pGPIOx->ODR=Value;
}


/*************************************************************************************************************
* @fn 										-GPIO_ToggleOutputPin
* @brief			-this function toogle GPIOx peripheral specific pin value
*
* @param[in]		-hold GPIOx peripheral adresses
* @param[in]		-reading pin number
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber){
	pGPIOx->ODR ^= (1<< PinNumber);
}

/*************************************************************************************************************
* @fn 										-GPIO_IRQConfig
* @brief				-the peripheral interrupt is enabled or disabled at processor level with irq number
*
* @param[in]			-the GPIOx pin irq number
* @param[in]			-ENABLE or DISABLE macros
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void GPIO_IRQConfig(uint8_t IRQNumber,uint8_t EnorDi){

	uint8_t tempReg=IRQNumber/32;
	uint8_t tempBit=IRQNumber%32;
	if(EnorDi == ENABLE){
		*(NVIC_ISER+tempReg) |= (1 << tempBit);
	}else if(EnorDi == DISABLE){
		*(NVIC_ICER+tempReg) |= (1 << tempBit);
	}

}

/*************************************************************************************************************
* @fn 										-GPIO_IRQPriorityConfig
* @brief				-the peripheral interrupt priority is setting with this function
*
* @param[in]			-the GPIOx pin irq number
* @param[in]			-Hold the priority level
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void GPIO_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority){

	uint8_t tempReg=IRQNumber/4;
	uint8_t tempBit=IRQNumber%4;

	*(NVIC_IPR+tempReg) |= (IRQPriority << ((tempBit*8)+NO_PR_BITS_IMPLEMENTED));


}

/*************************************************************************************************************
* @fn 										-GPIO_IRQHandling
* @brief				-the function clear the pending bit of GPIOx peripheral
*
* @param[in]			-the GPIOx pin number
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void GPIO_IRQHandling(uint8_t PinNumber){

	if(EXTI->PR & (1<<PinNumber)){
		EXTI->PR |= (1<<PinNumber);//pending register's bits is cleared by programming it to ‘1’.

	}

}
