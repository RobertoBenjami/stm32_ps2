#ifndef _PS2_CODEPAGE_H
#define _PS2_CODEPAGE_H

/* keyboard language settings */

#if ((defined PS2_KBDCLK) && (defined PS2_KBDDATA))

/* KEYMAP_US or KEYMAP_D or KEYMAP_HU */
#define KEYMAP_US

#define PS2_MAINKEYMAP_SIZE  104
#define PS2_NUMKEYMAP_SIZE    32
typedef struct {
  uint8_t noshift[PS2_MAINKEYMAP_SIZE];
  uint8_t shift[PS2_MAINKEYMAP_SIZE];
  uint8_t noshiftcaps[PS2_MAINKEYMAP_SIZE];
  uint8_t shiftcaps[PS2_MAINKEYMAP_SIZE];
  uint8_t numoff[PS2_NUMKEYMAP_SIZE];
  uint8_t numon[PS2_NUMKEYMAP_SIZE];
  uint8_t uses_altgr;
  uint8_t altgr[PS2_MAINKEYMAP_SIZE];
} PS2Keymap_t;

#ifdef KEYMAP_US
const PS2Keymap_t keymap = {
  // noshift
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '`', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
  0, 0, 'z', 's', 'a', 'w', '2', 0,
  0, 'c', 'x', 'd', 'e', '4', '3', 0,
  0, ' ', 'v', 'f', 't', 'r', '5', 0,
  0, 'n', 'b', 'h', 'g', 'y', '6', 0,
  0, 0, 'm', 'j', 'u', '7', '8', 0,
  0, ',', 'k', 'i', 'o', '0', '9', 0,
  0, '.', '/', 'l', ';', 'p', '-', 0,
  0, 0, '\'', 0, '[', '=', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, '\\', 0, 0,
  0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0},
  // shift
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '~', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
  0, 0, 'Z', 'S', 'A', 'W', '@', 0,
  0, 'C', 'X', 'D', 'E', '$', '#', 0,
  0, ' ', 'V', 'F', 'T', 'R', '%', 0,
  0, 'N', 'B', 'H', 'G', 'Y', '^', 0,
  0, 0, 'M', 'J', 'U', '&', '*', 0,
  0, '<', 'K', 'I', 'O', ')', '(', 0,
  0, '>', '?', 'L', ':', 'P', '_', 0,
  0, 0, '"', 0, '{', '+', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '}', 0, '|', 0, 0,
  0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0},
  // noshiftcaps
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '`', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '1', 0,
  0, 0, 'Z', 'S', 'A', 'W', '2', 0,
  0, 'C', 'X', 'D', 'E', '4', '3', 0,
  0, ' ', 'V', 'F', 'T', 'R', '5', 0,
  0, 'N', 'B', 'H', 'G', 'Y', '6', 0,
  0, 0, 'M', 'J', 'U', '7', '8', 0,
  0, ',', 'K', 'I', 'o', '0', '9', 0,
  0, '.', '/', 'L', ';', 'P', '-', 0,
  0, 0, '\'', 0, '[', '=', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, '\\', 0, 0,
  0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0},
  // shiftcaps
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '~', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '!', 0,
  0, 0, 'z', 's', 'a', 'w', '@', 0,
  0, 'c', 'x', 'd', 'e', '$', '#', 0,
  0, ' ', 'v', 'f', 't', 'r', '%', 0,
  0, 'n', 'b', 'h', 'g', 'y', '^', 0,
  0, 0, 'm', 'j', 'u', '&', '*', 0,
  0, '<', 'k', 'i', 'O', ')', '(', 0,
  0, '>', '?', 'l', ':', 'p', '_', 0,
  0, 0, '"', 0, '{', '+', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '}', 0, '|', 0, 0,
  0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0},
  // numoff
  {0, '1', 0, '4', '7', 0, 0, 0,
  '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
  PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
  0, 0, 0, PS2_F7 },
  // numon
  {0, '1', 0, '4', '7', 0, 0, 0,
  '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
  PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
  0, 0, 0, PS2_F7 },
  // uses_altgr
  0
};
#endif

