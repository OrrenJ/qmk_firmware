#include QMK_KEYBOARD_H

void set_base_layer_colors(void);

#include <print.h>
#include <string.h>
// HID has not yet been implemented for this keyboard
// #include "raw_hid.h"

#define MILLISECONDS_IN_SECOND 1000

// These are just to make it neater to use builtin HSV values in the keymap
#define RED { 0, 255, 255 }
#define CORAL { 11, 255, 255 }      // default 11, 176, 255
#define ORANGE { 28, 255, 255 }
#define GOLDEN { 30, 255, 255 }     // default 30, 218, 255
#define GOLD { 36, 255, 255 }
#define YELLOW { 43, 255, 255 }
#define CHART { 64, 255, 255 } 
#define GREEN { 85, 255, 255  }
#define SPRING { 106, 255, 255 }
#define TURQ { 123, 255, 255 }      // default 123, 90, 112
#define CYAN { 144, 255, 255 }
#define AZURE { 158, 255, 255 }     // default 132, 102, 255
#define BLUE { 170, 255, 255 }
#define PURPLE { 191, 255, 255 }
#define MAGENT { 213, 255, 255 }
#define PINK { 234, 255, 255 }      // 234, 128, 255
#define WHITE { 0, 0, 255 }

//========================================================== CONFIGURABLE DEFAULTS ==========================================================
#define RGB_DEFAULT_TIME_OUT 240
#define RGB_FAST_MODE_TIME_OUT 3
#define RGB_TIME_OUT_MAX 600
#define RGB_TIME_OUT_MIN 10
#define RGB_TIME_OUT_STEP 10

extern bool g_suspend_state;
extern rgb_config_t rgb_matrix_config;
bool disable_layer_color;

bool rgb_enabled_flag;                  // Current LED state flag. If false then LED is off.
bool rgb_time_out_enable;               // Idle LED toggle enable. If false then LED will not turn off after idle timeout.
bool rgb_time_out_fast_mode_enabled;    // Enable flag for RGB timeout fast mode
bool rgb_time_out_user_value;           // This holds the toggle value set by user with ROUT_TG. It's necessary as RGB_TOG changes timeout enable.
uint16_t rgb_time_out_seconds;          // Idle LED timeout value, in seconds not milliseconds
uint16_t rgb_time_out_saved_seconds;    // The saved user config for RGB timeout period
led_flags_t rgb_time_out_saved_flag;    // Store LED flag before timeout so it can be restored when LED is turned on again.

enum layout_names {
    _KL=0,       // Keys Layout: The main keyboard layout that has all the characters
    _FL,         // Function Layout: The function key activated layout with default functions and some added ones
};

enum tapdance_keycodes {
    TD_LGUI_ML = 0,     // Tap dance key to switch to mouse layer _ML
    TD_APP_YL,
    TD_CTRL_TERM,       // Tap dance key to open terminal on LCTRL double press
};

