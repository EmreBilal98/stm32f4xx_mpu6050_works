/*
 * stm32f407xx.h
 *
 *  Created on: Jan 21, 2026
 *      Author: emreb
 */

#ifndef INC_STM32F407XX_H_
#define INC_STM32F407XX_H_

#include <stdint.h>

#define __vo 				volatile

/*****************************************************************START:processor specific details************************************************************/

/*
 *ARM cortex M4 processor NVIC ISERx register Addresses
 */

//#define NVIC_ISER0	((__vo uint32_t*)0xE000E100)
//#define NVIC_ISER1	((__vo uint32_t*)0xE000E104)
//#define NVIC_ISER2	((__vo uint32_t*)0xE000E108)
//#define NVIC_ISER3	((__vo uint32_t*)0xE000E10C)

#define NVIC_ISER	((__vo uint32_t*)0xE000E100)

/*
 *ARM cortex M4 processor NVIC ICERx register Addresses
 */

//#define NVIC_ICER0	((__vo uint32_t*)0xE000E180)
//#define NVIC_ICER1	((__vo uint32_t*)0xE000E184)
//#define NVIC_ICER2	((__vo uint32_t*)0xE000E188)
//#define NVIC_ICER3	((__vo uint32_t*)0xE000E18C)

#define NVIC_ICER	((__vo uint32_t*)0xE000E180)

/*
 *ARM cortex M4 processor NVIC IPRx register Addresses
 */
#define NVIC_IPR	((__vo uint32_t*)0xE000E400)

#define NO_PR_BITS_IMPLEMENTED 4

//generic macros
#define ENABLE 				1
#define DISABLE 			0
#define SET 				ENABLE
#define RESET	 			DISABLE


//Memory adresses

#define FLASH_BASEADDR		0x08000000U
#define SRAM1_BASEADDR		0x20000000U
#define SRAM1_SIZE			0x0001C000U					//112kb
#define SRAM				SRAM1_BASEADDR
#define ROM					0x1FFF0000U
#define SRAM2				(SRAM + SRAM1_SIZE)
#define SYSTICK_BASEADDR    0xE000E010U

//BUS ADRESSES
#define APB1PERIPH_BASE		0x40000000U
#define APB2PERIPH_BASE		0x40010000U
#define AHB1PERIPH_BASE		0x40020000U
#define AHB2PERIPH_BASE		0x50000000U

/*
 * Peripheral addresses which are hanging on AHB1 bus
 * offsets are adding as addition
*/
#define GPIOA_BASEADDR		(AHB1PERIPH_BASE + 0x0000)
#define GPIOB_BASEADDR		(AHB1PERIPH_BASE + 0x0400)
#define GPIOC_BASEADDR		(AHB1PERIPH_BASE + 0x0800)
#define GPIOD_BASEADDR		(AHB1PERIPH_BASE + 0x0C00)
#define GPIOE_BASEADDR		(AHB1PERIPH_BASE + 0x1000)
#define GPIOF_BASEADDR		(AHB1PERIPH_BASE + 0x1400)
#define GPIOG_BASEADDR		(AHB1PERIPH_BASE + 0x1800)
#define GPIOH_BASEADDR		(AHB1PERIPH_BASE + 0x1C00)
#define GPIOI_BASEADDR		(AHB1PERIPH_BASE + 0x2000)

#define RCC_BASEADDR 		(AHB1PERIPH_BASE + 0x3800)

#define FLSH_BASEADDR		(AHB1PERIPH_BASE + 0x3C00)

