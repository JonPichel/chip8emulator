#include <stdlib.h>
#include <stdint.h>
#include <curses.h>

#include "chip8vm.h"
#include "settings.h"
#include "memory_window.h"
#include "registers_window.h"
#include "stack_window.h"

extern uint16_t stack[STACK_SIZE];
extern uint8_t sp;

WINDOW *stackwin;

void stack_setup_window(void) {
    stackwin = newwin(STACKWIN_HEIGHT, STACKWIN_WIDTH, REGWIN_HEIGHT, SCR_WIDTH + MEMWIN_WIDTH);
    box(stackwin, 0, 0);
    wrefresh(stackwin);
}

void stack_display(void) {
    int i;

    for (i = 0; i < STACK_SIZE; i++) {
        if (i == sp)
            wattron(stackwin, COLOR_PAIR(SP_PAIR));
        mvwprintw(stackwin, i + 1, 2, "[%X] %04X", i, stack[i]);
        if (i == sp)
            wattroff(stackwin, COLOR_PAIR(SP_PAIR));
    }

    wrefresh(stackwin);
}
