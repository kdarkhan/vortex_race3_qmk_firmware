/* Copyright 2021 OpenAnnePro community
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "hal.h"
#include "quantum.h"

// The stock bootloader stays in IAP mode when this RAM word (just past the end
// of ram0 in the linker script, so it survives reset) holds RESET_BL_MAGIC.
#define RAM_MAGIC_LOCATION 0x20003ffcU
#define RESET_BL_MAGIC     0x55aafaf5U

#ifdef RGB_MATRIX_ENABLE
void vortex_rgb_shutdown(void);
#endif

void bootloader_jump(void) {
#ifdef RGB_MATRIX_ENABLE
    vortex_rgb_shutdown();
#endif
    wait_us(10000); // 10 ms
    chSysDisable(); // mask all interrupts
    usbDisconnectBus(&USB_DRIVER); // disconnect usb
    *(volatile uint32_t *)RAM_MAGIC_LOCATION = RESET_BL_MAGIC;
    wait_us(50000); // 50 ms
    __disable_irq();
    NVIC_SystemReset();
}
