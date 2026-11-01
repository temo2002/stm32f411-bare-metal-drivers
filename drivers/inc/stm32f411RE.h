#ifndef INC_STM32F411_H_
#define INC_STM32F411_H_

#ifndef __weak
  #define __weak __attribute__((weak))
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>



#define __vo volatile

/***************************************************************
 * 					PROCESSOR SPESIFIC DETAILS
 ***************************************************************
 */


#define NVIC_ISER0 				((__vo uint32_t*) 0xE000E100 )
#define NVIC_ISER1 				((__vo uint32_t*) 0xE000E104 )
#define NVIC_ISER2 				((__vo uint32_t*) 0xE000E108 )
#define NVIC_ISER3 				((__vo uint32_t*) 0xE000E10c )


#define NVIC_ICER0 				((__vo uint32_t*) 0XE000E180 )
#define NVIC_ICER1 				((__vo uint32_t*) 0XE000E184 )
#define NVIC_ICER2 				((__vo uint32_t*) 0XE000E188 )
#define NVIC_ICER3 				((__vo uint32_t*) 0XE000E18c )


#define NVIC_PR_BASEADDR 		((__vo uint32_t*) 0xE000E400 )

#define NO_PR_BITS_IMPLEMENTED		4
/*
 * ********************************************************************
 * --------------------------- MEMORY BASE ---------------------------
 * ********************************************************************
 */
#define FLASH_BASEADDR       0x08000000U   // Startadresse for intern Flash (512 KB maks på F411RE)
#define SRAM1_BASEADDR       0x20000000U   // Startadresse for SRAM (128 KB, ingen SRAM2 på F411RE)
#define ROM                  0x1FFF0000U   // System Memory (BootROM, brukt for ST Bootloader)

/*
 * ********************************************************************
 * --------------------------- BUS BASE ------------------------------
 * ********************************************************************
 * Alle periferi-moduler er mappet inn i minneområdet (Memory mapped I/O).
 * Hver bus har sin egen baseadresse:
 *   - APB1: tregere periferi (timer, I2C, USART2, SPI2/3)
 *   - APB2: raskere periferi (USART1/6, SPI1/4, EXTI, SYSCFG)
 *   - AHB1: høy-båndbredde periferi (GPIO, RCC, DMA)
 *   - AHB2: spesialperiferi (USB OTG FS, RNG)
 */
#define PERIPH_BASEADDR           	0x40000000U  		// Startadresse for Peripheral-området
#define APB1PERIPH_BASEADDR       	PERIPH_BASEADDR  	// APB1-bus starter her (timer, I2C, USART2 etc.)
#define APB2PERIPH_BASEADDR      	0x40010000U   		// APB2-bus (USART1, USART6, SPI1, EXTI, SYSCFG etc.)
#define AHB1PERIPH_BASEADDR      	0x40020000U   		// AHB1-bus (GPIO, DMA, RCC etc.)
#define AHB2PERIPH_BASEADDR       	0x50000000U   		// AHB2-bus (USB OTG FS, RNG)

/*
 * ********************************************************************
 * --------------------------- GPIO BASE -----------------------------
 * ********************************************************************
 * Alle GPIO-porter i F411RE ligger på AHB1-bussen.
 * Hver port har et "gap" på 0x400 fra forrige.
 */
#define GPIOA_BASEADDR       (AHB1PERIPH_BASEADDR + 0x0000)  // GPIO Port A
#define GPIOB_BASEADDR       (AHB1PERIPH_BASEADDR + 0x0400)  // GPIO Port B
#define GPIOC_BASEADDR       (AHB1PERIPH_BASEADDR + 0x0800)  // GPIO Port C
#define GPIOD_BASEADDR       (AHB1PERIPH_BASEADDR + 0x0C00)  // GPIO Port D
#define GPIOE_BASEADDR       (AHB1PERIPH_BASEADDR + 0x1000)  // GPIO Port E
#define GPIOH_BASEADDR       (AHB1PERIPH_BASEADDR + 0x1C00)  // GPIO Port H
#define RCC_BASEADDR         (AHB1PERIPH_BASEADDR + 0x3800)  // Reset and Clock Control (klokkestyring)
// Merk: F411RE har ikke GPIOF eller GPIOG



/*
 * --------------------I2C--------------------
 */
