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

Flashing uses [kdarkhan/pok3rtool](https://github.com/kdarkhan/pok3rtool), a
fork of [pok3rtool](https://github.com/pok3r-custom/pok3rtool) with fixes for
the Race 3 (branch `race3-flash-repair`):

* `--no-wait` for `flash` and `reboot`: send the reset and return, instead of
  waiting for the keyboard to come back with pok3rtool's update interface,
  which QMK does not have. Without it, booting the flashed firmware ends with
  "No matching device found".
* Automatic repair after flashing: the stock bootloader occasionally stores
  parts of the image wrongly ("Checksum check failed"). The fork checks the
  image in 4 KB blocks and rewrites only the bad ones.
* The missing Python dependencies (`rarfile`, `pefile`, `dissect.cstruct`) and
  a crash when the keyboard disconnects during a reboot are fixed.

Install it with:

    pip install --user git+https://github.com/kdarkhan/pok3rtool@race3-flash-repair

1. Enter the bootloader with Pn + Esc on the default keymap (or, from the stock
   firmware, `pok3rtool reboot --bootloader`). It shows up as `04d9:1192`.
2. Flash, either with `qmk flash -kb vortex/race3 -km default` (which runs the
   command below) or directly:

       pok3rtool flash --no-wait qmk vortex_race3_default.bin   # or vortex_race3_default_iso.bin

   The keyboard reboots into QMK when the flash is done. The bootloader never
   starts a firmware that failed verification, so a failed flash can simply be
   run again.

With upstream pok3rtool, use `--no-boot` instead of `--no-wait`, run the
command again if it fails with "Checksum check failed", and replug the keyboard
afterwards.

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).
