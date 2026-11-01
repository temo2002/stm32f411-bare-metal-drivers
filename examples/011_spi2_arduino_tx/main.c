/*
 * 011_spi2_arduino_tx/main.c
 *
 * SPI2 master sending data to an Arduino SPI slave.
 *
 *  Created on: Oct 15, 2025
 *      Author: Temmam Altaie
 */

#include "stm32f411RE.h"

void delay(void)
{
	for (uint32_t i = 0; i < 500000; i++);
}

/*
 * PB14 --> SPI2_MISO
 * PB15 --> SPI2_MOSI
 * PB13 --> SPI2_SCLK
 * PB12 --> SPI2_NSS
 * ALT function mode : AF5
 */

void SPI2_GPIOInits(void)
{
	GPIO_Handle_t SPIPins;
	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU; // Pull-up for stabile linjer

	// SCK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	// NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPIPins);

	// MISO (må være konfigurert selv om den ikke brukes)
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPIPins);

	// MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);
}

void SPI2_Inits(void)
{
	SPI_Handle_t SPI2handle;

	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPI_Config.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPI_Config.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPI_Config.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV64;   // ~1.3 MHz for stabil kommunikasjon
	SPI2handle.SPI_Config.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPI_Config.SPI_CPOL = SPI_CPOL_LOW;                // Mode 0
	SPI2handle.SPI_Config.SPI_CPHA = SPI_CPHA_LOW;                // Mode 0
	SPI2handle.SPI_Config.SPI_SSM = SPI_SSM_DI;                   // Hardware-NSS

	SPI_Init(&SPI2handle);
}

void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GPIOBtn;

	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GPIOBtn);
}

int main(void)
{
	char user_data[] = "An Arduino Uno board is best suited for beginners who have just started using microcontrollers, on the other hand, Arduino Mega board is for enthusiasts who require a lot of I/O pins for their projects";

	GPIO_ButtonInit();
	SPI2_GPIOInits();
	SPI2_Inits();

	/*
	 * Når SSOE = 1 håndterer maskinvaren NSS automatisk.
	 * Når SPI aktiveres (SPE = 1) trekkes NSS lav,
	 * og når SPI deaktiveres (SPE = 0) går NSS høy igjen.
	 */
	SPI_SSOEConfig(SPI2, ENABLE);

	while (1)
	{
		// Vent til knapp trykkes (aktiv lav på PC13)
		while (GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));
		delay(); // enkel debouncing

		// ---------- Start overføring ----------
		SPI_PeripheralControl(SPI2, ENABLE); // NSS går lav

		uint8_t dataLen = strlen(user_data);

		// Send først lengden
		SPI_SendData(SPI2, &dataLen, 1);

		// Send selve teksten
		SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));

		// Vent til SPI-bussen er ferdig (busy-flagg lavt)
		while (SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG) == FLAG_SET);

		// Deaktiver SPI – NSS går høy
		SPI_PeripheralControl(SPI2, DISABLE);

		// ---------- Ferdig overføring ----------
	}
}
