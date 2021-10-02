#ifndef __CHIP8_SETTINGS_H__
#define __CHIP8_SETTINGS_H__

#include <curses.h>

#define PC_PAIR  1
#define I_PAIR   2
#define SP_PAIR  3
#define KEY_PAIR 4

#define PC_FG   COLOR_RED
#define PC_BG   COLOR_BLACK

#define I_FG    COLOR_BLUE
#define I_BG    COLOR_BLACK

#define SP_FG   COLOR_RED
#define SP_BG   COLOR_BLACK

#define KEY_FG  COLOR_RED
#define KEY_BG  COLOR_WHITE

#define ROM_DIR "../roms"

void settings_init(void);

#endif /* ___CHIP8_SETTINGS_H__ */