enum ctrl_keycodes {
    U_T_AUTO = SAFE_RANGE, // USB Extra Port Toggle Auto Detect / Always Active
    U_T_AGCR,              // USB Toggle Automatic GCR control
    DBG_TOG,               // DEBUG Toggle On / Off
    DBG_MTRX,              // DEBUG Toggle Matrix Prints
    DBG_KBD,               // DEBUG Toggle Keyboard Prints
    DBG_MOU,               // DEBUG Toggle Mouse Prints
    MD_BOOT,               // Restart into bootloader after hold timeout
    SEL_CPY,               // Select Copy. Select the word cursor is pointed at and copy, using double mouse click and ctrl+c
    ROUT_TG,               // Timeout Toggle. Toggle idle LED time out on or off
    ROUT_VI,               // Timeout Value Increase. Increase idle time out before LED disabled
    ROUT_VD,               // Timeout Value Decrease. Decrease idle time out before LED disabled
    ROUT_FM,               // RGB timeout fast mode toggle
    COPY_ALL,              // Copy all text using ctrl(a+c)
    TERMINAL,              // CTRL+ALT+T
    // custom codes
    MAIN_P,                 // previous color for the main alphanumeric keys
    MAIN_N,                 // next color for the main alphanumeric keys
    WASD_P,                 // previous color for wasd keys
    WASD_N,                 // next color for wasd keys
    MODS_P,                 // previous color for modifier keys
    MODS_N,                 // next color for modifier keys
    RGHT_P,                 // previous color for right side keys
    RGHT_N,                 // next color for right side keys
    MAIN_T,                 // toggle RGB scroll for the main alphanumeric keys 
    WASD_T,                 // toggle RGB scroll for WASD keys
    MODS_T,                 // toggle RGB scroll for modifier keeps
    RGHT_T,                 // toggle RGB scroll for right side keys
    E_SPD_U,                // RGB effect speed up
    E_SPD_D,                // RGB effect speed down
    RGB_INV,                // toggle invert color on key press
    RGB_TIM,                // toggle RGB time out
    RGB_SWT,                // switch RGB on and off
};

enum string_macro_keycodes {
    // The start of this enum should always be equal to end of ctrl_keycodes + 1
    G_INIT = TERMINAL + 1, // git init
    G_CLONE,               // git clone
    G_CONF,                // git config --global
    G_ADD,                 // git add
    G_DIFF,                // git diff
    G_RESET,               // git reset
    G_REBAS,               // git rebase
    G_BRANH,               // git branch
    G_CHECK,               // git checkout
    G_MERGE,               // git merge
    G_REMTE,               // git remote add
    G_FETCH,               // git fetch
    G_PULL,                // git pull
    G_PUSH,                // git push
    G_COMM,                // git commit
    G_STAT,                // git status
    G_LOG,                 // git log
};

static uint16_t idle_timer;             // Idle LED timeout timer
static uint8_t idle_second_counter;     // Idle LED seconds counter, counts seconds not milliseconds
static uint8_t key_event_counter;       // This counter is used to check if any keys are being held

bool rgb_effect_on = false;

const int PROGMEM colors[][3] = {
    RED,        // 0
    CORAL,      // 1
    ORANGE,     // 2
    GOLD,       // 3
    YELLOW,     // 4
    CHART,      // 5
    GREEN,      // 6
    SPRING,     // 7
    TURQ,       // 8
    CYAN,       // 9
    AZURE,      // 10
    BLUE,       // 11
    PURPLE,     // 12
    MAGENT,     // 13
    PINK,       // 14
    WHITE,      // 15
};

