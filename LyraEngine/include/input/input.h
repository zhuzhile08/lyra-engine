/*************************
 * @file input.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief A singleton wrapper around SDL input which polls events
 * 
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <functional>

#include <SDL.h>

#include <core/queue.h>

namespace lyra {

class Input {
public:
	// if you want documentation for the enums, please refer to the official SDL2 docs, since everything here is just modified SDL source code

	enum class InputType {
		TYPE_WINDOWEVENT    = 0x200, /**< Window state change */
		TYPE_SYSTEMEVENT,             /**< System specific event */

		/* Keyboard events */
		TYPE_KEYDOWN        = 0x300, /**< Key pressed */
		TYPE_KEYUP,                  /**< Key released */
		TYPE_TEXTEDITING,            /**< Keyboard text editing (composition) */
		TYPE_TEXTINPUT,              /**< Keyboard text input */
		TYPE_TEXTEDITING_EXT,       /**< Extended keyboard text editing (composition) */

		/* Mouse events */
		TYPE_MOUSEMOTION    = 0x400, /**< Mouse moved */
		TYPE_MOUSEBUTTONDOWN,        /**< Mouse button pressed */
		TYPE_MOUSEBUTTONUP,          /**< Mouse button released */
		TYPE_MOUSEWHEEL,             /**< Mouse wheel motion */

		/* Joystick events */
		TYPE_JOYAXISMOTION  = 0x600, /**< Joystick axis motion */
		TYPE_JOYBALLMOTION,          /**< Joystick trackball motion */
		TYPE_JOYHATMOTION,           /**< Joystick hat position change */
		TYPE_JOYBUTTONDOWN,          /**< Joystick button pressed */
		TYPE_JOYBUTTONUP,            /**< Joystick button released */
		TYPE_JOYDEVICEADDED,         /**< A new joystick has been inserted into the system */
		TYPE_JOYDEVICEREMOVED,       /**< An opened joystick has been removed */

		/* Game controller events */
		TYPE_CONTROLLERAXISMOTION  = 0x650, /**< Game controller axis motion */
		TYPE_CONTROLLERBUTTONDOWN,          /**< Game controller button pressed */
		TYPE_CONTROLLERBUTTONUP,            /**< Game controller button released */
		TYPE_CONTROLLERDEVICEADDED,         /**< A new Game controller has been inserted into the system */
		TYPE_CONTROLLERDEVICEREMOVED,       /**< An opened Game controller has been removed */
		TYPE_CONTROLLERDEVICEREMAPPED,      /**< The controller mapping was updated */
		TYPE_CONTROLLERTOUCHPADDOWN,        /**< Game controller touchpad was touched */
		TYPE_CONTROLLERTOUCHPADMOTION,      /**< Game controller touchpad finger was moved */
		TYPE_CONTROLLERTOUCHPADUP,          /**< Game controller touchpad finger was lifted */
		TYPE_CONTROLLERSENSORUPDATE,        /**< Game controller sensor was updated */

		/* Clipboard events */
		TYPE_CLIPBOARDUPDATE = 0x900, /**< The clipboard changed */

		/* Drag and drop events */
		TYPE_DROPFILE        = 0x1000, /**< The system requests a file open */
		TYPE_DROPTEXT,                 /**< text/plain drag-and-drop event */
		TYPE_DROPBEGIN,                /**< A new set of drops is beginning (NULL filename) */
		TYPE_DROPCOMPLETE,             /**< Current set of drops is now complete (NULL filename) */

		TYPE_UNKNOWN = 0xFFFF
	};

	// keyboard input
	enum class Keyboard {
		KEYBOARD_A = 4,
		KEYBOARD_B = 5,
		KEYBOARD_C = 6,
		KEYBOARD_D = 7,
		KEYBOARD_E = 8,
		KEYBOARD_F = 9,
		KEYBOARD_G = 10,
		KEYBOARD_H = 11,
		KEYBOARD_I = 12,
		KEYBOARD_J = 13,
		KEYBOARD_K = 14,
		KEYBOARD_L = 15,
		KEYBOARD_M = 16,
		KEYBOARD_N = 17,
		KEYBOARD_O = 18,
		KEYBOARD_P = 19,
		KEYBOARD_Q = 20,
		KEYBOARD_R = 21,
		KEYBOARD_S = 22,
		KEYBOARD_T = 23,
		KEYBOARD_U = 24,
		KEYBOARD_V = 25,
		KEYBOARD_W = 26,
		KEYBOARD_X = 27,
		KEYBOARD_Y = 28,
		KEYBOARD_Z = 29,

		KEYBOARD_1 = 30,
		KEYBOARD_2 = 31,
		KEYBOARD_3 = 32,
		KEYBOARD_4 = 33,
		KEYBOARD_5 = 34,
		KEYBOARD_6 = 35,
		KEYBOARD_7 = 36,
		KEYBOARD_8 = 37,
		KEYBOARD_9 = 38,
		KEYBOARD_0 = 39,

		KEYBOARD_RETURN = 40,
		KEYBOARD_ESCAPE = 41,
		KEYBOARD_BACKSPACE = 42,
		KEYBOARD_TAB = 43,
		KEYBOARD_SPACE = 44,

		KEYBOARD_MINUS = 45,
		KEYBOARD_EQUALS = 46,
		KEYBOARD_LEFTBRACKET = 47,
		KEYBOARD_RIGHTBRACKET = 48,
		KEYBOARD_BACKSLASH = 49, /**< Located at the lower left of the return
									*   key on ISO keyboards and at the right end
									*   of the QWERTY row on ANSI keyboards.
									*   Produces REVERSE SOLIDUS (backslash) and
									*   VERTICAL LINE in a US layout, REVERSE
									*   SOLIDUS and VERTICAL LINE in a UK Mac
									*   layout, NUMBER SIGN and TILDE in a UK
									*   Windows layout, DOLLAR SIGN and POUND SIGN
									*   in a Swiss German layout, NUMBER SIGN and
									*   APOSTROPHE in a German layout, GRAVE
									*   ACCENT and POUND SIGN in a French Mac
									*   layout, and ASTERISK and MICRO SIGN in a
									*   French Windows layout.
									*/
		KEYBOARD_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
									*   instead of 49 for the same key, but all
									*   OSes I've seen treat the two codes
									*   identically. So, as an implementor, unless
									*   your keyboard generates both of those
									*   codes and your OS treats them differently,
									*   you should generate KEYBOARD_BACKSLASH
									*   instead of this code. As a user, you
									*   should not rely on this code because SDL
									*   will never generate it with most (all?)
									*   keyboards.
									*/
		KEYBOARD_SEMICOLON = 51,
		KEYBOARD_APOSTROPHE = 52,
		KEYBOARD_GRAVE = 53, /**< Located in the top left corner (on both ANSI
								*   and ISO keyboards). Produces GRAVE ACCENT and
								*   TILDE in a US Windows layout and in US and UK
								*   Mac layouts on ANSI keyboards, GRAVE ACCENT
								*   and NOT SIGN in a UK Windows layout, SECTION
								*   SIGN and PLUS-MINUS SIGN in US and UK Mac
								*   layouts on ISO keyboards, SECTION SIGN and
								*   DEGREE SIGN in a Swiss German layout (Mac:
								*   only on ISO keyboards), CIRCUMFLEX ACCENT and
								*   DEGREE SIGN in a German layout (Mac: only on
								*   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
								*   French Windows layout, COMMERCIAL AT and
								*   NUMBER SIGN in a French Mac layout on ISO
								*   keyboards, and LESS-THAN SIGN and GREATER-THAN
								*   SIGN in a Swiss German, German, or French Mac
								*   layout on ANSI keyboards.
								*/
		KEYBOARD_COMMA = 54,
		KEYBOARD_PERIOD = 55,
		KEYBOARD_SLASH = 56,

		KEYBOARD_CAPSLOCK = 57,

		KEYBOARD_F1 = 58,
		KEYBOARD_F2 = 59,
		KEYBOARD_F3 = 60,
		KEYBOARD_F4 = 61,
		KEYBOARD_F5 = 62,
		KEYBOARD_F6 = 63,
		KEYBOARD_F7 = 64,
		KEYBOARD_F8 = 65,
		KEYBOARD_F9 = 66,
		KEYBOARD_F10 = 67,
		KEYBOARD_F11 = 68,
		KEYBOARD_F12 = 69,

		KEYBOARD_PRINTSCREEN = 70,
		KEYBOARD_SCROLLLOCK = 71,
		KEYBOARD_PAUSE = 72,
		KEYBOARD_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
									does send code 73, not 117) */
		KEYBOARD_HOME = 74,
		KEYBOARD_PAGEUP = 75,
		KEYBOARD_DELETE = 76,
		KEYBOARD_END = 77,
		KEYBOARD_PAGEDOWN = 78,
		KEYBOARD_RIGHT = 79,
		KEYBOARD_LEFT = 80,
		KEYBOARD_DOWN = 81,
		KEYBOARD_UP = 82,

		KEYBOARD_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
										*/
		KEYBOARD_KP_DIVIDE = 84,
		KEYBOARD_KP_MULTIPLY = 85,
		KEYBOARD_KP_MINUS = 86,
		KEYBOARD_KP_PLUS = 87,
		KEYBOARD_KP_ENTER = 88,
		KEYBOARD_KP_1 = 89,
		KEYBOARD_KP_2 = 90,
		KEYBOARD_KP_3 = 91,
		KEYBOARD_KP_4 = 92,
		KEYBOARD_KP_5 = 93,
		KEYBOARD_KP_6 = 94,
		KEYBOARD_KP_7 = 95,
		KEYBOARD_KP_8 = 96,
		KEYBOARD_KP_9 = 97,
		KEYBOARD_KP_0 = 98,
		KEYBOARD_KP_PERIOD = 99,

		KEYBOARD_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
											*   keyboards have over ANSI ones,
											*   located between left shift and Y.
											*   Produces GRAVE ACCENT and TILDE in a
											*   US or UK Mac layout, REVERSE SOLIDUS
											*   (backslash) and VERTICAL LINE in a
											*   US or UK Windows layout, and
											*   LESS-THAN SIGN and GREATER-THAN SIGN
											*   in a Swiss German, German, or French
											*   layout. */
		KEYBOARD_APPLICATION = 101, /**< windows contextual menu, compose */
		KEYBOARD_POWER = 102, /**< The USB document says this is a status flag,
								*   not a physical key - but some Mac keyboards
								*   do have a power key. */
		KEYBOARD_KP_EQUALS = 103,
		KEYBOARD_F13 = 104,
		KEYBOARD_F14 = 105,
		KEYBOARD_F15 = 106,
		KEYBOARD_F16 = 107,
		KEYBOARD_F17 = 108,
		KEYBOARD_F18 = 109,
		KEYBOARD_F19 = 110,
		KEYBOARD_F20 = 111,
		KEYBOARD_F21 = 112,
		KEYBOARD_F22 = 113,
		KEYBOARD_F23 = 114,
		KEYBOARD_F24 = 115,
		KEYBOARD_EXECUTE = 116,
		KEYBOARD_HELP = 117,
		KEYBOARD_MENU = 118,
		KEYBOARD_SELECT = 119,
		KEYBOARD_STOP = 120,
		KEYBOARD_AGAIN = 121,   /**< redo */
		KEYBOARD_UNDO = 122,
		KEYBOARD_CUT = 123,
		KEYBOARD_COPY = 124,
		KEYBOARD_PASTE = 125,
		KEYBOARD_FIND = 126,
		KEYBOARD_MUTE = 127,
		KEYBOARD_VOLUMEUP = 128,
		KEYBOARD_VOLUMEDOWN = 129,
	/* not sure whether there's a reason to enable these */
	/*     KEYBOARD_LOCKINGCAPSLOCK = 130,  */
	/*     KEYBOARD_LOCKINGNUMLOCK = 131, */
	/*     KEYBOARD_LOCKINGSCROLLLOCK = 132, */
		KEYBOARD_KP_COMMA = 133,
		KEYBOARD_KP_EQUALSAS400 = 134,

		KEYBOARD_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
												footnotes in USB doc */
		KEYBOARD_INTERNATIONAL2 = 136,
		KEYBOARD_INTERNATIONAL3 = 137, /**< Yen */
		KEYBOARD_INTERNATIONAL4 = 138,
		KEYBOARD_INTERNATIONAL5 = 139,
		KEYBOARD_INTERNATIONAL6 = 140,
		KEYBOARD_INTERNATIONAL7 = 141,
		KEYBOARD_INTERNATIONAL8 = 142,
		KEYBOARD_INTERNATIONAL9 = 143,
		KEYBOARD_LANG1 = 144, /**< Hangul/English toggle */
		KEYBOARD_LANG2 = 145, /**< Hanja conversion */
		KEYBOARD_LANG3 = 146, /**< Katakana */
		KEYBOARD_LANG4 = 147, /**< Hiragana */
		KEYBOARD_LANG5 = 148, /**< Zenkaku/Hankaku */
		KEYBOARD_LANG6 = 149, /**< reserved */
		KEYBOARD_LANG7 = 150, /**< reserved */
		KEYBOARD_LANG8 = 151, /**< reserved */
		KEYBOARD_LANG9 = 152, /**< reserved */

		KEYBOARD_ALTERASE = 153, /**< Erase-Eaze */
		KEYBOARD_SYSREQ = 154,
		KEYBOARD_CANCEL = 155,
		KEYBOARD_CLEAR = 156,
		KEYBOARD_PRIOR = 157,
		KEYBOARD_RETURN2 = 158,
		KEYBOARD_SEPARATOR = 159,
		KEYBOARD_OUT = 160,
		KEYBOARD_OPER = 161,
		KEYBOARD_CLEARAGAIN = 162,
		KEYBOARD_CRSEL = 163,
		KEYBOARD_EXSEL = 164,

		KEYBOARD_KP_00 = 176,
		KEYBOARD_KP_000 = 177,
		KEYBOARD_THOUSANDSSEPARATOR = 178,
		KEYBOARD_DECIMALSEPARATOR = 179,
		KEYBOARD_CURRENCYUNIT = 180,
		KEYBOARD_CURRENCYSUBUNIT = 181,
		KEYBOARD_KP_LEFTPAREN = 182,
		KEYBOARD_KP_RIGHTPAREN = 183,
		KEYBOARD_KP_LEFTBRACE = 184,
		KEYBOARD_KP_RIGHTBRACE = 185,
		KEYBOARD_KP_TAB = 186,
		KEYBOARD_KP_BACKSPACE = 187,
		KEYBOARD_KP_A = 188,
		KEYBOARD_KP_B = 189,
		KEYBOARD_KP_C = 190,
		KEYBOARD_KP_D = 191,
		KEYBOARD_KP_E = 192,
		KEYBOARD_KP_F = 193,
		KEYBOARD_KP_XOR = 194,
		KEYBOARD_KP_POWER = 195,
		KEYBOARD_KP_PERCENT = 196,
		KEYBOARD_KP_LESS = 197,
		KEYBOARD_KP_GREATER = 198,
		KEYBOARD_KP_AMPERSAND = 199,
		KEYBOARD_KP_DBLAMPERSAND = 200,
		KEYBOARD_KP_VERTICALBAR = 201,
		KEYBOARD_KP_DBLVERTICALBAR = 202,
		KEYBOARD_KP_COLON = 203,
		KEYBOARD_KP_HASH = 204,
		KEYBOARD_KP_SPACE = 205,
		KEYBOARD_KP_AT = 206,
		KEYBOARD_KP_EXCLAM = 207,
		KEYBOARD_KP_MEMSTORE = 208,
		KEYBOARD_KP_MEMRECALL = 209,
		KEYBOARD_KP_MEMCLEAR = 210,
		KEYBOARD_KP_MEMADD = 211,
		KEYBOARD_KP_MEMSUBTRACT = 212,
		KEYBOARD_KP_MEMMULTIPLY = 213,
		KEYBOARD_KP_MEMDIVIDE = 214,
		KEYBOARD_KP_PLUSMINUS = 215,
		KEYBOARD_KP_CLEAR = 216,
		KEYBOARD_KP_CLEARENTRY = 217,
		KEYBOARD_KP_BINARY = 218,
		KEYBOARD_KP_OCTAL = 219,
		KEYBOARD_KP_DECIMAL = 220,
		KEYBOARD_KP_HEXADECIMAL = 221,

		KEYBOARD_LCTRL = 224,
		KEYBOARD_LSHIFT = 225,
		KEYBOARD_LALT = 226, /**< alt, option */
		KEYBOARD_LGUI = 227, /**< windows, command (apple), meta */
		KEYBOARD_RCTRL = 228,
		KEYBOARD_RSHIFT = 229,
		KEYBOARD_RALT = 230, /**< alt gr, option */
		KEYBOARD_RGUI = 231, /**< windows, command (apple), meta */

		KEYBOARD_MODE = 257,    /**< I'm not sure if this is really not covered
									*   by any of the above, but since there's a
									*   special KMOD_MODE for it I'm adding it here
									*/

		/* @} *//* Usage page 0x07 */

		/**
		 *  \name Usage page 0x0C
		 *
		 *  These values are mapped from usage page 0x0C (USB consumer page).
		 */
		/* @{ */

		KEYBOARD_AUDIONEXT = 258,
		KEYBOARD_AUDIOPREV = 259,
		KEYBOARD_AUDIOSTOP = 260,
		KEYBOARD_AUDIOPLAY = 261,
		KEYBOARD_AUDIOMUTE = 262,
		KEYBOARD_MEDIASELECT = 263,
		KEYBOARD_WWW = 264,
		KEYBOARD_MAIL = 265,
		KEYBOARD_CALCULATOR = 266,
		KEYBOARD_COMPUTER = 267,
		KEYBOARD_AC_SEARCH = 268,
		KEYBOARD_AC_HOME = 269,
		KEYBOARD_AC_BACK = 270,
		KEYBOARD_AC_FORWARD = 271,
		KEYBOARD_AC_STOP = 272,
		KEYBOARD_AC_REFRESH = 273,
		KEYBOARD_AC_BOOKMARKS = 274,

		/* @} *//* Usage page 0x0C */

		/**
		 *  \name Walther keys
		 *
		 *  These are values that Christian Walther added (for mac keyboard?).
		 */
		/* @{ */

		KEYBOARD_BRIGHTNESSDOWN = 275,
		KEYBOARD_BRIGHTNESSUP = 276,
		KEYBOARD_DISPLAYSWITCH = 277, /**< display mirroring/dual display
											switch, video mode switch */
		KEYBOARD_KBDILLUMTOGGLE = 278,
		KEYBOARD_KBDILLUMDOWN = 279,
		KEYBOARD_KBDILLUMUP = 280,
		KEYBOARD_EJECT = 281,
		KEYBOARD_SLEEP = 282,

		KEYBOARD_APP1 = 283,
		KEYBOARD_APP2 = 284,

		/* @} *//* Walther keys */

		/**
		 *  \name Usage page 0x0C (additional media keys)
		 *
		 *  These values are mapped from usage page 0x0C (USB consumer page).
		 */
		/* @{ */

		KEYBOARD_AUDIOREWIND = 285,
		KEYBOARD_AUDIOFASTFORWARD = 286,

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

	/**
	 * @brief check if a key has been pressed on the keyboard
	 * 
	 * @param event the key to check for
	 * @return true if yes
	 * @return false if no
	 */
	static bool check_keyboard(Keyboard event) {
		if (m_keyboardState[static_cast<SDL_Scancode>(event)]) return true;
		else return false;
	}

	/**
	 * @brief update the keys. Called every frame
	 */
	static void update() {
		SDL_PumpEvents();
	}

private:
	static const uint8* m_keyboardState;
};

} // namespace lyra
