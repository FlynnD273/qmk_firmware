#include QMK_KEYBOARD_H

#define COMBO_COUNT 1

bool override_color = false;

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// Tap dance enums
enum {
    X_CTL,
    SOME_OTHER_DANCE
};

td_state_t cur_dance(tap_dance_state_t *state);

// For the x tap dance. Put it here so it can be used in any keymap
void x_finished(tap_dance_state_t *state, void *user_data);
void x_reset(tap_dance_state_t *state, void *user_data);

enum layers {
    L_DEF = 0,
    L_SET,
    L_GAM,
    L_GAR,
    L_FNR,
    L_NUM,
    L_FNL,
    L_CAP,
};

enum custom_keycodes {
    XX_DELL = SAFE_RANGE,
    XX_GREY,
    XX_TERM,
};

enum combo_events {
    C_DELL = 0,
};

const uint16_t PROGMEM delete_combo[]  = {KC_BACKSLASH, KC_BACKSPACE, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
    [C_DELL] = COMBO(delete_combo, KC_DELETE),
};

enum {
    TD_SPC_DOT,
    TD_DOUBLE_WIN,
};

void dance_space_each(tap_dance_state_t *state, void *user_data) {
    switch (state->count) {
        case 1:
            tap_code(KC_SPC);
            break;
        case 2: // .
            tap_code(KC_BACKSPACE);
            register_code(KC_LALT);
            tap_code(KC_KP_4);
            tap_code(KC_KP_6);
            unregister_code(KC_LALT);
            tap_code(KC_SPC);
            break;
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_SPC_DOT] = ACTION_TAP_DANCE_FN_ADVANCED(dance_space_each, NULL, NULL),
    [TD_DOUBLE_WIN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, x_finished, x_reset),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case XX_DELL:
            if (record-> event.pressed) {
                tap_code(KC_HOME);
                tap_code(KC_HOME);
                SEND_STRING(SS_DOWN(X_LSFT));
                tap_code(KC_END);
                tap_code(KC_END);
                SEND_STRING(SS_UP(X_LSFT));
                tap_code(KC_BACKSPACE);
                tap_code(KC_DELETE);
                return false;
            }
            break;
        case XX_GREY:
            if (record-> event.pressed) {
                SEND_STRING(SS_DOWN(X_LCTL));
                SEND_STRING(SS_DOWN(X_LGUI));
                tap_code(KC_C);
                SEND_STRING(SS_UP(X_LCTL));
                SEND_STRING(SS_UP(X_LGUI));
                return false;
            }
            break;
        case XX_TERM:
            if (record-> event.pressed) {
                register_code(KC_LGUI);
                tap_code(KC_R);
                unregister_code(KC_LGUI);
                _delay_ms(250);
                SEND_STRING("wezterm-gui.exe");
                _delay_ms(250);
                tap_code(KC_ENTER);
                return false;
            }
            break;
        default:
        break;
    }
    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    if (override_color) {
        return state;
    }

    uint8_t h = 0, s = 255, v = 150;
    switch (get_highest_layer(state)) {
        case L_DEF:
            h = 148;
            break;
        case L_FNL:
        case L_FNR:
        case L_CAP:
            h = 220;
            break;
        case L_SET:
        case L_NUM:
            h = 0;
            break;
        case L_GAM:
            h = 85;
            break;
        case L_GAR:
            h = 40;
            break;
        default:
        break;
    }
    rgb_matrix_sethsv_noeeprom(h, s, v);

    return state;
}

