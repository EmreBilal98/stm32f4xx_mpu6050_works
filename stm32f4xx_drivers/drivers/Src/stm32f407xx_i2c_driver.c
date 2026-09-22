/*
 * stm32f407xx_i2c_driver.c
 *
 *  Created on: Feb 9, 2026
 *      Author: emreb
 */
#include "stm32f407xx_i2c_driver.h"
#include "stm32f407xx_rcc_driver.h"

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx,uint8_t slaveAddress,uint8_t RorW);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);

/*************************************************************************************************************
* @fn 										-I2C_PeripheralClockControl
* @brief									-This function enables or disables peripheral clock for the given I2C port
*
* @param[in]								-base address of the I2Cx peripheral
* @param[in]								-ENABLE or DISABLE macros
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void I2C_PeripheralClockControl(I2C_RegDef_t *pI2Cx,uint8_t EnorDi){

	if(EnorDi == ENABLE){
		if(pI2Cx == I2C1)
			I2C1_PCLK_EN();
		else if(pI2Cx == I2C2)
			I2C2_PCLK_EN();
		else if(pI2Cx == I2C3)
			I2C3_PCLK_EN();
	}
	else{
		if(pI2Cx == I2C1)
			I2C1_PCLK_DI();
		else if(pI2Cx == I2C2)
			I2C2_PCLK_DI();
		else if(pI2Cx == I2C3)
			I2C3_PCLK_DI();
	}


}

/*************************************************************************************************************
* @fn 										-I2C_PeripheralControl
* @brief									-This function enables or disables peripheral for the given I2C port
*
* @param[in]								-base address of the I2Cx peripheral
* @param[in]								-ENABLE or DISABLE macros
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx,uint8_t EnorDi){

	if(EnorDi == ENABLE){
			pI2Cx->CR1 |= (1<<I2C_CR1_PE);
	}
	else{
		pI2Cx->CR1 &= ~(1<<I2C_CR1_PE);
	}

}

/*************************************************************************************************************
* @fn 										-I2C_Init
* @brief			-This function make the init configuration of selected I2C peripheral
*
* @param[in]		-Hold the base address and values of the registers of I2C peripheral
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void I2C_Init(I2C_Handle_t *pI2CHandle){

	//open the peripheral clock
	I2C_PeripheralClockControl(pI2CHandle->pI2Cx, ENABLE);

	uint32_t tempReg=0;

	tempReg |= (pI2CHandle->I2CConfig.I2C_ACKControl << I2C_CR1_ACK);
	pI2CHandle->pI2Cx->CR1 =tempReg;

	//configure the freq
	tempReg=0;
	tempReg|=(RCC_GetPCLK1Value()/1000000U);//divided by 1MHz because FREQ value want it like that
	pI2CHandle->pI2Cx->CR2 =(tempReg & 0x3F);

	//program the device own address
	tempReg =0;
	tempReg |= pI2CHandle->I2CConfig.I2C_DeviceAddress << 1;
	tempReg |= (1<<14);//user manual note that  this 14. bit always be 1 by software
	tempReg &= ~(1<<15);//ADDMODE will be 0 for 7 bit slave address
	pI2CHandle->pI2Cx->OAR1 = tempReg;

	//CCR calculations
	uint16_t ccrCalc=0;
	tempReg=0;
	if(pI2CHandle->I2CConfig.I2C_SCLSpeed <= I2C_SCL_SPEED_SM){
		ccrCalc=RCC_GetPCLK1Value()/(2*pI2CHandle->I2CConfig.I2C_SCLSpeed);
	}
	else{
		tempReg |= (1<<15);//fast mode
		if(pI2CHandle->I2CConfig.I2C_FMDutyCycle == I2C_FM_DUTY_2){
			ccrCalc=RCC_GetPCLK1Value()/(3*pI2CHandle->I2CConfig.I2C_SCLSpeed);
		}
		else if(pI2CHandle->I2CConfig.I2C_FMDutyCycle == I2C_FM_DUTY_16_9){
			tempReg |= (1<<14);//duty cycle 16/9
			ccrCalc=RCC_GetPCLK1Value()/(25*pI2CHandle->I2CConfig.I2C_SCLSpeed);
		}

	}
	tempReg |= (ccrCalc & 0xFFF);
	pI2CHandle->pI2Cx->CCR = tempReg;

	tempReg=0;
	if(pI2CHandle->I2CConfig.I2C_SCLSpeed <= I2C_SCL_SPEED_SM){
			tempReg=(RCC_GetPCLK1Value()/1000000U)+1;
		}
		else{
			tempReg=((300*RCC_GetPCLK1Value())/1000000000U)+1;
		}
	pI2CHandle->pI2Cx->TRISE = (tempReg & 0x3F);



}

/*************************************************************************************************************
* @fn 										-I2C_DeInit
* @brief			-this function reset the I2Cx peripheral
*
* @param[in]		-Hold peripheral base addresses
*
* @return			-none
* @Note				-none
***************************************************************************************************************/
void I2C_DeInit(I2C_RegDef_t *pI2Cx){
	if(pI2Cx == I2C1)
			I2C1_REG_RESET();
		else if(pI2Cx == I2C2)
			I2C2_REG_RESET();
		else if(pI2Cx == I2C3)
			I2C3_REG_RESET();

}
static void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx,uint8_t slaveAddress,uint8_t RorW){
	slaveAddress =(slaveAddress <<1);//first bit is cleared for the r/nw bit
	if(RorW==WRITE)
	slaveAddress &= ~(1);//first bit is pulled to low for write information
	else if(RorW==READ)
	slaveAddress |= (1);//first bit is pulled to high for read information
	pI2Cx->DR = slaveAddress;
}
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx){
	pI2Cx->CR1 |= (1 << I2C_CR1_START);
}
void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx){
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle){

	uint32_t dummyRead;

	if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL)){
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){
			if(pI2CHandle->RxSize == 1){
				//first disable the ack
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

				//For Clearing the AADR flag we must read SR1 and SR2
				dummyRead=pI2CHandle->pI2Cx->SR1;
				dummyRead=pI2CHandle->pI2Cx->SR2;
				(void)dummyRead;

			}
		}
		else{
			//For Clearing the AADR flag we must read SR1 and SR2
			dummyRead=pI2CHandle->pI2Cx->SR1;
			dummyRead=pI2CHandle->pI2Cx->SR2;
			(void)dummyRead;
		}
	}else{

		//slave mode
		//For Clearing the AADR flag we must read SR1 and SR2
		dummyRead=pI2CHandle->pI2Cx->SR1;
		dummyRead=pI2CHandle->pI2Cx->SR2;
		(void)dummyRead;
	}
}