#define I2C1_BASEADDR        (APB1PERIPH_BASEADDR + 0x5400)  // I2C1
#define I2C2_BASEADDR        (APB1PERIPH_BASEADDR + 0x5800)  // I2C2
#define I2C3_BASEADDR        (APB1PERIPH_BASEADDR + 0x5C00)  // I2C3


/*
 * --------------------SPI--------------------
 */
#define SPI1_BASEADDR        (APB2PERIPH_BASEADDR + 0x3000)  	// SPI1 (også I2S1)
#define SPI2_BASEADDR        (APB1PERIPH_BASEADDR + 0x3800)  	// SPI2 (også I2S2)
#define SPI3_BASEADDR        (APB1PERIPH_BASEADDR + 0x3C00)  	// SPI3 (også I2S3)
#define SPI4_BASEADDR        (APB2PERIPH_BASEADDR + 0x3400)  	// SPI4
#define SPI5_BASEADDR 		 (APB2PERIPH_BASEADDR + 0x5000)	 	// SPI5

/*
 * --------------------USART--------------------
 */

#define USART1_BASEADDR      (APB2PERIPH_BASEADDR + 0x1000)  // USART1
#define USART2_BASEADDR      (APB1PERIPH_BASEADDR + 0x4400)  // USART2
#define USART6_BASEADDR      (APB2PERIPH_BASEADDR + 0x1400)  // USART6


/*
 * --------------------OTHERS--------------------
 */
#define SYSCFG_BASEADDR      (APB2PERIPH_BASEADDR + 0x3800)  // System Configuration Controller
#define EXTI_BASEADDR        (APB2PERIPH_BASEADDR + 0x3C00)  // External Interrupt/Event Controller



/*
 * ********************************************************************
 * --------------------------- GPIO REGISTER STRUCT ------------------
 * ********************************************************************
 * Datasheet ref: RM0383, kapittel 8 (General-purpose I/Os)
 * Hver GPIO-port har disse registrene, i gitt rekkefølge.
 */
typedef struct
{
	__vo uint32_t MODER;    // GPIO port mode register           (offset 0x00)
	__vo uint32_t OTYPER;   // GPIO port output type register    (offset 0x04)
	__vo uint32_t OSPEEDR;  // GPIO port output speed register   (offset 0x08)
	__vo uint32_t PUPDR;    // GPIO port pull-up/pull-down reg   (offset 0x0C)
	__vo uint32_t IDR;      // GPIO port input data register     (offset 0x10)
	__vo uint32_t ODR;      // GPIO port output data register    (offset 0x14)
	__vo uint32_t BSRR;     // GPIO port bit set/reset register  (offset 0x18)
	__vo uint32_t LCKR;     // GPIO port configuration lock reg  (offset 0x1C)
	__vo uint32_t AFR[2];   // AFR[0] = low  (pins 0–7), AFR[1] = high (pins 8–15) (offset 0x20, 0x24)
} GPIO_RegDef_t;


/*
 * ********************************************************************
 * --------------------------- RCC REGISTER STRUCT -------------------
 * ********************************************************************
 * Datasheet ref: RM0383, kapittel 6 (Reset and Clock Control)
 * Brukes for å enable/disable klokker til periferi, resette moduler,
 * konfigurere systemklokker og PLL.
 */