const int PROGMEM keyids[] = {
    [KC_ESC]    = 0,  
    [KC_F1]     = 1,   
    [KC_F2]     = 2,   
    [KC_F3]     = 3,   
    [KC_F4]     = 4,   
    [KC_F5]     = 5,   
    [KC_F6]     = 6,   
    [KC_F7]     = 7,  
    [KC_F8]     = 8,   
    [KC_F9]     = 9,   
    [KC_F10]    = 10,  
    [KC_F11]    = 11,  
    [KC_F12]    = 12,           
    [KC_PSCR]   = 13, 
    [KC_SLCK]   = 14, 
    [KC_PAUS]   = 15,
    [KC_GRV]    = 16,  
    [KC_1]      = 17,    
    [KC_2]      = 18,    
    [KC_3]      = 19,    
    [KC_4]      = 20,    
    [KC_5]      = 21,    
    [KC_6]      = 22,    
    [KC_7]      = 23,    
    [KC_8]      = 24,    
    [KC_9]      = 25,    
    [KC_0]      = 26,    
    [KC_MINS]   = 27, 
    [KC_EQL]    = 28,  
    [KC_BSPC]   = 29, 
    [KC_INS]    = 30,  
    [KC_HOME]   = 31, 
    [KC_PGUP]   = 32,
    [KC_TAB]    = 33,  
    [KC_Q]      = 34,    
    [KC_W]      = 35,    
    [KC_E]      = 36,    
    [KC_R]      = 37,    
    [KC_T]      = 38,    
    [KC_Y]      = 39,    
    [KC_U]      = 40,    
    [KC_I]      = 41,    
    [KC_O]      = 42,    
    [KC_P]      = 43,    
    [KC_LBRC]   = 44, 
    [KC_RBRC]   = 45,
    [KC_BSLS]   = 46, 
    [KC_DEL]    = 47,  
    [KC_END]    = 48,  
    [KC_PGDN]   = 49,
    [KC_CAPS]   = 50, 
    [KC_A]      = 51,    
    [KC_S]      = 52,   
    [KC_D]      = 53,    
    [KC_F]      = 54,    
    [KC_G]      = 55,    
    [KC_H]      = 56,    
    [KC_J]      = 57,    
    [KC_K]      = 58,    
    [KC_L]      = 59,    
    [KC_SCLN]   = 60, 
    [KC_QUOT]   = 61, 
    [KC_ENT]    = 62,
    [KC_LSFT]   = 63, 
    [KC_Z]      = 64,    
    [KC_X]      = 65,    
    [KC_C]      = 66,    
    [KC_V]      = 67,    
    [KC_B]      = 68,    
    [KC_N]      = 69,    
    [KC_M]      = 70,    
    [KC_COMM]   = 71, 
    [KC_DOT]    = 72,  
    [KC_SLSH]   = 73, 
    [KC_RSFT]   = 74,                            
    [KC_UP]     = 75,
    [KC_LCTL]   = 76, 
    [KC_LGUI]   = 77, 
    [KC_LALT]   = 78,                   
    [KC_SPC]    = 79,                             
    [KC_RALT]   = 80, 
    [TT(1)]     = 81,   
    [KC_APP]    = 82,  
    [KC_RCTL]   = 83,          
    [KC_LEFT]   = 84, 
    [KC_DOWN]   = 85, 
    [KC_RGHT]   = 86,
};

// highlight duration
int highlights[] = {
    [0]     = 0,  
    [1]     = 0,   
    [2]     = 0,   
    [3]     = 0,   
    [4]     = 0,   
    [5]     = 0,   
    [6]     = 0,   
    [7]     = 0,  
    [8]     = 0,   
    [9]     = 0,   
    [10]    = 0,  
    [11]    = 0,  
    [12]    = 0,           
    [13]    = 0, 
    [14]    = 0, 
    [15]    = 0,
    [16]    = 0,  
    [17]    = 0,    
    [18]    = 0,    
    [19]    = 0,    
    [20]    = 0,    
    [21]    = 0,    
    [22]    = 0,    
    [23]    = 0,    
    [24]    = 0,    
    [25]    = 0,    
    [26]    = 0,    
    [27]    = 0, 
    [28]    = 0,  
    [29]    = 0, 
    [30]    = 0,  
    [31]    = 0, 
    [32]    = 0,
    [33]    = 0,  
    [34]    = 0,    
    [35]    = 0,    
    [36]    = 0,    
    [37]    = 0,    
    [38]    = 0,    
    [39]    = 0,    
    [40]    = 0,    
    [41]    = 0,    
    [42]    = 0,    
    [43]    = 0,    
    [44]    = 0, 
    [45]    = 0,
    [46]    = 0, 
    [47]    = 0,  
    [48]    = 0,  
    [49]    = 0,
    [50]    = 0, 
    [51]    = 0,    
    [52]    = 0,   
    [53]    = 0,    
    [54]    = 0,    
    [55]    = 0,    
    [56]    = 0,    
    [57]    = 0,    
    [58]    = 0,    
    [59]    = 0,    
    [60]    = 0, 
    [61]    = 0, 
    [62]    = 0,
    [63]    = 0, 
    [64]    = 0,    
    [65]    = 0,    
    [66]    = 0,    
    [67]    = 0,    
    [68]    = 0,    
    [69]    = 0,    
    [70]    = 0,    
    [71]    = 0, 
    [72]    = 0,  
    [73]    = 0, 
    [74]    = 0,                            
    [75]    = 0,
    [76]    = 0, 
    [77]    = 0, 
    [78]    = 0,                   
    [79]    = 0,                             
    [80]    = 0, 
    [81]    = 0,   
    [82]    = 0,  
    [83]    = 0,          
    [84]    = 0, 
    [85]    = 0, 
    [86]    = 0,
};

