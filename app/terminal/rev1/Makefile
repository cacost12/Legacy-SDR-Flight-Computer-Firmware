# --------------------------------------------------------------
# Terminal Makefile (based on gcc)
#
# --------------------------------------------------------------

################################################################
# target
################################################################
TARGET = terminal


################################################################
# building variables
################################################################

# debug build?
DEBUG = 1

# optimization
OPT = -Og


################################################################
# paths
################################################################

# Build path
BUILD_DIR = build

# Library directory
LIB_DIR = ../../../lib

# Driver directory
DRIVER_DIR = ../../../driver

# Module directory
MOD_DIR = ../../../mod

# MCU Init Code Directory
INIT_DIR = ../../../init

# Utilities directory
UTIL_DIR = $(LIB_DIR)/util


################################################################
# source
################################################################

# C sources
C_SOURCES =                                                           \
main.c                                                                \
$(MOD_DIR)/command_handler/command_handler.c                          \
$(MOD_DIR)/command_handler/commands.c                                 \
$(INIT_DIR)/init.c                                                    \
$(UTIL_DIR)/zav_error.c                                               \
$(DRIVER_DIR)/baro/baro.c                                             \
$(DRIVER_DIR)/flash/flash.c                                           \
$(DRIVER_DIR)/ignition/ignition.c                                     \
$(DRIVER_DIR)/imu/imu.c                                               \
$(DRIVER_DIR)/led/led.c                                               \
$(MOD_DIR)/sensor/sensor.c                                            \
$(DRIVER_DIR)/usb/usb.c                                               \
$(INIT_DIR)/config/Src/stm32h7xx_it.c                                 \
$(INIT_DIR)/config/Src/stm32h7xx_hal_msp.c                            \
$(INIT_DIR)/config/Src/system_stm32h7xx.c                             \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c    \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c       \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c    \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash.c     \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash_ex.c  \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c      \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hsem.c      \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c       \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma_ex.c    \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdma.c      \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c       \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c    \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c           \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c       \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c    \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_exti.c      \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c       \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c    \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c      \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c   \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi.c       \
$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi_ex.c    

# ASM sources
ASM_SOURCES =  \
startup_stm32h750xx.s


################################################################
# binaries
################################################################

PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S


################################################################
# CFLAGS
################################################################

# cpu
CPU = -mcpu=cortex-m7

# fpu
FPU = -mfpu=fpv5-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =                 \
-DUSE_HAL_DRIVER         \
-DSTM32H750xx            \
-DLEGACY_FLIGHT_COMPUTER \
-DA0003_REV1             \
-DTERMINAL       


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =                                           \
-I.                                                    \
-I$(MOD_DIR)/command_handler                           \
-I$(LIB_DIR)                                           \
-I$(INIT_DIR)                                          \
-I$(UTIL_DIR)                                          \
-I$(INIT_DIR)/config/Inc                               \
-I$(DRIVER_DIR)/baro                                   \
-I$(DRIVER_DIR)/buzzer                                 \
-I$(DRIVER_DIR)/flash                                  \
-I$(DRIVER_DIR)/ignition                               \
-I$(DRIVER_DIR)/imu                                    \
-I$(DRIVER_DIR)/led                                    \
-I$(MOD_DIR)/sensor                                    \
-I$(DRIVER_DIR)/usb                                    \
-I$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Inc          \
-I$(LIB_DIR)/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy   \
-I$(LIB_DIR)/Drivers/CMSIS/Device/ST/STM32H7xx/Include \
-I$(LIB_DIR)/Drivers/CMSIS/Include

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


################################################################
# LDFLAGS
################################################################

# link script
LDSCRIPT = STM32H750VBTx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


################################################################
# build the application
################################################################

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		


################################################################
# flash to device
################################################################
flash:
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x08000000
	

################################################################
# erase device flash
################################################################
erase:
	st-flash erase


################################################################
# clean up
################################################################
clean:
	-rm -fR $(BUILD_DIR)

  
################################################################
# dependencies
################################################################
-include $(wildcard $(BUILD_DIR)/*.d)


# *** EOF ***