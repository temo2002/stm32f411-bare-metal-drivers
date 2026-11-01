/*
 * stm32f411_i2c_driver.c
 *
 *  Created on: Sep 10, 2025
 *      Author: Temmam Altaie
 */
#include "stm32f411_gpio_driver.h"





/*
 * *********************************************************************
 * 					STATIC APIs Supported by this driver
 * *********************************************************************
 */

//Prototypes:
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void	I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr);
static void	I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);
//Function declarations




/*
 * I2C Start Condition
 */
/***************************************************************
 * @fn       - I2C_GenerateStartCondition
 *
 * @brief    - This function enables or disables peripheral clock
 *             for the given I2C port
 *
 * @I2C_RegDef_t *pI2Cx - base address of the I2C peripheral
 * @uint8_t ENorDI 		- ENABLE or DISABLE macros
 *
 * @return   - none
 *
 * @Note     - none
 *
 **************************************************************/
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_START);
	}


/*
 * I2C Generate stop condition
 */
/***************************************************************
 * @fn       - I2C_GenerateStopCondition
 *
 * @brief    - This function enables or disables peripheral clock
 *             for the given I2C port
 *
 * @I2C_RegDef_t *pI2Cx - base address of the I2C peripheral
 * @uint8_t ENorDI 		- ENABLE or DISABLE macros
 *
 * @return   - none
 *
 * @Note     - none
 *
 **************************************************************/
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
	}



/*
 * I2C Execute adrress phase
 */
/***************************************************************
 * @fn       - I2C_ExecuteAddressPhase
 *
 * @brief    - This function enables or disables peripheral clock
 *             for the given I2C port
 *
 * @I2C_RegDef_t *pI2Cx - base address of the I2C peripheral
 * @uint8_t ENorDI 		- ENABLE or DISABLE macros
 *
 * @return   - none
 *
 * @Note     - none
 *
 **************************************************************/
static void	I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= ~(1); //SlaveAddr is Slave Address + r/nw bit=0
	pI2Cx->DR = SlaveAddr;
}

static void	I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr |= 1; //SlaveAddr is Slave Address + r/nw bit=1
	pI2Cx->DR = SlaveAddr;
}



/*
 * I2C Clear Address Flag
 */
/***************************************************************
 * @fn       - I2C_ClearADDRFlag
 *
 * @brief    - This function enables or disables peripheral clock
 *             for the given I2C port
 *
 * @I2C_RegDef_t *pI2Cx - base address of the I2C peripheral
 * @uint8_t ENorDI 		- ENABLE or DISABLE macros
 *
 * @return   - none
 *
 * @Note     - none
 *
 **************************************************************/
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t dummy_read;
	//Check for device mode
	if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
	{
		//device is in master mode
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if(pI2CHandle->RxSize == 1)
			{
				//First disable the ack
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

				//clear the ADDR flag (Read SR1, Read SR2)
				dummy_read = pI2CHandle->pI2Cx->SR1;
				dummy_read = pI2CHandle->pI2Cx->SR2;
				(void)dummy_read;
			}
		}else
		{
			//clear the ADDR flag (Read SR1, Read SR2)
			dummy_read = pI2CHandle->pI2Cx->SR1;
			dummy_read = pI2CHandle->pI2Cx->SR2;
			(void)dummy_read;
		}
	}else
	{
		//device is in slave mode
		//clear the ADDR flag (Read SR1, Read SR2)
		dummy_read = pI2CHandle->pI2Cx->SR1;
		dummy_read = pI2CHandle->pI2Cx->SR2;
		(void)dummy_read;
	}
}
/*
 * *********************************************************************
 * 					APIs Supported by this driver
 * *********************************************************************
 */

/*
 * I2C Peripheral Control
 */
/***************************************************************
 * @fn       - I2C_PeripheralControl
 *
 * @brief    - Enables or disables the I2C peripheral.
 *
 * @param[in] pI2Cx   - Base address of the I2C peripheral
 *                      (I2C1, I2C2, I2C3).
 *
 * @param[in] ENorDI  - ENABLE or DISABLE macro
 *                      (ENABLE = turn on I2C, DISABLE = turn off I2C).
 *
 * @return   - None
 *
 * @note     - This function sets/clears the PE (Peripheral Enable)
 *             bit in the I2C_CR1 register.
 *           - The PE bit must be set before any I2C communication
 *             can take place.
 *           - To reconfigure certain bits in CR1/CR2, the peripheral
 *             may need to be disabled first (PE = 0).
 *
 **************************************************************/
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t ENorDI)
{
    if(ENorDI == ENABLE)
    {	//SET
        pI2Cx->CR1 |= (1 << I2C_CR1_PE);
    }
    else
    {	//CLEAR
        pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
    }
}



