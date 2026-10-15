/*
 * Copyright (c) 2023 Darkhan Kubigenov
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
#include "quantum.h"
#include <stdint.h>

// Matrix layout (positions are defined in info.json):
//                 COL1   COL2   COL3   COL4   COL5   COL6   COL7   COL8   COL9   COL10  COL11  
//   ROW1  11  { KC_NO, K10,   K12,   K14,   K16,   K18,   K1A,   K1C,   K0D,   K00,   K04,   },
//   ROW2  6   { K2E,   K11,   K13,   K15,   K17,   K19,   K1B,   K1D,   K1E,   K01,   K05,   },
//   ROW3  7   { K51,   K20,   K22,   K24,   K26,   K28,   K2A,   K2C,   K0E,   K02,   K06,   },
//   ROW4  8   { K50,   K21,   K23,   K25,   K27,   K29,   K2B,   K2D,   K4D,   K03,   KC_NO, },
//   ROW5  13  { K52,   K30,   K32,   K34,   K36,   K38,   K3A,   K3C,   K55,   K09,   K07,   },
//   ROW6  22  { K53,   K31,   K33,   K35,   K37,   K39,   K3B,   K3D,   K4C,   K08,   K0A,   },
//   ROW7  23  { K54,   K40,   K42,   K44,   K46,   K48,   K4A,   K56,   K58,   KC_NO, K0B,   },
//   ROW8  24  { KC_NO, K41,   K43,   K45,   K47,   K49,   K4B,   K59,   K57,   KC_NO, K0C,   },
//   pcb pins:      4      5      61     25     62     37     1      2      3      28     12
