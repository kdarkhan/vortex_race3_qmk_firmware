/* Copyright 2026 Darkhan Kubigenov
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

/*
 * RGB driver for the Vortex Race 3, reverse engineered from the stock v1.02.05
 * firmware (functions at 0x65ec, 0x6fe4, 0x7014, 0x63d4 and the BFTM1 ISR).
 *
 * Hardware:
 * - 3x MBIA043 (16 ch, 10-bit PWM) daisy chained, one chip per colour. The chip
 *   closest to the MCU drives red, the last one blue. OUTn is LED column n.
 *   SDI = A8, DCLK = A14, LE = A15, GCLK = C0 (GPTM1 CH0, 14.4 MHz).
 * - LED rows (8) are selected through a 74HCT138: A0..A2 = C4..C6, enable = B1
 *   (high blanks all rows). C1 switches the LED power on.
 * - Hardware LED index = column * 8 + row. Every key sits at the same position
 *   as its LED when columns are numbered in the stock firmware order, which
 *   differs from our matrix only by col 0 (A3) being stock column 8.
 *   Extra LEDs: 89/88 left/right of the spacebar, 90 next to caps lock. 83 and 78 are
 *   presumably the ISO-only keys and are not used.
 *
 * Protocol (MBIA043): bits are shifted MSB first on DCLK rising edges. LE high
 * during the last DCLK = "data latch" (one channel per chip), LE high during
 * 2 DCLKs = "global latch" (show the 16 latched channels). The stock firmware
 * also writes 0x00C to an undocumented configuration register at startup.
 */

#include "quantum.h"
#include "hal.h"

#define LED_ROWS 8
#define LED_COLS 16
#define LED_CHIPS 3
#define LED_HW_COUNT 96

// Extra LEDs appended after the keys in keyboard.json, in this order
#define VORTEX_EXTRA_LED_CAPS 90
#define VORTEX_EXTRA_LED_SPACE_LEFT 89
#define VORTEX_EXTRA_LED_SPACE_RIGHT 88

#define PIN_SDI (1U << 8)   // A8
#define PIN_DCLK (1U << 14) // A14
#define PIN_LE (1U << 15)   // A15

#define LINE_GCLK PAL_LINE(IOPORTC, 0)
#define LINE_LED_POWER PAL_LINE(IOPORTC, 1)
#define LINE_ROW_EN PAL_LINE(IOPORTB, 1)

// Row period, the stock firmware uses 60000 cycles at 72 MHz
#define ROW_PERIOD_US 833

// Turns 8-bit colour values into 10-bit PWM values, gamma 2.8
static const uint16_t gamma10[256] = {
       0,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
       1,    1,    1,    1,    1,    1,    1,    1,    1,    2,    2,    2,    2,    2,    3,    3,
       3,    3,    4,    4,    4,    5,    5,    5,    6,    6,    7,    7,    7,    8,    8,    9,
      10,   10,   11,   11,   12,   13,   13,   14,   15,   15,   16,   17,   18,   19,   20,   20,
      21,   22,   23,   24,   25,   26,   27,   29,   30,   31,   32,   33,   35,   36,   37,   38,
      40,   41,   43,   44,   46,   47,   49,   50,   52,   54,   55,   57,   59,   61,   63,   64,
      66,   68,   70,   72,   74,   77,   79,   81,   83,   85,   88,   90,   92,   95,   97,  100,
     102,  105,  107,  110,  113,  115,  118,  121,  124,  127,  130,  133,  136,  139,  142,  145,
     149,  152,  155,  158,  162,  165,  169,  172,  176,  180,  183,  187,  191,  195,  199,  203,
     207,  211,  215,  219,  223,  227,  232,  236,  240,  245,  249,  254,  258,  263,  268,  273,
     277,  282,  287,  292,  297,  302,  308,  313,  318,  323,  329,  334,  340,  345,  351,  357,
     362,  368,  374,  380,  386,  392,  398,  404,  410,  417,  423,  429,  436,  442,  449,  455,
     462,  469,  476,  483,  490,  497,  504,  511,  518,  525,  533,  540,  548,  555,  563,  571,
     578,  586,  594,  602,  610,  618,  626,  634,  643,  651,  660,  668,  677,  685,  694,  703,
     712,  721,  730,  739,  748,  757,  766,  776,  785,  795,  804,  814,  824,  833,  843,  853,
     863,  873,  884,  894,  904,  915,  925,  936,  946,  957,  968,  979,  990, 1001, 1012, 1023,
};

// QMK LED index -> hardware LED index
static uint8_t hw_index[RGB_MATRIX_LED_COUNT];

