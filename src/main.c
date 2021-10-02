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
#include "selection_screen.h"
#include "info_window.h"
#include "memory_window.h"
#include "registers_window.h"
#include "stack_window.h"
#include "timers_window.h"
#include "keypad_window.h"

#define WHITECHAR '#'
#define BLACKCHAR ' '

#define CPU_SPEED_HZ    500
#define TIMER_SPEED_HZ  60
#define CPU_US          1000000 / CPU_SPEED_HZ
#define TIMER_US        1000000 / TIMER_SPEED_HZ

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
    char *selected_rom;
    struct timespec current_time, cpu_last_time, timer_last_time;
    bool running, step;
    int diff_us;
    chtype c;
    
    // Initialise ncurses
    initscr();
    start_color();
    settings_init();

    cbreak();                       // Disable line buffering
    noecho();                       // Don't echo characters
    curs_set(0);                    // Make cursor invisible
    keypad(stdscr, true);

    selected_rom = select_rom();

    // Create windows
    gamewin = newwin(SCR_HEIGHT, SCR_WIDTH, 0, 0);
    info_setup_window();
    memory_setup_window();
    registers_setup_window();
    stack_setup_window();
    timers_setup_window();
    keypad_setup_window();

    // Cursor options
    nodelay(gamewin, !debug_mode);  // false -> blocking, true -> non-blocking
    keypad(gamewin, true);          // Allow arrow keys etc.

    chip8_init();
    if (chip8_loadrom(selected_rom) == -1) {
    //if ((chip8_loadrom("/home/jonathanpc/Programming/C/chip8emulator/roms/games/Pong (1 player).ch8")) == -1) {
    //if ((chip8_loadrom("/home/jonathanpc/Programming/C/chip8emulator/roms/demos/Particle Demo [zeroZshadow, 2008].ch8")) == -1) {
    //if ((chip8_loadrom("/home/jonathanpc/Programming/C/chip8emulator/roms/demos/Maze [David Winter, 199x].ch8")) == -1) {
        endwin();
        perror("Loading ROM file");
        exit(EXIT_FAILURE);
    }

    clock_gettime(CLOCK_MONOTONIC, &cpu_last_time);
    clock_gettime(CLOCK_MONOTONIC, &timer_last_time);
    running = true;
    while(running) {
        // Update debug windows
        if (debug_mode || true) {
            memory_display();
            registers_display();
            stack_display();
            timers_display();
            keypad_display();
        }

        // Get input
        //memset(key, 0, sizeof(uint8_t) * NUM_KEYS);
        step = false;
        while (!step && (c = wgetch(gamewin)) != ERR) {
            info_message("Pressed key: %c", c);
            switch(c) {
                case '0':
                    if (debug_mode)
                        key[0x0] = !key[0x0];
                    else
                        key[0x0] = 1;
                    break;
                case '1':
                    if (debug_mode)
                        key[0x1] = !key[0x1];
                    else
                        key[0x1] = 1;
                    break;
                case '2':
                    if (debug_mode)
                        key[0x2] = !key[0x2];
                    else
                        key[0x2] = 1;
                    break;
                case '3':
                    if (debug_mode)
                        key[0x3] = !key[0x3];
                    else
                        key[0x3] = 1;
                    break;
                case '4':
                    if (debug_mode)
                        key[0x4] = !key[0x4];
                    else
                        key[0x4] = 1;
                    break;
                case '5':
                    if (debug_mode)
                        key[0x5] = !key[0x5];
                    else
                        key[0x5] = 1;
                    break;
                case '6':
                    if (debug_mode)
                        key[0x6] = !key[0x6];
                    else
                        key[0x6] = 1;
                    break;
                case '7':
                    if (debug_mode)
                        key[0x7] = !key[0x7];
                    else
                        key[0x7] = 1;
                    break;
                case '8':
                    if (debug_mode)
                        key[0x8] = !key[0x8];
                    else
                        key[0x8] = 1;
                    break;
                case '9':
                    if (debug_mode)
                        key[0x9] = !key[0x9];
                    else
                        key[0x9] = 1;
                    break;
                case 'q':
                    if (debug_mode)
                        key[0xA] = !key[0xA];
                    else
                        key[0xA] = 1;
                    break;
                case 'w':
                    if (debug_mode)
                        key[0xB] = !key[0xB];
                    else
                        key[0xB] = 1;
                    break;
                case 'e':
                    if (debug_mode)
                        key[0xC] = !key[0xC];
                    else
                        key[0xC] = 1;
                    break;
                case 'r':
                    if (debug_mode)
                        key[0xD] = !key[0xD];
                    else
                        key[0xD] = 1;
                    break;
                case 't':
                    if (debug_mode)
                        key[0xE] = !key[0xE];
                    else
                        key[0xE] = 1;
                    break;
                case 'y':
                    if (debug_mode)
                        key[0xF] = !key[0xF];
                    else
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
            // Update keypad screen
            keypad_display();
        }

        // Process instruction

        clock_gettime(CLOCK_MONOTONIC, &current_time);
        diff_us = (current_time.tv_sec - cpu_last_time.tv_sec) * 1000000 + (current_time.tv_nsec - cpu_last_time.tv_nsec) / 1000;
        if (diff_us >= CPU_US) {
            cpu_last_time = current_time;
            if (chip8_cycle() == -1) {
                running = false;
                info_message("Unknown instruction!");
            }
        }
            
        // Draw if needed
        if (chip8_draw_flag) {
            draw();
            chip8_draw_flag = false;
        }
        
        // Update timers if needed
        diff_us = (current_time.tv_sec - timer_last_time.tv_sec) * 1000000 + (current_time.tv_nsec - timer_last_time.tv_nsec) / 1000;
        if (diff_us >= TIMER_US) {
            chip8_tick();
            timer_last_time = current_time;
        }
        
        // TODO: Make a beep noise if sound_timer > 0
    }

    endwin();
    perror("Error");
    return 0;
}

