/*************************
 * @file input_enums.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief enums wrapped around the SDL input enums to handle inputs
 * 
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <SDL_scancode.h>
#include <SDL_keycode.h>

namespace lyra {

namespace input {

// if you want documentation for this file, please refer to the official SDL2 docs, since everything here is just modified SDL source code

// keyboard input
enum class Keyboard {
    KEYBOARD_RETURN = '\r',
    KEYBOARD_ESCAPE = '\x1B',
    KEYBOARD_BACKSPACE = '\b',
    KEYBOARD_TAB = '\t',
    KEYBOARD_SPACE = ' ',
    KEYBOARD_EXCLAIM = '!',
    KEYBOARD_QUOTEDBL = '"',
    KEYBOARD_HASH = '#',
    KEYBOARD_PERCENT = '%',
    KEYBOARD_DOLLAR = '$',
    KEYBOARD_AMPERSAND = '&',
    KEYBOARD_QUOTE = '\'',
    KEYBOARD_LEFTPAREN = '(',
    KEYBOARD_RIGHTPAREN = ')',
    KEYBOARD_ASTERISK = '*',
    KEYBOARD_PLUS = '+',
    KEYBOARD_COMMA = ',',
    KEYBOARD_MINUS = '-',
    KEYBOARD_PERIOD = '.',
    KEYBOARD_SLASH = '/',
    KEYBOARD_0 = '0',
    KEYBOARD_1 = '1',
    KEYBOARD_2 = '2',
    KEYBOARD_3 = '3',
    KEYBOARD_4 = '4',
    KEYBOARD_5 = '5',
    KEYBOARD_6 = '6',
    KEYBOARD_7 = '7',
    KEYBOARD_8 = '8',
    KEYBOARD_9 = '9',
    KEYBOARD_COLON = ':',
    KEYBOARD_SEMICOLON = ';',
    KEYBOARD_LESS = '<',
    KEYBOARD_EQUALS = '=',
    KEYBOARD_GREATER = '>',
    KEYBOARD_QUESTION = '?',
    KEYBOARD_AT = '@',

    /*
       Skip uppercase letters
     */

    KEYBOARD_LEFTBRACKET = '[',
    KEYBOARD_BACKSLASH = '\\',
    KEYBOARD_RIGHTBRACKET = ']',
    KEYBOARD_CARET = '^',
    KEYBOARD_UNDERSCORE = '_',
    KEYBOARD_BACKQUOTE = '`',
    KEYBOARD_a = 'a',
    KEYBOARD_b = 'b',
    KEYBOARD_c = 'c',
    KEYBOARD_d = 'd',
    KEYBOARD_e = 'e',
    KEYBOARD_f = 'f',
    KEYBOARD_g = 'g',
    KEYBOARD_h = 'h',
    KEYBOARD_i = 'i',
    KEYBOARD_j = 'j',
    KEYBOARD_k = 'k',
    KEYBOARD_l = 'l',
    KEYBOARD_m = 'm',
    KEYBOARD_n = 'n',
    KEYBOARD_o = 'o',
    KEYBOARD_p = 'p',
    KEYBOARD_q = 'q',
    KEYBOARD_r = 'r',
    KEYBOARD_s = 's',
    KEYBOARD_t = 't',
    KEYBOARD_u = 'u',
    KEYBOARD_v = 'v',
    KEYBOARD_w = 'w',
    KEYBOARD_x = 'x',
    KEYBOARD_y = 'y',
    KEYBOARD_z = 'z',

    KEYBOARD_CAPSLOCK = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CAPSLOCK),

    KEYBOARD_F1 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F1),
    KEYBOARD_F2 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F2),
    KEYBOARD_F3 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F3),
    KEYBOARD_F4 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F4),
    KEYBOARD_F5 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F5),
    KEYBOARD_F6 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F6),
    KEYBOARD_F7 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F7),
    KEYBOARD_F8 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F8),
    KEYBOARD_F9 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F9),
    KEYBOARD_F10 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F10),
    KEYBOARD_F11 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F11),
    KEYBOARD_F12 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F12),

    KEYBOARD_PRINTSCREEN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PRINTSCREEN),
    KEYBOARD_SCROLLLOCK = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SCROLLLOCK),
    KEYBOARD_PAUSE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAUSE),
    KEYBOARD_INSERT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_INSERT),
    KEYBOARD_HOME = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_HOME),
    KEYBOARD_PAGEUP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEUP),
    KEYBOARD_DELETE = '\x7F',
    KEYBOARD_END = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_END),
    KEYBOARD_PAGEDOWN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEDOWN),
    KEYBOARD_RIGHT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RIGHT),
    KEYBOARD_LEFT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LEFT),
    KEYBOARD_DOWN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DOWN),
    KEYBOARD_UP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UP),

    KEYBOARD_NUMLOCKCLEAR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_NUMLOCKCLEAR),
    KEYBOARD_KP_DIVIDE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DIVIDE),
    KEYBOARD_KP_MULTIPLY = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MULTIPLY),
    KEYBOARD_KP_MINUS = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MINUS),
    KEYBOARD_KP_PLUS = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUS),
    KEYBOARD_KP_ENTER = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_ENTER),
    KEYBOARD_KP_1 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_1),
    KEYBOARD_KP_2 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_2),
    KEYBOARD_KP_3 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_3),
    KEYBOARD_KP_4 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_4),
    KEYBOARD_KP_5 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_5),
    KEYBOARD_KP_6 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_6),
    KEYBOARD_KP_7 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_7),
    KEYBOARD_KP_8 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_8),
    KEYBOARD_KP_9 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_9),
    KEYBOARD_KP_0 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_0),
    KEYBOARD_KP_PERIOD = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERIOD),

    KEYBOARD_APPLICATION = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APPLICATION),
    KEYBOARD_POWER = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_POWER),
    KEYBOARD_KP_EQUALS = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EQUALS),
    KEYBOARD_F13 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F13),
    KEYBOARD_F14 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F14),
    KEYBOARD_F15 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F15),
    KEYBOARD_F16 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F16),
    KEYBOARD_F17 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F17),
    KEYBOARD_F18 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F18),
    KEYBOARD_F19 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F19),
    KEYBOARD_F20 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F20),
    KEYBOARD_F21 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F21),
    KEYBOARD_F22 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F22),
    KEYBOARD_F23 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F23),
    KEYBOARD_F24 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F24),
    KEYBOARD_EXECUTE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EXECUTE),
    KEYBOARD_HELP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_HELP),
    KEYBOARD_MENU = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MENU),
    KEYBOARD_SELECT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SELECT),
    KEYBOARD_STOP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_STOP),
    KEYBOARD_AGAIN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AGAIN),
    KEYBOARD_UNDO = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UNDO),
    KEYBOARD_CUT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CUT),
    KEYBOARD_COPY = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_COPY),
    KEYBOARD_PASTE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PASTE),
    KEYBOARD_FIND = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_FIND),
    KEYBOARD_MUTE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MUTE),
    KEYBOARD_VOLUMEUP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_VOLUMEUP),
    KEYBOARD_VOLUMEDOWN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_VOLUMEDOWN),
    KEYBOARD_KP_COMMA = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COMMA),
    KEYBOARD_KP_EQUALSAS400 =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EQUALSAS400),

    KEYBOARD_ALTERASE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_ALTERASE),
    KEYBOARD_SYSREQ = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SYSREQ),
    KEYBOARD_CANCEL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CANCEL),
    KEYBOARD_CLEAR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CLEAR),
    KEYBOARD_PRIOR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PRIOR),
    KEYBOARD_RETURN2 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RETURN2),
    KEYBOARD_SEPARATOR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SEPARATOR),
    KEYBOARD_OUT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_OUT),
    KEYBOARD_OPER = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_OPER),
    KEYBOARD_CLEARAGAIN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CLEARAGAIN),
    KEYBOARD_CRSEL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CRSEL),
    KEYBOARD_EXSEL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EXSEL),

    KEYBOARD_KP_00 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_00),
    KEYBOARD_KP_000 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_000),
    KEYBOARD_THOUSANDSSEPARATOR =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_THOUSANDSSEPARATOR),
    KEYBOARD_DECIMALSEPARATOR =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DECIMALSEPARATOR),
    KEYBOARD_CURRENCYUNIT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYUNIT),
    KEYBOARD_CURRENCYSUBUNIT =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYSUBUNIT),
    KEYBOARD_KP_LEFTPAREN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTPAREN),
    KEYBOARD_KP_RIGHTPAREN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTPAREN),
    KEYBOARD_KP_LEFTBRACE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTBRACE),
    KEYBOARD_KP_RIGHTBRACE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTBRACE),
    KEYBOARD_KP_TAB = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_TAB),
    KEYBOARD_KP_BACKSPACE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BACKSPACE),
    KEYBOARD_KP_A = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_A),
    KEYBOARD_KP_B = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_B),
    KEYBOARD_KP_C = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_C),
    KEYBOARD_KP_D = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_D),
    KEYBOARD_KP_E = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_E),
    KEYBOARD_KP_F = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_F),
    KEYBOARD_KP_XOR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_XOR),
    KEYBOARD_KP_POWER = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_POWER),
    KEYBOARD_KP_PERCENT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERCENT),
    KEYBOARD_KP_LESS = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LESS),
    KEYBOARD_KP_GREATER = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_GREATER),
    KEYBOARD_KP_AMPERSAND = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AMPERSAND),
    KEYBOARD_KP_DBLAMPERSAND =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLAMPERSAND),
    KEYBOARD_KP_VERTICALBAR =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_VERTICALBAR),
    KEYBOARD_KP_DBLVERTICALBAR =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLVERTICALBAR),
    KEYBOARD_KP_COLON = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COLON),
    KEYBOARD_KP_HASH = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HASH),
    KEYBOARD_KP_SPACE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_SPACE),
    KEYBOARD_KP_AT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AT),
    KEYBOARD_KP_EXCLAM = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EXCLAM),
    KEYBOARD_KP_MEMSTORE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSTORE),
    KEYBOARD_KP_MEMRECALL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMRECALL),
    KEYBOARD_KP_MEMCLEAR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMCLEAR),
    KEYBOARD_KP_MEMADD = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMADD),
    KEYBOARD_KP_MEMSUBTRACT =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSUBTRACT),
    KEYBOARD_KP_MEMMULTIPLY =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMMULTIPLY),
    KEYBOARD_KP_MEMDIVIDE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMDIVIDE),
    KEYBOARD_KP_PLUSMINUS = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUSMINUS),
    KEYBOARD_KP_CLEAR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEAR),
    KEYBOARD_KP_CLEARENTRY = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEARENTRY),
    KEYBOARD_KP_BINARY = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BINARY),
    KEYBOARD_KP_OCTAL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_OCTAL),
    KEYBOARD_KP_DECIMAL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DECIMAL),
    KEYBOARD_KP_HEXADECIMAL =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HEXADECIMAL),

    KEYBOARD_LCTRL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LCTRL),
    KEYBOARD_LSHIFT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LSHIFT),
    KEYBOARD_LALT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LALT),
    KEYBOARD_LGUI = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LGUI),
    KEYBOARD_RCTRL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RCTRL),
    KEYBOARD_RSHIFT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RSHIFT),
    KEYBOARD_RALT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RALT),
    KEYBOARD_RGUI = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RGUI),

    KEYBOARD_MODE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MODE),

    KEYBOARD_AUDIONEXT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIONEXT),
    KEYBOARD_AUDIOPREV = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOPREV),
    KEYBOARD_AUDIOSTOP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOSTOP),
    KEYBOARD_AUDIOPLAY = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOPLAY),
    KEYBOARD_AUDIOMUTE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOMUTE),
    KEYBOARD_MEDIASELECT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIASELECT),
    KEYBOARD_WWW = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_WWW),
    KEYBOARD_MAIL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MAIL),
    KEYBOARD_CALCULATOR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CALCULATOR),
    KEYBOARD_COMPUTER = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_COMPUTER),
    KEYBOARD_AC_SEARCH = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_SEARCH),
    KEYBOARD_AC_HOME = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_HOME),
    KEYBOARD_AC_BACK = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BACK),
    KEYBOARD_AC_FORWARD = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_FORWARD),
    KEYBOARD_AC_STOP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_STOP),
    KEYBOARD_AC_REFRESH = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_REFRESH),
    KEYBOARD_AC_BOOKMARKS = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BOOKMARKS),

    KEYBOARD_BRIGHTNESSDOWN =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_BRIGHTNESSDOWN),
    KEYBOARD_BRIGHTNESSUP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_BRIGHTNESSUP),
    KEYBOARD_DISPLAYSWITCH = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DISPLAYSWITCH),
    KEYBOARD_KBDILLUMTOGGLE =
        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMTOGGLE),
    KEYBOARD_KBDILLUMDOWN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMDOWN),
    KEYBOARD_KBDILLUMUP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KBDILLUMUP),
    KEYBOARD_EJECT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EJECT),
    KEYBOARD_SLEEP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SLEEP),
    KEYBOARD_APP1 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP1),
    KEYBOARD_APP2 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP2),

    KEYBOARD_AUDIOREWIND = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOREWIND),
    KEYBOARD_AUDIOFASTFORWARD = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOFASTFORWARD),

    KEYBOARD_UNKNOWN = 0
};

