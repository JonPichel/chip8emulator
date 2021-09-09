#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <curses.h>

#include "chip8vm.h"
#include "settings.h"
#include "info_window.h"
#include "memory_window.h"
#include "registers_window.h"

extern uint16_t pc;
extern uint16_t I;
extern uint8_t V[NUM_REGS];

WINDOW *regwin;

void registers_setup_window(void) {
    regwin = newwin(REGWIN_HEIGHT, REGWIN_WIDTH, 0, SCR_WIDTH + MEMWIN_WIDTH);
    box(regwin, 0, 0);
    wrefresh(regwin);
}

void registers_display(void) {
    int x, y, n;

    for (y = 0; y < 4; y++) {
        wmove(regwin, 1 + y, 2);
        for (x = 0; x < 4; x++) {
            n = y * 4 + x;
            wprintw(regwin, "V%X: %02X ", n, V[n]);
        }
    }

    wmove(regwin, 6, 2);
    wprintw(regwin, "PC: %04X", pc);
    wmove(regwin, 6, REGWIN_WIDTH - 9);
    wprintw(regwin, "I: %04X", I);

    box(regwin, 0, 0);
    wrefresh(regwin);
}

