#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <curses.h>

#include "chip8vm.h"
#include "settings.h"
#include "info_window.h"
#include "memory_window.h"
#include "registers_window.h"
#include "stack_window.h"

#define WHITECHAR '#'
#define BLACKCHAR ' '

#define CLOCK_MS 17

/* Peripheral ports defined in chip8vm.c */
extern uint8_t key[NUM_KEYS];
extern uint8_t gfx[GFX_SIZE];
extern bool chip8_draw_flag;

WINDOW *gamewin;

bool debug_mode = true;

void draw(void) {
    int x, y;
    for (y = 0; y < SCR_HEIGHT; y++) {
        wmove(gamewin, y, 0);
        for (x = 0; x < SCR_WIDTH; x++) {
            waddch(gamewin, ((gfx[y * SCR_WIDTH + x] == 0x1) ? WHITECHAR : BLACKCHAR));
        }
    }
    wrefresh(gamewin);
}

int main(void) {
    chtype c;
    struct timespec current_time, last_time;
    bool running, step;
    int diff_ms;
    
    // Initialise ncurses
    initscr();
    start_color();
    settings_init();

    // Create windows
    gamewin = newwin(SCR_HEIGHT, SCR_WIDTH, 0, 0);
    info_setup_window();
    memory_setup_window();
    registers_setup_window();
    stack_setup_window();

    // Cursor options
    cbreak();                       // Disable line buffering
    noecho();                       // Don't echo characters
    curs_set(0);                    // Make cursor invisible
    nodelay(gamewin, !debug_mode);  // false -> blocking, true -> non-blocking
    keypad(gamewin, true);          // Allow arrow keys etc.

    chip8_init();
    if ((chip8_loadrom("/home/jonathanpc/Programming/C/chip8emulator/roms/demos/Particle Demo [zeroZshadow, 2008].ch8")) == -1) {
    //if ((chip8_loadrom("/home/jonathanpc/Programming/C/chip8emulator/roms/demos/Maze [David Winter, 199x].ch8")) == -1) {
        endwin();
        perror("Loading ROM file");
        exit(EXIT_FAILURE);
    }
    clock_gettime(CLOCK_MONOTONIC, &last_time);
    running = true;
    while(running) {
        // Update debug windows
        if (true) {
            memory_display();
            registers_display();
            stack_display();
        }

        // Get input
        memset(key, 0, sizeof(uint8_t) * NUM_KEYS);
        step = false;
        while (!step && (c = wgetch(gamewin)) != ERR) {
            info_message("Pressed key: %c", c);
            switch(c) {
                case '0':
                    key[0x0] = 1;
                    break;
                case '1':
                    key[0x1] = 1;
                    break;
                case '2':
                    key[0x2] = 1;
                    break;
                case '3':
                    key[0x3] = 1;
                    break;
                case '4':
                    key[0x4] = 1;
                    break;
                case '5':
                    key[0x5] = 1;
                    break;
                case '6':
                    key[0x6] = 1;
                    break;
                case '7':
                    key[0x7] = 1;
                    break;
                case '8':
                    key[0x8] = 1;
                    break;
                case '9':
                    key[0x9] = 1;
                    break;
                case 'q':
                    key[0xA] = 1;
                    break;
                case 'w':
                    key[0xB] = 1;
                    break;
                case 'e':
                    key[0xC] = 1;
                    break;
                case 'r':
                    key[0xD] = 1;
                    break;
                case 't':
                    key[0xE] = 1;
                    break;
                case 'y':
                    key[0xF] = 1;
                    break;
                case 'd':
                    // Toggle debug mode
                    debug_mode = !debug_mode;
                    nodelay(gamewin, !debug_mode); // If debug -> blocking, else -> non-blocking
                    break;
                case 's':
                    // Step instruction
                    step = true;
                    break;
                case KEY_UP:
                    info_message("Travelling up memory");
                    memory_up();
                    break;
                case KEY_DOWN:
                    info_message("Travelling down memory");
                    memory_down();
                    break;
            }
        }

        // Process instruction
        if (chip8_cycle() == -1) {
            running = false;
            info_message("Unknown instruction!");
        }
            
        
        // Draw if needed
        if (chip8_draw_flag) {
            draw();
            chip8_draw_flag = false;
        }
        
        // Update timers if needed
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        diff_ms = (current_time.tv_sec - last_time.tv_sec) * 1000 + (current_time.tv_nsec - last_time.tv_nsec) / 1000;
        if (diff_ms >= CLOCK_MS) {
            chip8_tick();
            last_time = current_time;
        }
        
        // TODO: Make a beep noise if sound_timer > 0
    }

    endwin();
    perror("Error");
    return 0;
}

