#include <stdlib.h>
#include <stdint.h>
#include <curses.h>

#include "chip8vm.h"
#include "settings.h"
#include "memory_window.h"
#include "registers_window.h"
#include "stack_window.h"
#include "timers_window.h"
#include "keypad_window.h"

extern uint8_t key[NUM_KEYS];

WINDOW *keywin;

uint8_t display_order[] = {
    0x1, 0x2, 0x3, 0xC,
    0x4, 0x5, 0x6, 0xD,
    0x7, 0x8, 0x9, 0xE,
    0xA, 0x0, 0xB, 0xF
};

void keypad_setup_window(void) {
    keywin = newwin(KEYWIN_HEIGHT, KEYWIN_WIDTH, REGWIN_HEIGHT, SCR_WIDTH + MEMWIN_WIDTH + STACKWIN_WIDTH);
    box(keywin, 0, 0);
    wrefresh(keywin);
}

void keypad_display(void) {
    for (int i = 0; i < NUM_KEYS; i++) {
        if (key[display_order[i]] == 1)
            wattron(keywin, COLOR_PAIR(KEY_PAIR));
        mvwprintw(keywin, 2 + i / 4 * 4, 3 + (i % 4) * 4, "%X", display_order[i]);
        if (key[display_order[i]] == 1)
            wattroff(keywin, COLOR_PAIR(KEY_PAIR));
    }
    wrefresh(keywin);
}