/*
 * I2C Peripheral Clock setup
 */
/***************************************************************
 * @fn       - I2C_PeriphClockControl
 *
 * @brief    - This function enables or disables peripheral clock
 *             for the given I2C port
 *
 * @I2C_RegDef_t *pI2Cx - base address of the I2C peripheral
 * @uint8_t ENorDI 		- ENABLE or DISABLE macros
 *
 * @return   - none
 *
 * @Note     - none
 *
 **************************************************************/
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t ENorDI)
{
	if(ENorDI == ENABLE)
	{	//ENABLE
		if(pI2Cx == I2C1)
		{
			I2C1_PCLK_EN();
		}
		else if(pI2Cx == I2C2)
		{
			I2C2_PCLK_EN();
		}
		else if(pI2Cx == I2C3)
		{
			I2C3_PCLK_EN();
		}
	}
	else
	{	//DISABLE
		if(pI2Cx == I2C1)
		{
			I2C1_PCLK_DI();
		}
		else if(pI2Cx == I2C2)
		{
			I2C2_PCLK_DI();
		}
		else if(pI2Cx == I2C3)
		{
			I2C3_PCLK_DI();
		}
	}
}








/*
 * I2C Init
 */
/***************************************************************
 * @fn       - I2C_Init
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
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
    uint32_t tempreg = 0;
    uint16_t ccr_value = 0;

    // 1. Enable peripheral clock
    I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

    // 2. Configure CR2 (FREQ field) = PCLK1 frequency in MHz
    tempreg = (RCC_GetPCLK1Value() / 1000000U) & 0x3F;
    pI2CHandle->pI2Cx->CR2 = tempreg;

    // 3. Program own device address in OAR1
    tempreg = (pI2CHandle->I2C_Config.I2C_DeviceAddress << 1);
    tempreg |= (1 << 14); // Bit 14 must always be kept at 1
    pI2CHandle->pI2Cx->OAR1 = tempreg;

    // 4. Configure CCR (Clock Control Register)
    if (pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
    {
        // Standard mode (100 kHz)
        ccr_value = RCC_GetPCLK1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
        pI2CHandle->pI2Cx->CCR = (ccr_value & 0xFFF);
    }
    else
    {
        // Fast mode (Fm)
        pI2CHandle->pI2Cx->CCR |= (1 << 15); // set FM bit
        pI2CHandle->pI2Cx->CCR |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << 14);

        if (pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2)
        {
            ccr_value = RCC_GetPCLK1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
        }
        else
        {
            ccr_value = RCC_GetPCLK1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
        }
        pI2CHandle->pI2Cx->CCR |= (ccr_value & 0xFFF);
    }

    // 5. Configure TRISE
    if (pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
    {
        // Standard mode
        pI2CHandle->pI2Cx->TRISE = (RCC_GetPCLK1Value() / 1000000U) + 1; // FREQ + 1
    }
    else
    {
        // Fast mode
        pI2CHandle->pI2Cx->TRISE = ((RCC_GetPCLK1Value() * 300) / 1000000U) + 1;
    }

    // 6. Enable the peripheral (PE=1)
    pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_PE);

    // 7. Enable ACK if configured
    if (pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
    {
        pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
    }
}





/*
 * I2C DeInit
 */
/***************************************************************
 * @fn       - I2C_DeInit
 *
 * @brief    - Resets all registers of the specified I2C port
 *             to their default reset values.
 *
 * @param[in] pI2Cx - Base address of the I2C port to reset
 *
 * @return   - None
 *
 * @note     - This function uses the RCC APB1 peripheral reset
 *             register to perform the reset.
 *
 **************************************************************/
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{	//RESET REGISTERS
	if(pI2Cx == I2C1)
	{
		I2C1_REG_RESET();
	}
	else if(pI2Cx == I2C2)
	{
		I2C2_REG_RESET();
	}
	else if(pI2Cx == I2C3)
	{
		I2C3_REG_RESET();
	}
}



/*
 * I2C Get Flag Status
 */
/***************************************************************
 * @fn       - I2C_GetFlagStatus
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
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
	if(pI2Cx->SR1 & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}



/*
 * I2C - Master send Data
 */