/*
 * Peripheral addresses which are hanging on APB1 bus
 * offsets are adding as addition
*/
#define SPI2_BASEADDR		(APB1PERIPH_BASE + 0x3800)
#define SPI3_BASEADDR		(APB1PERIPH_BASE + 0x3C00)
#define I2C1_BASEADDR		(APB1PERIPH_BASE + 0x5400)
#define I2C2_BASEADDR		(APB1PERIPH_BASE + 0x5800)
#define I2C3_BASEADDR		(APB1PERIPH_BASE + 0x5C00)
#define USART2_BASEADDR		(APB1PERIPH_BASE + 0x4400)
#define USART3_BASEADDR		(APB1PERIPH_BASE + 0x4800)
#define UART4_BASEADDR		(APB1PERIPH_BASE + 0x4C00)
#define UART5_BASEADDR		(APB1PERIPH_BASE + 0x5000)
#define TIM2_BASEADDR		(APB1PERIPH_BASE + 0x0000)
#define TIM3_BASEADDR		(APB1PERIPH_BASE + 0x0400)
#define TIM4_BASEADDR		(APB1PERIPH_BASE + 0x0800)
#define TIM5_BASEADDR		(APB1PERIPH_BASE + 0x0C00)
#define TIM6_BASEADDR		(APB1PERIPH_BASE + 0x1000)
#define TIM7_BASEADDR		(APB1PERIPH_BASE + 0x1400)
#define TIM12_BASEADDR		(APB1PERIPH_BASE + 0x1800)
#define TIM13_BASEADDR		(APB1PERIPH_BASE + 0x1C00)
#define TIM14_BASEADDR		(APB1PERIPH_BASE + 0x2000)
#define PWR_BASEADDR		(APB1PERIPH_BASE + 0x7000)

/*
 * Peripheral addresses which are hanging on APB2 bus
 * offsets are adding as addition
*/
#define SPI1_BASEADDR		(APB2PERIPH_BASE + 0x3000)
#define SPI4_BASEADDR		(APB2PERIPH_BASE + 0x3400)
#define USART1_BASEADDR		(APB2PERIPH_BASE + 0x1000)
#define USART6_BASEADDR		(APB2PERIPH_BASE + 0x1400)
#define EXTI_BASEADDR		(APB2PERIPH_BASE + 0x3C00)
#define SYSCFG_BASEADDR		(APB2PERIPH_BASE + 0x3800)
#define TIM1_BASEADDR		(APB2PERIPH_BASE + 0x0000)
#define TIM8_BASEADDR		(APB2PERIPH_BASE + 0x0400)
#define TIM9_BASEADDR		(APB2PERIPH_BASE + 0x4000)
#define TIM10_BASEADDR		(APB2PERIPH_BASE + 0x4400)
#define TIM11_BASEADDR		(APB2PERIPH_BASE + 0x4800)


/**********************************************peripheral register definition structures*****************************************************************/

typedef struct {
	__vo uint32_t MODER;		//offset:0x00
	__vo uint32_t OTYPER;		//offset:0x04
	__vo uint32_t OSPEEDR;		//offset:0x08
	__vo uint32_t PUPDR;		//offset:0x0C
	__vo uint32_t IDR;			//offset:0x10
	__vo uint32_t ODR;			//offset:0x14
	__vo uint32_t BSRR;			//offset:0x18
	__vo uint32_t LCKR;			//offset:0x1C
	__vo uint32_t AFR[2];		//offset:0x20 for both (AFR[0])AFRl and (AFR[1])AFRH registers
}GPIO_RegDef_t;


typedef struct {
	__vo uint32_t CR;			//offset:0x00
	__vo uint32_t PLLCFGR;		//offset:0x04
	__vo uint32_t CFGR;			//offset:0x08
	__vo uint32_t CIR;			//offset:0x0C
	__vo uint32_t AHB1RSTR;		//offset:0x10
	__vo uint32_t AHB2RSTR;		//offset:0x14
	__vo uint32_t AHB3RSTR;		//offset:0x18
	uint32_t RESERVED0;			//offset:0x18
	__vo uint32_t APB1RSTR;		//offset:0x20
	__vo uint32_t APB2RSTR;		//offset:0x24
	uint32_t RESERVED1[2];		//offset:0x28
	__vo uint32_t AHB1ENR;		//offset:0x30
	__vo uint32_t AHB2ENR;		//offset:0x34
	__vo uint32_t AHB3ENR;		//offset:0x38
	uint32_t RESERVED2;			//offset:0x3C
	__vo uint32_t APB1ENR;		//offset:0x40
	__vo uint32_t APB2ENR;		//offset:0x44
	uint32_t RESERVED3[2];		//offset:0x48
	__vo uint32_t AHB1LPENR;	//offset:0x50
	__vo uint32_t AHB2LPENR;	//offset:0x54
	__vo uint32_t AHB3LPENR;	//offset:0x58
	uint32_t RESERVED4;			//offset:0x5C
	__vo uint32_t APB1LPENR;	//offset:0x60
	__vo uint32_t APB2LPENR;	//offset:0x64
	uint32_t RESERVED5[2];		//offset:0x68
	__vo uint32_t BDCR;			//offset:0x70
	__vo uint32_t CSR;			//offset:0x74
	uint32_t RESERVED6[2];		//offset:0x78
	__vo uint32_t SSCGR;		//offset:0x80
	__vo uint32_t PLLI2SCFGR;	//offset:0x84
}RCC_RegDef_t;