// Colours per hardware LED, written by the RGB matrix code
static uint8_t colors[LED_HW_COUNT][LED_CHIPS];
static bool    colors_dirty;

// PWM values per row, word index = column * 3 + chip. The ISR shows `frames[front]`
// and swaps in the other buffer at the start of a frame when `frame_pending` is set.
static uint16_t      frames[2][LED_ROWS][LED_COLS * LED_CHIPS];
static volatile bool frame_pending;
static uint8_t       front;
static uint8_t       row;
static uint8_t       config_countdown;

static inline void delay_short(void) {
    __NOP();
    __NOP();
}

// LE high for `clocks` DCLK pulses: 1 = data latch, 2 = global latch
static void send_command(uint8_t clocks) {
    GPIOA->RR = PIN_LE;
    delay_short();
    GPIOA->SRR = PIN_LE;
    while (clocks--) {
        GPIOA->RR = PIN_DCLK;
        delay_short();
        GPIOA->SRR = PIN_DCLK;
        __NOP();
    }
    GPIOA->RR = PIN_LE;
}

// Shift out the top `bits` bits of `value`, MSB first
static void send_bits(uint16_t value, uint8_t bits) {
    while (bits--) {
        __NOP();
        GPIOA->RR = PIN_DCLK;
        if (value & 0x8000) {
            GPIOA->SRR = PIN_SDI;
        } else {
            GPIOA->RR = PIN_SDI;
        }
        GPIOA->SRR = PIN_DCLK;
        value <<= 1;
    }
}

// Same sequence the stock firmware sends a few times after power up
static void send_config(void) {
    send_command(18);
    send_bits(0x00C << 6, 10);
    send_bits(0x00C << 6, 10);
    send_bits(0, 2);
    GPIOA->RR  = PIN_DCLK;
    GPIOA->SRR = PIN_LE;
    send_bits(0x0C << 8, 8);
    GPIOA->SRR = PIN_SDI;
    GPIOA->RR  = PIN_LE;
    delay_short();
    GPIOA->SRR = PIN_LE;
}

// Shift one row into the chips' buffers, shown by the next global latch
static void send_row(const uint16_t *words) {
    if (config_countdown) {
        config_countdown--;
        if ((config_countdown & ~0x0C) == 0) {
            send_config();
        }
    }

    // OUT15 first; per latch the farthest chip (blue) gets its bits first
    for (int out = LED_COLS - 1; out >= 0; out--) {
        const uint16_t *w = &words[out * LED_CHIPS];
        GPIOA->RR         = PIN_LE;
        send_bits(w[2] << 6, 10);
        send_bits(w[1] << 6, 10);
        uint16_t last = w[0] << 6;
        send_bits(last, 9);
        // 10th bit with LE high: data latch
        GPIOA->RR  = PIN_DCLK;
        GPIOA->SRR = PIN_LE;
        if (last & (1U << 6)) {
            GPIOA->SRR = PIN_SDI;
        } else {
            GPIOA->RR = PIN_SDI;
        }
        GPIOA->SRR = PIN_DCLK;
    }
    GPIOA->SRR = PIN_SDI;
}

static void row_timer_cb(GPTDriver *gptp) {
    (void)gptp;

    // Blank, show the row shifted in during the previous tick, unblank
    palSetLine(LINE_ROW_EN);
    chSysPolledDelayX(US2RTC(REALTIME_COUNTER_CLOCK, 10));
    send_command(2);
    GPIOA->SRR = PIN_LE;
    GPIOC->SRR = ((row & 1) ? (1U << 4) : (1U << (4 + 16))) | ((row & 2) ? (1U << 5) : (1U << (5 + 16))) | ((row & 4) ? (1U << 6) : (1U << (6 + 16)));
    palClearLine(LINE_ROW_EN);

    row = (row + 1) % LED_ROWS;
    if (row == 0 && frame_pending) {
        front ^= 1;
        frame_pending = false;
    }
    send_row(frames[front][row]);
}

static const GPTConfig row_timer_config = {
    .frequency = 1000000,
    .callback  = row_timer_cb,
};