/***************************************************************
 * @fn       - I2C_MasterSendData
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
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	//1. Generate the START condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. Confirm that the start generation is completed by checking
	// the SB flag in the SR1
	// Note: Until SB is cleared SCL will be stretched (pulled to LOW)
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

	//3. Send the address of the slave with r/nw bit set to w(0) (total 8 bits)
	I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);

	//4. Confirm that the address phase is completed by checking the ADDR Flag in SR1
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

	//5. Clear the ADDR flag according to its software sequence
	// Note: Until ADDR is cleared SCL will be stretched (pulled to LOW)
	I2C_ClearADDRFlag(pI2CHandle);

	//6. Send the data until Len becomes 0

	while(Len > 0)
	{
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		pTxBuffer++;
		Len--;
	}

	//7. When Len becomes zero wait for TXE=1 and BTF=1 before generating the stop condition
	// Note: TXE=1, BTF=1, means that both SR and DR are empty and next transmission should begin
	// When BTF=1 SCL will be stretched (Pulled to LOW)

	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));

	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));


	//8. Generate stop condition and master doesn't need to wait for the completion of stop condition.
	//Note: Generating STOP, automatically clears the BTF
	if(Sr == I2C_DISABLE_SR){
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
	}

}




/*
 * I2C - Master Receive Data
 */
/***************************************************************
 * @fn       - I2C_MasterSendData
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
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr)
{


	//1. Generate the START Condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. Confirm that start condition generation is completed by checking the SB flag in the SR1
	//Note: Until SB is cleared SCL will be stretched (Pulled to LOW)
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

	//3. send the address of the slave with r/nw bit set to R(1) (total 8 bits)
	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, SlaveAddr);

	//4. Wait until address phase is completed by checking the ADDR flag in the SR1
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

	//Procedure to read 1 byte from slave
	if(Len == 1)
	{
		//Disable Acking
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

		//Clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//Wait until RXNE becomes 1
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

		//Generate STOP condition
		if(Sr == I2C_DISABLE_SR){
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}

		//Read data in to buffer
		*pRxBuffer = pI2CHandle->pI2Cx->DR;
	}


	//Procedure to read data from slave when Len > 1
	if(Len > 1)
	{
		//Clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//Read the data until Len becomes Zero
		for ( uint32_t i = Len ; 1 > 0 ; i--)
		{
			//Wait until RXNE becomes 1
			while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

			if(i == 2) //If last 2 bytes are remaining
			{
				//Clear the ack bit
				I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);


				//Generate STOP condition
				if(Sr == I2C_DISABLE_SR){
					I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
				}
			}

			//Read the data from data register into buffer
			*pRxBuffer = pI2CHandle->pI2Cx->DR;

			//Increment the buffer address
			pRxBuffer++;
		}
	}


	//Re-enable Acking
	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);
	}
}



void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t ENorDI)
{
	if(ENorDI == I2C_ACK_ENABLE)
	{
		//Enable the ACK
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	}else
	{
		//Disable the ACK
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
	}

}





/*********************************************************************
 * @fn      		  - I2C_MasterSendDataIT
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -  Complete the below code . Also include the function prototype in header file

 */
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pTxBuffer, uint32_t Len,uint8_t SlaveAddr,uint8_t Sr)
{

	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

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
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              - Complete the below code . Also include the fn prototype in header file

 */
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr)
{

	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = Len; //Rxsize is used in the ISR code to manage the data reception
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	}

	return busystate;
}

/*
 * Interrupt configuration
 */