typedef struct{
	__vo uint32_t IMR;          //offset:0x00
	__vo uint32_t EMR;          //offset:0x04
	__vo uint32_t RTSR;         //offset:0x08
	__vo uint32_t FTSR;         //offset:0x0C
	__vo uint32_t SWIER;        //offset:0x10
	__vo uint32_t PR;           //offset:0x14
}EXTI_RegDef_t;

typedef struct{
	__vo uint32_t MEMRMP;       //offset:0x00
	__vo uint32_t PMC;          //offset:0x04
	__vo uint32_t EXTICR[4];    //offset:0x08
	__vo uint32_t CMPCR;        //offset:0x14
}SYSCFG_RegDef_t;

typedef struct {
	__vo uint32_t CR1;			//offset:0x00
	__vo uint32_t CR2;			//offset:0x04
	__vo uint32_t SR;			//offset:0x08
	__vo uint32_t DR;			//offset:0x0C
	__vo uint32_t CRCPR;		//offset:0x10
	__vo uint32_t RXCRCR;		//offset:0x14
	__vo uint32_t TXCRCR;		//offset:0x18
	__vo uint32_t I2SCFGR;		//offset:0x1C
	__vo uint32_t I2SPR;		//offset:0x20
}SPI_RegDef_t;

typedef struct {
	__vo uint32_t CR1;			//offset:0x00
	__vo uint32_t CR2;			//offset:0x04
	__vo uint32_t OAR1;			//offset:0x08
	__vo uint32_t OAR2;			//offset:0x0C
	__vo uint32_t DR;			//offset:0x10
	__vo uint32_t SR1;			//offset:0x14
	__vo uint32_t SR2;			//offset:0x18
	__vo uint32_t CCR;			//offset:0x1C
	__vo uint32_t TRISE;		//offset:0x20
	__vo uint32_t FLTR;			//offset:0x24
}I2C_RegDef_t;

typedef struct {
	__vo uint32_t SR;			//offset:0x00
	__vo uint32_t DR;			//offset:0x04
	__vo uint32_t BRR;			//offset:0x08
	__vo uint32_t CR1;			//offset:0x0C
	__vo uint32_t CR2;			//offset:0x10
	__vo uint32_t CR3;			//offset:0x14
	__vo uint32_t GTPR;			//offset:0x18
}USART_RegDef_t;

typedef struct {
	__vo uint32_t CR1;			//offset:0x00
	__vo uint32_t CR2;			//offset:0x04
	__vo uint32_t SMCR;			//offset:0x08
	__vo uint32_t DIER;			//offset:0x0C
	__vo uint32_t SR;			//offset:0x10
	__vo uint32_t EGR;			//offset:0x14
	__vo uint32_t CCMR1;		//offset:0x18
	__vo uint32_t CCMR2;		//offset:0x1C
	__vo uint32_t CCER;			//offset:0x20
	__vo uint32_t CNT;			//offset:0x24
	__vo uint32_t PSC;			//offset:0x28
	__vo uint32_t ARR;			//offset:0x2C
	__vo uint32_t RCR;			//offset:0x30
	__vo uint32_t CCR1;			//offset:0x34
	__vo uint32_t CCR2;			//offset:0x38
	__vo uint32_t CCR3;			//offset:0x3C
	__vo uint32_t CCR4;			//offset:0x40
	__vo uint32_t BDTR;			//offset:0x44
	__vo uint32_t DCR;			//offset:0x48
	__vo uint32_t DMAR;			//offset:0x4C
	__vo uint32_t OR;			//offset:0x50
}TIM_RegDef_t;