void keyboard_post_init_user (void) {
    layer_move(0);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [L_DEF] = LAYOUT_60_ansi(
        KC_ESC,            KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,    KC_EQL,       KC_BSPC,
        KC_TAB,              KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,    KC_RBRC,    KC_BSLS,
        LT(L_CAP, KC_CAPS),    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,               KC_ENT,
        SC_LSPO,                 KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,                     SC_RSPC,
        KC_LCTL, TD(TD_DOUBLE_WIN),        KC_LALT,                     TD(TD_SPC_DOT),                    KC_RALT,  KC_LGUI,                  MO(L_FNR),  KC_RCTL
        ),

    [L_SET] = LAYOUT_60_ansi(
        XXXXXXX,   RGB_SPI, RGB_SPD, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,     XXXXXXX,
        XXXXXXX,     RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX,
        TO(L_DEF),     RGB_VAI, RGB_VAD, RGB_SAI, RGB_SAD, TO(L_GAM), XXXXXXX, TO(L_GAR), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          KC_NUM,
        _______,         XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                 _______,
        XXXXXXX,  XXXXXXX, XXXXXXX,                          XXXXXXX,                             XXXXXXX,  XXXXXXX,      XXXXXXX,   XXXXXXX
        ),

    [L_GAM] = LAYOUT_60_ansi(
        KC_ESC,             KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,    KC_EQL,      KC_BSPC,
        KC_TAB,               KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,    KC_RBRC,   KC_BSLS,
        KC_CAPS,                KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,              KC_ENT,
        KC_LSFT,                  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,                    KC_RSFT,
        KC_LCTL, MO(L_FNL),     KC_LALT,                              KC_SPC,                   KC_RALT,  KC_LGUI,                  MO(L_FNR), KC_RCTL
        ),

    [L_GAR] = LAYOUT_60_ansi(
        KC_ESC,             KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,    KC_EQL,     KC_BSPC,
        KC_TAB,               KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,    KC_RBRC,  KC_BSLS,
        KC_CAPS,                KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,             KC_ENT,
        KC_LSFT,                  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_UP,                     KC_RSFT,
        KC_LCTL, MO(L_FNL),     KC_LALT,                              KC_SPC,                   KC_LEFT,  KC_DOWN,                  KC_RIGHT, KC_RCTL
        ),

    [L_FNR] = LAYOUT_60_ansi(
        XXXXXXX,   KC_P1,  KC_P2,   KC_P3,   KC_P4,   KC_P5,   KC_P6,   KC_P7,   KC_P8,    KC_P9,   KC_P0,     XXXXXXX, XXXXXXX,   KC_DEL,
        XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX,   XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX,      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_LEFT,  KC_DOWN, KC_UP,   KC_RIGHT, XXXXXXX,  XXXXXXX,        _______,
        KC_LSFT,        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX,  TO(L_NUM),              KC_RSFT,
        KC_LCTL, XXXXXXX, KC_LALT,                          KC_SPACE,                            KC_RALT,   KC_LGUI,       _______, KC_RCTL
        ),

    [L_NUM] = LAYOUT_60_ansi(
        XXXXXXX,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX,   XXXXXXX,  KC_KP_SLASH,  XXXXXXX, XXXXXXX,             KC_BSPC,
        XXXXXXX,      XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX,   KC_KP_ASTERISK,  KC_P7,   KC_P8, KC_P9,             KC_BSLS,
        XXXXXXX,        XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    KC_KP_MINUS,            KC_P4,   KC_P5,  KC_P6,              KC_ENT,
        SC_LSPO,          XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_KP_PLUS,                     KC_P1,   KC_P2,  KC_P3,                SC_RSPC,
        XXXXXXX, MO(L_FNL), XXXXXXX,                          KC_SPACE,                                                    KC_P0,  KC_DOT,   TO(L_DEF),  XXXXXXX
        ),

    [L_FNL] = LAYOUT_60_ansi(
        KC_GRV,   KC_F1,    KC_F2,      KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,    KC_F11,    KC_F12,      KC_DEL,
        G(KC_TAB),    XXXXXXX,    XXXXXXX,  XXXXXXX,  G(KC_R),  XX_TERM,  XXXXXXX,  KC_HOME,  XXXXXXX,  KC_END,   XXXXXXX,   XXXXXXX,   XXXXXXX,  XXXXXXX,
        TO(L_SET),    KC_MPRV,  KC_MPLY,    KC_MNXT,  XXXXXXX,  XXXXXXX,  KC_LEFT,  KC_DOWN,  KC_UP,    KC_RIGHT, XXXXXXX,   XXXXXXX,            _______,
        KC_LSFT,        KC_VOLD,  KC_VOLU,    KC_MUTE,  G(KC_V),  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,                    KC_RSFT,
        KC_LCTL, XXXXXXX,  KC_LALT,                               G(KC_UP),                               KC_RALT,   KC_LGUI,          TO(L_DEF),  KC_RCTL
        ),

    [L_CAP] = LAYOUT_60_ansi(
        XXXXXXX,  XXXXXXX, XXXXXXX,    XXXXXXX,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   KC_F24,
        KC_PSCR,   KC_WBAK, KC_WREF,    KC_WFWD,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        _______,      XX_DELL, LGUI(KC_E),     LALT(KC_F4), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,        XXXXXXX,
        XXXXXXX,        XXXXXXX, XXXXXXX,    XXXXXXX,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,               XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX,                              XX_GREY,                               XXXXXXX,  XXXXXXX,      XXXXXXX, XXXXXXX
        ),
};

td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        // TD_DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
    }

    // Assumes no one is trying to type the same letter three times (at least not quickly).
    // If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
    // an exception here to return a 'TD_TRIPLE_SINGLE_TAP', and define that enum just like 'TD_DOUBLE_SINGLE_TAP'
    if (state->count == 3) {
        if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
        else return TD_TRIPLE_HOLD;
    } else return TD_UNKNOWN;
}

// Create an instance of 'td_tap_t' for the 'x' tap dance.
static td_tap_t xtap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void x_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: layer_on(L_FNL); break;
        case TD_SINGLE_HOLD: layer_on(L_FNL); break;
        case TD_DOUBLE_TAP: register_code(KC_LGUI); break;
        case TD_DOUBLE_HOLD: register_code(KC_LGUI); break;
        // Last case is for fast typing. Assuming your key is `f`:
        // For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
        // In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
        // case TD_DOUBLE_SINGLE_TAP: tap_code(KC_X); register_code(KC_X); break;
        default: break;
    }
}

void x_reset(tap_dance_state_t *state, void *user_data) {
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: layer_off(L_FNL); break;
        case TD_SINGLE_HOLD: layer_off(L_FNL); break;
        case TD_DOUBLE_TAP: unregister_code(KC_LGUI); break;
        case TD_DOUBLE_HOLD: unregister_code(KC_LGUI); break;
        // case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_X); break;
        default: break;
    }
    xtap_state.state = TD_NONE;
}