const int num_colors = sizeof(colors)/sizeof(colors[0]);

uint8_t flag_keys_main = 13;    // MAGENT
uint8_t flag_keys_wasd = 0;     // RED
uint8_t flag_keys_mods = 9;     // CYAN
uint8_t flag_keys_rght = 7;     // SPRING

bool flag_rgb_effect_main = false;
bool flag_rgb_effect_wasd = false;
bool flag_rgb_effect_mods = false;
bool flag_rgb_effect_rght = false;

int rgb_effects_delay_min = 4;
int rgb_effects_delay_max = 60;
int rgb_effects_delay = 40;
int rgb_effects_delay_current_count = 0;
int rgb_effects_counter = 0;

bool flag_rgb_invert_press = true;
bool flag_rgb_on = true;
const int highlight_duration = 100;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_KL] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,           KC_PSCR, KC_SLCK, KC_PAUS,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,  KC_END,  KC_PGDN,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,                            KC_UP,
        KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                             KC_RALT, TT(1),   KC_APP,  KC_RCTL,          KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_FL] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            KC_MUTE, _______, _______,
        _______, MAIN_P,  MAIN_N,  WASD_P,  WASD_N,  MODS_P,  MODS_N,  RGHT_P,  RGHT_N,  MAIN_T,  WASD_T,  MODS_T,  RGHT_T,  _______,   KC_MPLY, KC_MSTP, KC_VOLU,
        _______, RGB_SPD, RGB_VAI, RGB_SPI, RGB_HUI, RGB_TIM, _______, U_T_AUTO,U_T_AGCR,_______, _______, E_SPD_D, E_SPD_U, _______,   KC_MPRV, KC_MNXT, KC_VOLD,
        _______, RGB_RMOD,RGB_VAD, RGB_MOD, RGB_HUD, _______, _______, _______, _______, _______, _______, RGB_INV, _______,
        _______, _______, RGB_SWT, _______, _______, MD_BOOT, NK_TOGG, _______, _______, _______, _______, _______,                              _______,
        _______, _______, _______,                   _______,                            _______, _______, _______, _______,            _______, _______, _______
    ),
    /*
    [X] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, TG_NKRO, _______, _______, _______, _______, _______,                            _______,
        _______, _______, _______,                   _______,                            _______, _______, _______, _______,          _______, _______, _______
    ),
    [RGB] = LAYOUT(
        ESC: 0,   F1: 1,    F2: 2,    F3: 3,    F4: 4,    F5: 5,    F6: 6,    F7: 7,    F8: 8,    F9: 9,    F10: 10,  F11: 11,  F12: 12,            PSCR: 13, SLCK: 14, PAUS: 15,
        GRV: 16,  1: 17,    2: 18,    3: 19,    4: 20,    5: 21,    6: 22,    7: 23,    8: 24,    9: 25,    0: 26,    MINS: 27, EQL: 28,  BSPC: 29, INS: 30,  HOME: 31, PGUP: 32,
        TAB: 33,  Q: 34,    W: 35,    E: 36,    R: 37,    T: 38,    Y: 39,    U: 40,    I: 41,    O: 42,    P: 43,    LBRC: 44, RBRC: 45, BSLS: 46, DEL: 47,  END: 48,  PGDN: 49,
        CAPS: 50, A: 51,    S: 52,    D: 53,    F: 54,    G: 55,    H: 56,    J: 57,    K: 58,    L: 59,    SCLN: 60, QUOT: 61, ENT: 62,
        LSFT: 63, Z: 64,    X: 65,    C: 66,    V: 67,    B: 68,    N: 69,    M: 70,    COMM: 71, DOT: 72,  SLSH: 73, RSFT: 74,                               UP: 75,
        LCTL: 76, LGUI: 77, LALT: 78,                   SPC: 79,                                  RALT: 80, Fn: 81,   APP: 82,  RCTL: 83,           LEFT: 84, DOWN: 85, RGHT: 86
    ),
	[MATRIX] = LAYOUT(
	    0,       1,       2,       3,       4,       5,       6,       7,       8,       9,       10,      11,      12,                 13,      14,      15,
	    16,      17,      18,      19,      20,      21,      22,      23,      24,      25,      26,      27,      28,      29,        30,      31,      32,
	    33,      34,      35,      36,      37,      38,      39,      40,      41,      42,      43,      44,      45,      46,        47,      48,      49,
	    50,      51,      52,      53,      54,      55,      56,      57,      58,      59,      60,      61,      62,
	    63,      64,      65,      66,      67,      68,      69,      70,      71,      72,      73,      74,                                   75,
	    76,      77,      78,                        79,                                 80,      81,      82,      83,                 84,      85,      86
	),
    */
};

