# MCU
MCU = cortex-m3
ARMV = 7
USE_FPU = no
MCU_FAMILY = HT32
MCU_SERIES = HT32F165x
MCU_LDSCRIPT = HT32F1654_RACE3
MCU_STARTUP = ht32f165x

BOARD = RACE_3

# Bootloader selection: stock Vortex bootloader, enter it with LShift+RShift+B
BOOTLOADER = custom
PROGRAM_CMD = pok3rtool flash --no-boot qmk $(BUILD_DIR)/$(TARGET).bin

# RGB matrix is enabled in keyboard.json
SRC += rgb_driver.c
