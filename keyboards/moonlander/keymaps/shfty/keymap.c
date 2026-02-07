// TODO
//
// Move modules into userspace
//
// Vim command layer
//      Ability to save from normal mode (:w<CR>)
//      Passthrough for F-keys
//      Rename enter keycode / extern variable to 'normal'
//      Set key color or vim keycode
//      Quick-style tap / hold setup
//          Tap to toggle, hold to shift
//      State change callbacks
//          Setup sound effects
//      Use text objects to represent common key move sequences, share between modes
//      Testing and polishing pass
//
// RGB override support
//      Ex:
//          Having chord modifiers light up when in specific states
//          Vim mode
//      How to implement? Framebuffer applied on top of keycode colors?
//
// Create custom sounds for modifier and layer shifting
//      Override startup / shutdown sounds
//      Implement vim mode shift sounds
//      Mouse / gaming deactivation sounds
//
// Support for locking if a chord key-down occurs in Pressed state
//      Would allow using one-shot with nav layer for quick cursor moves
//
// Polished sc-im workflow
//      Proper tab formatting for generated keymap
//          Use 7-7-6-6-3 layout with tabbed split between halves
//      ledmap generation via similar principle
//
// Improve palette usage
//      Not happy with F-keys disappearing in symbol / number layers
//      Also not happy with using the same color in the navigation layer
//      Orange exclusively representing numbers seems wasteful?

#include QMK_KEYBOARD_H

#include "keycodes.h"
#include "layers.h"

#include "rgb/rgb.h"
#include "quick/quick.h"
#include "chord/chord.h"
#include "vim/vim.h"

#ifdef AUDIO_ENABLE
#define PLAY_SONG_CHECKED(song) PLAY_SONG(song)
#else
#define PLAY_SONG_CHECKED(song) do {} while(false)
#endif

#ifdef AUDIO_ENABLE
float song_shift[][2][2] = {
    [C_Pressed] = SONG(E__NOTE(_FS5), E__NOTE(_REST)),
    [C_Latched] = SONG(E__NOTE(_A5), E__NOTE(_REST)),
    [C_Unlatched] = SONG(E__NOTE(_CS6), E__NOTE(_REST)),
    [C_Inactive] = SONG(E__NOTE(_FS6), E__NOTE(_REST)),
};

float song_ctrl[][2][2] = {
    [C_Pressed] = SONG(E__NOTE(_CS5), E__NOTE(_REST)),
    [C_Latched] = SONG(E__NOTE(_E5), E__NOTE(_REST)),
    [C_Unlatched] = SONG(E__NOTE(_GS5), E__NOTE(_REST)),
    [C_Inactive] = SONG(E__NOTE(_CS6), E__NOTE(_REST)),
};

float song_alt[][2][2] = {
    [C_Pressed] = SONG(E__NOTE(_GS5), E__NOTE(_REST)),
    [C_Latched] = SONG(E__NOTE(_B5), E__NOTE(_REST)),
    [C_Unlatched] = SONG(E__NOTE(_DS6), E__NOTE(_REST)),
    [C_Inactive] = SONG(E__NOTE(_GS6), E__NOTE(_REST)),
};

float song_symbols[][2][2] = {
    [C_Pressed] = SONG(E__NOTE(_E5), E__NOTE(_REST)),
    [C_Latched] = SONG(E__NOTE(_G5), E__NOTE(_REST)),
    [C_Unlatched] = SONG(E__NOTE(_B5), E__NOTE(_REST)),
    [C_Inactive] = SONG(E__NOTE(_E6), E__NOTE(_REST)),
};

float song_numbers[][2][2] = {
    [C_Pressed] = SONG(E__NOTE(_B5), E__NOTE(_REST)),
    [C_Latched] = SONG(E__NOTE(_D6), E__NOTE(_REST)),
    [C_Unlatched] = SONG(E__NOTE(_FS6), E__NOTE(_REST)),
    [C_Inactive] = SONG(E__NOTE(_B6), E__NOTE(_REST)),
};

float song_utility[][2][2] = {
    [C_Pressed] = SONG(E__NOTE(_A5), E__NOTE(_REST)),
    [C_Latched] = SONG(E__NOTE(_C6), E__NOTE(_REST)),
    [C_Unlatched] = SONG(E__NOTE(_E6), E__NOTE(_REST)),
    [C_Inactive] = SONG(E__NOTE(_A6), E__NOTE(_REST)),
};

float song_mouse[][2] =  SONG( E__NOTE(_DS5), E__NOTE(_FS5), E__NOTE(_AS5), E__NOTE(_DS6), E__NOTE(_REST));
float song_gaming[][2] = SONG( E__NOTE(_AS5), E__NOTE(_CS6), E__NOTE(_F6), E__NOTE(_AS6), E__NOTE(_REST));
#endif

void layer_move_checked(uint16_t layer) {
    if(get_highest_layer(layer_state) != layer) {
        layer_move(layer);
    }
}

QUICK_MODIFIER(quick_shift, KC_LSHIFT, song_shift);
QUICK_MODIFIER(quick_control, KC_LCTRL, song_ctrl);
QUICK_MODIFIER(quick_alt, KC_LALT, song_alt);
QUICK_LAYER(quick_utility, L_Utility, song_utility);

Quick quick[] = {
    (Quick) { QSM_RSHIFT, &quick_shift },
    (Quick) { QSM_LSHIFT, &quick_shift },
    (Quick) { QSM_RCTRL, &quick_control },
    (Quick) { QSM_LCTRL, &quick_control },
    (Quick) { QSM_RALT, &quick_alt },
    (Quick) { QSM_LALT, &quick_alt },
    (Quick) { QSL_UTILITY, &quick_utility, Q_Inactive, true },
};

CHORD_LAYER(chord_symbols, L_Symbols, song_symbols)
CHORD_LAYER(chord_numbers, L_Numbers, song_numbers)
CHORD_LAYER(chord_navigation, L_Navigation, song_utility)

Chord chord[] = {
    (Chord) { CSL_SYMBOLS, &chord_symbols },
    (Chord) { CSL_NUMBERS, &chord_numbers },
    (Chord) { CSL_NAVIGATION, &chord_navigation },
};

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if(!quick_process_record(quick, sizeof(quick) / sizeof(Quick), keycode, record)) {
        return false;
    }
    
    if(!chord_process_record(chord, sizeof(chord) / sizeof(Chord), keycode, record)) {
        return false;
    }
    
    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    switch(get_highest_layer(state)) {
        case L_Mouse:
            PLAY_SONG_CHECKED(song_mouse);
            break;
        case L_Gaming:
            PLAY_SONG_CHECKED(song_gaming);
            break;
    }

    return state;
}

