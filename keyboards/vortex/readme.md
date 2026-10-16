# Vortex Race 3

75% keyboard with per-key RGB, based on the Holtek HT32F1654.

* Keyboard Maintainer: kdarkhan
* Hardware Supported: Vortex Race 3 (RGB), ANSI PCB
* Hardware Availability: discontinued

This firmware runs from 0x3400 behind the stock Vortex bootloader, which stays
installed. The stock firmware is overwritten.

Make example for this keyboard (after setting up your build environment):

    make vortex/race3:default

## Flashing

1. Enter the bootloader with LShift + RShift + B (or, from the stock firmware,
   `pok3rtool reboot --bootloader`). It shows up as `04d9:1192`.
2. Flash with [pok3rtool](https://github.com/pok3r-custom/pok3rtool):

       pok3rtool flash --no-boot qmk vortex_race3_default.bin

   The bootloader's write verification occasionally fails; just run the
   command again. The bootloader does not start a firmware that failed
   verification.
3. Unplug and replug the keyboard.

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).