typedef struct
{
	__vo uint32_t CR;          // Clock control register                      (0x00)
	__vo uint32_t PLLCFGR;     // PLL configuration register                  (0x04)
	__vo uint32_t CFGR;        // Clock configuration register                (0x08)
	__vo uint32_t CIR;         // Clock interrupt register                    (0x0C)
	__vo uint32_t AHB1RSTR;    // AHB1 peripheral reset register              (0x10)
	__vo uint32_t AHB2RSTR;    // AHB2 peripheral reset register              (0x14)
	uint32_t Reserved1[2];     // 0x18, 0x1C (ikke brukt)
	__vo uint32_t APB1RSTR;    // APB1 peripheral reset register              (0x20)
	__vo uint32_t APB2RSTR;    // APB2 peripheral reset register              (0x24)
	uint32_t Reserved2[2];     // 0x28, 0x2C (ikke brukt)
	__vo uint32_t AHB1ENR;     // AHB1 peripheral clock enable register       (0x30)
	__vo uint32_t AHB2ENR;     // AHB2 peripheral clock enable register       (0x34)
	uint32_t Reserved3[2];     // 0x38, 0x3C (ikke brukt)
	__vo uint32_t APB1ENR;     // APB1 peripheral clock enable register       (0x40)
	__vo uint32_t APB2ENR;     // APB2 peripheral clock enable register       (0x44)
	uint32_t Reserved4[2];     // 0x48, 0x4C (ikke brukt)
	__vo uint32_t AHB1LPENR;   // AHB1 low power enable register              (0x50)
	__vo uint32_t AHB2LPENR;   // AHB2 low power enable register              (0x54)
	uint32_t Reserved5[2];     // 0x58, 0x5C (ikke brukt)
	__vo uint32_t APB1LPENR;   // APB1 low power enable register              (0x60)
	__vo uint32_t APB2LPENR;   // APB2 low power enable register              (0x64)
	uint32_t Reserved6[2];     // 0x68, 0x6C (ikke brukt)
	__vo uint32_t BDCR;        // Backup domain control register              (0x70)
	__vo uint32_t CSR;         // Clock control & status register             (0x74)
	uint32_t Reserved7[2];     // 0x78, 0x7C (ikke brukt)
	__vo uint32_t SSCGR;       // Spread spectrum clock generation register   (0x80)
	__vo uint32_t PLLI2SCFGR;  // PLLI2S configuration register               (0x84)
	__vo uint32_t DCKCFGR;     // Dedicated clock configuration register      (0x8C)
} RCC_RegDef_t;


/*
 * **************************************************************************************************
 * --------------------------- PERIPHERAL REGISTER DEFENITIONS --------------------------------------
 * **************************************************************************************************
 * Peripheral register definition structure for EXTI
 */
typedef struct
{
	__vo uint32_t IMR;
	__vo uint32_t EMR;
	__vo uint32_t RTSR;
	__vo uint32_t FTSR;
	__vo uint32_t SWIER;
	__vo uint32_t PR;
} EXTI_RegDef_t;



/*
 * Peripheral register definition structure for SYSCFG
 */
typedef struct
{
    __vo uint32_t MEMRMP;      // 0x00
    __vo uint32_t PMC;         // 0x04
    __vo uint32_t EXTICR[4];   // 0x08, 0x0C, 0x10, 0x14
    uint32_t RESERVED1[2];     // 0x18, 0x1C (må fylles inn for å hoppe riktig til neste offset)
    __vo uint32_t CMPCR;       // 0x20
} SYSCFG_RegDef_t;



/*
 * Peripheral register definition structure for I2C
 */
typedef struct
{
	__vo uint32_t CR1;
	__vo uint32_t CR2;
	__vo uint32_t OAR1;
	__vo uint32_t OAR2;
	__vo uint32_t DR;
	__vo uint32_t SR1;
	__vo uint32_t SR2;
	__vo uint32_t CCR;
	__vo uint32_t TRISE;
	__vo uint32_t FLTR;
}I2C_RegDef_t;



/*
 * Peripheral register definition structure for SPI
 */
typedef struct
{
	__vo uint32_t	CR1;
	__vo uint32_t 	CR2;
	__vo uint32_t 	SR;
	__vo uint32_t 	DR;
	__vo uint32_t 	CRCPR;
	__vo uint32_t 	RXCRCR;
	__vo uint32_t 	TXCRCR;
	__vo uint32_t 	I2SCFGR;
	__vo uint32_t 	I2SPR;
}SPI_RegDef_t;



/*
 * Peripheral register definition structure for USART
 */
typedef struct
{
	__vo uint32_t SR;
	__vo uint32_t DR;
	__vo uint32_t BRR;
	__vo uint32_t CR1;
	__vo uint32_t CR2;
	__vo uint32_t CR3;
	__vo uint32_t GTPR;
}USART_RegDef_t;



/*
 * ********************************************************************
 * --------------------------- PERIPHERAL DEFINITIONS ----------------
 * ********************************************************************
 */

/* ---------------- GPIO ---------------- */
#define GPIOA        ((GPIO_RegDef_t*) GPIOA_BASEADDR)
#define GPIOB        ((GPIO_RegDef_t*) GPIOB_BASEADDR)
#define GPIOC        ((GPIO_RegDef_t*) GPIOC_BASEADDR)
#define GPIOD        ((GPIO_RegDef_t*) GPIOD_BASEADDR)
#define GPIOE        ((GPIO_RegDef_t*) GPIOE_BASEADDR)
#define GPIOH        ((GPIO_RegDef_t*) GPIOH_BASEADDR)

