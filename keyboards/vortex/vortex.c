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

#define RESET_COUNTER_START 0x55aafaf0U

#ifdef RGB_MATRIX_ENABLE
void vortex_rgb_shutdown(void);
#endif

/*
 * Watchdog: the stock bootloader enables it before starting the firmware
 * (WDTMR1 = 0x5FFF, WDTMR0 = 0x1AFFF: ~4s at LSI/32, reset on underflow) and
 * counts watchdog resets at RAM_MAGIC_LOCATION, from RESET_COUNTER_START up
 * to RESET_BL_MAGIC. After 5 of them it stays in the bootloader. Feeding it
 * from a timer interrupt keeps it quiet during normal operation, but a
 * firmware that crashes stops the feeding and ends up back in the
 * bootloader, where it can be reflashed without a debugger.
 */
static virtual_timer_t watchdog_timer;

static void watchdog_feed(virtual_timer_t *vtp, void *p) {
    (void)vtp;
    (void)p;
    *(volatile uint32_t *)&WDT->CR = 0x5FA00001; // RSKEY | WDTRS
}

void keyboard_pre_init_kb(void) {
    watchdog_feed(NULL, NULL);
    chVTObjectInit(&watchdog_timer);
    chVTSetContinuous(&watchdog_timer, TIME_MS2I(500), watchdog_feed, NULL);

    keyboard_pre_init_user();
}

void housekeeping_task_kb(void) {
    // Once the firmware has run for a while, forget earlier watchdog resets so
    // that only crashes in a row send the keyboard to the bootloader
    static bool reset_counter_cleared = false;
    if (!reset_counter_cleared && timer_read32() > 10000) {
        volatile uint32_t *counter = (volatile uint32_t *)RAM_MAGIC_LOCATION;
        if (*counter > RESET_COUNTER_START && *counter < RESET_BL_MAGIC) {
            *counter = RESET_COUNTER_START;
        }
        reset_counter_cleared = true;
    }

    housekeeping_task_user();
}

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
