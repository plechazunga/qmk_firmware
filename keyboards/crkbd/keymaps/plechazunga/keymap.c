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

extern uint8_t  is_master;
static uint32_t oled_timer = 0;

#ifdef RGB_MATRIX_ENABLE
    static uint32_t hypno_timer;
#endif

enum custom_keycodes {
    BSP_DEL = SAFE_RANGE,
    RGB_RST,  // Reset RGB
    RGB_UND,  // Toggle RGB underglow as layer indicator
    RGB_IDL,  // RGB Idling animations
    RGB_MAP,  // RGB_MATRIX_TYPING_HEATMAP
    RGB_NXS,  // RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
    RGB_SOL,  // RGB_MATRIX_SOLID_COLOR
    RGB_CYC,  // RGB_MATRIX_CYCLE_ALL
    RGB_DUO,  // RGB_MATRIX_RAINBOW_PINWHEELS
    RGB_SCR   // RGB_MATRIX_CYCLE_LEFT_RIGHT
};

typedef union {
    uint32_t raw;
    struct {
        bool     rgb_layer_change        : 1;
        bool     rgb_matrix_idle_anim    : 1;
        uint8_t  rgb_matrix_active_mode  : 4;
        uint8_t  rgb_matrix_idle_mode    : 4;
        uint8_t  rgb_matrix_active_speed : 8;
        uint8_t  rgb_matrix_idle_speed   : 8;
        uint16_t rgb_matrix_idle_timeout : 16;
    };
} user_config_t;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_ESC,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_MINS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_BSPC,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_RSFT,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, KC_LCTL,  KC_ENT,     KC_SPC, KC_LCTL, KC_RALT
                                      //`--------------------------'  `--------------------------'

  ),

  [1] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_LEFT, KC_DOWN,   KC_UP,KC_RIGHT, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT,   MO(3), KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

  [2] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                      KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_MINS,  KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS,  KC_GRV,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI,   MO(3),  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

  [3] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        RESET, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  )
};

#ifdef OLED_DRIVER_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_master) {
        return OLED_ROTATION_270;
    } else {
        return OLED_ROTATION_180;
    }
}

void render_crkbd_logo(void) {
    static const char PROGMEM crkbd_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
        0};
    oled_write_P(crkbd_logo, false);
}

#    ifdef RGB_MATRIX_ENABLE
const char *rgb_matrix_anim_oled_text(uint8_t mode) {
    switch (mode) {
        case RGB_MATRIX_TYPING_HEATMAP:
            return PSTR("Heat ");
        case RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS:
            return PSTR("Nexus");
        case RGB_MATRIX_SOLID_COLOR:
            return PSTR("Solid");
        case RGB_MATRIX_CYCLE_ALL:
            return PSTR("Cycle");
        case RGB_MATRIX_RAINBOW_PINWHEELS:
            return PSTR("Wheel");
        case RGB_MATRIX_CYCLE_LEFT_RIGHT:
            return PSTR("Wave ");
        default:
            return PSTR("");
    }
}
#    endif

#    ifdef RGB_MATRIX_ENABLE
    oled_write_P(PSTR("\n"), false);
    oled_write_P(PSTR("\n"), false);

    if (rgb_matrix_config.enable) {
        if (user_config.rgb_matrix_idle_anim) {
            oled_write_P(rgb_matrix_anim_oled_text(user_config.rgb_matrix_active_mode), false);
            oled_write_P(rgb_matrix_anim_oled_text(user_config.rgb_matrix_idle_mode), false);
        } else {
            oled_write_P(PSTR("\n"), false);
            oled_write_P(rgb_matrix_anim_oled_text(rgb_matrix_get_mode()), false);
        }
      } else {
        oled_write_P(PSTR("\n"), false);
        oled_write_P(PSTR("\n"), false);
      }

#ifdef RGB_MATRIX_ENABLE

