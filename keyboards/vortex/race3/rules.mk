# MCU
MCU = cortex-m3
ARMV = 7
USE_FPU = no
MCU_FAMILY = HT32
MCU_SERIES = HT32F165x
MCU_LDSCRIPT = HT32F1654_RACE3
MCU_STARTUP = ht32f165x

BOARD = RACE_3

# Bootloader selection: stock Vortex bootloader, enter it with Pn+Esc (default keymap)
BOOTLOADER = custom
# --no-wait: QMK has no update interface for pok3rtool to wait for after the reset
PROGRAM_CMD = pok3rtool flash --no-wait qmk $(BUILD_DIR)/$(TARGET).bin

# RGB matrix is enabled in keyboard.json; set RGB_MATRIX_ENABLE = no in a
# keymap's rules.mk to build without it (the LEDs are then switched off)
SRC += rgb_driver.c
