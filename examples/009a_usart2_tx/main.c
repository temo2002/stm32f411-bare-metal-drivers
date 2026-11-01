/*
 * 009a_usart2_tx/main.c
 *
 * USART2 transmit (PA2 TX, PA3 RX) on button press.
 *
 *  Created on: Sep 24, 2025
 *      Author: Temmam Altaie
 */

#include "stm32f411RE.h"

char msg[1024] = "USART Tx testing...\n\r";
void delay(void)
{
	uint32_t bigdelay, halfbigdalay, quarterbigdelay;
	bigdelay = 5000000;
	halfbigdalay = bigdelay / 2;
	quarterbigdelay = halfbigdalay / 2;
	for(uint32_t i = 0; i < quarterbigdelay; i ++);
}

USART_Handle_t Usart2_Handle;


/*
 * Tx -> PA2
 * Rx -> PA3
 */


void USART2_GPIOinit(void)
{	//Enable periclock for Port A
	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_PeriClockControl(GPIOC, ENABLE);

	GPIO_Handle_t USARTPins, GPIOBtn;

	//Felles konfigurasjon for USART Tx og Rx
	USARTPins.pGPIOx = GPIOA;
	USARTPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN; //Alternate function
	USARTPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP; //Push Pull
	USARTPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU; //Enable Pullup
	USARTPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	USARTPins.GPIO_PinConfig.GPIO_PinAltFunMode = 7; //AF07 = USART2

	//Tx = PA2
	USARTPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
	GPIO_Init(&USARTPins);

	//Rx = PA3
	USARTPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_Init(&USARTPins);

	// Knapp på PC13 (input)
	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinOPType = GPIO_NO_PUPD;  // Ingen pull-up/down
	GPIO_Init(&GPIOBtn);
}

void USART2_Init(void)
{	//Initialize the USART configuration
	USART_PeriClockControl(USART2, ENABLE);
	Usart2_Handle.pUSARTx = USART2;
	Usart2_Handle.USART_Config.USART_Baud = USART_STD_BAUD_115200; //Set the desired baudrate
	Usart2_Handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS; //Word length - 8BIT
	Usart2_Handle.USART_Config.USART_Mode = USART_MODE_ONLY_TX; //Only transmitting
	Usart2_Handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE; //Disable Paritymode
	Usart2_Handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE; //Not used
	Usart2_Handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1; //Stopbit length 1

	USART_Init(&Usart2_Handle);
}

int main(void)
{
	USART2_GPIOinit();

	USART2_Init();

	USART_PeripheralControl(USART2, ENABLE);



	while(1)
		{
			// Når knappen trykkes (PC13 = 0)
			//while(!GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13))
			while(1)
			{
				delay(); // Enkel debounce
				USART_SendData(&Usart2_Handle, (uint8_t*)msg, strlen(msg));
			}
		}

	return 0;
}
