/* ---------------- RCC, SYSCFG, EXTI, PWR ---------------- */
#define RCC          ((RCC_RegDef_t*) RCC_BASEADDR)
#define EXTI         ((EXTI_RegDef_t*) EXTI_BASEADDR)
#define SYSCFG       ((SYSCFG_RegDef_t*) SYSCFG_BASEADDR)

/* ---------------- I2C ---------------- */
#define I2C1         ((I2C_RegDef_t*) I2C1_BASEADDR)
#define I2C2         ((I2C_RegDef_t*) I2C2_BASEADDR)
#define I2C3         ((I2C_RegDef_t*) I2C3_BASEADDR)

/* ---------------- SPI ---------------- */
#define SPI1         ((SPI_RegDef_t*) SPI1_BASEADDR)
#define SPI2         ((SPI_RegDef_t*) SPI2_BASEADDR)
#define SPI3         ((SPI_RegDef_t*) SPI3_BASEADDR)
#define SPI4		 ((SPI_RegDef_t*) SPI4_BASEADDR)
#define SPI5		 ((SPI_RegDef_t*) SPI5_BASEADDR)


/* ---------------- USART ---------------- */
#define USART1 		((USART_RegDef_t*) USART1_BASEADDR)
#define USART2 		((USART_RegDef_t*) USART2_BASEADDR)
#define USART6 		((USART_RegDef_t*) USART6_BASEADDR)

/*
 * ********************************************************************
 * --------------------------- CLOCK ENABLE MACROS -------------------
 * ********************************************************************
 */

/* ---------------- GPIO (AHB1) ---------------- */
#define GPIOA_PCLK_EN()   (RCC->AHB1ENR |= (1 << 0))
#define GPIOB_PCLK_EN()   (RCC->AHB1ENR |= (1 << 1))
#define GPIOC_PCLK_EN()   (RCC->AHB1ENR |= (1 << 2))
#define GPIOD_PCLK_EN()   (RCC->AHB1ENR |= (1 << 3))
#define GPIOE_PCLK_EN()   (RCC->AHB1ENR |= (1 << 4))
#define GPIOH_PCLK_EN()   (RCC->AHB1ENR |= (1 << 7))

/* ---------------- RCC / System ---------------- */
#define CRC_PCLK_EN()     (RCC->AHB1ENR |= (1 << 12))
#define DMA1_PCLK_EN()    (RCC->AHB1ENR |= (1 << 21))
#define DMA2_PCLK_EN()    (RCC->AHB1ENR |= (1 << 22))
#define OTGFS_PCLK_EN()   (RCC->AHB2ENR |= (1 << 7))
#define PWR_PCLK_EN()     (RCC->APB1ENR |= (1 << 28))
#define SYSCFG_PCLK_EN()  (RCC->APB2ENR |= (1 << 14))

/* ---------------- I2C (APB1) ---------------- */
#define I2C1_PCLK_EN()    (RCC->APB1ENR |= (1 << 21))
#define I2C2_PCLK_EN()    (RCC->APB1ENR |= (1 << 22))
#define I2C3_PCLK_EN()    (RCC->APB1ENR |= (1 << 23))

/* ---------------- SPI ---------------- */
#define SPI1_PCLK_EN()    (RCC->APB2ENR |= (1 << 12))
#define SPI2_PCLK_EN()    (RCC->APB1ENR |= (1 << 14))
#define SPI3_PCLK_EN()    (RCC->APB1ENR |= (1 << 15))
#define SPI4_PCLK_EN()    (RCC->APB2ENR |= (1 << 13))
#define SPI5_PCLK_EN()    (RCC->APB2ENR |= (1 << 20))

/* ---------------- USART ---------------- */
#define USART1_PCLK_EN()  (RCC->APB2ENR |= (1 << 4))
#define USART2_PCLK_EN()  (RCC->APB1ENR |= (1 << 17))
#define USART6_PCLK_EN()  (RCC->APB2ENR |= (1 << 5))