typedef struct {
    volatile uint32_t CTRL;   // Offset: 0x00
    volatile uint32_t LOAD;   // Offset: 0x04
    volatile uint32_t VAL;    // Offset: 0x08
    volatile uint32_t CALIB;  // Offset: 0x0C
} SysTick_RegDef_t;


/*
 * Peripheral definitions (peripheral base addresses typecasted to xxx_RegDef_t)
*/
#define GPIOA			((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB			((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC			((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD			((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE			((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOF			((GPIO_RegDef_t*)GPIOF_BASEADDR)
#define GPIOG			((GPIO_RegDef_t*)GPIOG_BASEADDR)
#define GPIOH			((GPIO_RegDef_t*)GPIOH_BASEADDR)
#define GPIOI			((GPIO_RegDef_t*)GPIOI_BASEADDR)

#define RCC				((RCC_RegDef_t*)RCC_BASEADDR)

#define EXTI			((EXTI_RegDef_t*)EXTI_BASEADDR)

#define SYSCFG			((SYSCFG_RegDef_t*)SYSCFG_BASEADDR)

#define SPI1			((SPI_RegDef_t*)SPI1_BASEADDR)
#define SPI2			((SPI_RegDef_t*)SPI2_BASEADDR)
#define SPI3			((SPI_RegDef_t*)SPI3_BASEADDR)
#define SPI4			((SPI_RegDef_t*)SPI4_BASEADDR)

#define I2C1			((I2C_RegDef_t*)I2C1_BASEADDR)
#define I2C2			((I2C_RegDef_t*)I2C2_BASEADDR)
#define I2C3			((I2C_RegDef_t*)I2C3_BASEADDR)

#define USART1 			((USART_RegDef_t*)USART1_BASEADDR)
#define USART2 			((USART_RegDef_t*)USART2_BASEADDR)
#define USART3 			((USART_RegDef_t*)USART3_BASEADDR)
#define UART4 			((USART_RegDef_t*)UART4_BASEADDR)
#define UART5 			((USART_RegDef_t*)UART5_BASEADDR)
#define USART6			((USART_RegDef_t*)USART6_BASEADDR)

#define TIM1			((TIM_RegDef_t*)TIM1_BASEADDR)
#define TIM2			((TIM_RegDef_t*)TIM2_BASEADDR)
#define TIM3			((TIM_RegDef_t*)TIM3_BASEADDR)
#define TIM4			((TIM_RegDef_t*)TIM4_BASEADDR)
#define TIM5			((TIM_RegDef_t*)TIM5_BASEADDR)
#define TIM6			((TIM_RegDef_t*)TIM6_BASEADDR)
#define TIM7			((TIM_RegDef_t*)TIM7_BASEADDR)
#define TIM8			((TIM_RegDef_t*)TIM8_BASEADDR)
#define TIM9			((TIM_RegDef_t*)TIM9_BASEADDR)
#define TIM10			((TIM_RegDef_t*)TIM10_BASEADDR)
#define TIM11			((TIM_RegDef_t*)TIM11_BASEADDR)
#define TIM12			((TIM_RegDef_t*)TIM12_BASEADDR)
#define TIM13			((TIM_RegDef_t*)TIM13_BASEADDR)
#define TIM14			((TIM_RegDef_t*)TIM14_BASEADDR)

#define SysTick             ((SysTick_RegDef_t*)SYSTICK_BASEADDR)


/*
 * Clock Enable Peripherals for GPIOx peripherals
 */

#define GPIOA_PCLK_EN()	(RCC->AHB1ENR |= (1 << 0))
#define GPIOB_PCLK_EN()	(RCC->AHB1ENR |= (1 << 1))
#define GPIOC_PCLK_EN()	(RCC->AHB1ENR |= (1 << 2))
#define GPIOD_PCLK_EN()	(RCC->AHB1ENR |= (1 << 3))
#define GPIOE_PCLK_EN()	(RCC->AHB1ENR |= (1 << 4))
#define GPIOF_PCLK_EN()	(RCC->AHB1ENR |= (1 << 5))
#define GPIOG_PCLK_EN()	(RCC->AHB1ENR |= (1 << 6))
#define GPIOH_PCLK_EN()	(RCC->AHB1ENR |= (1 << 7))
#define GPIOI_PCLK_EN()	(RCC->AHB1ENR |= (1 << 8))