#ifdef KEYMAP_HU
const PS2Keymap_t keymap = {
  // noshift
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '0', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
  0, 0, 'y', 's', 'a', 'w', '2', 0,
  0, 'c', 'x', 'd', 'e', '4', '3', 0,
  0, ' ', 'v', 'f', 't', 'r', '5', 0,
  0, 'n', 'b', 'h', 'g', 'z', '6', 0,
  0, 0, 'm', 'j', 'u', '7', '8', 0,
  0, ',', 'k', 'i', 'o', 'ö', '9', 0,
  0, '.', '-', 'l', 'é', 'p', 'ü', 0,
  0, 0, 'á', 0, 'õ', 'ó', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, 'ú', 0, '\\', 0, 0,
  0, 'í', 0, 0, 0, 0, PS2_BACKSPACE, 0},
  // shift
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '§', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '\'', 0,
  0, 0, 'Y', 'S', 'A', 'W', '\"', 0,
  0, 'C', 'X', 'D', 'E', '!', '+', 0,
  0, ' ', 'V', 'F', 'T', 'R', '%', 0,
  0, 'N', 'B', 'H', 'G', 'Z', '/', 0,
  0, 0, 'M', 'J', 'U', '=', '(', 0,
  0, '?', 'K', 'I', 'O', 'Ö', ')', 0,
  0, ':', '_', 'L', 'É', 'P', 'Ü', 0,
  0, 0, 'Á', 0, 'Õ', 'Ó', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, 'Ú', 0, '|', 0, 0,
  0, 'Í', 0, 0, 0, 0, PS2_BACKSPACE, 0},
  // noshiftcaps
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '0', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '1', 0,
  0, 0, 'Y', 'S', 'A', 'W', '2', 0,
  0, 'C', 'X', 'D', 'E', '4', '3', 0,
  0, ' ', 'V', 'F', 'T', 'R', '5', 0,
  0, 'N', 'B', 'H', 'G', 'Z', '6', 0,
  0, 0, 'M', 'J', 'U', '7', '8', 0,
  0, ',', 'K', 'I', 'O', 'Ö', '9', 0,
  0, '.', '-', 'L', 'É', 'P', 'Ü', 0,
  0, 0, 'Á', 0, 'Õ', 'Ó', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, 'Ú', 0, '\\', 0, 0,
  0, 'Í', 0, 0, 0, 0, PS2_BACKSPACE, 0},
  // shiftcaps
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '§', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '\'', 0,
  0, 0, 'y', 's', 'a', 'w', '\"', 0,
  0, 'c', 'x', 'd', 'e', '!', '+', 0,
  0, ' ', 'v', 'f', 't', 'r', '%', 0,
  0, 'n', 'b', 'h', 'g', 'z', '/', 0,
  0, 0, 'm', 'j', 'u', '=', '(', 0,
  0, '?', 'k', 'i', 'o', 'ö', ')', 0,
  0, ':', '_', 'l', 'é', 'p', 'ü', 0,
  0, 0, 'á', 0, 'õ', 'ó', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, 'ú', 0, '|', 0, 0,
  0, 'í', 0, 0, 0, 0, PS2_BACKSPACE, 0},
  // numoff
  {0, PS2_END, 0, PS2_LEFTARROW, PS2_HOME, 0, 0, 0,
  PS2_INSERT, PS2_DELETE, PS2_DOWNARROW, 0, PS2_LEFTARROW, PS2_UPARROW, PS2_ESC, 0 /*NumLock*/,
  PS2_F11, '+', PS2_PAGEDOWN, '-', '*', PS2_PAGEUP, PS2_SCROLL, 0,
  0, 0, 0, PS2_F7 },
  // numon
  {0, '1', 0, '4', '7', 0, 0, 0,
  '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
  PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
  0, 0, 0, PS2_F7 },
  // uses_altgr
  1,
  // altgr
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '0', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, '\\', '~', 0,
  0, 0, '>', 'ð', 'ä', '|', '¡', 0,
  0, '&', '#', 'Ð', 'Ä', '¢', '^', 0,
  0, ' ', '@', '[', 't', 'r', '°', 0,
  0, '}', '{', 'h', ']', 'z', '²', 0,
  0, 0, 'm', 'j', '€', '`', 'ÿ', 0,
  0, ';', '³', 'i', 'o', '½', '´', 0,
  0, '.', '*', '£', '$', 'p', '¨', 0,
  0, 0, 'ß', 0, '÷', '¸', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, 'ú', 0, '\\', 0, 0,
  0, '<', 0, 0, 0, 0, PS2_BACKSPACE, 0}
};
#endif

