/*
 * stm32f411_gpio_driver.c
 *
 *  Created on: Aug 28, 2025
 *      Author: Temmam Altaie
 */
#include "stm32f411_gpio_driver.h"

/*
 * *********************************************************************
 * 					APIs Supported by this driver
 * *********************************************************************
 */



/*
 * Peripheral Clock setup
 */
/***************************************************************
 * @fn       - GPIO_PeriphClockControl
 *
 * @brief    - This function enables or disables peripheral clock
 *             for the given GPIO port
 *
 * @GPIO_RegDef_t *pGPIOx - base address of the GPIO peripheral
 * @uint8_t ENorDI - ENABLE or DISABLE macros
 *
 * @return   - none
 *
 * @Note     - none
 *
 **************************************************************/

void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t ENorDI)
{
    if(ENorDI == ENABLE)
    {
        if(pGPIOx == GPIOA)
        {
            GPIOA_PCLK_EN();
        }
        else if(pGPIOx == GPIOB)
        {
            GPIOB_PCLK_EN();
        }
        else if(pGPIOx == GPIOC)
        {
            GPIOC_PCLK_EN();
        }
        else if(pGPIOx == GPIOD)
        {
            GPIOD_PCLK_EN();
        }
        else if(pGPIOx == GPIOE)
        {
            GPIOE_PCLK_EN();
        }
        else if(pGPIOx == GPIOH)
        {
            GPIOH_PCLK_EN();
        }
    }
    else // DISABLE
    {
        if(pGPIOx == GPIOA)
        {
            GPIOA_PCLK_DI();
        }
        else if(pGPIOx == GPIOB)
        {
            GPIOB_PCLK_DI();
        }
        else if(pGPIOx == GPIOC)
        {
            GPIOC_PCLK_DI();
        }
        else if(pGPIOx == GPIOD)
        {
            GPIOD_PCLK_DI();
        }
        else if(pGPIOx == GPIOE)
        {
            GPIOE_PCLK_DI();
        }
        else if(pGPIOx == GPIOH)
        {
            GPIOH_PCLK_DI();
        }
    }
}



/*
 * GPIO Init
 */
/***************************************************************
 * @fn       - GPIO_Init
 *
 * @brief    - Configures a specific GPIO pin according to the
 *             settings in the GPIO handle. This includes mode,
 *             speed, output type, pull-up/pull-down, and (if
 *             applicable) alternate function. If interrupt mode
 *             is selected, the EXTI registers are also configured.
 *
 * @param[in] pGPIOHandle - Pointer to a GPIO handle structure
 *                          containing the base address of the
 *                          GPIO port and pin configuration.
 *
 * @return   - none
 *
 * @note     - The peripheral clock for the port must be enabled
 *             before calling this function.
 *
 **************************************************************/

void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp=0;

	//enable the peripheral clock
	GPIO_PeriClockControl(pGPIOHandle->pGPIOx, ENABLE);

	//1. Configure the mode of gpio pin

	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
		//The non interrupt mode
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 *pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER &= ~( 0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); //clearing
		pGPIOHandle->pGPIOx->MODER |= temp; //setting
	}else
	{
		 if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
		 {

			 EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber );

			 EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber );

		 }else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		 {
			 EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber );

			 EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber );

		 }else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		 {
			 EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber );

			 EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber );
		 }

		 uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;
		 uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
		 uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		 SYSCFG_PCLK_EN();
		 SYSCFG->EXTICR[temp1] = portcode << (temp2 * 4);

		 EXTI->IMR |= 1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;
	}

	temp = 0;

	//2. Configure the speed
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR &= ~( 0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); //clearing
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

	temp = 0;
	//3. Configure the pupd settings
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR &= ~( 0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); //clearing
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	temp = 0;

	//4. Configure the optype
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OTYPER &= ~( 0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //clearing
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	temp = 0;

	//5. Configure the alt functionality
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	{
		//Configure the alt function registers.
		uint8_t temp1, temp2;

		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;
		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << ( 4 * temp2));
		pGPIOHandle->pGPIOx->AFR[temp1] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << ( 4 * temp2));
	}
}