/*
 * Clock Disable Peripherals for GPIOx peripherals
 */
#define GPIOA_PCLK_DI()	(RCC->AHB1ENR &= ~(1 << 0))
#define GPIOB_PCLK_DI()	(RCC->AHB1ENR &= ~(1 << 1))
#define GPIOC_PCLK_DI()	(RCC->AHB1ENR &= ~(1 << 2))
#define GPIOD_PCLK_DI()	(RCC->AHB1ENR &= ~(1 << 3))
#define GPIOE_PCLK_DI()	(RCC->AHB1ENR &= ~(1 << 4))
#define GPIOF_PCLK_DI()	(RCC->AHB1ENR &= ~(1 << 5))
#define GPIOG_PCLK_DI()	(RCC->AHB1ENR &= ~(1 << 6))
#define GPIOH_PCLK_DI()	(RCC->AHB1ENR &= ~(1 << 7))
#define GPIOI_PCLK_DI()	(RCC->AHB1ENR &= ~(1 << 8))


/*
 * Reset for GPIOx peripherals
 * do while help us to do multiple C statement in one macro
 * we reset the bit after set because if the bit stay set we cant use this peripheral again.we must just trigger
 */
#define GPIOA_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0));}while(0)
#define GPIOB_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 1)); (RCC->AHB1RSTR &= ~(1 << 1));}while(0)
#define GPIOC_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 2)); (RCC->AHB1RSTR &= ~(1 << 2));}while(0)
#define GPIOD_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 3)); (RCC->AHB1RSTR &= ~(1 << 3));}while(0)
#define GPIOE_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 4)); (RCC->AHB1RSTR &= ~(1 << 4));}while(0)
#define GPIOF_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 5)); (RCC->AHB1RSTR &= ~(1 << 5));}while(0)
#define GPIOG_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 6)); (RCC->AHB1RSTR &= ~(1 << 6));}while(0)
#define GPIOH_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 7)); (RCC->AHB1RSTR &= ~(1 << 7));}while(0)
#define GPIOI_REG_RESET()	do{(RCC->AHB1RSTR |= (1 << 8)); (RCC->AHB1RSTR &= ~(1 << 8));}while(0)


/*
 * Clock Enable Peripherals for I2Cx peripherals
 */

#define I2C1_PCLK_EN()	(RCC->APB1ENR |= (1 << 21))
#define I2C2_PCLK_EN()	(RCC->APB1ENR |= (1 << 22))
#define I2C3_PCLK_EN()	(RCC->APB1ENR |= (1 << 23))


/*
 * Clock Dısable Peripherals for I2Cx peripherals
 */

#define I2C1_PCLK_DI()	(RCC->APB1ENR &= ~(1 << 21))
#define I2C2_PCLK_DI()	(RCC->APB1ENR &= ~(1 << 22))
#define I2C3_PCLK_DI()	(RCC->APB1ENR &= ~(1 << 23))

/*
 * Reset for I2Cx peripherals
 * do while help us to do multiple C statement in one macro
 * we reset the bit after set because if the bit stay set we cant use this peripheral again.we must just trigger
 */
#define I2C1_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 21)); (RCC->APB1RSTR &= ~(1 << 21));}while(0)
#define I2C2_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 22)); (RCC->APB1RSTR &= ~(1 << 22));}while(0)
#define I2C3_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 23)); (RCC->APB1RSTR &= ~(1 << 23));}while(0)


/*
 * Clock Enable Peripherals for SPIx peripherals
 */

#define SPI1_PCLK_EN()	(RCC->APB2ENR |= (1 << 12))
#define SPI2_PCLK_EN()	(RCC->APB1ENR |= (1 << 14))
#define SPI3_PCLK_EN()	(RCC->APB1ENR |= (1 << 15))
#define SPI4_PCLK_EN()	(RCC->APB2ENR |= (1 << 13))