#ifdef KEYMAP_D
const PS2Keymap_t keymap = {
  // without shift
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '^', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
  0, 0, 'y', 's', 'a', 'w', '2', 0,
  0, 'c', 'x', 'd', 'e', '4', '3', 0,
  0, ' ', 'v', 'f', 't', 'r', '5', 0,
  0, 'n', 'b', 'h', 'g', 'z', '6', 0,
  0, 0, 'm', 'j', 'u', '7', '8', 0,
  0, ',', 'k', 'i', 'o', '0', '9', 0,
  0, '.', '-', 'l', PS2_o_DIAERESIS, 'p', PS2_SHARP_S, 0,
  0, 0, PS2_a_DIAERESIS, 0, PS2_u_DIAERESIS, '\'', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '+', 0, '#', 0, 0,
  0, '<', 0, 0, 0, 0, PS2_BACKSPACE, 0,
  0, '1', 0, '4', '7', 0, 0, 0,
  '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
  PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
  0, 0, 0, PS2_F7 },
  // with shift
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, PS2_DEGREE_SIGN, 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
  0, 0, 'Y', 'S', 'A', 'W', '"', 0,
  0, 'C', 'X', 'D', 'E', '$', PS2_SECTION_SIGN, 0,
  0, ' ', 'V', 'F', 'T', 'R', '%', 0,
  0, 'N', 'B', 'H', 'G', 'Z', '&', 0,
  0, 0, 'M', 'J', 'U', '/', '(', 0,
  0, ';', 'K', 'I', 'O', '=', ')', 0,
  0, ':', '_', 'L', PS2_O_DIAERESIS, 'P', '?', 0,
  0, 0, PS2_A_DIAERESIS, 0, PS2_U_DIAERESIS, '`', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '*', 0, '\'', 0, 0,
  0, '>', 0, 0, 0, 0, PS2_BACKSPACE, 0,
  0, '1', 0, '4', '7', 0, 0, 0,
  '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
  PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
  0, 0, 0, PS2_F7 },
  1,
  // with altgr
  {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, 0, 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, '@', 0, 0,
  0, 0, 0, 0, 0, 0, PS2_SUPERSCRIPT_TWO, 0,
  0, 0, 0, 0, PS2_CURRENCY_SIGN, 0, PS2_SUPERSCRIPT_THREE, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, PS2_MICRO_SIGN, 0, 0, '{', '[', 0,
  0, 0, 0, 0, 0, '}', ']', 0,
  0, 0, 0, 0, 0, 0, '\\', 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '~', 0, '#', 0, 0,
  0, '|', 0, 0, 0, 0, PS2_BACKSPACE, 0,
  0, '1', 0, '4', '7', 0, 0, 0,
  '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
  PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
  0, 0, 0, PS2_F7 }
};
#endif

#endif  /* #if ((defined PS2_KBDCLK) && defined PS2_KBDDATA)) */

#endif  /* _PS2_CODEPAGE_H */
