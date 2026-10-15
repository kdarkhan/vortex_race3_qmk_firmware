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
#include "config_led.h"


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

// Obins stock firmware has something similar to this already enabled, but disabled by default in QMK
#define PERMISSIVE_HOLD

// SPI configuration
/* #define SPI_DRIVER SPID1 */
/* #define SPI_SCK_PIN A0 */
/* #define SPI_MOSI_PIN A1 */
/* #define SPI_MISO_PIN A2 */

/* // Flash configuration */
/* #define EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN A3 */
/* #define EXTERNAL_FLASH_SPI_CLOCK_DIVISOR 16 */
/* #define EXTERNAL_FLASH_PAGE_SIZE 256 */
/* #define EXTERNAL_FLASH_SECTOR_SIZE 4096 */
/* #define EXTERNAL_FLASH_BLOCK_SIZE 4096 */
/* #define EXTERNAL_FLASH_SIZE (256 * 1024) // 2M-bit flash size */

/* // Wear-leveling driver configuration */
/* #define WEAR_LEVELING_LOGICAL_SIZE 1024 */
/* #define WEAR_LEVELING_BACKING_SIZE (WEAR_LEVELING_LOGICAL_SIZE * 2) */
