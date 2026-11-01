# Build one example for the STM32F411RE (Cortex-M4F).
# Usage:  make EXAMPLE=002_led_button
#         make list
#         make clean
# Requires the GNU Arm Embedded Toolchain (arm-none-eabi-gcc) on PATH.

EXAMPLE ?= 001_led_toggle
LINKER  ?= linker/STM32F411RETX_FLASH.ld
BUILD   := build/$(EXAMPLE)
TARGET  := $(BUILD)/$(EXAMPLE).elf

CC := arm-none-eabi-gcc

CPU := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS  := $(CPU) -std=gnu11 -g -O0 -Wall -ffunction-sections -fdata-sections \
           -DSTM32 -DSTM32F4 -DSTM32F411RETx -Idrivers/inc --specs=nano.specs
ASFLAGS := $(CPU) -g -x assembler-with-cpp
LDFLAGS := $(CPU) -T$(LINKER) --specs=nosys.specs --specs=nano.specs \
           -Wl,-Map=$(BUILD)/$(EXAMPLE).map -Wl,--gc-sections -static \
           -Wl,--start-group -lc -lm -Wl,--end-group

SRCS := $(wildcard drivers/src/*.c) examples/$(EXAMPLE)/main.c
OBJS := $(patsubst %.c,$(BUILD)/%.o,$(SRCS)) $(BUILD)/startup/startup_stm32f411retx.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/startup/%.o: startup/%.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

list:
	@ls examples

clean:
	rm -rf build

-include $(OBJS:.o=.d)

.PHONY: all list clean