static const PWMConfig gclk_config = {
    .frequency = 72000000,
    .period    = 4, // CRR = 4: 72 MHz / 5 = 14.4 MHz
    .callback  = NULL,
    .channels =
        {
            {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL},
            {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
            {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
            {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
        },
};

static uint8_t stock_col(uint8_t col) {
    // our col 0 (A3) is stock column 8, our cols 1..8 are stock 0..7
    if (col == 0) return 8;
    if (col <= 8) return col - 1;
    return col;
}

static void init(void) {
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        for (uint8_t c = 0; c < MATRIX_COLS; c++) {
            uint8_t i = g_led_config.matrix_co[r][c];
            if (i != NO_LED && i < RGB_MATRIX_LED_COUNT) {
                hw_index[i] = stock_col(c) * LED_ROWS + r;
            }
        }
    }
    hw_index[RGB_MATRIX_LED_COUNT - 3] = VORTEX_EXTRA_LED_CAPS;
    hw_index[RGB_MATRIX_LED_COUNT - 2] = VORTEX_EXTRA_LED_SPACE_LEFT;
    hw_index[RGB_MATRIX_LED_COUNT - 1] = VORTEX_EXTRA_LED_SPACE_RIGHT;

    palSetLine(LINE_ROW_EN);
    palSetLineMode(LINE_ROW_EN, PAL_MODE_HT32_AF(AFIO_GPIO) | PAL_HT32_MODE_DIR);
    palClearLine(LINE_LED_POWER);
    palSetLineMode(LINE_LED_POWER, PAL_MODE_HT32_AF(AFIO_GPIO) | PAL_HT32_MODE_DIR);
    GPIOA->SRR = PIN_SDI | PIN_DCLK | PIN_LE;
    palSetGroupMode(IOPORTA, PIN_SDI | PIN_DCLK | PIN_LE, 0, PAL_MODE_HT32_AF(AFIO_GPIO) | PAL_HT32_MODE_DIR);
    GPIOC->SRR = (1U << 4) | (1U << 5) | (1U << 6);
    palSetGroupMode(IOPORTC, (1U << 4) | (1U << 5) | (1U << 6), 0, PAL_MODE_HT32_AF(AFIO_GPIO) | PAL_HT32_MODE_DIR);

    wait_ms(10);
    palSetLine(LINE_LED_POWER);
    wait_us(100);

    config_countdown = 0xff;
    row              = 0;
    send_row(frames[front][0]);
    send_command(2);
    GPIOA->SRR = PIN_LE;

    palSetLineMode(LINE_GCLK, PAL_MODE_HT32_AF(AFIO_TM) | PAL_HT32_MODE_DIR);
    pwmStart(&PWMD_GPTM1, &gclk_config);
    pwmEnableChannel(&PWMD_GPTM1, 0, 2);

    gptStart(&GPTD_BFTM1, &row_timer_config);
    gptStartContinuous(&GPTD_BFTM1, ROW_PERIOD_US);
}

static void set_color(int index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < 0 || index >= RGB_MATRIX_LED_COUNT) return;
    uint8_t *c = colors[hw_index[index]];
    if (c[0] != r || c[1] != g || c[2] != b) {
        c[0]         = r;
        c[1]         = g;
        c[2]         = b;
        colors_dirty = true;
    }
}

static void set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        set_color(i, r, g, b);
    }
}

static void flush(void) {
    // the ISR has not picked up the previous frame yet, try again next time
    if (!colors_dirty || frame_pending) return;

    uint16_t(*back)[LED_COLS * LED_CHIPS] = frames[front ^ 1];
    for (uint8_t r = 0; r < LED_ROWS; r++) {
        for (uint8_t c = 0; c < LED_COLS; c++) {
            uint8_t hw = c * LED_ROWS + r;
            for (uint8_t chip = 0; chip < LED_CHIPS; chip++) {
                back[r][c * LED_CHIPS + chip] = hw < LED_HW_COUNT ? gamma10[colors[hw][chip]] : 0;
            }
        }
    }
    colors_dirty  = false;
    frame_pending = true;
}

// Called before jumping to the bootloader. The stock firmware also turns the LEDs
// off first (0x64ec): after a reset the LED pins float and the drivers keep
// whatever they last showed, which made bootloader flash erase/write unreliable.
void vortex_rgb_shutdown(void) {
    gptStopTimer(&GPTD_BFTM1);
    gptStop(&GPTD_BFTM1);
    palSetLine(LINE_ROW_EN);
    palClearLine(LINE_LED_POWER);
    pwmStop(&PWMD_GPTM1);
}

const rgb_matrix_driver_t rgb_matrix_driver = {
    .init          = init,
    .flush         = flush,
    .set_color     = set_color,
    .set_color_all = set_color_all,
};

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {
    if (!rgb_matrix_indicators_advanced_user(led_min, led_max)) {
        return false;
    }
    if (host_keyboard_led_state().caps_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(RGB_MATRIX_LED_COUNT - 3, 255, 255, 255);
    }
    return true;
}