// mouse input
enum class Mouse {
    MOUSE_LEFT = 1,
    MOUSE_MIDDLE,
    MOUSE_RIGHT,
    MOUSE_X1,
    MOUSE_X2
};

// controller input
enum class Controller {
    SDL_CONTROLLER_BUTTON_INVALID = -1,
    SDL_CONTROLLER_BUTTON_A,
    SDL_CONTROLLER_BUTTON_B,
    SDL_CONTROLLER_BUTTON_X,
    SDL_CONTROLLER_BUTTON_Y,
    SDL_CONTROLLER_BUTTON_BACK,
    SDL_CONTROLLER_BUTTON_GUIDE,
    SDL_CONTROLLER_BUTTON_START,
    SDL_CONTROLLER_BUTTON_LEFTSTICK,
    SDL_CONTROLLER_BUTTON_RIGHTSTICK,
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
    SDL_CONTROLLER_BUTTON_DPAD_UP,
    SDL_CONTROLLER_BUTTON_DPAD_DOWN,
    SDL_CONTROLLER_BUTTON_DPAD_LEFT,
    SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
    SDL_CONTROLLER_BUTTON_MISC1,    /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */
    SDL_CONTROLLER_BUTTON_PADDLE1,  /* Xbox Elite paddle P1 */
    SDL_CONTROLLER_BUTTON_PADDLE2,  /* Xbox Elite paddle P3 */
    SDL_CONTROLLER_BUTTON_PADDLE3,  /* Xbox Elite paddle P2 */
    SDL_CONTROLLER_BUTTON_PADDLE4,  /* Xbox Elite paddle P4 */
    SDL_CONTROLLER_BUTTON_TOUCHPAD, /* PS4/PS5 touchpad button */
    SDL_CONTROLLER_BUTTON_MAX
};

} // namespace input

} // namespace lyra