void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx,uint8_t EnOrDi){
	if(EnOrDi ==ENABLE){
		//Implement the code to enable ITBUFEN Control Bit
		pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVTEN Control Bit
		pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);
	}
	else{
		//Implement the code to disable ITBUFEN Control Bit
		pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to disable ITEVTEN Control Bit
		pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN);

		//Implement the code to disable ITERREN Control Bit
		pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITERREN);


	}
}
/*************************************************************************************************************
* @fn 										-I2C_GetFlagStatus
* @brief			-this function control the I2Cx peripheral flags
*
* @param[in]		-Hold peripheral base addresses
* @param[in]		-The flag bitfield address
*
* @return			-the status of the flag
* @Note				-none
***************************************************************************************************************/
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx,uint32_t FlagName){
	 return (pI2Cx->SR1 & FlagName);
}
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx,uint8_t EnOrDi){

	if(EnOrDi==ENABLE)
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	else if(EnOrDi==DISABLE)
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);

}
/*********************************************************************
 * @fn      		  - I2C_MasterSendData
 *
 * @brief             - I2c send function
 *
 * @param[in]         -	I2Cx peripheral settings
 * @param[in]         -	Tx Buffer Address
 * @param[in]         -	Tx Length
 * @param[in]         -	slave address
 * @param[in]         -	Repeated state status
 *
 * @return            -
 *
 * @Note              -

 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle,uint8_t *pTxBuffer,uint32_t Len,uint8_t slaveAddress,uint8_t SR_EnOrDı){


	//1. Generate the START condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. confirm that start generation is completed by checking the SB flag in the SR1
	//Note: Until SB is cleared SCL will be stretched (pulled to LOW)
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1<<I2C_SR1_SB)));


	//3. Send the address of the slave with r/nw bit set to w(0) (total 8 bits )
	I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx,slaveAddress,WRITE);

	//4. Confirm that address phase is completed by checking the ADDR flag in the SR1
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1<<I2C_SR1_ADDR)));


	//5. clear the ADDR flag according to its software sequence
	// Note: Until ADDR is cleared SCL will be stretched (pulled to LOW)
	I2C_ClearADDRFlag(pI2CHandle);

	//6. send the data until Len becomes 0
	while(Len>0){

		while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1<<I2C_SR1_TxE)));
		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		pTxBuffer++;
		Len--;

	}


	//7. when Len becomes zero wait for TXE=1 and BTF=1 before generating the STOP condition
	//Note: TXE=1, BTF=1, means that both SR and DR are empty and next transmission should begin
	// when BTF=1 SCL will be stretched (pulled to LOW)
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1<<I2C_SR1_TxE)));
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1<<I2C_SR1_BTF)));



	//8. Generate STOP condition and master need not to wait for the completion of stop condition.
	//Note: generating STOP, automatically clears the BTF
	if(SR_EnOrDı == DISABLE)
	I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

}
/*********************************************************************
 * @fn      		  - I2C_MasterReceiveData
 *
 * @brief             - I2c receive function
 *
 * @param[in]         -	I2Cx peripheral settings
 * @param[in]         -	Rx Buffer Address
 * @param[in]         -	Rx Length
 * @param[in]         -	slave address
 * @param[in]         -	Repeated state status
 *
 * @return            -
 *
 * @Note              -

 */
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer,uint32_t Len,uint8_t slaveAddress,uint8_t SR_EnOrDı){


	//1. Generate the START condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. confirm that start generation is completed by checking the SB flag in the SR1
	//Note: Until SB is cleared SCL will be stretched (pulled to LOW)
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1<<I2C_SR1_SB)));


	//3. Send the address of the slave with r/nw bit set to w(0) (total 8 bits )
	I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx,slaveAddress,READ);

	//4. Confirm that address phase is completed by checking the ADDR flag in the SR1
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1<<I2C_SR1_ADDR)));

	if(Len == 1){
		//Disable Acking
		I2C_ManageAcking(pI2CHandle->pI2Cx,DISABLE);


		//clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//wait until RXNE becomes 1
		while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1<<I2C_SR1_RxNE)));

		//generate STOP condition
		if(SR_EnOrDı == DISABLE)
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		//read data in to buffer
		*pRxBuffer=pI2CHandle->pI2Cx->DR;

	}

	//procedure to read data from slave when Len > 1
	if (Len > 1)
	{
	//clear the ADDR flag
	I2C_ClearADDRFlag(pI2CHandle);

	//read the data untill Len becomes zero
	for (uint32_t i =Len; i > 0 ; i--)
	{
	//wait until RxNe becomes 1
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1<<I2C_SR1_RxNE)));

	if (i == 2) //if last 2 bytes are remaining
	{
	//clear the ack bit
	I2C_ManageAcking(pI2CHandle->pI2Cx,DISABLE);

	//generate STOP condition
	if(SR_EnOrDı == DISABLE)
	I2C_GenerateStopCondition(pI2CHandle->pI2Cx);


	}
	//read the data from data register in to buffer
	*pRxBuffer=pI2CHandle->pI2Cx->DR;
	//increment the buffer address
	pRxBuffer++;
	}

	}

	//re-Enable acking
	if(pI2CHandle->I2CConfig.I2C_ACKControl == I2C_ACK_ENABLE)
	I2C_ManageAcking(pI2CHandle->pI2Cx,ENABLE);



}