extern led_config_t g_led_config;
void rgb_matrix_layer_helper(uint8_t hue, uint8_t sat, uint8_t val, uint8_t led_type) {
    HSV hsv = {hue, sat, val};
    if (hsv.v > rgb_matrix_config.hsv.v) {
        hsv.v = rgb_matrix_config.hsv.v;
    }

    RGB rgb = hsv_to_rgb(hsv);
    for (uint8_t i = 0; i < DRIVER_LED_TOTAL; i++) {
        if (HAS_FLAGS(g_led_config.flags[i], led_type)) {
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }
}

void check_default_layer(uint8_t type) {
    switch (get_highest_layer(default_layer_state)) {
        case _COLEMAKDHM:
            rgb_matrix_layer_helper(THEME_HSV, type);
            break;
        case _GAMING:
            rgb_matrix_layer_helper(HSV_RED, type);
            break;
    }
}

void rgb_matrix_indicators_user(void) {
  if (
    user_config.rgb_layer_change && !g_suspend_state && rgb_matrix_config.enable &&
      (!user_config.rgb_matrix_idle_anim || rgb_matrix_get_mode() != user_config.rgb_matrix_idle_mode)
  )
    {
        switch (get_highest_layer(layer_state)) {
            case _GAMING_EXT:
                rgb_matrix_layer_helper(HSV_PURPLE, LED_FLAG_UNDERGLOW);
                break;
            case _SYM:
                rgb_matrix_layer_helper(HSV_GOLDENROD, LED_FLAG_UNDERGLOW);
                break;
            case _NAV:
                rgb_matrix_layer_helper(HSV_SPRINGGREEN, LED_FLAG_UNDERGLOW);
                break;
            case _UTIL:
                rgb_matrix_layer_helper(HSV_PINK, LED_FLAG_UNDERGLOW);
                break;
            case _NUMPAD:
                rgb_matrix_layer_helper(HSV_CORAL, LED_FLAG_UNDERGLOW);
                break;
            default: {
                check_default_layer(LED_FLAG_UNDERGLOW);
                break;
            }
        }
    }
}

void rgb_matrix_update_current_mode(uint8_t mode, uint8_t speed) {
    rgb_matrix_config.speed = speed;
    rgb_matrix_mode_noeeprom(mode);
    eeconfig_update_user(user_config.raw);
}

void rgb_matrix_update_dynamic_mode(uint8_t mode, uint8_t speed, bool active) {
    if (active) {
        user_config.rgb_matrix_active_speed = speed;
        user_config.rgb_matrix_active_mode  = mode;
    } else {
        user_config.rgb_matrix_idle_speed = speed;
        user_config.rgb_matrix_idle_mode  = mode;
    }
}

void rgb_matrix_update_mode(uint8_t mode, uint8_t speed, bool active) {
    if (user_config.rgb_matrix_idle_anim) {
        rgb_matrix_update_dynamic_mode(mode, speed, active);
    }
    if (active || !user_config.rgb_matrix_idle_anim) {
        rgb_matrix_update_current_mode(mode, speed);
    }
}

void rgb_matrix_set_defaults(void) {
    rgb_matrix_config.enable = 1;
    rgb_matrix_sethsv_noeeprom(THEME_HSV);

    user_config.rgb_layer_change        = false;
    user_config.rgb_matrix_idle_anim    = true;
    user_config.rgb_matrix_idle_timeout = 60000;

    rgb_matrix_update_dynamic_mode(RGB_MATRIX_CYCLE_ALL, RGB_MATRIX_ANIMATION_SPEED_SLOWER, false);
    rgb_matrix_update_dynamic_mode(RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS, RGB_MATRIX_ANIMATION_SPEED_DEFAULT, true);

    eeprom_update_block(&rgb_matrix_config, EECONFIG_RGB_MATRIX, sizeof(rgb_matrix_config));
}

void matrix_scan_rgb(void) {
    if (user_config.rgb_matrix_idle_anim && rgb_matrix_get_mode() == user_config.rgb_matrix_active_mode && timer_elapsed32(hypno_timer) > user_config.rgb_matrix_idle_timeout) {
        if (user_config.rgb_layer_change) {
            rgb_matrix_layer_helper(0, 0, 0, LED_FLAG_UNDERGLOW);
        }
        rgb_matrix_update_current_mode(user_config.rgb_matrix_idle_mode, user_config.rgb_matrix_idle_speed);
    }
}

void matrix_scan_user(void) {
    static bool has_ran_yet;
    if (!has_ran_yet) {
        has_ran_yet = true;
        startup_user();
    }
    matrix_scan_rgb();
}

void eeconfig_init_user(void) {
    user_config.raw = 0;
    rgb_matrix_mode_noeeprom(user_config.rgb_matrix_active_mode);
    keyboard_init();
}

void keyboard_post_init_user(void) {
    user_config.raw = eeconfig_read_user();
    rgb_matrix_set_defaults();
    rgb_matrix_enable_noeeprom();
}
#endif

#ifdef RGB_MATRIX_ENABLE
        case COLEMAK:
            if (record->event.pressed) {
                user_config.rgb_matrix_idle_timeout = 60000;
                rgb_matrix_update_mode(RGB_MATRIX_CYCLE_ALL, RGB_MATRIX_ANIMATION_SPEED_SLOWER, false);
            }
            return true;
        case GAMING:
            if (record->event.pressed) {
                if (!user_config.rgb_layer_change) {
                    user_config.rgb_layer_change = true;
                }
                user_config.rgb_matrix_idle_timeout = 10000;
                rgb_matrix_update_mode(RGB_MATRIX_RAINBOW_PINWHEELS, RGB_MATRIX_ANIMATION_SPEED_SLOW, false);
            }
            return true;
        case RGB_RST:
            if (record->event.pressed) {
                rgb_matrix_set_defaults();
                rgb_matrix_enable();
            }
            break;
        case RGB_UND:  // Toggle separate underglow status
            if (record->event.pressed) {
                user_config.rgb_layer_change ^= 1;
                eeconfig_update_user(user_config.raw);
                if (user_config.rgb_layer_change) {
                    layer_state_set(layer_state);  // This is needed to immediately set the layer color (looks better)
                } else {
                    rgb_matrix_layer_helper(0, 0, 0, LED_FLAG_UNDERGLOW);
                }
            }
            break;
        case RGB_IDL:  // Toggle idle/heatmap animation
            if (record->event.pressed) {
                user_config.rgb_matrix_idle_anim ^= 1;
                if (user_config.rgb_matrix_idle_anim) {
                    rgb_matrix_update_mode(user_config.rgb_matrix_active_mode, user_config.rgb_matrix_active_speed, true);
                } else {
                    rgb_matrix_update_current_mode(user_config.rgb_matrix_idle_mode, user_config.rgb_matrix_idle_speed);
                }
            }
            break;
        case RGB_MAP:
            if (record->event.pressed) {
                rgb_matrix_update_mode(RGB_MATRIX_TYPING_HEATMAP, rgb_matrix_config.speed, true);
            }
            break;
        case RGB_NXS:
            if (record->event.pressed) {
                rgb_matrix_update_mode(RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS, RGB_MATRIX_ANIMATION_SPEED_DEFAULT, true);
            }
            break;
        case RGB_SOL:
            if (record->event.pressed) {
                rgb_matrix_update_mode(RGB_MATRIX_SOLID_COLOR, rgb_matrix_config.speed, false);
            }
            break;
        case RGB_CYC:
            if (record->event.pressed) {
                rgb_matrix_update_mode(RGB_MATRIX_CYCLE_ALL, RGB_MATRIX_ANIMATION_SPEED_SLOWER, false);
            }
            break;
        case RGB_DUO:
            if (record->event.pressed) {
                rgb_matrix_update_mode(RGB_MATRIX_RAINBOW_PINWHEELS, RGB_MATRIX_ANIMATION_SPEED_SLOW, false);
            }
            break;
        case RGB_SCR:
            if (record->event.pressed) {
                rgb_matrix_update_mode(RGB_MATRIX_CYCLE_LEFT_RIGHT, RGB_MATRIX_ANIMATION_SPEED_SLOW, false);
            }
            break;
#endif
    }
    return true;
}