/*
 * GPIO DeInit
 */
/***************************************************************
 * @fn       - GPIO_DeInit
 *
 * @brief    - Resets all registers of the specified GPIO port
 *             to their default reset values.
 *
 * @param[in] pGPIOx - Base address of the GPIO port to reset
 *
 * @return   - None
 *
 * @note     - This function uses the RCC AHB1 peripheral reset
 *             register to perform the reset.
 *
 **************************************************************/

void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if(pGPIOx == GPIOA)
	        {
	            GPIOA_REG_RESET();
	        }
	else if(pGPIOx == GPIOB)
	        {
				GPIOB_REG_RESET();
	        }
	else if(pGPIOx == GPIOC)
	        {
				GPIOC_REG_RESET();
	        }
	else if(pGPIOx == GPIOD)
	        {
				GPIOD_REG_RESET();
	        }
	else if(pGPIOx == GPIOE)
	        {
				GPIOE_REG_RESET();
	        }
	else if(pGPIOx == GPIOH)
	        {
				GPIOH_REG_RESET();
	        }
}



/*
 * Read from input pin
 */
/***************************************************************
 * @fn       - GPIO_ReadFromInputPin
 *
 * @brief    - Reads the logic level of the specified GPIO pin.
 *
 * @param[in] pGPIOx     - Base address of the GPIO peripheral
 * @param[in] PinNumber  - GPIO pin number (0–15) to be read
 *
 * @return   - 0 if the pin is low, 1 if the pin is high
 *
 * @note     - The pin must be configured as input before
 *             calling this function.
 *
 **************************************************************/
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;
	value = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001);
	return value;
}



/*
 * Read from input port
 */
/***************************************************************
 * @fn       - GPIO_ReadFromInputPort
 *
 * @brief    - Reads the 16-bit input data register (IDR) of the
 *             specified GPIO port.
 *
 * @param[in] pGPIOx  - Base address of the GPIO peripheral
 *
 * @return    - 16-bit value representing the logic level of all
 *              pins in the port (0 = low, 1 = high for each bit)
 *
 * @note      - Make sure the port pins are configured as input
 *              before calling this function.
 *
 **************************************************************/

uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t value;
	value = (uint16_t)pGPIOx->IDR;
	return value;
}



/*
 * Write to output pin
 */
/***************************************************************
 * @fn       - GPIO_WriteToOutputPin
 *
 * @brief    - This function writes a logic level (High or Low)
 *             to the specified GPIO pin.
 *
 * @param[in] pGPIOx     - Base address of the GPIO peripheral
 * @param[in] PinNumber  - GPIO pin number (0–15) to be modified
 * @param[in] Value      - Desired logic state for the pin
 *                         (GPIO_PIN_SET or GPIO_PIN_RESET)
 *
 * @return   - none
 *
 * @note     - Only the specified pin is affected; all other pins
 *             in the port remain unchanged.
 *
 **************************************************************/
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
		pGPIOx->ODR |= (1 << PinNumber);
	}else
	{
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
}



/*
 * Write to output port
 */
/***************************************************************
 * @fn       - GPIO_WriteToOutputPort
 *
 * @brief    - This function writes a 16-bit value to the output
 *             data register (ODR) of the specified GPIO port.
 *
 * @param[in] pGPIOx  - Base address of the GPIO peripheral
 * @param[in] Value   - 16-bit value to be written to the port
 *
 * @return   - None
 *
 * @note     - All pins of the port will be affected by this
 *             operation.
 *
 **************************************************************/
void  GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx,  uint16_t Value)
{
	pGPIOx->ODR = Value;
}



/*
 * Toggle Output Pin
 */
