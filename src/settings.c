#include <curses.h>

#include "settings.h"

void settings_init(void) {
    init_pair(PC_PAIR, PC_FG, PC_BG);
    init_pair(I_PAIR, I_FG, I_BG);
    init_pair(SP_PAIR, SP_FG, SP_BG);
    init_pair(KEY_PAIR, KEY_FG, KEY_BG);
}