#define L_BASE 0
#define L_LOWER 2
#define L_RAISE 4
#define L_ADJUST 8

void oled_render_layer_state(void) {
    oled_write_P(PSTR("Layer: "), false);
    switch (layer_state) {
        case L_BASE:
            oled_write_ln_P(PSTR("Default"), false);
            break;
        case L_LOWER:
            oled_write_ln_P(PSTR("Lower"), false);
            break;
        case L_RAISE:
            oled_write_ln_P(PSTR("Raise"), false);
            break;
        case L_ADJUST:
        case L_ADJUST|L_LOWER:
        case L_ADJUST|L_RAISE:
        case L_ADJUST|L_LOWER|L_RAISE:
            oled_write_ln_P(PSTR("Adjust"), false);
            break;
    }
}


char keylog_str[24] = {};

const char code_to_name[60] = {
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\',
    '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

void set_keylog(uint16_t keycode, keyrecord_t *record) {
  char name = ' ';
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) ||
        (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX)) { keycode = keycode & 0xFF; }
  if (keycode < 60) {
    name = code_to_name[keycode];
  }

  // update keylog
  snprintf(keylog_str, sizeof(keylog_str), "%dx%d, k%2d : %c",
           record->event.key.row, record->event.key.col,
           keycode, name);
}

void oled_render_keylog(void) {
    oled_write(keylog_str, false);
}

void render_bootmagic_status(bool status) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM logo[][2][3] = {
        {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
        {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
    };
    if (status) {
        oled_write_ln_P(logo[0][0], false);
        oled_write_ln_P(logo[0][1], false);
    } else {
        oled_write_ln_P(logo[1][0], false);
        oled_write_ln_P(logo[1][1], false);
    }
}

void oled_render_logo(void) {
    static const char PROGMEM crkbd_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
        0};
    oled_write_P(crkbd_logo, false);
}

void oled_task_user(void) {
    if (is_master) {
        oled_render_layer_state();
        oled_render_keylog();
    } else {
        oled_render_logo();
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    set_keylog(keycode, record);
  }
  return true;
}
#endif // OLED_DRIVER_ENABLE