/***************************************************************
 * @fn       - GPIO_ToggleOutputPin
 *
 * @brief    - This function toggles the output state of the
 *             specified GPIO pin (i.e., changes from High to Low
 *             or Low to High).
 *
 * @param[in] pGPIOx     - base address of the GPIO peripheral
 * @param[in] PinNumber  - GPIO pin number to be toggled
 *
 * @return   - none
 *
 * @note     - Make sure the pin is configured as output before
 *             calling this function.
 *
 **************************************************************/
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{	//^= - XOR operand -> Bitshifting
	pGPIOx->ODR ^= ( 1 << PinNumber);
}



/*
 * Interrupt configuration
 */
/***************************************************************
 * @fn       - GPIO_IRQITConfig
 *
 * @brief    - Enables or disables a specific interrupt line in
 *             the NVIC (Nested Vectored Interrupt Controller).
 *
 * @param[in] IRQNumber  - Interrupt request (IRQ) number to be
 *                         configured
 *
 * @param[in] ENorDI     - ENABLE or DISABLE macro
 *
 * @return   - None
 *
 * @note     - This function only configures the NVIC registers
 *             (ISERx/ICERx). The EXTI line itself must be
 *             configured separately in GPIO_Init.
 *
 **************************************************************/
void GPIO_IRQITConfig(uint8_t IRQNumber, uint8_t ENorDI)
{
	if(ENorDI == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//Program ISER0 REGISTER
			*NVIC_ISER0 |= ( 1 << IRQNumber);

		}else if(IRQNumber > 31 && IRQNumber < 64 )
		{
			//Program ISER1 REGISTER
			*NVIC_ISER1 |= ( 1 << (IRQNumber % 32));

		}else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			//Program ISER2 REGISTER
			*NVIC_ISER2 |= ( 1 << (IRQNumber % 64));
		}
	}else
	{
		if(IRQNumber <= 31)
		{
			//Program ICER0 REGISTER
			*NVIC_ICER0 |= ( 1 << IRQNumber);

		}else if((IRQNumber > 31 && IRQNumber < 64 ))
		{
			*NVIC_ICER1 |= ( 1 << (IRQNumber % 32));

		}else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			*NVIC_ICER2 |= ( 1 << (IRQNumber % 64));
		}
	}
}



/*
 * Interrupt priority configuration
 */
/***************************************************************
 * @fn       - GPIO_IRQPriorityConfig
 *
 * @brief    - Configures the priority of a specific interrupt in
 *             the NVIC. Lower numerical values mean higher
 *             priority.
 *
 * @param[in] IRQNumber   - Interrupt request (IRQ) number
 * @param[in] IRQPriority - Priority level to assign (0 = highest)
 *
 * @return   - None
 *
 * @note     - The number of priority bits implemented depends on
 *             the processor (STM32F411 implements 4 bits).
 *             This function only sets the NVIC_IPR registers.
 *
 **************************************************************/
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. First lets find out the ipr register
	uint8_t iprx = IRQNumber /4;
	uint8_t iprx_section = IRQNumber %4;

	uint8_t shift_amount = ( 8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASEADDR + iprx ) &= ~(0xFF << (8 * iprx_section)); // clear
	*(NVIC_PR_BASEADDR + iprx ) |= (IRQPriority << shift_amount); // set

}



/*
 * GPIO interrupt handling
 */
/***************************************************************
 * @fn       - GPIO_IRQHandling
 *
 * @brief    - Clears the pending interrupt flag for a given pin
 *             in the EXTI (External Interrupt) pending register.
 *
 * @param[in] PinNumber - GPIO pin number (0–15) that triggered
 *                        the interrupt
 *
 * @return   - None
 *
 * @note     - This function must be called inside the ISR
 *             (Interrupt Service Routine) to clear the EXTI_PR
 *             flag. Otherwise, the ISR will be called again
 *             immediately.
 *
 **************************************************************/
void GPIO_IRQHandling(uint8_t PinNumber)
{
	//clear the exti pr register corresponding to the pin number
	if(EXTI->PR & ( 1 << PinNumber))
	{
		//Clear
		EXTI->PR |= (1 << PinNumber);
	}
}



















