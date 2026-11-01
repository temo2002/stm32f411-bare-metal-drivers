/*
 * 006_i2c_master_send/main.c
 *
 * I2C1 master (PB8 SCL, PB9 SDA) sending data to an Arduino slave at 0x68 on button press.
 *
 *  Created on: Sep 18, 2025
 *      Author: Temmam Altaie
 */

#include "stm32f411RE.h"
#define MY_ADDR 		0x61
#define SLAVE_ADDR  	0x68


void delay(void)
{
	uint32_t bigdelay, halfbigdalay, quarterbigdelay;
	bigdelay = 5000000;
	halfbigdalay = bigdelay / 2;
	quarterbigdelay = halfbigdalay / 2;
	for(uint32_t i = 0; i < quarterbigdelay; i ++);
}

I2C_Handle_t I2C1Handle;

//some data
uint8_t some_data[] = "Armin er jæwli deilig\n";


/*
 * PB8 -> I2C1_SCL
 * PB9 -> I2C1_SDA
 */

void I2C1_GPIOInits(void)
{	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_PeriClockControl(GPIOC, ENABLE);
	GPIO_Handle_t I2CPins,GPIOBtn;

	I2CPins.pGPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;

	//SCL
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GPIO_Init(&I2CPins);


	//SDA
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
	GPIO_Init(&I2CPins);

	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinOPType = GPIO_NO_PUPD;



	GPIO_Init(&GPIOBtn);

}


void I2C1_Inits(void)
{

	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

	I2C_Init(&I2C1Handle);


}

int main(void){


	//I2C Pin Init
	I2C1_GPIOInits();

	//I2C Peripheral Configuration
	I2C1_Inits();

	//Enable the
	//I2C_PeripheralControl(I2C1, ENABLE);
	//GPIO_PeriClockControl(GPIOB, ENABLE);


	//Send some data to the slave


	while(1)
	{
			while(! GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13)){
				delay();

				I2C_MasterSendData(&I2C1Handle, some_data, strlen((char*)some_data), SLAVE_ADDR, I2C_DISABLE_SR);

			}
	}


}