/*********************************************************************
 * @fn      		  - I2C_MasterSendDataIT
 *
 * @brief             - I2c send function with interrupt
 *
 * @param[in]         -	I2Cx peripheral settings
 * @param[in]         -	Tx Buffer Address
 * @param[in]         -	Tx Length
 * @param[in]         -	slave address
 * @param[in]         -	Repeated state status
 *
 * @return            - to return the bus state
 *
 * @Note              -

 */
uint8_t  I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pTxBuffer,uint32_t Len,uint8_t slaveAddress,uint8_t SR_EnOrDı)
{

	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = slaveAddress;
		pI2CHandle->Sr = SR_EnOrDı;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);


		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVTEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

	}

	return busystate;

}
/*********************************************************************
 * @fn      		  - I2C_MasterReceiveDataIT
 *
 * @brief             - I2c receive function with interrupt
 *
 * @param[in]         -	I2Cx peripheral settings
 * @param[in]         -	Rx Buffer Address
 * @param[in]         -	Rx Length
 * @param[in]         -	slave address
 * @param[in]         -	Repeated state status
 *
 * @return            - to return the bus state
 *
 * @Note              -

 */
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer,uint32_t Len,uint8_t slaveAddress,uint8_t SR_EnOrDı)
{

	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = Len; //Rxsize is used in the ISR code to manage the data reception
		pI2CHandle->DevAddr = slaveAddress;
		pI2CHandle->Sr = SR_EnOrDı;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);


		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVTEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

	}

	return busystate;
}
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle){
	//Implement the code to disable ITBUFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN);

	//Implement the code to disable ITEVTEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN);

	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->RxSize = 0;

	if(pI2CHandle->I2CConfig.I2C_ACKControl == I2C_ACK_ENABLE)
	I2C_ManageAcking(pI2CHandle->pI2Cx, ENABLE);

}
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle){
	//Implement the code to disable ITBUFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN);

	//Implement the code to disable ITEVTEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN);

	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->TxLen = 0;
	pI2CHandle->TxRxState = I2C_READY;

	if(pI2CHandle->I2CConfig.I2C_ACKControl == I2C_ACK_ENABLE)
	I2C_ManageAcking(pI2CHandle->pI2Cx, ENABLE);


}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle){
	if(pI2CHandle->RxSize == 1){
		//load from DR
		*pI2CHandle->pRxBuffer =pI2CHandle->pI2Cx->DR ;

		//decrement Length
		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxSize > 1){

		if(pI2CHandle->RxLen == 2 ){

			I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

		}
		//load from DR
		*pI2CHandle->pRxBuffer =pI2CHandle->pI2Cx->DR ;

		//decrement Length
		pI2CHandle->RxLen--;

		//increment address of receive buffer
		pI2CHandle->pRxBuffer++;

	}

	if(pI2CHandle->RxLen == 0){
		//close the ı2c data communication
		//generate STOP condition
		if(pI2CHandle->Sr == DISABLE)
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		//close the ı2c rx
		I2C_CloseReceiveData(pI2CHandle);

		//notify
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);

	}
}

