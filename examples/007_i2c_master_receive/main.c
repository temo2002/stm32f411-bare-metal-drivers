/*
 * 007_i2c_master_receive/main.c
 *
 * I2C1 master receiving data from an Arduino slave at 0x68 (polling).
 *
 *  Created on: Sep 18, 2025
 *      Author: Temmam Altaie
 */

#include "stm32f411RE.h"
#define MY_ADDR 		0x61    // Egen I2C-adresse (STM32 som master kan ha en adresse)
#define SLAVE_ADDR  	0x68    // Adresse til Arduino-slaven





// Enkel delay-funksjon for å debounce knapp og skape ventetid
void delay(void)
{
	uint32_t bigdelay, halfbigdalay, quarterbigdelay;
	bigdelay = 5000000;
	halfbigdalay = bigdelay / 2;
	quarterbigdelay = halfbigdalay / 2;
	for(uint32_t i = 0; i < quarterbigdelay; i ++);
}

I2C_Handle_t I2C1Handle;   // Global handle for I2C1

// Buffer til å lagre data mottatt fra slave
uint8_t rcv_buff[32];


/*
 * PB8 -> I2C1_SCL
 * PB7 -> I2C1_SDA
 * PC13 -> Button (input)
 */
void I2C1_GPIOInits(void)
{
	GPIO_PeriClockControl(GPIOB, ENABLE);   // Slå på klokke for port B (I2C-pinner)
	GPIO_PeriClockControl(GPIOC, ENABLE);   // Slå på klokke for port C (knapp)

	GPIO_Handle_t I2CPins, GPIOBtn;

	// Felles konfigurasjon for I2C-pinnene
	I2CPins.pGPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;      // Alternativ funksjon
	I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;    // Open-drain (I2C-standard)
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;   // Pull-up
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;              // AF4 = I2C1

	// SCL = PB8
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GPIO_Init(&I2CPins);

	// SDA = PB9
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
	GPIO_Init(&I2CPins);

	// Knapp på PC13 (input)
	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinOPType = GPIO_NO_PUPD;  // Ingen pull-up/down
	GPIO_Init(&GPIOBtn);
}


void I2C1_Inits(void)
{
	I2C1Handle.pI2Cx = I2C1;                          // Velg I2C1 peripheral
	I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;  // Slå på ACK
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;      // STM32s egen adresse
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;  // Duty cycle (ikke relevant i SM)
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;  // Standard mode (100 kHz)

	I2C_Init(&I2C1Handle);  // Kall til driverens init-funksjon
}


int main(void)
{
	uint8_t commandcode, len;

	// Init I2C-pinner
	I2C1_GPIOInits();

	// Init I2C peripheral
	I2C1_Inits();

	// Aktiver ACK etter at PE=1 er satt
	I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);

	while(1)
	{
		// Når knappen trykkes (PC13 = 0)
		while(!GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13))
		{
			delay(); // Enkel debounce

			// Først sender vi kommando 0x51 → spør om lengden på data
			commandcode = 0x51;
			I2C_MasterSendData(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR);

			// Mottar 1 byte (lengde)
			I2C_MasterReceiveData(&I2C1Handle, &len, 1, SLAVE_ADDR, I2C_ENABLE_SR);

			// Deretter sender vi kommando 0x52 → be om selve dataene
			commandcode = 0x52;
			I2C_MasterSendData(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR);

			// Mottar 'len' antall bytes inn i rcv_buff
			I2C_MasterReceiveData(&I2C1Handle, rcv_buff, len, SLAVE_ADDR, I2C_DISABLE_SR);


		}
	}
}