/* ---------------- Timers ---------------- */
#define TIM1_PCLK_EN()    (RCC->APB2ENR |= (1 << 0))
#define TIM2_PCLK_EN()    (RCC->APB1ENR |= (1 << 0))
#define TIM3_PCLK_EN()    (RCC->APB1ENR |= (1 << 1))
#define TIM4_PCLK_EN()    (RCC->APB1ENR |= (1 << 2))
#define TIM5_PCLK_EN()    (RCC->APB1ENR |= (1 << 3))
#define TIM9_PCLK_EN()    (RCC->APB2ENR |= (1 << 16))
#define TIM10_PCLK_EN()   (RCC->APB2ENR |= (1 << 17))
#define TIM11_PCLK_EN()   (RCC->APB2ENR |= (1 << 18))

/* ---------------- ADC / SDIO ---------------- */
#define ADC1_PCLK_EN()    (RCC->APB2ENR |= (1 << 8))
#define SDIO_PCLK_EN()    (RCC->APB2ENR |= (1 << 11))


/*
 * ********************************************************************
 * --------------------------- CLOCK DISABLE MACROS ------------------
 * ********************************************************************
 */

/* ---------------- GPIO (AHB1) ---------------- */
#define GPIOA_PCLK_DI()   (RCC->AHB1ENR &= ~(1 << 0))
#define GPIOB_PCLK_DI()   (RCC->AHB1ENR &= ~(1 << 1))
#define GPIOC_PCLK_DI()   (RCC->AHB1ENR &= ~(1 << 2))
#define GPIOD_PCLK_DI()   (RCC->AHB1ENR &= ~(1 << 3))
#define GPIOE_PCLK_DI()   (RCC->AHB1ENR &= ~(1 << 4))
#define GPIOH_PCLK_DI()   (RCC->AHB1ENR &= ~(1 << 7))

/* ---------------- RCC / System ---------------- */
#define CRC_PCLK_DI()     (RCC->AHB1ENR &= ~(1 << 12))
#define DMA1_PCLK_DI()    (RCC->AHB1ENR &= ~(1 << 21))
#define DMA2_PCLK_DI()    (RCC->AHB1ENR &= ~(1 << 22))
#define OTGFS_PCLK_DI()   (RCC->AHB2ENR &= ~(1 << 7))
#define PWR_PCLK_DI()     (RCC->APB1ENR &= ~(1 << 28))
#define SYSCFG_PCLK_DI()  (RCC->APB2ENR &= ~(1 << 14))

/* ---------------- I2C (APB1) ---------------- */
#define I2C1_PCLK_DI()    (RCC->APB1ENR &= ~(1 << 21))
#define I2C2_PCLK_DI()    (RCC->APB1ENR &= ~(1 << 22))
#define I2C3_PCLK_DI()    (RCC->APB1ENR &= ~(1 << 23))

/* ---------------- SPI ---------------- */
#define SPI1_PCLK_DI()    (RCC->APB2ENR &= ~(1 << 12))
#define SPI2_PCLK_DI()    (RCC->APB1ENR &= ~(1 << 14))
#define SPI3_PCLK_DI()    (RCC->APB1ENR &= ~(1 << 15))
#define SPI4_PCLK_DI()    (RCC->APB2ENR &= ~(1 << 13))
#define SPI5_PCLK_DI()    (RCC->APB2ENR &= ~(1 << 20))

/* ---------------- USART ---------------- */
#define USART1_PCLK_DI()  (RCC->APB2ENR &= ~(1 << 4))
#define USART2_PCLK_DI()  (RCC->APB1ENR &= ~(1 << 17))
#define USART6_PCLK_DI()  (RCC->APB2ENR &= ~(1 << 5))

/* ---------------- Timers ---------------- */
#define TIM1_PCLK_DI()    (RCC->APB2ENR &= ~(1 << 0))
#define TIM2_PCLK_DI()    (RCC->APB1ENR &= ~(1 << 0))
#define TIM3_PCLK_DI()    (RCC->APB1ENR &= ~(1 << 1))
#define TIM4_PCLK_DI()    (RCC->APB1ENR &= ~(1 << 2))
#define TIM5_PCLK_DI()    (RCC->APB1ENR &= ~(1 << 3))
#define TIM9_PCLK_DI()    (RCC->APB2ENR &= ~(1 << 16))
#define TIM10_PCLK_DI()   (RCC->APB2ENR &= ~(1 << 17))
#define TIM11_PCLK_DI()   (RCC->APB2ENR &= ~(1 << 18))

