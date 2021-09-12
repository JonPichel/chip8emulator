#include <stdlib.h>
#include <stdint.h>
#include <curses.h>

#include "chip8vm.h"
#include "memory_window.h"
#include "registers_window.h"
#include "stack_window.h"
#include "timers_window.h"

extern uint8_t delay_timer;
extern uint8_t sound_timer;

WINDOW *timewin;

void timers_setup_window(void) {
    timewin = newwin(TIMEWIN_HEIGHT, TIMEWIN_WIDTH, REGWIN_HEIGHT + STACKWIN_HEIGHT, SCR_WIDTH + MEMWIN_WIDTH);
    box(timewin, 0, 0);
    wrefresh(timewin);
}

void timers_display(void) {
    mvwprintw(timewin, 1, 2, "DEL: %02X", delay_timer);
    mvwprintw(timewin, 3, 2, "SND: %02X", sound_timer);
    box(timewin, 0, 0);
    wrefresh(timewin);
}
