/*
 * stm32f407xx_spi_driver.h
 *
 *  Created on: Jan 26, 2026
 *      Author: emreb
 */

#ifndef INC_STM32F407XX_SPI_DRIVER_H_
#define INC_STM32F407XX_SPI_DRIVER_H_

#include "stm32f407xx.h"
#include "stddef.h"//NULL kullanmak için lazım

typedef struct{
	uint8_t SPI_DeviceMode;		/*!< possible values from @SPI_DEVICE_MODE_TYPES>*/
	uint8_t SPI_BusConfig;		/*!< possible values from @SPI_BUS_CONFIG_TYPES>*/
	uint8_t SPI_SclkSpeed;		/*!< possible values from @SPI_CSPEED_PRESCALER>*/
	uint8_t SPI_DFF;			/*!< possible values from @SPI_DFF_TYPES>*/
	uint8_t SPI_CPOL;			/*!< possible values from @SPI_CPOL_TYPES>*/
	uint8_t SPI_CPHA; 			/*!< possible values from @SPI_CPHA_TYPES>*/
	uint8_t SPI_SSM;			/*!< possible values from @SPI_SSM_TYPES>*/
}SPI_Config_t;

typedef struct{
	SPI_RegDef_t *pSPIx;
	SPI_Config_t SPIConfig;
	uint8_t *pTxBuffer;	/* !<To store the app.Tx buffer address > */
	uint8_t *pRxBuffer;	/* !<To store the app.Rx buffer address > */
	uint32_t TxLen;     /* !<to store Tx len > */
	uint32_t RxLen;     /* !<to store Tx len > */
	uint8_t TxState;    /* !<to store Tx state > */
	uint8_t RxState;	/* !<to store Tx state > */

}SPI_Handle_t;

/*
 *	@SPI_DEVICE_MODE_TYPES
 *	select device master or slave
*/
#define SPI_DEVICE_MODE_MASTER			1
#define SPI_DEVICE_MODE_SLAVE			0

/*
 *	@SPI_BUS_CONFIG_TYPES
 *	SPI bus selection.full duplex,half duplex,simplex
*/
#define SPI_BUS_CONFIG_FD				1
#define SPI_BUS_CONFIG_HD				2
#define SPI_BUS_CONFIG_S_RXONLY			3

/*
 *	@SPI_CSPEED_PRESCALER
 *	SPI clock speed prescaler selection
*/
#define SPI_SPEED_PRESCALER_2			0
#define SPI_SPEED_PRESCALER_4			1
#define SPI_SPEED_PRESCALER_8			2
#define SPI_SPEED_PRESCALER_16			3
#define SPI_SPEED_PRESCALER_32			4
#define SPI_SPEED_PRESCALER_64			5
#define SPI_SPEED_PRESCALER_128			6
#define SPI_SPEED_PRESCALER_256			7

/*
 *	@SPI_DFF_TYPES
 *	SPI register bit selection
*/
#define SPI_DFF_8B						0
#define SPI_DFF_16B						1

/*
 *	@SPI_CPOL_TYPES
 *	SPI clock polarity selection
*/
#define SPI_CLOCK_IDLE_LOW				0
#define SPI_CLOCK_IDLE_HIGH				1

/*
 *	@SPI_CPHA_TYPES
 *	SPI clock phase selection
*/
#define SPI_CPHA_LOW					0
#define SPI_CPHA_HIGH					1

/*
 *	@SPI_SSM_TYPES
 *	SPI software slave management selection
*/
#define SPI_SSM_EN						1
#define SPI_SSM_DI						0

/************************************************************************************************************************************
 	 	 	 	 	 MCU specific peripheral register data bitfield macros
 ***********************************************************************************************************************************/

/*
 * Bit position definitions of SPI_CR1
 */
#define SPI_CR1_CPHA 					0
#define SPI_CR1_CPOL 					1
#define SPI_CR1_MSTR 					2
#define SPI_CR1_BR 						3
#define SPI_CR1_SPE						6
#define SPI_CR1_LSBFIRST				7
#define SPI_CR1_SSI						8
#define SPI_CR1_SSM 					9
#define SPI_CR1_RXONLY 					10
#define SPI_CR1_DFF						11
#define SPI CR1 CRCNEXT					12
#define SPI_CR1_CRCEN					13
#define SPI_CR1_BIDIOE					14
#define SPI_CR1_BIDIMODE				15


/*
 * Bit position definitions of SPI_CR2
 */
#define SPI_CR2_RXDMAEN					0
#define SPI_CR2_TXDMAEN					1
#define	SPI_CR2_SSOE					2
#define SPI_CR2_FRF						4
#define SPI_CR2_ERRIE					5
#define SPI_CR2_RXNEIE					6
#define SPI_CR2_TXEIE					7

/*
 * Bit position definitions of SPI_SR
 */
#define SPI_SR_RXNE						0
#define SPI_SR_TXE						1
#define	SPI_SR_CHSIDE					2
#define SPI_SR__UDR						3
#define SPI_SR_CRCERR					4
#define SPI_SR_MODF						5
#define SPI_SR_OVR						6
#define SPI_SR_BSY						7
#define SPI_SR_FRE						8


/*
 * SPI related status flags
 */

#define SPI_TXE_FLAG 					(1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG 					(1 << SPI_SR_RXNE)
#define SPI_BSY_FLAG					(1 << SPI_SR_BSY)

/*
 * SPI interrrupt macros
 */
#define SPI_READY						0
#define SPI_BUSY_IN_RX					1
#define SPI_BUSY_IN_TX					2

/*
 * possible SPI Application events
 */
#define SPI_EVENT_TX_CMPLT				1
#define SPI_EVENT_RX_CMPLT				2
#define SPI_EVENT_OVR_ERR				3
#define SPI_EVENT_CRC_ERR				4


/********************************************************************************************************************
 	 	 	 	 	 	 	 	 	 * APIS supported by this driver
 	 	 	 	 	 	 * For more information about the APIS check the function definitions
********************************************************************************************************************/

/*
 * peripheral clock control
 */
void SPI_PeripheralClockControl(SPI_RegDef_t *pSPIx,uint8_t EnorDi);
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx,uint8_t EnorDi);


/*
 * Init Deinit control
 */
void SPI_Init(SPI_Handle_t *pSPIOHandle);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx,uint8_t EnOrDi);
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx,uint8_t EnOrDi);
void SPI_DeInit(SPI_RegDef_t *pSPIx);


/*
 *	Data Send and Receive
 */
void SPI_SendData(SPI_RegDef_t *pSPIx,uint8_t *pTxBuffer,uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx,uint8_t *pRxBuffer,uint32_t Len);
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pTxBuffer,uint32_t Len);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pRxBuffer,uint32_t Len);


/*
 * IRQ configuration and ISR handling
 */
void SPI_IRQConfig(uint8_t IRQNumber,uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);

/*
 * flag control functions
 */
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx,uint32_t FlagName);

/*other peripheral control APIs*/
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);

/*Application Callback*/
void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t Event);
#endif /* INC_STM32F407XX_SPI_DRIVER_H_ */
