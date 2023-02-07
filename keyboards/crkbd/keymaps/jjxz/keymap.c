/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include QMK_KEYBOARD_H
#include <stdio.h>

typedef union {
    uint32_t raw;
    struct {
        bool isCtrl :1; 
    };
} user_config_t;

user_config_t user_config;

enum custom_keycodes {
    FN_MO13 = SAFE_RANGE,
    FN_MO23
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3(
        MT(MOD_LGUI, KC_TAB),  KC_Q, KC_W, KC_E,    KC_R,  KC_T,        KC_Y,   KC_U,  KC_I,    KC_O,   KC_P,    KC_EQL, 
        MT(MOD_LALT, KC_CAPS), KC_A, KC_S, KC_D,    KC_F,  KC_G,        KC_H,   KC_J,  KC_K,    KC_L,   KC_SCLN, KC_QUOT, 
        KC_LSFT,               KC_Z, KC_X, KC_C,    KC_V,  KC_B,        KC_N,   KC_M,  KC_COMM, KC_DOT, KC_SLSH, KC_RSFT,
                                           KC_LCTL, FN_MO13, KC_BSPC,     KC_SPC, FN_MO23, KC_RCTL
    ),
    [1] = LAYOUT_split_3x6_3(
        MT(MOD_LGUI, KC_ESC), KC_GRV,  KC_UP,   KC_NO,   KC_VOLD, KC_VOLU,   KC_INS,  KC_LPRN, KC_RPRN, KC_HOME, KC_END, KC_MINS,
        KC_TRNS,              KC_LEFT, KC_DOWN, KC_RGHT, KC_MNXT, KC_MPRV,   KC_MINS, KC_LBRC, KC_RBRC, KC_PGUP, KC_PGDN, KC_NO,
        KC_TRNS,              KC_NO,   KC_NO,   KC_NO,   KC_MPLY, KC_MUTE,   KC_EQL,  KC_NO,   KC_TRNS, KC_TRNS, KC_BSLS, KC_NO,
                                                KC_TRNS, KC_TRNS,   KC_TRNS,   KC_ENT,  KC_TRNS, KC_TRNS
    ),
    [2] = LAYOUT_split_3x6_3(
        MT(MOD_LGUI, KC_ESC), KC_NO,   KC_MS_U, KC_NO,   KC_NO,   KC_NO,        KC_BTN1,   KC_7,    KC_8, KC_9, KC_NO, KC_WH_U,
        KC_NO,                KC_MS_L, KC_MS_D, KC_MS_R, KC_NO,   KC_NO,        KC_BTN2,   KC_4,    KC_5, KC_6, KC_0,  KC_WH_D,
        KC_TRNS,              KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,        KC_NO,   KC_1,    KC_2, KC_3, KC_NO, KC_TRNS,
                                                KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS,   KC_TRNS
    ),
    [3] = LAYOUT_split_3x6_3(
        MAGIC_TOGGLE_CTL_GUI, KC_F1, KC_F2, KC_F3,   KC_F4,   KC_F5,    KC_NO,   KC_7,    KC_8, KC_9, KC_NO, KC_NO, 
        KC_NO,                KC_F6, KC_F7, KC_F8,   KC_F9,   KC_F10,   KC_NO,   KC_4,    KC_5, KC_6, KC_0,  KC_NO, 
        KC_TRNS,              KC_NO, KC_NO, KC_NO,   KC_F11,  KC_F12,   KC_NO,   KC_1,    KC_2, KC_3, KC_NO, KC_TRNS, 
                                            KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS
    )
};

void keyboard_post_init_user(void) {
  // Call the keymap level matrix init.

  // Read the user config from EEPROM
  user_config.raw = eeconfig_read_user();
}

// bool isCtrl = true;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case FN_MO13:
            if (record->event.pressed) {
                layer_on(1);
                update_tri_layer(1, 2, 3);
            } else {
                layer_off(1);
                update_tri_layer(1, 2, 3);
            }
            return false;
            break;
        case FN_MO23:
            if (record->event.pressed) {
                layer_on(2);
                update_tri_layer(1, 2, 3);
            } else {
                layer_off(2);
                update_tri_layer(1, 2, 3);
            }
            return false;
            break;
        case MAGIC_TOGGLE_CTL_GUI:
            if (record->event.pressed) {
                // isCtrl = !isCtrl; 
                user_config.isCtrl ^= 1;
                eeconfig_update_user(user_config.raw);  
            }
        default:
            return true;
    }
}