/* ---------------- ADC / SDIO ---------------- */
#define ADC1_PCLK_DI()    (RCC->APB2ENR &= ~(1 << 8))
#define SDIO_PCLK_DI()    (RCC->APB2ENR &= ~(1 << 11))

/* ---------------- Watchdog ---------------- */
#define WWDG_PCLK_EN()    (RCC->APB1ENR |= (1 << 11))
#define WWDG_PCLK_DI()    (RCC->APB1ENR &= ~(1 << 11))



/*
 * Register reset Macros for GPIOx Peripherals
 */
#define GPIOA_REG_RESET() 	do{(RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); } while(0)
#define GPIOB_REG_RESET() 	do{(RCC->AHB1RSTR |= (1 << 1)); (RCC->AHB1RSTR &= ~(1 << 1)); } while(0)
#define GPIOC_REG_RESET() 	do{(RCC->AHB1RSTR |= (1 << 2)); (RCC->AHB1RSTR &= ~(1 << 2)); } while(0)
#define GPIOD_REG_RESET() 	do{(RCC->AHB1RSTR |= (1 << 3)); (RCC->AHB1RSTR &= ~(1 << 3)); } while(0)
#define GPIOE_REG_RESET() 	do{(RCC->AHB1RSTR |= (1 << 4)); (RCC->AHB1RSTR &= ~(1 << 4)); } while(0)
#define GPIOH_REG_RESET() 	do{(RCC->AHB1RSTR |= (1 << 7)); (RCC->AHB1RSTR &= ~(1 << 7)); } while(0)


/*
 * Register reset Macros for I2Cx Peripherals
 */
#define I2C1_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 21)); (RCC->APB1RSTR &= ~(1 << 21)); } while (0)
#define I2C2_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 22)); (RCC->APB1RSTR &= ~(1 << 22)); } while (0)
#define I2C3_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 23)); (RCC->APB1RSTR &= ~(1 << 23)); } while (0)


/*
 * Register reset Macros for SPIx Peripherals
 */
#define SPI1_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 12)); (RCC->APB2RSTR &= ~(1 << 12)); } while (0)
#define SPI2_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 14)); (RCC->APB1RSTR &= ~(1 << 14)); } while (0)
#define SPI3_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 15)); (RCC->APB1RSTR &= ~(1 << 15)); } while (0)
#define SPI4_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 13)); (RCC->APB2RSTR &= ~(1 << 13)); } while (0)
#define SPI5_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 20)); (RCC->APB2RSTR &= ~(1 << 20)); } while (0)

/*
 * Register reset Macros for USARTx Peripherals
 */
#define USART1_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 4 )); (RCC->APB2RSTR &= ~(1 << 4 )); } while (0)
#define USART2_REG_RESET()	do{(RCC->APB1RSTR |= (1 << 17)); (RCC->APB1RSTR &= ~(1 << 17)); } while (0)
#define USART6_REG_RESET()	do{(RCC->APB2RSTR |= (1 << 5 )); (RCC->APB2RSTR &= ~(1 << 5 )); } while (0)



#define GPIO_BASEADDR_TO_CODE(x)   ((x == GPIOA) ? 0 :\
                                    (x == GPIOB) ? 1 :\
                                    (x == GPIOC) ? 2 :\
                                    (x == GPIOD) ? 3 :\
                                    (x == GPIOE) ? 4 :\
                                    (x == GPIOH) ? 7 : 0)


/*********************************************************************
 * 							Some generic macros
 * *******************************************************************
 */
#define ENABLE 			1
#define DISABLE 		0
#define SET 			ENABLE
#define RESET 			DISABLE
#define GPIO_PIN_SET 	SET
#define GPIO_PIN_RESET 	RESET
#define FLAG_SET 		SET
#define FLAG_RESET		RESET


/*****************************************************
 *  				MACROS -> INTERRUPT
 *****************************************************/
#define IRQ_NO_EXTI0 		6
#define IRQ_NO_EXTI1 		7
#define IRQ_NO_EXTI2 		8
#define IRQ_NO_EXTI3 		9
#define IRQ_NO_EXTI4 		10
#define IRQ_NO_EXTI9_5 		23
#define IRQ_NO_EXTI15_10	40
#define IRQ_NO_SPI1			35
#define IRQ_NO_SPI2			36
#define IRQ_NO_SPI3			51
#define IRQ_NO_SPI4
#define IRQ_NO_I2C1_EV		31
#define IRQ_NO_I2C1_ER		32
#define IRQ_NO_I2C2_EV		33
#define IRQ_NO_I2C2_ER		34
#define IRQ_NO_I2C3_EV		72
#define IRQ_NO_I2C3_ER		73
#define NVIC_IRQ_PRI0 		0
#define NVIC_IRQ_PRI15 		15


