# STM32F411RE bare-metal drivers

Peripheral drivers for the STM32F411RE written from scratch in C, without HAL or CMSIS.
Register definitions, GPIO, I2C, SPI, USART and RCC drivers, plus small example programs
that exercise them on a Nucleo-F411RE board (LED on PA5, user button on PC13).

Developed as a learning project in STM32CubeIDE while following the Udemy course
[Mastering Microcontroller with Embedded Driver Development](https://www.udemy.com/course/mastering-microcontroller-with-peripheral-driver-development/).
The driver structure follows the course; the code is my own implementation for the STM32F411RE,
and `009b_usart1_tx_course` is the course reference version of the USART example.

## Structure

```
drivers/
  inc/   stm32f411RE.h (register maps, base addresses, IRQ numbers)
         stm32f411_{gpio,i2c,spi,usart,rcc}_driver.h
  src/   stm32f411_{gpio,i2c,spi,usart,rcc}_driver.c
examples/        one folder per example, each with its own main.c
linker/          STM32F411RETX_FLASH.ld, STM32F411RETX_RAM.ld
startup/         startup_stm32f411retx.s
Makefile         builds a single example with arm-none-eabi-gcc
```

## Drivers

| Peripheral | Features |
|------------|----------|
| GPIO | Init, read/write, toggle, alternate functions, EXTI interrupts (IRQ config, priority, handler) |
| I2C  | Master send/receive in polling and interrupt mode, repeated start, ACK control, event and error IRQ handling |
| SPI  | Send/receive in polling and interrupt mode, configurable device mode, clock speed, CPOL/CPHA, frame format and slave management |
| USART | Send/receive in polling and interrupt mode, baud rate, word length, parity, stop bits and hardware flow control |
| RCC  | Helpers returning the PCLK1, PCLK2 and PLL output clock frequencies |

## Examples

| Folder | What it does |
|--------|--------------|
| `001_led_toggle` | Toggle the on-board LED (PA5) |
| `002_led_button` | LED controlled by the user button (PC13) |
| `005_button_interrupt` | Button handled through the EXTI15_10 interrupt |
| `006_i2c_master_send` | I2C1 master sending data to an Arduino slave (0x68) |
| `007_i2c_master_receive` | I2C1 master receiving data from an Arduino slave (polling) |
| `008_i2c_master_receive_it` | Same, interrupt driven |
| `009a_usart2_tx` | USART2 transmit (PA2/PA3) |
| `009b_usart1_tx_course` | USART1 transmit (PA9/PA10), course reference version |
| `010_spi2_tx` | SPI2 master transmit (PB12 to PB15) |
| `011_spi2_arduino_tx` | SPI2 master sending data to an Arduino slave |
| `012_usart2_printf` | `printf` retargeted to USART2 |

Example numbers follow the order they were written in, so some numbers are skipped.

## Build

Install the [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
and make sure `arm-none-eabi-gcc` is on your `PATH`.

```
make list                       # show available examples
make EXAMPLE=002_led_button     # builds build/002_led_button/002_led_button.elf
make clean
```

Flash the resulting `.elf` with STM32CubeProgrammer or OpenOCD, or import the sources into
STM32CubeIDE and add `drivers/inc` to the include paths.

## Hardware

- STM32F411RET6 (Cortex-M4F), Nucleo-F411RE
- Arduino used as I2C/SPI slave in examples 006 to 008 and 011
