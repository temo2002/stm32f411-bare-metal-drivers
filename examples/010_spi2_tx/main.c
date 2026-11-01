/*
 * 010_spi2_tx/main.c
 *
 * SPI2 master transmit (PB12 NSS, PB13 SCK, PB14 MISO, PB15 MOSI).
 *
 *  Created on: Oct 14, 2025
 *      Author: Temmam Altaie
 */

#include "stm32f411RE.h"

/*
 * FOR SPI2 we can use the following:

 * ALTFUNC 	-> AF05
 * PB12 	-> NSS
 * PB13 	-> SCK
 * PB14 	-> MISO
 * PB15 	-> MOSI

 */


//Initialize GPIO for SPI-pins
void SPI2_GPIOInit(void)
{
	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	//SCK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	//NSS
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	//GPIO_Init(&SPIPins);

	//MISO
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	//GPIO_Init(&SPIPins);

	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);
}


void SPI2_Init(void)
{
	SPI_Handle_t SPI2handle;
	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPI_Config.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPI_Config.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPI_Config.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2; //Generates SCLK of 8Mhz
	SPI2handle.SPI_Config.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPI_Config.SPI_CPOL = SPI_CPOL_HIGH;
	SPI2handle.SPI_Config.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPI_Config.SPI_SSM = SPI_SSM_EN; //Software slave management enabled for NSS-pin

	SPI_Init(&SPI2handle);
}



int main(void)
{
	char user_data[] = "Hello World";

	//This function is used to initialize the GPIO pins to behave as SPI2 Pins
	SPI2_GPIOInit();

	//This function is used to initialize the SPI2 Peripheral Parameters
	SPI2_Init();

	//This makes NSS internal signal to high
	SPI_SSIConfig(SPI2, ENABLE);

	//Enable the SPI2 Peripheral
	SPI_PeripheralControl(SPI2, ENABLE);

	//To send data
	SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));

	while(1);

	return 0;
}