/*
 * ***************************************************************************************
 * --------------------------- BIT POSITION DEFENITIONS OF I2C PERIPHERAL ----------------
 * ***************************************************************************************
 */


/*
 * Bit position definitions for I2C_CR1
 */

#define I2C_CR1_PE 				0
#define I2C_CR1_NOSTRETCH 		7
#define I2C_CR1_START 			8
#define I2C_CR1_STOP			9
#define I2C_CR1_ACK				10
#define I2C_CR1_SWRST			15

/*
 * Bit position definitions for I2C_CR2
 */
#define I2C_CR2_FREQ 			0
#define I2C_CR2_ITERREN 		8
#define I2C_CR2_ITEVTEN 		9
#define I2C_CR2_ITBUFEN			10

/*
 * Bit position definitions for I2C_OAR1
 */
#define I2C_OAR1_ADD0 			0
#define I2C_OAR1_ADD71			1
#define I2C_OAR1_ADD98 			8
#define I2C_OAR1_ADDMODE 		15

/*
 * Bit position definitions for I2C_SR1
 */
#define I2C_SR1_SB	 			0
#define I2C_SR1_ADDR	 		1
#define I2C_SR1_BTF	 			2
#define I2C_SR1_STOPF	 		4
#define I2C_SR1_RXNE	 		6
#define I2C_SR1_TXE	 			7
#define I2C_SR1_BERR	 		8
#define I2C_SR1_ARLO	 		9
#define I2C_SR1_AF	 			10
#define I2C_SR1_OVR	 			11
#define I2C_SR1_TIMEOUT 		14

/*
 * Bit position definitions for I2C_SR2
 */
#define I2C_SR2_MSL	 			0
#define I2C_SR2_BUSY	 		1
#define I2C_SR2_TRA	 			2
#define I2C_SR2_GENCALL	 		4
#define I2C_SR2_DUALF	 		7

/*
 * Bit position definitions for I2C_CCR
 */
#define I2C_CCR_CCR				0
#define I2C_CCR_DUTY			14
#define I2C_CCR_FS				15


/*
 * ***************************************************************************************
 * --------------------------- BIT POSITION DEFENITIONS OF SPI PERIPHERAL --------------
 * ***************************************************************************************
 */

/*
 * Bit position definitions for SPI control register 1 (SPI_CR1)
 */
#define SPI_CR1_CPHA 					0
#define SPI_CR1_CPOL 					1
#define SPI_CR1_MSTR 					2
#define SPI_CR1_BR	 					3
#define SPI_CR1_SPE 					6
#define SPI_CR1_LSB_FIRST 				7
#define SPI_CR1_SSI 					8
#define SPI_CR1_SSM 					9
#define SPI_CR1_RX_ONLY 				10
#define SPI_CR1_DFF 					11
#define SPI_CR1_CRC_NEXT 				12
#define SPI_CR1_CRC_EN 					13
#define SPI_CR1_BIDI_OE 				14
#define SPI_CR1_BIDI_MODE 				15


/*
 * Bit position definitions for SPI control register 2 (SPI_CR2)
 */
#define SPI_CR2_RXDMAEN 				0
#define SPI_CR2_TXDMAEN 				1
#define SPI_CR2_SSOE 					2
#define SPI_CR2_FRF 					4
#define SPI_CR2_ERRIE 					5
#define SPI_CR2_RXNEIE 					6
#define SPI_CR2_TXEIE					7


/*
 * Bit position definitions for SPI status register (SPI_SR)
 */
#define SPI_SR_RXNE 					0
#define SPI_SR_TXE 						1
#define SPI_SR_CHSIDE 					2
#define SPI_SR_UDR 						3
#define SPI_SR_CRC_ERR 					4
#define SPI_SR_MODF 					5
#define SPI_SR_OVR						6
#define SPI_SR_BSY						7
#define SPI_SR_FRE						8


/*
 * Bit position definitions for SPI data register (SPI_DR)
 */
#define SPI_DR_DR15_0 					0


/*
 * Bit position definitions for SPI CRC polynomial register (SPI_CRCPR)
 */