/***************************************************************
 * @fn       - I2C_IRQInterruptConfig
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
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t ENorDI)
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
 * @fn       - I2C_IRQPriorityConfig
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
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. First lets find out the ipr register
	uint8_t iprx = IRQNumber /4;
	uint8_t iprx_section = IRQNumber %4;

	uint8_t shift_amount = ( 8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASEADDR + iprx ) &= ~(0xFF << (8 * iprx_section)); // clear
	*(NVIC_PR_BASEADDR + iprx ) |= (IRQPriority << shift_amount); // set
}



static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
	{
		if(pI2CHandle->TxLen > 0)
		{
			//1. Load the data into DR
			pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);
			//2. Decrement the TxLen
			pI2CHandle->TxLen--;
			//3. Increment the buffer address
			pI2CHandle->pTxBuffer++;

		}
	}
}
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	//Implement the code to disable ITBUFEN Control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	//Implement the code to disable ITEVFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;

	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
	{
			I2C_ManageAcking(pI2CHandle->pI2Cx,ENABLE);
	}

}

void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	//Implement the code to disable ITBUFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);
	//Implement the code to disable ITEVFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->RxLen = 0;

}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	//RXNE flag is set
	if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
	{
		//We have to do the data reception
		if(pI2CHandle->RxSize == 1)
		{
			*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;

			pI2CHandle->RxLen--;

		}


		if(pI2CHandle->RxSize > 1)
		{
			 if(pI2CHandle->RxLen == 2)
			 {
				 //clear the ack bit
				 I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
			 }

			 //Read the DR
			*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
			pI2CHandle->pRxBuffer++;
			pI2CHandle->RxLen--;
		}

		if(pI2CHandle->RxLen == 0)
		{
			//Close the I2C data reception and notify the application

			//1. Generate the start condition
			if(pI2CHandle->Sr == I2C_DISABLE_SR)
			{
				I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
			}
			//2. Close the I2C rx
			I2C_CloseReceiveData(pI2CHandle);
			//3. Notify the application
			I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);
		}
	}
}




void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	//Interrupt handling for master and slave mode of a device

	uint32_t temp1, temp2, temp3;

	temp1 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN);
	temp2 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_SB);

	//1. Handle For interrupt generated by SB evenet
	//Note: SB flag is only applicable in Master Mode
	if(temp1 && temp3)
	{
		//SB flag is set
		//The interrupt is generated because of SB event
		//This block will not be executed in slave mode because SB->0 in SM
		//In this block lets execute the address phase
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx,pI2CHandle->DevAddr);
		}
	}

	//2. Handle for interrupt generated by ADDR event
	//Note: When master mode 	-> Address is sent
	// 		When slave mode 	-> Address matched with own address
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR);
	if(temp1 && temp3)
	{
		//ADDR flag is set
		I2C_ClearADDRFlag(pI2CHandle);

	}

	//3. Handle for interrupt generated by BTF (Byte Transfer Finished) event
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF);
	if(temp1 && temp3)
	{
		//BTF flag is set
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			//Make sure that TXE is also set
			if(pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE))
			{
				//BTF, TXE = 1
				if(pI2CHandle->TxLen == 0)
				{
					//1. Generate the STOP condition
					if(pI2CHandle->Sr == I2C_DISABLE_SR)
					{
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
					}

					//2. Reset all the member elements of the handle structure
					I2C_CloseSendData(pI2CHandle);

					//3. Notify the application about transmission complete
					I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
				}

			}
		}else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			;
		}

	}


	//4. Handle for interrupt generated by STOPf event
	//Note: Stop detection flag is applicable only in slave mode.
	//For master this flag will
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF);
	if(temp1 && temp3)
	{
		//STOPF flag is set
		//Clear the STOPF (i.e.1) Read SR1 2) write to CR1

		pI2CHandle->pI2Cx->CR1 |= 0x0000;
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);

	}

	//5. Handle for interrupt generated by TXE event
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE);
	if(temp1 && temp2 && temp3)
	{
		//Check for device mode
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			//TXE flag is set
			//We have to do the data transmission
			I2C_MasterHandleTXEInterrupt(pI2CHandle);
		}

	}

	//6. Handle for interrupt generated by RXNE event
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RXNE);
	if(temp1 && temp2 && temp3)
	{
		//Check Device Mode
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			I2C_MasterHandleRXNEInterrupt(pI2CHandle);
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

void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{
    uint32_t temp1, temp2;

    // Know the status of ITERREN control bit in CR2
    temp2 = (pI2CHandle->pI2Cx->CR2) & (1 << I2C_CR2_ITERREN);

    /*********************** Check for Bus error ************************************/
    temp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_BERR);
    if (temp1 && temp2)
    {
        // Clear the Bus error flag
        pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_BERR);

        // Notify application
        I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_BERR);
    }

    /*********************** Check for Arbitration lost error ************************************/
    temp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_ARLO);
    if (temp1 && temp2)
    {
        // Clear the arbitration lost flag
        pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_ARLO);

        // Notify application
        I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_ARLO);
    }

    /*********************** Check for ACK failure error ************************************/
    temp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_AF);
    if (temp1 && temp2)
    {
        // Clear the ACK failure flag
        pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_AF);

        // Notify application
        I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_AF);
    }

    /*********************** Check for Overrun/Underrun error ************************************/
    temp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_OVR);
    if (temp1 && temp2)
    {
        // Clear the Overrun/Underrun flag
        pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_OVR);

        // Notify application
        I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_OVR);
    }

    /*********************** Check for Timeout error ************************************/
    temp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_TIMEOUT);
    if (temp1 && temp2)
    {
        // Clear the Timeout flag
        pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_TIMEOUT);

        // Notify application
        I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_TIMEOUT);
    }
}