#ifdef _______
#undef _______
#define _______ {0, 0, 0}

const uint8_t PROGMEM ledmap[][DRIVER_LED_TOTAL][3] = {
    [_FL] = {
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          PURPLE,  _______, _______,
        _______, _______, RED,     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, PURPLE,  _______, _______,
        _______, _______, PURPLE,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, PURPLE,  PURPLE,  PURPLE,
        _______, _______, PURPLE,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, RED,     _______, _______, GREEN,   _______, _______, _______, _______, _______, _______,                            _______,
        _______, _______, _______,                   _______,                            _______, WHITE,   _______, _______,          _______, _______, _______
    },
};

#undef _______
#define _______ KC_TRNS
#endif

// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {
    // Enable or disable debugging
    debug_enable=true;
    debug_matrix=true;
    debug_keyboard=true;
    debug_mouse=true;

    idle_second_counter = 0;                            // Counter for number of seconds keyboard has been idle.
    key_event_counter = 0;                              // Counter to determine if keys are being held, neutral at 0.
    rgb_time_out_seconds = RGB_DEFAULT_TIME_OUT;        // RGB timeout initialized to its default configure in keymap.h
    rgb_time_out_enable = false;                        // Disable RGB timeout by default. Enable using toggle key.
    rgb_time_out_user_value = false;                    // Has to have the same initial value as rgb_time_out_enable.
    rgb_enabled_flag = true;                            // Initially, keyboard RGB is enabled. Change to false config.h initializes RGB disabled.
    rgb_time_out_fast_mode_enabled = false;             // RGB timeout fast mode disabled initially.
    rgb_time_out_saved_flag = rgb_matrix_get_flags();   // Save RGB matrix state for when keyboard comes back from ide.
};

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
}

// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {
    if(rgb_time_out_enable && rgb_enabled_flag) {
        // If the key event counter is not zero then some key was pressed down but not released, thus reset the timeout counter.
        if (key_event_counter) {
            idle_second_counter = 0;
        } else if (timer_elapsed(idle_timer) > MILLISECONDS_IN_SECOND) {
            idle_second_counter++;
            idle_timer = timer_read();
        }

        if (idle_second_counter >= rgb_time_out_seconds) {
            rgb_time_out_saved_flag = rgb_matrix_get_flags();
            rgb_matrix_set_flags(LED_FLAG_NONE);
            rgb_matrix_disable_noeeprom();
            rgb_enabled_flag = false;
            idle_second_counter = 0;
        }
    }

    if(flag_rgb_on) {
        rgb_matrix_enable_noeeprom();
        rgb_matrix_set_flags(rgb_time_out_saved_flag);
    } else {
        rgb_matrix_set_flags(LED_FLAG_NONE);
        rgb_matrix_disable_noeeprom();
    }
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint32_t key_timer;

    // Increment key event counter for every press and decrement for every release.
    if (record->event.pressed) {
        key_event_counter++;
        highlights[keyids[keycode]] = 60 * 1000;
    } else {
        key_event_counter--;
        highlights[keyids[keycode]] = highlight_duration;
    }

    if (rgb_time_out_enable) {
        idle_timer = timer_read();
        // Reset the seconds counter. Without this, something like press> leave x seconds> press, would be x seconds on the effective counter not 0 as it should.
        idle_second_counter = 0;
        if (!rgb_enabled_flag) {
            rgb_matrix_enable_noeeprom();
            rgb_matrix_set_flags(rgb_time_out_saved_flag);
            rgb_enabled_flag = true;
        }
    }

    switch (keycode) {
        case U_T_AUTO:
            if (record->event.pressed && MODS_SHIFT && MODS_CTRL) {
                TOGGLE_FLAG_AND_PRINT(usb_extra_manual, "USB extra port manual mode");
            }
            return false;
        case U_T_AGCR:
            if (record->event.pressed && MODS_SHIFT && MODS_CTRL) {
                TOGGLE_FLAG_AND_PRINT(usb_gcr_auto, "USB GCR auto mode");
            }
            return false;
        case MD_BOOT:
            if (record->event.pressed) {
                key_timer = timer_read32();
            } else {
                if (timer_elapsed32(key_timer) >= 500) {
                    reset_keyboard();
                }
            }
            return false;
    }

    if (record->event.pressed) {

        switch (keycode) {
            case DBG_TOG:
                TOGGLE_FLAG_AND_PRINT(debug_enable, "Debug mode");
                return false;
            case DBG_MTRX:
                TOGGLE_FLAG_AND_PRINT(debug_matrix, "Debug matrix");
                return false;
            case DBG_KBD:
                TOGGLE_FLAG_AND_PRINT(debug_keyboard, "Debug keyboard");
                return false;
            case DBG_MOU:
                TOGGLE_FLAG_AND_PRINT(debug_mouse, "Debug mouse");
                return false;
            case RGB_TOG:
                rgb_time_out_enable = rgb_time_out_user_value;
                switch (rgb_matrix_get_flags()) {
                    case LED_FLAG_ALL: {
                        rgb_matrix_set_flags(LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER);
                        rgb_matrix_set_color_all(0, 0, 0);
                    }
                    break;
                    case LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER: {
                        rgb_matrix_set_flags(LED_FLAG_UNDERGLOW);
                        rgb_matrix_set_color_all(0, 0, 0);
                    }
                    break;
                    case LED_FLAG_UNDERGLOW: {
                        // This line is for LED idle timer. It disables the toggle so you can turn off LED completely if you like
                        rgb_time_out_enable = false;
                        rgb_matrix_set_flags(LED_FLAG_NONE);
                        rgb_matrix_disable_noeeprom();
                    }
                    break;
                    default: {
                        rgb_matrix_set_flags(LED_FLAG_ALL);
                        rgb_matrix_enable_noeeprom();
                    }
                    break;
                }
                return false;
            // custom key codes
            case MAIN_P:
                if(flag_keys_main > 0)
                    flag_keys_main--;
                else
                    flag_keys_main = num_colors - 1;
                return false;
            case MAIN_N:
                if(flag_keys_main < num_colors - 1)
                    flag_keys_main++;
                else
                    flag_keys_main = 0;
                return false;
            case WASD_P:
                if(flag_keys_wasd > 0)
                    flag_keys_wasd--;
                else
                    flag_keys_wasd = num_colors - 1;
                return false;
            case WASD_N:
                if(flag_keys_wasd < num_colors - 1)
                    flag_keys_wasd++;
                else
                    flag_keys_wasd = 0;
                return false;
            case MODS_P:
                if(flag_keys_mods > 0)
                    flag_keys_mods--;
                else
                    flag_keys_mods = num_colors - 1;
                return false;
            case MODS_N:
                if(flag_keys_mods < num_colors - 1)
                    flag_keys_mods++;
                else
                    flag_keys_mods = 0;
                return false;
            case RGHT_P:
                if(flag_keys_rght > 0)
                    flag_keys_rght--;
                else
                    flag_keys_rght = num_colors - 1;
                return false;
            case RGHT_N:
                if(flag_keys_rght < num_colors - 1)
                    flag_keys_rght++;
                else
                    flag_keys_rght = 0;
                return false;
            case MAIN_T:
                flag_rgb_effect_main = !flag_rgb_effect_main;
                return false;
            case WASD_T:
                flag_rgb_effect_wasd = !flag_rgb_effect_wasd;
                return false;
            case MODS_T:
                flag_rgb_effect_mods = !flag_rgb_effect_mods;
                return false;
            case RGHT_T:
                flag_rgb_effect_rght = !flag_rgb_effect_rght;
                return false;
            case E_SPD_D:
                if(rgb_effects_delay > rgb_effects_delay_min)
                    rgb_effects_delay--;
                return false;
            case E_SPD_U:
                if(rgb_effects_delay < rgb_effects_delay_max)
                    rgb_effects_delay++;
                return false;
            case RGB_INV:
                flag_rgb_invert_press = !flag_rgb_invert_press;
                return false;
            case RGB_TIM:
                rgb_time_out_enable = !rgb_time_out_enable;
                return false;
            case RGB_SWT:
                flag_rgb_on = !flag_rgb_on;
                return false;
        }
    }
    return true;
}