/*
 * Clock Dısable Peripherals for SPIx peripherals
 */

#define SPI1_PCLK_DI()	(RCC->APB2ENR &= ~(1 << 12))
#define SPI2_PCLK_DI()	(RCC->APB1ENR &= ~(1 << 14))
#define SPI3_PCLK_DI()	(RCC->APB1ENR &= ~(1 << 15))
#define SPI4_PCLK_DI()	(RCC->APB2ENR &= ~(1 << 13))

/*
 * Reset for SPIx peripherals
 * do while help us to do multiple C statement in one macro
 * we reset the bit after set because if the bit stay set we cant use this peripheral again.we must just trigger
 */
#define SPI1_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 12)); (RCC->APB2RSTR &= ~(1 << 12));}while(0)
#define SPI2_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 14)); (RCC->APB1RSTR &= ~(1 << 14));}while(0)
#define SPI3_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 15)); (RCC->APB1RSTR &= ~(1 << 15));}while(0)
#define SPI4_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 13)); (RCC->APB2RSTR &= ~(1 << 13));}while(0)


/*
 * Clock Enable Peripherals for USARTx peripherals
 */

#define USART1_PCLK_EN()	(RCC->APB2ENR |= (1 << 4))
#define USART2_PCLK_EN()	(RCC->APB1ENR |= (1 << 17))
#define USART3_PCLK_EN()	(RCC->APB1ENR |= (1 << 18))
#define USART6_PCLK_EN()	(RCC->APB2ENR |= (1 << 5))
#define UART4_PCLK_EN()		(RCC->APB1ENR |= (1 << 19))
#define UART5_PCLK_EN()		(RCC->APB1ENR |= (1 << 20))

/*
 * Clock Disable Peripherals for USARTx peripherals
 */

#define USART1_PCLK_DI()	(RCC->APB2ENR &= ~(1 << 4))
#define USART2_PCLK_DI()	(RCC->APB1ENR &= ~(1 << 17))
#define USART3_PCLK_DI()	(RCC->APB1ENR &= ~(1 << 18))
#define USART6_PCLK_DI()	(RCC->APB2ENR &= ~(1 << 5))
#define UART4_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 19))
#define UART5_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 20))

/*
 * Reset for USARTx peripherals
 * do while help us to do multiple C statement in one macro
 * we reset the bit after set because if the bit stay set we cant use this peripheral again.we must just trigger
 */
#define USART1_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 4)); (RCC->APB2RSTR &= ~(1 << 4));}while(0)
#define USART2_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 17)); (RCC->APB1RSTR &= ~(1 << 17));}while(0)
#define USART3_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 18)); (RCC->APB1RSTR &= ~(1 << 18));}while(0)
#define UART4_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 19)); (RCC->APB1RSTR &= ~(1 << 19));}while(0)
#define UART5_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 20)); (RCC->APB1RSTR &= ~(1 << 20));}while(0)
#define USART6_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 5)); (RCC->APB2RSTR &= ~(1 << 5));}while(0)

/*
 * Clock Enable Peripherals for TIMx peripherals
 */

#define TIM1_PCLK_EN()		(RCC->APB2ENR |= (1 << 0))
#define TIM2_PCLK_EN()		(RCC->APB1ENR |= (1 << 0))
#define TIM3_PCLK_EN()		(RCC->APB1ENR |= (1 << 1))
#define TIM4_PCLK_EN()		(RCC->APB1ENR |= (1 << 2))
#define TIM5_PCLK_EN()		(RCC->APB1ENR |= (1 << 3))
#define TIM6_PCLK_EN()		(RCC->APB1ENR |= (1 << 4))
#define TIM7_PCLK_EN()		(RCC->APB1ENR |= (1 << 5))
#define TIM8_PCLK_EN()		(RCC->APB2ENR |= (1 << 1))
#define TIM9_PCLK_EN()		(RCC->APB2ENR |= (1 << 16))
#define TIM10_PCLK_EN()		(RCC->APB2ENR |= (1 << 17))
#define TIM11_PCLK_EN()		(RCC->APB2ENR |= (1 << 18))
#define TIM12_PCLK_EN()		(RCC->APB1ENR |= (1 << 6))
#define TIM13_PCLK_EN()		(RCC->APB1ENR |= (1 << 7))
#define TIM14_PCLK_EN()		(RCC->APB1ENR |= (1 << 8))


