/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: Jan 26, 2026
 *      Author: emreb
 */
#include "stdint.h"
#include "stm32f407xx_spi_driver.h"

/*************************************************************************************************************
* @fn 										-SPI_PeripheralClockControl
* @brief									-This function enables or disables peripheral clock for the given SPI port
*
* @param[in]								-base address of the SPIx peripheral
* @param[in]								-ENABLE or DISABLE macros
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void SPI_PeripheralClockControl(SPI_RegDef_t *pSPIx,uint8_t EnorDi){

	if(EnorDi == ENABLE){
		if(pSPIx == SPI1)
			SPI1_PCLK_EN();
		else if(pSPIx == SPI2)
			SPI2_PCLK_EN();
		else if(pSPIx == SPI3)
			SPI3_PCLK_EN();
		else if(pSPIx == SPI4)
			SPI4_PCLK_EN();
	}
	else{
		if(pSPIx == SPI1)
			SPI1_PCLK_DI();
		else if(pSPIx == SPI2)
			SPI2_PCLK_DI();
		else if(pSPIx == SPI3)
			SPI3_PCLK_DI();
		else if(pSPIx == SPI4)
			SPI4_PCLK_DI();
	}


}

/*************************************************************************************************************
* @fn 										-SPI_PeripheralControl
* @brief									-This function enables or disables peripheral for the given SPI port
*
* @param[in]								-base address of the SPIx peripheral
* @param[in]								-ENABLE or DISABLE macros
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx,uint8_t EnorDi){

	if(EnorDi == ENABLE){
			pSPIx->CR1 |= (1<<SPI_CR1_SPE);
	}
	else{
		pSPIx->CR1 &= ~(1<<SPI_CR1_SPE);
	}


}


/*************************************************************************************************************
* @fn 										-SPI_Init
* @brief			-This function make the init configuration of selected spi peripheral
*
* @param[in]		-Hold the base address and values of the registers of spi peripheral
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void SPI_Init(SPI_Handle_t *pSPIOHandle){

	//periperal clock enable

	SPI_PeripheralClockControl(pSPIOHandle->pSPIx, ENABLE);

	//first lets configure the SPI_CR1 register

	//set the device mode
	uint32_t tempReg =0;
	tempReg=(pSPIOHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR);

	//set the bus config
	if(pSPIOHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD){
		//bidimode must be cleared
		tempReg &= ~(1 << SPI_CR1_BIDIMODE);

	}
	else if(pSPIOHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD){
		//bidimode must be setted
		tempReg |= (1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIOHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_S_RXONLY){
		//bidimode must be cleared
		tempReg &= ~(1 << SPI_CR1_BIDIMODE);
		//RXONLY must be setted
		tempReg |= (1 << SPI_CR1_RXONLY);
	}

	//Configure the spi serial clock speed
	tempReg |= (pSPIOHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR);

	//Configure the DFF
	tempReg |= (pSPIOHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF);

	//Configure the CPOL
	tempReg |= (pSPIOHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL);

	//Configure the CPHA
	tempReg |= (pSPIOHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA);

	//configure the ssm
	tempReg |= (pSPIOHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM);

	pSPIOHandle->pSPIx->CR1 =tempReg;


}
/*************************************************************************************************************
* @fn 										-SPI_SSOEConfig
* @brief			-this function write the SSI bit of CR1
*
* @param[in]		-Hold peripheral base addresses
* @param[in]		-Enable or disable
*
* @return			-none
* @Note				-This function just used when the SSM bit is reset the slave management is hardware mode.
* The SSOE bit activate the NSS pin.
***************************************************************************************************************/
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx,uint8_t EnOrDi){
	if(EnOrDi == ENABLE)
		pSPIx->CR2 |= (1<<SPI_CR2_SSOE);
	else
		pSPIx->CR2 &= ~(1<<SPI_CR2_SSOE);
}
/*************************************************************************************************************
* @fn 										-SSIConfig
* @brief			-this function write the SSI bit of CR1
*
* @param[in]		-Hold peripheral base addresses
* @param[in]		-Enable or disable
*
* @return			-none
* @Note				-This function just used when the SSM bit is set.The SSI bit force the NSS pin to ignore the IO value
***************************************************************************************************************/
void SPI_SSIConfig(SPI_RegDef_t *pSPIx,uint8_t EnOrDi){
	if(EnOrDi == ENABLE)
		pSPIx->CR1 |= (1<<SPI_CR1_SSI);
	else
		pSPIx->CR1 &= ~(1<<SPI_CR1_SSI);
}
/*************************************************************************************************************
* @fn 										-SPI_DeInit
* @brief			-this function reset the SPIx peripheral
*
* @param[in]		-Hold peripheral base addresses
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void SPI_DeInit(SPI_RegDef_t *pSPIx){
	if(pSPIx == SPI1)
			SPI1_REG_RESET();
		else if(pSPIx == SPI2)
			SPI2_REG_RESET();
		else if(pSPIx == SPI3)
			SPI3_REG_RESET();
		else if(pSPIx == SPI4)
			SPI4_REG_RESET();


}

/*************************************************************************************************************
* @fn 										-SPI_GetFlagStatus
* @brief			-this function control the SPIx peripheral flags
*
* @param[in]		-Hold peripheral base addresses
* @param[in]		-The flag bitfield address
*
* @return			-the status of the flag
* @Note				-none
***************************************************************************************************************/
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx,uint32_t FlagName){
	 return (pSPIx->SR & FlagName);
}
/*************************************************************************************************************
* @fn 										-SPI_SendData
* @brief			-this function send data with SPIx peripheral
*
* @param[in]		-Hold peripheral base addresses
* @param[in]		-Transmit data buffer
* @param[in]		-data length
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void SPI_SendData(SPI_RegDef_t *pSPIx,uint8_t *pTxBuffer,uint32_t Len){

	while(Len>0){

		//wait until TxE is set
		while(!SPI_GetFlagStatus(pSPIx,SPI_TXE_FLAG));

		//check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF)){
			//16 bit data
			//load the data in to the DR
			pSPIx->DR = *((uint16_t *)pTxBuffer);
			Len--;
			Len--;
			(uint16_t *)pTxBuffer++;
		}
		else{
			//8 bit data
			//load the data in to the DR
			pSPIx->DR = *(pTxBuffer);
			Len--;
			pTxBuffer++;
		}
	}

}

/*************************************************************************************************************
* @fn 										-SPI_ReceiveData
* @brief			-this function send data with SPIx peripheral
*
* @param[in]		-Hold peripheral base addresses
* @param[in]		-Receive data buffer
* @param[in]		-data length
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void SPI_ReceiveData(SPI_RegDef_t *pSPIx,uint8_t *pRxBuffer,uint32_t Len){

	while(Len>0){

		while(!SPI_GetFlagStatus(pSPIx,SPI_RXNE_FLAG));

		if((pSPIx->CR1 >> SPI_CR1_DFF) & SPI_DFF_16B){
			//16 bit reading
			*((uint16_t *)pRxBuffer)=pSPIx->DR;
			Len--;
			Len--;
			(uint16_t *)pRxBuffer++;

		}
		else{
			//8 bit reading
			 *(pRxBuffer)=pSPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}

}
/*************************************************************************************************************
* @fn 										-SPI_SendDataIT
* @brief			-this function send data with SPIx peripheral when interrupt triggered
*
* @param[in]		-Hold the base address and values of the registers of spi peripheral
* @param[in]		-Transmit data buffer
* @param[in]		-data length
*
* @return			-TX interrupt busy state
* @Note				-none
***************************************************************************************************************/
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pTxBuffer,uint32_t Len){

	uint8_t state=pSPIHandle->TxState;
	if(state != SPI_BUSY_IN_TX){
	//1. Save the Tx buffer address and Len information in some global variables
	pSPIHandle->pTxBuffer =pTxBuffer;
	pSPIHandle->TxLen=Len;

	//2. Mark the SPI state as busy in transmission so that
	// no other code can take over same SPI peripheral until transmission is over
	pSPIHandle->TxState=SPI_BUSY_IN_TX;

	//3. Enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
	pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);

	//4. Data Transmission will be handled by the ISR code ( will implement later)
	}
	return state;
}
/*************************************************************************************************************
* @fn 										-SPI_ReceiveDataIT
* @brief			-this function send data with SPIx peripheral when interrupt triggered
*
* @param[in]		-Hold the base address and values of the registers of spi peripheral
* @param[in]		-Receive data buffer
* @param[in]		-data length
*
* @return			-RX interrupt busy state
* @Note				-none
***************************************************************************************************************/
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pRxBuffer,uint32_t Len){

	uint8_t state=pSPIHandle->TxState;
	if(state != SPI_BUSY_IN_RX){
	//1. Save the Rx buffer address and Len information in some global variables
	pSPIHandle->pRxBuffer =pRxBuffer;
	pSPIHandle->RxLen=Len;

	//2. Mark the SPI state as busy in transmission so that
	// no other code can take over same SPI peripheral until transmission is over
	pSPIHandle->RxState=SPI_BUSY_IN_RX;

	//3. Enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
	pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);

	//4. Data Transmission will be handled by the ISR code ( will implement later)
	}
	return state;

}
/*************************************************************************************************************
* @fn 										-SPI_IRQConfig
* @brief				-the peripheral interrupt is enabled or disabled at processor level with irq number
*
* @param[in]			-the SPIx peripheral irq number
* @param[in]			-ENABLE or DISABLE macros
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void SPI_IRQConfig(uint8_t IRQNumber,uint8_t EnorDi){
	uint8_t tempReg=IRQNumber/32;
	uint8_t tempBit=IRQNumber%32;
	if(EnorDi == ENABLE){
		*(NVIC_ISER+tempReg) |= (1 << tempBit);
	}else if(EnorDi == DISABLE){
		*(NVIC_ICER+tempReg) |= (1 << tempBit);
	}
}
/*************************************************************************************************************
* @fn 										-SPI_IRQPriorityConfig
* @brief				-the peripheral interrupt priority is setting with this function
*
* @param[in]			-the SPIx peripheral irq number
* @param[in]			-Hold the priority level
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void SPI_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority){
	uint8_t tempReg=IRQNumber/4;
	uint8_t tempBit=IRQNumber%4;

	*(NVIC_IPR+tempReg) |= (IRQPriority << ((tempBit*8)+NO_PR_BITS_IMPLEMENTED));
}
static void spi_txe_interrupt_handle(SPI_Handle_t *pHandle){

			//check the DFF bit in CR1
			if(pHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF)){
				//16 bit data
				//load the data in to the DR
				pHandle->pSPIx->DR = *((uint16_t *)pHandle->pTxBuffer);
				pHandle->TxLen--;
				pHandle->TxLen--;
				(uint16_t *)pHandle->pTxBuffer++;
			}
			else{
				//8 bit data
				//load the data in to the DR
				pHandle->pSPIx->DR = *(pHandle->pTxBuffer);
				pHandle->TxLen--;
				pHandle->pTxBuffer++;
			}

			if(!pHandle->TxLen){
				//close the spi communication
				SPI_CloseTransmission(pHandle);
				SPI_ApplicationEventCallback(pHandle,SPI_EVENT_TX_CMPLT);


			}
}



static void spi_rxne_interrupt_handle(SPI_Handle_t *pHandle){

	if((pHandle->pSPIx->CR1 >> SPI_CR1_DFF) & SPI_DFF_16B){
				//16 bit reading
				*((uint16_t *)pHandle->pRxBuffer)=pHandle->pSPIx->DR;
				pHandle->RxLen--;
				pHandle->RxLen--;
				(uint16_t *)pHandle->pRxBuffer++;

	}
	else{
				//8 bit reading
				 *(pHandle->pRxBuffer)=pHandle->pSPIx->DR;
				 pHandle->RxLen--;
				 pHandle->pRxBuffer++;
	}

	if(!pHandle->RxLen){
		SPI_CloseReception(pHandle);
		SPI_ApplicationEventCallback(pHandle,SPI_EVENT_RX_CMPLT);

	}

}
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pHandle){

	uint8_t temp;

	//clear the ovr flag
	if(pHandle->TxState!=SPI_BUSY_IN_TX){
		temp=pHandle->pSPIx->DR;
		temp=pHandle->pSPIx->SR;
	}
	(void)temp;

	//inform the application
	SPI_ApplicationEventCallback(pHandle,SPI_EVENT_OVR_ERR);

}

void SPI_IRQHandling(SPI_Handle_t *pHandle){
	uint8_t temp1,temp2;
	//first let check for TXE
	temp1=pHandle->pSPIx->SR &( 1 <<SPI_SR_TXE);//TXE flag control
	temp2=pHandle->pSPIx->CR2 &( 1 <<SPI_CR2_TXEIE);//TXEIE control if TXEIE set transmit interrrupt be unmasked
	if(temp1 && temp2){
		spi_txe_interrupt_handle(pHandle);

	}

	//let check for RXE
	temp1=pHandle->pSPIx->SR &( 1 <<SPI_SR_RXNE);//RXE flag control
	temp2=pHandle->pSPIx->CR2 &( 1 <<SPI_CR2_RXNEIE);//RXNEIE control if RXNEIE set receive interrrupt be unmasked
	if(temp1 && temp2){
		spi_rxne_interrupt_handle(pHandle);
	}

	//let check for ovr
	temp1=pHandle->pSPIx->SR &( 1 <<SPI_SR_OVR);//OVR flag control(ovr flag be '1' when system read buffer,another data come to system
	temp2=pHandle->pSPIx->CR2 &( 1 <<SPI_CR2_ERRIE);//ERRIE control if ERRIE set error interrrupt be unmasked to handle the error

	if(temp1 && temp2){
		spi_ovr_err_interrupt_handle(pHandle);
	}
}

void SPI_CloseTransmission(SPI_Handle_t *pHandle){
	pHandle->pSPIx->CR2 &= ~( 1 <<SPI_CR2_TXEIE);
	pHandle->pTxBuffer =NULL;
	pHandle->TxLen=0;
	pHandle->TxState=SPI_READY;

}
void SPI_CloseReception(SPI_Handle_t *pHandle){
	pHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pHandle->pRxBuffer =NULL;
	pHandle->RxLen=0;
	pHandle->RxState=SPI_READY;

}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx){
	uint8_t temp;
	temp=pSPIx->DR;
	temp=pSPIx->SR;
	(void)temp;
}

__attribute__((weak)) void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t Event){
	//this is a week implementation.the application may override this function
}