void set_layer_color(int layer) {

    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {

        HSV hsv = {
            .h = 0,
            .s = 0,
            .v = 0
        };

        // trackers for function keys
        if(layer == 1 && (i == 17 || i == 18 || i == 25)) {
            hsv.h = colors[flag_keys_main][0] + (i == 25 && flag_rgb_effect_main ? rgb_effects_counter : 0);
            hsv.s = colors[flag_keys_main][1];
            hsv.v = colors[flag_keys_main][2];
        } else if(layer == 1 && (i == 19 || i == 20 || i == 26)) {
            hsv.h = colors[flag_keys_wasd][0] + (i == 26 && flag_rgb_effect_wasd ? rgb_effects_counter : 0);
            hsv.s = colors[flag_keys_wasd][1];
            hsv.v = colors[flag_keys_wasd][2];
        } else if(layer == 1 && (i == 21 || i == 22 || i == 27)) {
            hsv.h = colors[flag_keys_mods][0] + (i == 27 && flag_rgb_effect_mods ? rgb_effects_counter : 0);
            hsv.s = colors[flag_keys_mods][1];
            hsv.v = colors[flag_keys_mods][2];
        } else if(layer == 1 && (i == 23 || i == 24 || i == 28)) {
            hsv.h = colors[flag_keys_rght][0] + (i == 28 && flag_rgb_effect_rght ? rgb_effects_counter : 0);
            hsv.s = colors[flag_keys_rght][1];
            hsv.v = colors[flag_keys_rght][2];
        } else if(layer == 1 && i == 38) {
            int color = rgb_time_out_enable ? 11 : 15;
            hsv.h = colors[color][0];
            hsv.s = colors[color][1];
            hsv.v = colors[color][2];
        } else if(layer == 1 && i == 61){
            int color = flag_rgb_invert_press ? 6 : 15;
            hsv.h = colors[color][0];
            hsv.s = colors[color][1];
            hsv.v = colors[color][2];
        } else if(layer == 1 && (i == 44 || i == 45)){
            hsv.h = colors[0][0] + rgb_effects_counter;
            hsv.s = colors[0][1];
            hsv.v = colors[0][2];
        } else {
            hsv.h = pgm_read_byte(&ledmap[layer][i][0]);
            hsv.s = pgm_read_byte(&ledmap[layer][i][1]);
            hsv.v = pgm_read_byte(&ledmap[layer][i][2]);
        }

        // set color
        if (hsv.h || hsv.s || hsv.v) {
            RGB rgb = hsv_to_rgb(hsv);
            float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
            rgb_matrix_set_color(i, f * rgb.r, f * rgb.g, f * rgb.b);
        } else {
            // Only deactivate non-defined key LEDs at layers other than FN. Because at FN we have RGB adjustments and need to see them live.
            // If the values are all false then it's a transparent key and deactivate LED at this layer
            rgb_matrix_set_color(i, 0, 0, 0);
        }
    }
}