static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle){
	if(pI2CHandle->TxLen > 0){

					//load to DR
					pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

					//decrement Length
					pI2CHandle->TxLen--;

					//increment address of transmit buffer
					pI2CHandle->pTxBuffer++;

					}
}

void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx,uint8_t data){
	pI2Cx->DR =data;
}
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx){
	return (uint8_t)pI2Cx->DR;
}
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle){

	//Interrupt handling for both master and slave mode of a device
	uint32_t temp1,temp2,temp3;

	temp1=pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN);
	temp2=pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);

	temp3=pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_SB);

	//1. Handle For interrupt generated by SB event
	// Note : SB flag is only applicable in Master mode
	if(temp1 && temp3){

		//The interrupt is generated because of SB event
		//This block will not be executed in slave mode because for slave SB is always zero
		//In this block lets executed the address phase

		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, pI2CHandle->DevAddr, READ);
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, pI2CHandle->DevAddr, WRITE);

	}
	//2. Handle For interrupt generated by ADDR event
	//Note : When master mode: Address is sent
	//When Slave mode: Address matched with own address
    temp3=pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR);
    if(temp1 && temp3){

    	//ADDR flag is set
    	I2C_ClearADDRFlag(pI2CHandle);

    }



	//3. Handle For interrupt generated by BTF(Byte Transfer Finished) event
	temp3=pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF);

	if(temp1 && temp3){

		//BTF flag is set
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX){

			if(pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_TxE)){
				//BTF=1,TXE=1
				if(pI2CHandle->TxLen == 0){
				//1. generate the STOP condition
				if(pI2CHandle->Sr == DISABLE)
				I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
				//2. reset all the member elements of the handle structure.
				I2C_CloseSendData(pI2CHandle);

				//3. notify the application about transmission complete
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
				}
			}
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){
			;
		}


	}

	//4. Handle For interrupt generated by STOPF event
	// Note: Stop detection flag is applicable only slave mode . For master this flag will be unused
	temp3=pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF);

	if(temp1 && temp3){

		//STOPF flag is set
		//Clear the STOPF (i.e 1) read SR1 2) Write to CR1 )|sr1 readed on temp3
		pI2CHandle->pI2Cx->CR1 |= 0x0000;//clearing stopf flag

		//Notify the application that STOP is detected
		I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_STOP);

	}


	//5. Handle For interrupt generated by TXE event
	temp3=pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TxE);

	if(temp1 && temp2 && temp3){

		//TXE flag is set
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL)){//master mode
			//we have to do the data transmission
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX){
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
			}
		}else{//slave mode

			if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))//transmitter mode
			I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_REQ);



		}

	}

	//6. Handle For interrupt generated by RXNE event
	temp3=pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RxNE);

	if(temp1 && temp2 && temp3){

		//RxNE flag is set
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL)){//master mode
		//we have to do the data receiving
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){
			I2C_MasterHandleRXNEInterrupt(pI2CHandle);
		}
	  }
		else{
			if(!(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA)))//receive mode
			I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_RCV);
		}
	}




}
/*********************************************************************
 * @fn      		  - I2C_ER_IRQHandling
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              - Complete the code also define these macros in the driver
						header file
						#define I2C_ERROR_BERR  3
						#define I2C_ERROR_ARLO  4
						#define I2C_ERROR_AF    5
						#define I2C_ERROR_OVR   6
						#define I2C_ERROR_TIMEOUT 7

 */
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle){

	uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);