void eeconfig_init_user(void) {
    user_config.isCtrl = true;
    eeconfig_update_user(user_config.raw);
}

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (!is_keyboard_master()) {
    return OLED_ROTATION_270;  // flips the display 180 degrees if offhand
  }
  return OLED_ROTATION_270;
}

#define L_BASE 0
#define L_LOWER 2
#define L_RAISE 4
#define L_ADJUST 8

void oled_render_layer_state(bool master) {
    oled_write_ln_P(PSTR("Layer"), false);
    switch (layer_state) {
        case L_BASE:
            oled_write_ln_P(PSTR("QWERTY"), false);
            break;
        case L_LOWER:
            oled_write_ln_P(master ? PSTR("ARROWS") : PSTR("SYMBOLS"), false);
            break;
        case L_RAISE:
            oled_write_ln_P(master ? PSTR("M-DIR") : PSTR("NUMPAD"), false);
            break;
        case L_ADJUST:
        case L_ADJUST|L_LOWER:
        case L_ADJUST|L_RAISE:
        case L_ADJUST|L_LOWER|L_RAISE:
            oled_write_ln_P(master? PSTR("FN") : PSTR("NUMPAD"), false);
            break;
    }
}
void oled_render_platform_mode(void) {
    if (user_config.isCtrl) {
        oled_write_ln_P(PSTR("CTRL"), false);
    } else {
        oled_write_ln_P(PSTR("GUI"), false);
    }
}

void oled_render_locks(void) { 
    led_t lock_state = host_keyboard_led_state();
    oled_write_ln_P(lock_state.caps_lock ? PSTR("CAPS") : PSTR(""), false);
}


// char keylog_str[24] = {};

// const char code_to_name[60] = {
//     ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
//     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
//     'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
//     '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
//     'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\',
//     '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

// void set_keylog(uint16_t keycode, keyrecord_t *record) {
//   char name = ' ';
//     if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) ||
//         (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX)) { keycode = keycode & 0xFF; }
//   if (keycode < 60) {
//     name = code_to_name[keycode];
//   }

//   // update keylog
//   snprintf(keylog_str, sizeof(keylog_str), "%dx%d, k%2d : %c",
//            record->event.key.row, record->event.key.col,
//            keycode, name);
// }

// void oled_render_keylog(void) {
//     oled_write(keylog_str, false);
// }

// void render_bootmagic_status(bool status) {
//     /* Show Ctrl-Gui Swap options */
//     static const char PROGMEM logo[][2][3] = {
//         {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
//         {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
//     };
//     if (status) {
//         oled_write_ln_P(logo[0][0], false);
//         oled_write_ln_P(logo[0][1], false);
//     } else {
//         oled_write_ln_P(logo[1][0], false);
//         oled_write_ln_P(logo[1][1], false);
//     }
// }

void oled_render_logo(void) {
    static const char PROGMEM crkbd_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
        0};
    oled_write_P(crkbd_logo, false);
}

// void oled_render_osmode(void) {
//     if (isCtrl) {
//         oled_write_P(PSTR("CTRL"), false);
//     } else {
//         oled_write_P(PSTR("GUI"), false);
//     }
// }

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_render_layer_state(true);
        oled_write_ln_P(PSTR(""), false);
        oled_render_platform_mode();
        oled_write_ln_P(PSTR(""), false);
        oled_render_locks();
        // oled_render_keylog();
    } else {
        oled_render_layer_state(false);
        oled_write_ln_P(PSTR(""), false);
        oled_render_locks();
        // oled_render_logo();

        // oled_render_osmode();
    }
    return false;
}

// bool process_record_user(uint16_t keycode, keyrecord_t *record) {
//   if (record->event.pressed) {
//     set_keylog(keycode, record);
//   }
//   return true;
// }
#endif // OLED_ENABLE
