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

#include QMK_KEYBOARD_H

enum layers {
    BASE_HRM, // default: QWERTY with home row mods
    BASE,     // plain QWERTY fallback: Fn+W switches to it, Fn+D back to BASE_HRM
    MAC,      // on top of either base when on macOS: swaps left Win and Alt
    NAV,      // hold Caps Lock on BASE_HRM: HJKL arrows
    FN1,
    PN,
};

enum custom_keycodes {
    EURO = SAFE_RANGE, // types €, see process_record_user
    MAC_ON,            // force macOS mode (MAC layer on)
    MAC_OFF,           // force Linux mode (MAC layer off)
};

// Home row mods, same as the Sofle keymap
#define HRM_A LSFT_T(KC_A)
#define HRM_S LALT_T(KC_S)
#define HRM_D LCTL_T(KC_D)
#define HRM_F LGUI_T(KC_F)
#define HRM_J RGUI_T(KC_J)
#define HRM_K RCTL_T(KC_K)
#define HRM_L RALT_T(KC_L)
#define HRM_SCLN RSFT_T(KC_SCLN)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * BASE_HRM: tap for the letter, hold for the modifier
 *   A = Shift, S = Alt, D = Ctrl, F = GUI   |   J = GUI, K = Ctrl, L = Alt, ; = Shift
 *   Caps Lock: tap = Esc, hold = NAV layer (like the Sofle's Esc key)
 * Pn (next to Delete) = PN layer, Fn (right of right Alt) = FN1 layer
 */
[BASE_HRM] = LAYOUT_75_ansi(
    KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  MO(PN),  KC_DEL,
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_HOME,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_PGUP,
    LT(NAV, KC_ESC), HRM_A, HRM_S,   HRM_D,   HRM_F,   KC_G,    KC_H,    HRM_J,   HRM_K,   HRM_L,   HRM_SCLN,KC_QUOT, KC_ENT,  KC_PGDN,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END,
    KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                    KC_RALT, MO(FN1), KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
),
[BASE] = LAYOUT_75_ansi(
    KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  MO(PN),  KC_DEL,
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_HOME,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_PGUP,
    KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,  KC_PGDN,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END,
    KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                    KC_RALT, MO(FN1), KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
),
/*
 * MAC: left Win/Alt become Alt/Win (Option/Command in Mac order)
 * Turned on by OS detection, or by hand with Fn+M / Fn+L; never saved to EEPROM
 */
[MAC] = LAYOUT_75_ansi(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, KC_LALT, KC_LGUI,                   _______,                   _______, _______, _______, _______, _______, _______
),
// HJKL arrows, vim style
[NAV] = LAYOUT_75_ansi(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______,                   _______,                   _______, _______, _______, _______, _______, _______
),
// Fn + W: plain QWERTY, Fn + D: home row mods (both remembered across replugs)
// Fn + F1..F6: mute, volume down/up, prev/play-pause/next track
// Fn + F10..F12: Print Screen, Scroll Lock, Pause; Fn + Del: Insert
// Fn + 4: €
// Fn + M: macOS mode (swap left Win/Alt), Fn + L: Linux mode
[FN1] = LAYOUT_75_ansi(
    _______, KC_MUTE, KC_VOLD, KC_VOLU, KC_MPRV, KC_MPLY, KC_MNXT, _______, _______, _______, KC_PSCR, KC_SCRL, KC_PAUS, _______, KC_INS,
    _______, _______, _______, _______, EURO,    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, PDF(BASE), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, PDF(BASE_HRM), _______, _______, _______, _______, _______, MAC_OFF, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, MAC_ON,  _______, _______, _______, _______, _______, _______,
    _______, _______, _______,                   _______,                   _______, _______, _______, _______, _______, _______
),
// Pn + 1..-: toggle, next/prev effect, hue, saturation, brightness, speed
[PN] = LAYOUT_75_ansi(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, RM_TOGG, RM_NEXT, RM_PREV, RM_HUEU, RM_HUED, RM_SATU, RM_SATD, RM_VALU, RM_VALD, RM_SPDU, RM_SPDD, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______,                   _______,                   _______, _______, _______, _______, _______, _______
),
};
// clang-format on

bool process_detected_host_os_user(os_variant_t os) {
    switch (os) {
        case OS_MACOS:
        case OS_IOS:
            layer_on(MAC);
            break;
        case OS_UNSURE:
            break; // keep the current mode, fix it by hand if needed
        default:
            layer_off(MAC);
            break;
    }
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case EURO:
            if (record->event.pressed) {
                if (layer_state_is(MAC)) {
                    // Built into the standard US layout, no Unicode Hex Input needed
                    tap_code16(LALT(LSFT(KC_2)));
                } else {
                    // Linux: ISO "< >" key + 4, with KDE's xkb options lv3:lsgt_switch
                    // ("< >" = AltGr, never used on ANSI) and eurosign:4 (AltGr+4 = €).
                    // Expected ~/.config/kxkbrc (System Settings > Keyboard > Key Bindings):
                    //   [Layout]
                    //   Options=caps:escape,eurosign:4,lv3:lsgt_switch
                    //   ResetOldOptions=true
                    register_code(KC_NUBS);
                    tap_code(KC_4);
                    unregister_code(KC_NUBS);
                }
            }
            return false;
        case MAC_ON:
            if (record->event.pressed) {
                layer_on(MAC);
            }
            return false;
        case MAC_OFF:
            if (record->event.pressed) {
                layer_off(MAC);
            }
            return false;
    }
    return true;
}
