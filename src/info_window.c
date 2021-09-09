#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <curses.h>

#include "chip8vm.h"
#include "info_window.h"

WINDOW *infowin;

static int nlines, ncols;

void info_setup_window(void) {
    infowin = newwin(LINES - SCR_HEIGHT, SCR_WIDTH, SCR_HEIGHT, 0);
    getmaxyx(infowin, nlines, ncols);
    box(infowin, 0, 0);
    wrefresh(infowin);
}

void info_message(char *format, ...) {
    int length = ncols - 2, strlength;
    char buffer[length];
    va_list valist;

    va_start(valist, format);

    vsnprintf(buffer, length, format, valist);

    
    strlength = strlen(buffer);
    memset(&buffer[strlength], ' ', length - strlength - 1);
    buffer[length - 1] = '\0';

    mvwprintw(infowin, 1, 1, buffer);
    wrefresh(infowin);

    va_end(valist);
}

