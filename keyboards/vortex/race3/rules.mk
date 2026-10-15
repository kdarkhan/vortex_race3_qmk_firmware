# MCU
MCU = cortex-m3
ARMV = 7
USE_FPU = no
MCU_FAMILY = HT32
MCU_SERIES = HT32F165x
# TODO darkhan
# MCU_LDSCRIPT = HT32F52342_ANNEPRO2
MCU_LDSCRIPT = HT32F1654_RACE3
MCU_STARTUP = ht32f165x

BOARD = RACE_3

# OPT_DEFS = -DCORTEX_VTOR_INIT=0x3400

# Bootloader selection
BOOTLOADER = custom
PROGRAM_CMD = annepro2_tools --boot $(BUILD_DIR)/$(TARGET).bin

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = no      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = no        # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
NKRO_ENABLE = no            # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output

# Wear-levelling driver
# EEPROM_DRIVER = wear_leveling
EEPROM_DRIVER = transient
# WEAR_LEVELING_DRIVER = spi_flash

# Custom RGB matrix handling
RGB_MATRIX_ENABLE = no
RGB_MATRIX_DRIVER = custom

# Keys
# CUSTOM_MATRIX = lite
KEY_LOCK_ENABLE = no

# Other features
RAW_ENABLE = no
MIDI_ENABLE = no
VIRTSER_ENABLE = no
COMBO_ENABLE = no

# Vortex
# SRC = matrix.c