/***********************Check for Bus error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//This is Bus error

		//Implement the code to clear the buss error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

		//Implement the code to notify the application about the error
	   I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_BERR);
	}

/***********************Check for arbitration lost error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{
		//This is arbitration lost error

		//Implement the code to clear the arbitration lost error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_ARLO);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_ARLO);

	}

/***********************Check for ACK failure  error************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error

	    //Implement the code to clear the ACK failure error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_AF);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_AF);
	}

/***********************Check for Overrun/underrun error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun

	    //Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_OVR);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_OVR);
	}

/***********************Check for Time out error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error

	    //Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_TIMEOUT);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_TIMEOUT);
	}
}

/*************************************************************************************************************
* @fn 										-I2C_IRQConfig
* @brief				-the peripheral interrupt is enabled or disabled at processor level with irq number
*
* @param[in]			-the I2Cx peripheral irq number
* @param[in]			-ENABLE or DISABLE macros
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void I2C_IRQConfig(uint8_t IRQNumber,uint8_t EnorDi){
	uint8_t tempReg=IRQNumber/32;
	uint8_t tempBit=IRQNumber%32;
	if(EnorDi == ENABLE){
		*(NVIC_ISER+tempReg) |= (1 << tempBit);
	}else if(EnorDi == DISABLE){
		*(NVIC_ICER+tempReg) |= (1 << tempBit);
	}
}
/*************************************************************************************************************
* @fn 										-I2C_IRQPriorityConfig
* @brief				-the peripheral interrupt priority is setting with this function
*
* @param[in]			-the I2Cx peripheral irq number
* @param[in]			-Hold the priority level
*
* @return									-none
* @Note										-none
***************************************************************************************************************/
void I2C_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority){
	uint8_t tempReg=IRQNumber/4;
	uint8_t tempBit=IRQNumber%4;

	*(NVIC_IPR+tempReg) |= (IRQPriority << ((tempBit*8)+NO_PR_BITS_IMPLEMENTED));
}

__attribute__((weak))void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t Event){
	//this is a week implementation.the application may override this function
}
