# Vortex Race 3

75% keyboard with per-key RGB, based on the Holtek HT32F1654.

* Keyboard Maintainer: kdarkhan
* Hardware Supported: Vortex Race 3 (RGB), ANSI and ISO PCBs
* Hardware Availability: discontinued

This firmware runs from 0x3400 behind the stock Vortex bootloader, which stays
installed. The stock firmware is overwritten.

## Layouts

Vortex ships one stock firmware for ANSI (US) and one for ISO ("EU"); the ISO
language variants (ISO-DE, ISO-NOR, ISO-UK, ...) share the same PCB and only
differ in keycap legends, so pick the layout in your OS.

| Keymap        | Layout        | Boards                      |
|---------------|---------------|-----------------------------|
| `default`     | `LAYOUT_ansi` | ANSI (US)                   |
| `default_iso` | `LAYOUT_iso`  | ISO (DE, NOR, UK, FR, ...)  |

On the ISO PCB the ISO Enter uses the ANSI Enter switch position, the ANSI `\`
position is not populated, and the extra `#` and `\|` keys sit on matrix
positions that are empty on ANSI boards.

Note: ISO layout in this dir was not tested since I had access only to ANSI version.

Holding Pn (or Fn) gives the RGB controls on the number row (toggle, effect,
hue, saturation, brightness, speed) and the bootloader on Esc.

Make example for this keyboard (after setting up your build environment):

    make vortex/race3:default
    make vortex/race3:default_iso

## Flashing

1. Enter the bootloader with Pn + Esc on the default keymap (or, from the stock
   firmware, `pok3rtool reboot --bootloader`). It shows up as `04d9:1192`.
2. Flash with [pok3rtool](https://github.com/pok3r-custom/pok3rtool):

       pok3rtool flash --no-boot qmk vortex_race3_default.bin   # or vortex_race3_default_iso.bin

   The bootloader's write verification occasionally fails; just run the
   command again. The bootloader does not start a firmware that failed
   verification.
3. Unplug and replug the keyboard.

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).
