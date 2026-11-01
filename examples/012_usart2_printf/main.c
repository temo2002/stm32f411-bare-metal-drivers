/*
 * 012_usart2_printf/main.c
 *
 * printf retargeted to USART2 via _write().
 *
 *      Author: Temmam Altaie
 */

#include "stm32f411RE.h"
#include <stdio.h>
#include <string.h>

USART_Handle_t usart2_handle;

/*
 * USART2 GPIO init (PA2 = TX, PA3 = RX)
 */
void USART2_GPIOInit(void)
{
    GPIO_Handle_t usart_gpios;

    usart_gpios.pGPIOx = GPIOA;
    usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = 7; // USART2 AF7

    // USART2 TX (PA2)
    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
    GPIO_Init(&usart_gpios);

    // USART2 RX (PA3)
    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
    GPIO_Init(&usart_gpios);
}

/*
 * USART2 peripheral init
 */
void USART2_Init(void)
{
    usart2_handle.pUSARTx = USART2;
    usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
    usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    usart2_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
    usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;

    USART_Init(&usart2_handle);
}

/*
 * Retarget printf to USART2
 */
int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++)
    {
        USART_SendData(&usart2_handle, (uint8_t*)ptr, 1);
        ptr++;
    }
    return len;
}

int main(void)
{
    // Init USART2 GPIO and peripheral
    USART2_GPIOInit();
    USART2_Init();
    USART_PeripheralControl(USART2, ENABLE);

    printf("Hello from STM32F411RE!\r\n");

    while (1)
    {
        printf("Looping...\r\n");
        for (volatile int i = 0; i < 1000000; i++); // simple delay
    }
}
