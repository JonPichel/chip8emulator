#include <stdlib.h>
#include <stdint.h>
#include <curses.h>

#include "chip8vm.h"
#include "settings.h"
#include "memory_window.h"

extern uint8_t memory[MEM_SIZE];
extern uint16_t pc;
extern uint16_t I;

WINDOW *memwin;

static uint16_t start = 0;
static int nlines;

void memory_setup_window(void) {
    memwin = newwin(LINES, MEMWIN_WIDTH, 0, SCR_WIDTH);
    nlines = getmaxy(memwin);
    box(memwin, 0, 0);
    wrefresh(memwin);
}

void memory_display(void) {
    int line, word, addr;

    for (line = 0; line < nlines - 2; line++) {
        wmove(memwin, line + 1, 1);
        wprintw(memwin, " %04X: ", start + line * 0x10);

        for (word = 0; word < 8; word++) {
            addr = start + line * 0x10 + 2 * word;
            wprintw(memwin, "%02X", memory[addr]);
            wprintw(memwin, "%02X ", memory[addr + 1]);
        }
    }

    if (pc >= start && pc <= addr + 1) {
        wmove(memwin, 1 + (pc - start) / 0x10, (pc - start) % 0x10 * 2 + 8 + (pc - start) % 0x10 / 2);
        wattron(memwin, COLOR_PAIR(PC_PAIR));
        wprintw(memwin, "%02X", memory[pc]);
        wattroff(memwin, COLOR_PAIR(PC_PAIR));
    }
    if (I >= start && I <= addr + 1) {
        wmove(memwin, 1 + (I - start) / 0x10, (I - start) % 0x10 * 2 + 8 + (I - start) % 0x10 / 2);
        wattron(memwin, COLOR_PAIR(I_PAIR));
        wprintw(memwin, "%02X", memory[I]);
        wattroff(memwin, COLOR_PAIR(I_PAIR));
    }

    box(memwin, 0, 0);
    wrefresh(memwin);
}

void memory_up(void) {
    if (start > 0x0) {
        start -= 0x10;
        memory_display();
    }
}

void memory_down(void) {
    if (start < 0x1000) {
        start += 0x10;
        memory_display();
    }
}
