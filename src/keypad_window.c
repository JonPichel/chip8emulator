#include <stdlib.h>
#include <stdint.h>
#include <curses.h>

#include "chip8vm.h"
#include "memory_window.h"
#include "registers_window.h"
#include "stack_window.h"
#include "timers_window.h"
#include "keypad_window.h"

extern uint8_t key[NUM_KEYS];

WINDOW *keywin;

void keypad_setup_window(void) {
    keywin = newwin(KEYWIN_HEIGHT, KEYWIN_WIDTH, REGWIN_HEIGHT, SCR_WIDTH + MEMWIN_WIDTH + STACKWIN_WIDTH);
    box(keywin, 0, 0);
    wrefresh(keywin);
}

void keypad_display(void) {
    return;
}
