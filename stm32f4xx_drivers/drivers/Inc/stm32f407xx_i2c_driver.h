/*
 * stm32f407xx_i2c_driver.h
 *
 *  Created on: Feb 9, 2026
 *      Author: emreb
 */

#ifndef INC_STM32F407XX_I2C_DRIVER_H_
#define INC_STM32F407XX_I2C_DRIVER_H_

#include "stm32f407xx.h"
#include "stddef.h"//NULL kullanmak için lazım

#define WRITE 	0
#define READ	1

typedef struct{
	uint16_t I2C_SCLSpeed;		/*!< possible values from @I2C_SCL_SPEED_TYPES>*/
	uint8_t I2C_DeviceAddress;
	uint8_t I2C_ACKControl;		/*!< possible values from @I2C_ACK_CONTROL>*/
	uint8_t I2C_FMDutyCycle;			/*!< possible values from @I2C_FM_DUTYCYCLE>*/
}I2C_Config_t;

typedef struct{
	I2C_RegDef_t *pI2Cx;
	I2C_Config_t I2CConfig;
	uint8_t *pTxBuffer;	/*!< To store the app. Tx Buffer address>*/
	uint8_t *pRxBuffer;	/*!< To store the app. Rx Buffer address>*/
	uint32_t TxLen;/*!< To store Tx Len>*/
	uint32_t RxLen;/*!< To store Rx Len>*/
	uint8_t TxRxState;/*!< To store communicate state.possible values from @I2C_TXRX_STATE>*/
	uint8_t DevAddr;/*!< To store slave device address>*/
	uint32_t RxSize; /*!< To storeRx size>*/
	uint8_t Sr;/*!< To store Repeated start value>*/
}I2C_Handle_t;


/*
 *	@I2C_SCL_SPEED
 *	I2C clock speed selection
*/
#define I2C_SCL_SPEED_SM				100000
#define I2C_SCL_SPEED_FM4K				400000
#define I2C_SCL_SPEED_FM2K				200000

/*
 *	@I2C_ACK_CONTROL
 *	I2C acknowledge control
*/
#define I2C_ACK_ENABLE					1
#define I2C_ACK_DISABLE					0

/*
 *	@I2C_FM_DUTYCYCLE
 *	I2C duty cycle selection
*/

#define I2C_FM_DUTY_2					0
#define I2C_FM_DUTY_16_9				1

/*
 *	@I2C_TXRX_STATE
 *	I2C duty cycle selection
*/
#define I2C_READY						0
#define I2C_BUSY_IN_RX					1
#define I2C_BUSY_IN_TX					2

/*
 * I2C application events macros
 */

#define I2C_EV_TX_CMPLT					0
#define I2C_EV_RX_CMPLT					1
#define I2C_EV_STOP						2
#define I2C_ERROR_BERR					3
#define I2C_ERROR_ARLO					4
#define I2C_ERROR_AF					5
#define I2C_ERROR_OVR					6
#define I2C_ERROR_TIMEOUT				7
#define I2C_EV_DATA_REQ					8
#define I2C_EV_DATA_RCV					9


/************************************************************************************************************************************
 	 	 	 	 	 MCU specific peripheral register data bitfield macros
 ***********************************************************************************************************************************/


/*
 * Bit position definitions of I2C_CR1
 */

#define I2C_CR1_PE							0
#define I2C_CR1_SMBUS						1
#define I2C_CR1_SMBTYPE						3
#define I2C_CR1_ENARP						4
#define I2C_CR1_ENPEC						5
#define I2C_CR1_ENGC						6
#define I2C_CR1_NOSTRETCH					7
#define I2C_CR1_START						8
#define I2C_CR1_STOP						9
#define I2C_CR1_ACK							10
#define I2C_CR1_POS							11
#define I2C_CR1_PEC							12
#define I2C_CR1_ALERT						13
#define I2C_CR1_SWRST						15

/*
 * Bit position definitions of I2C_CR2
 */
#define I2C_CR2_FREQ						0
#define I2C_CR2_ITERREN						8
#define I2C_CR2_ITEVTEN						9
#define I2C_CR2_ITBUFEN						10
#define I2C_CR2_DMAEN						11
#define I2C_CR2_LAST						12

/*
 * Bit position definitions of I2C_SR1
 */
#define I2C_SR1_SB							0
#define I2C_SR1_ADDR						1
#define I2C_SR1_BTF							2
#define I2C_SR1_ADD10						3
#define I2C_SR1_STOPF						4
#define I2C_SR1_RxNE						6
#define I2C_SR1_TxE		 					7
#define I2C_SR1_BERR						8
#define I2C_SR1_ARLO						9
#define I2C_SR1_AF							10
#define I2C_SR1_OVR							11
#define I2C_SR1_PECERR						12
#define I2C_SR1_TIMEOUT						14
#define I2C_SR1_SMBALERT					15

/*
 * Bit position definitions of I2C_SR2
 */
#define I2C_SR2_MSL							0
#define I2C_SR2_BUSY						1
#define I2C_SR2_TRA							2
#define I2C_SR2_GENCALL						4
#define I2C_SR2_SMBDEFAULT					5
#define I2C_SR2_SMBHOST						6
#define I2C_SR2_DUALF						7
#define I2C_SR2_PEC							8

/*
 * Bit position definitions of I2C_CCR
 */
#define I2C_CCR_CCR							0
#define I2C_SR2_DUTY						14
#define I2C_SR2_FS							15



/********************************************************************************************************************
 	 	 	 	 	 	 	 	 	 * APIS supported by this driver
 	 	 	 	 	 	 * For more information about the APIS check the function definitions
********************************************************************************************************************/

/*
 * peripheral clock control
 */
void I2C_PeripheralClockControl(I2C_RegDef_t *pI2Cx,uint8_t EnorDi);
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx,uint8_t EnorDi);

/*
 * Init Deinit control
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);


/*
 *	Data Send and Receive
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle,uint8_t *pTxBuffer,uint32_t Len,uint8_t slaveAddress,uint8_t SR_EnOrDı);
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer,uint32_t Len,uint8_t slaveAddress,uint8_t SR_EnOrDı);

void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx,uint8_t data);
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx);

uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pTxBuffer,uint32_t Len,uint8_t slaveAddress,uint8_t SR_EnOrDı);
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer,uint32_t Len,uint8_t slaveAddress,uint8_t SR_EnOrDı);

void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);
/*helper APIs*/
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx,uint8_t EnOrDi);
void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);
void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx,uint8_t EnOrDi);
/*
 * IRQ configuration and ISR handling
 */
void I2C_IRQConfig(uint8_t IRQNumber,uint8_t EnorDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority);

void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);

/*
 * flag control functions
 */
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx,uint32_t FlagName);

/*Application Callback*/
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t Event);


#endif /* INC_STM32F407XX_I2C_DRIVER_H_ */