#define SPI_CRCPR_CRCPOLY15_0 			0


/*
 * Bit position definitions for SPI RX CRC register (SPI_RXCRCR)(
 */
#define SPI_RXCRCR_RXCRC15_0 				0


/*
 * Bit position definitions for SPI TX CRC register (SPI_TXCRCR)
 */
#define SPI_TXCRCR_TXCRC15_0 				0


/*
 * Bit position definitions for SPI_I2S configuration register (SPI_I2SCFGR)
 */
#define SPI_I2SCFGR_CHLEN 					0
#define SPI_I2SCFGR_DATLEN 					1
#define SPI_I2SCFGR_CKPOL 					3
#define SPI_I2SCFGR_I2SSTD 					4
#define SPI_I2SCFGR_PCMSY_NC 				7
#define SPI_I2SCFGR_I2SCFG 					8
#define SPI_I2SCFGR_I2SE 					10
#define SPI_I2SCFGR_I2SMOD 					11


/*
 * Bit position definitions for SPI_I2S prescaler register (SPI_I2SPR)
 */
#define SPI_I2SPR_I2SDIV 					0
#define SPI_I2SPR_ODD 					8
#define SPI_I2SPR_MCKOE 					9



/*
 * ***************************************************************************************
 * --------------------------- BIT POSITION DEFENITIONS OF USART PERIPHERAL --------------
 * ***************************************************************************************
 */

/*
 * Bit position definitions for Status register (USART_SR)
 */

#define USART_SR_PE 			0
#define USART_SR_FE 			1
#define USART_SR_NF 			2
#define USART_SR_ORE 			3
#define USART_SR_IDLE 			4
#define USART_SR_RXNE 			5
#define USART_SR_TC 			6
#define USART_SR_TXE 			7
#define USART_SR_LBD			8
#define USART_SR_CTS			9


/*
 * Bit position definitions for Data register (USART_DR)
 */

#define USART_BRR_DIVFRAC 		0
#define USART_BRR_DIVMANT 		4


/*
 * Bit position definitions for Control register 1 (USART_CR1)
 */
#define USART_CR1_SBK 			0
#define USART_CR1_RWU			1
#define USART_CR1_RE			2
#define USART_CR1_TE 			3
#define USART_CR1_IDLEIE 		4
#define USART_CR1_RXNEIE 		5
#define USART_CR1_TCIE 			6
#define USART_CR1_TXEIE 		7
#define USART_CR1_PEIE 			8
#define USART_CR1_PS 			9
#define USART_CR1_PCE			10
#define USART_CR1_WAKE			11
#define USART_CR1_M 			12
#define USART_CR1_UE			13
#define USART_CR1_OVER8 		15



/*
 * Bit position definitions for Control register 2 (USART_CR2)
 */
#define USART_CR2_ADD 			0
#define USART_CR2_LBDL 			5
#define USART_CR2_LBDIE 		6
#define USART_CR2_LBCL 			8
#define USART_CR2_CPHA 			9
#define USART_CR2_CPOL 			10
#define USART_CR2_CLKEN 		11
#define USART_CR2_STOP 			12
#define USART_CR2_LINEN 		14


/*
 * Bit position definitions for Control register 3 (USART_CR3)
 */
#define USART_CR3_EIE 			0
#define USART_CR3_IREN 			1
#define USART_CR3_IRLP 			2
#define USART_CR3_HDSEL 		3
#define USART_CR3_NACK 			4
#define USART_CR3_SCEN 			5
#define USART_CR3_DMAR 			6
#define USART_CR3_DMAT 			7
#define USART_CR3_RTSE 			8
#define USART_CR3_CTSE 			9
#define USART_CR3_CTSIE 		10
#define USART_CR3_ONEBIT 		11


/*
 * Bit position definitions for Guard time and prescaler register (USART_GTPR)
 */
#define USART_GTPR_PSC 			0
#define USART_GTPR_GT 			8




/*
 * ********************************************************************
 * --------------------------- END OF FILE ---------------------------
 * ********************************************************************
 */

#include "stm32f411_gpio_driver.h"
#include "stm32f411_spi_driver.h"
#include "stm32f411_i2c_driver.h"
#include "stm32f411_usart_driver.h"
#include "stm32f411_rcc_driver.h"

#endif /* INC_STM32F411_H_ */
