/*
 * stm32f407xx_gpio_driver.h
 *
 *  Created on: Jan 22, 2026
 *      Author: emreb
 */

#ifndef INC_STM32F407XX_GPIO_DRIVER_H_
#define INC_STM32F407XX_GPIO_DRIVER_H_

#include "stm32f407xx.h"

/*
 *This is a Configuration structure for a gpio pin
 */

typedef struct{
	uint8_t GPIO_PinNumber;		/*!< possible values from @GPIO_PIN_NUMBER>*/
	uint8_t GPIO_PinMode;		/*!< possible values from @GPIO_PIN_MODES>*/
	uint8_t GPIO_PinSpeed;		/*!< possible values from @GPIO_PIN_SPEED>*/
	uint8_t GPIO_PinPuPdControl;/*!< possible values from @GPIO_PIN_PUPD>*/
	uint8_t GPIO_PinOPType;		/*!< possible values from @GPIO_PIN_OTYPES>*/
	uint8_t GPIO_PinAltFunMode; /*!< possible values from @GPIO_PIN_ALTERNATE>*/
}GPIO_PinConfig_t;

/*
 *This is a handle structure for a gpio pin
 */

typedef struct{

	GPIO_RegDef_t *pGPIOx;				 /*!< This holds the base address of the GPIO port to which the pin belongs >*/
	GPIO_PinConfig_t GPIO_PinConfig;		 /*< This holds GPIO pin configuration settings >*/


}GPIO_Handle_t;

/*
 *	@GPIO_PIN_MODES
 *	GPIO pin possible modes
*/
#define GPIO_MODE_IN 		0
#define GPIO_MODE_OUT 		1
#define GPIO_MODE_ALTFN 	2
#define GPIO_MODE_ANALOG 	3
#define GPIO_MODE_IT_FT		4
#define GPIO_MODE_IT_RT		5
#define GPIO_MODE_IT_RFT	6

/*
 *	@GPIO_PIN_OTYPES
 *	GPIO pin possible output types
*/
#define GPIO_OP_TYPE_PP 	0
#define GPIO_OP_TYPE_OD 	1

/*
 * 	@GPIO_PIN_SPEED
 *	GPIO pin possible speed choices
*/
#define GPIO_SPEED_LS 		0
#define GPIO_SPEED_MS 		1
#define GPIO_SPEED_HS 		2
#define GPIO_SPEED_VHS 		3

/*
 * 	@GPIO_PIN_PUPD
 *	GPIO pin pull-up pull-down choices
*/
#define GPIO_PIN_NONE 		0
#define GPIO_PIN_PU 		1
#define GPIO_PIN_PD 		2

/*
 * 	@GPIO_PIN_ALTERNATE
 *	GPIO pin possible alternate function choices
*/
#define GPIO_ALFC_AF0 		0
#define GPIO_ALFC_AF1 		1
#define GPIO_ALFC_AF2 		2
#define GPIO_ALFC_AF3 		3
#define GPIO_ALFC_AF4 		4
#define GPIO_ALFC_AF5 		5
#define GPIO_ALFC_AF6 		6
#define GPIO_ALFC_AF7 		7
#define GPIO_ALFC_AF8 		8
#define GPIO_ALFC_AF9 		9
#define GPIO_ALFC_AF10 		10
#define GPIO_ALFC_AF11 		11
#define GPIO_ALFC_AF12 		12
#define GPIO_ALFC_AF13 		13
#define GPIO_ALFC_AF14 		14
#define GPIO_ALFC_AF15 		15

/*
 * 	@GPIO_PIN_NUMBER
 *	GPIO pin possible alternate function choices
*/
#define GPIO_PIN_NUMBER_0        0
#define GPIO_PIN_NUMBER_1        1
#define GPIO_PIN_NUMBER_2        2
#define GPIO_PIN_NUMBER_3        3
#define GPIO_PIN_NUMBER_4        4
#define GPIO_PIN_NUMBER_5        5
#define GPIO_PIN_NUMBER_6        6
#define GPIO_PIN_NUMBER_7        7
#define GPIO_PIN_NUMBER_8        8
#define GPIO_PIN_NUMBER_9        9
#define GPIO_PIN_NUMBER_10       10
#define GPIO_PIN_NUMBER_11       11
#define GPIO_PIN_NUMBER_12       12
#define GPIO_PIN_NUMBER_13       13
#define GPIO_PIN_NUMBER_14       14
#define GPIO_PIN_NUMBER_15       15





/********************************************************************************************************************
 	 	 	 	 	 	 	 	 	 * APIS supported by this driver
 	 	 	 	 	 	 * For more information about the APIS check the function definitions
********************************************************************************************************************/

/*
 * peripheral clock control
 */
void GPIO_PeripheralClockControl(GPIO_RegDef_t *pGPIOx,uint8_t EnorDi);


/*
 * Init Deinit control
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

/*
 *	Data Read and write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber,uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx,uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber);

/*
 * IRQ configuration and ISR handling
 */
void GPIO_IRQConfig(uint8_t IRQNumber,uint8_t EnorDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);





#endif /* INC_STM32F407XX_GPIO_DRIVER_H_ */
