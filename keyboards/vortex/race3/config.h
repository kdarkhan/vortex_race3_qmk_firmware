/*
 * Copyright (c) 2018 Charlie Waters
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

#pragma once

#include "pin_defs.h"

// QMK derives CPU_CLOCK from CK_SYS (the 144 MHz PLL) on HT32, but the core
// runs at CK_AHB = 72 MHz, which made every wait_us() twice as long.
#define REALTIME_COUNTER_CLOCK 72000000UL


// #define DIODE_DIRECTION ROW2COL

// key matrix size
// #define MATRIX_ROWS 8
// #define MATRIX_COLS 11
// layer size: MATRIX_ROWS * MATRIX_COLS * sizeof(uint16_t) = 140 bytes

// outputs (rows are pulled low)
// #define MATRIX_ROW_PINS \ -
//    { C9, A5, A6, A7, C11, C13, C14, C15 }

// inputs (columns are sampled)
//#define MATRIX_COL_PINS \ -
//    { A3, A4, B10, D0, B11, A11, A0, A1, A2, D1, C10 }


// External SPI flash (GigaDevice GD25Q41B, 4 Mbit) on SPI1, same pins and
// commands as the stock firmware uses. QMK keeps its EEPROM data there.
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN B7
#define SPI_MOSI_PIN B8
#define SPI_MISO_PIN B9

#define EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN B6
#define EXTERNAL_FLASH_SPI_CLOCK_DIVISOR 16 // 72 MHz / 16 = 4.5 MHz
#define EXTERNAL_FLASH_PAGE_SIZE 256
#define EXTERNAL_FLASH_SECTOR_SIZE 4096
// QMK erases "blocks" with command 0xD8, which is a 64 KB erase on this chip
#define EXTERNAL_FLASH_BLOCK_SIZE (64 * 1024)
#define EXTERNAL_FLASH_SIZE (512 * 1024)

// Use the 64 KB block at 0x60000, away from the start of the flash where the
// stock firmware may keep its data. (QMK's range check rejects the last block.)
#define WEAR_LEVELING_EXTERNAL_FLASH_BLOCK_OFFSET 6