void set_layer_color_custom(int layer) {

    // highlight timeout
    for(int i = 0; i < 87; i++)
        if(highlights[i] > 0)
            highlights[i]--;

    // rgb cycle
    // if(rgb_effects_delay_current_count < rgb_effects_delay) {
    //     rgb_effects_delay_current_count++;
    // } else {
    //     rgb_effects_delay_current_count = 0;

    //     if(rgb_effects_counter == 255)
    //         rgb_effects_counter = 0;
    //     else
    //         rgb_effects_counter++;
    // }

    switch(layer) {
        case _KL:
            set_base_layer_colors();
            break;
        case _FL:
            set_layer_color(_FL);
            break;
        default:
            break;  
    }
}

void set_base_layer_colors() {
    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        int _color = 0;
        int _offset = 0;
        switch(i) {
            // WASD
            case 35:
            case 51:
            case 52:
            case 53:
                _color = flag_keys_wasd;
                // rgb effect
                if(flag_rgb_effect_wasd)
                    _offset = rgb_effects_counter;
                break;
            // modifiers, escape, and f5-f8
            case 0:
            case 5:
            case 6:
            case 7:
            case 8:
            case 29:
            case 33:
            case 50:
            case 62:
            case 63:
            case 74:
            case 76:
            case 77:
            case 78:
            case 80:
            case 81:
            case 82:
            case 83:
                _color = flag_keys_mods;
                // rgb effect
                if(flag_rgb_effect_mods)
                    _offset = rgb_effects_counter;
                break;
            // right
            case 13:
            case 14:
            case 15:
            case 30:
            case 31:
            case 32:
            case 47:
            case 48:
            case 49:
            case 75:
            case 84:
            case 85:
            case 86:
                _color = flag_keys_rght;
                // rgb effect
                if(flag_rgb_effect_rght)
                    _offset = rgb_effects_counter;
                break;
            // rest of the LEDs
            default:
                _color = flag_keys_main;
                // rgb effect
                if(flag_rgb_effect_main)
                    _offset = rgb_effects_counter;
                break;
        }

        HSV hsv = {
            .h = colors[_color][0] + _offset,
            .s = colors[_color][1],
            .v = colors[_color][2],
        };

        if( (i == 50 && (host_keyboard_leds() & 1<<USB_LED_CAPS_LOCK)) || (flag_rgb_invert_press && i < 87 && highlights[i] > 0))
            hsv.h = (hsv.h + 128) % 255;

        if (hsv.h || hsv.s || hsv.v) {
            RGB rgb = hsv_to_rgb(hsv);
            float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
            rgb_matrix_set_color(i, f * rgb.r, f * rgb.g, f * rgb.b);
        } else /*if (layer != 1)*/ {
            // Only deactivate non-defined key LEDs at layers other than FN. Because at FN we have RGB adjustments and need to see them live.
            // If the values are all false then it's a transparent key and deactivate LED at this layer
            rgb_matrix_set_color(i, 0, 0, 0);
        }
    }
}

void rgb_matrix_indicators_user(void) {
    if (g_suspend_state || disable_layer_color ||
        rgb_matrix_get_flags() == LED_FLAG_NONE ||
        rgb_matrix_get_flags() == LED_FLAG_UNDERGLOW) {
            return;
        }
    set_layer_color_custom(get_highest_layer(layer_state));
}