/*
 * Clock Disable Peripherals for TIMx peripherals
 */

#define TIM1_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 0))
#define TIM2_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 0))
#define TIM3_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 1))
#define TIM4_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 2))
#define TIM5_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 3))
#define TIM6_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 4))
#define TIM7_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 5))
#define TIM8_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 1))
#define TIM9_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 16))
#define TIM10_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 17))
#define TIM11_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 18))
#define TIM12_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 6))
#define TIM13_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 7))
#define TIM14_PCLK_DI()		(RCC->APB1ENR &= ~(1 << 8))

/*
 * Reset for TIMx peripherals
 * do while help us to do multiple C statement in one macro
 * we reset the bit after set because if the bit stay set we cant use this peripheral again.we must just trigger
 */
#define TIM1_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 0)); (RCC->APB2RSTR &= ~(1 << 0));}while(0)
#define TIM2_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 0)); (RCC->APB1RSTR &= ~(1 << 0));}while(0)
#define TIM3_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 1)); (RCC->APB1RSTR &= ~(1 << 1));}while(0)
#define TIM4_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 2)); (RCC->APB1RSTR &= ~(1 << 2));}while(0)
#define TIM5_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 3)); (RCC->APB1RSTR &= ~(1 << 3));}while(0)
#define TIM6_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 4)); (RCC->APB1RSTR &= ~(1 << 4));}while(0)
#define TIM7_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 5)); (RCC->APB1RSTR &= ~(1 << 5));}while(0)
#define TIM8_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 1)); (RCC->APB2RSTR &= ~(1 << 1));}while(0)
#define TIM9_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 16)); (RCC->APB2RSTR &= ~(1 << 16));}while(0)
#define TIM10_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 17)); (RCC->APB2RSTR &= ~(1 << 17));}while(0)
#define TIM11_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 18)); (RCC->APB2RSTR &= ~(1 << 18));}while(0)
#define TIM12_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 6)); (RCC->APB1RSTR &= ~(1 << 6));}while(0)
#define TIM13_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 7)); (RCC->APB1RSTR &= ~(1 << 7));}while(0)
#define TIM14_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 8)); (RCC->APB1RSTR &= ~(1 << 8));}while(0)



/*
 * Clock Enable Peripherals for SYSCFG peripherals
 */
#define SYSCFG_PCLK_EN()	(RCC->APB2ENR |= (1 << 14))

/*
 *find gpıo port macro
 */

#define GPIO_BASEADDR_TO_CODE(x) (((uint32_t)(x) - (AHB1PERIPH_BASE))/1024U)


/*EXTI IRQ numbers*/
#define IRQ_NO_EXTI0		6
#define IRQ_NO_EXTI1		7
#define IRQ_NO_EXTI2		8
#define IRQ_NO_EXTI3		9
#define IRQ_NO_EXTI4		10
#define IRQ_NO_EXTI9_5		23
#define IRQ_NO_EXTI15_10	40

/*SPIx IRQ numbers*/
#define IRQ_NO_SPI1			35
#define IRQ_NO_SPI2			36
#define IRQ_NO_SPI3			51

/*I2Cx IRQ numbers*/
#define IRQ_NO_I2C1_EV		31
#define IRQ_NO_I2C1_ER		32

/*USARTx IRQ numbers*/
#define IRQ_NO_USART1		37
#define IRQ_NO_USART2		38
#define IRQ_NO_USART3		39
#define IRQ_NO_UART4		52
#define IRQ_NO_UART5		53
#define IRQ_NO_USART6		71

//peripheral drivers
#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_spi_driver.h"
#include "stm32f407xx_i2c_driver.h"
#include "stm32f407xx_usart_driver.h"
#include "stm32f407xx_rcc_driver.h"
#include "stm32f407xx_timers_driver.h"

#endif /* INC_STM32F407XX_H_ */
