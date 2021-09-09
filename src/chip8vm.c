#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "chip8vm.h"

/* CPU state */
uint8_t     memory[MEM_SIZE];    // Memory space
uint8_t     V[NUM_REGS];            // Data registers
uint16_t    I;                  // Address register
uint16_t    pc;                 // Program counter
uint16_t    stack[STACK_SIZE];   // Stack to store return addresses
uint8_t     sp;                 // Stack pointer

/* Peripherals */
uint8_t     delay_timer;    // Delay timer: used to time the events of games
uint8_t     sound_timer;    // Sound timer: when value is nonzero, beep
uint8_t     key[NUM_KEYS];      // Keypad state
uint8_t     gfx[GFX_SIZE];   // Screen state

/* Auxiliary variables */
uint16_t    opcode;         // To store the current opcode
bool        chip8_draw_flag;      // Set -> update screen

uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
    0xF0, 0x80, 0xF0, 0x80, 0x80    // F
};

void chip8_init(void) {
    pc = START_ROM_ADDR;
    opcode = 0;
    I = 0;
    sp = 0;

    memset(V, 0, sizeof(uint8_t) * NUM_REGS);
    memset(stack, 0, sizeof(uint16_t) * STACK_SIZE);
    memset(memory, 0, sizeof(uint8_t) * MEM_SIZE);
    memset(key, 0, sizeof(bool) * NUM_KEYS);
    memset(gfx, 0, sizeof(bool) * GFX_SIZE);

    // Load fontset to memory
    for (int i = 0; i < 80; i++)
        memory[i] = fontset[i];

    chip8_draw_flag = true;
    delay_timer = 0;
    sound_timer = 0;
    srand(time(NULL));
}

int chip8_loadrom(char *pathname) {
    FILE *fgame;

    if ((fgame = fopen(pathname, "rb")) == NULL)
        return -1;

    fread(&memory[START_ROM_ADDR], 1, MAX_ROM_SIZE, fgame);

    fclose(fgame);

    return 0;
}

uint8_t draw_sprite(uint8_t vx, uint8_t vy, uint8_t n) {
    // Draw a sprite at coordinate (VX, VY) that is 8 pixels wide and N pixels high
    // Each row of 8 pixels is read as bit-coded starting from memory location I
    // VF is set to 1 if any of the pixels are flipped from set to unset; otherwise it is set to 0
    uint8_t x, y;
    uint16_t gfx_base = vy * SCR_WIDTH + vx;
    uint8_t pixel_row;

    uint8_t flipped_set = 0;
    for (y = 0; y < n; y++) { 
        pixel_row = memory[I + y];
        for (x = 0; x < 8; x++) {
            if (x < SCR_WIDTH && y < SCR_HEIGHT) {
                flipped_set |= gfx[gfx_base + y * SCR_WIDTH + x] & (pixel_row >> (7 - x)) & 0x01;
                gfx[gfx_base + y * SCR_WIDTH + x] ^= (pixel_row >> (7 - x)) & 0x01;
            }
        }
    }

    return flipped_set;
}

int chip8_cycle(void) {
    if (pc >= (MEM_SIZE - 1))
        return -1;

    // Fetch instruction
    opcode = memory[pc] << 8 | memory[pc + 1];
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    // Process instruction
    switch(opcode & 0xF000) {
        case 0x0000:
            switch(opcode & 0x0FFF) {
                case 0x0E0:
                    // 00E0: Clear the screen
                    memset(gfx, 0, sizeof(uint8_t) * GFX_SIZE);
                    chip8_draw_flag = true;
                    break;
                case 0x0EE:
                    // 00EE: Return from a subroutine
                    if (sp > 0 && sp <= STACK_SIZE)
                        pc = stack[--sp];
                    else
                        return -1;
                    break;
                default:
                    // 0NNN: Call machine code routine at address NNN
                    // Modern interpreters ignore this instruction
                    return -1;
            }
            break;
        case 0x1000:
            // 1NNN: Jump to address NNN
            pc = opcode & 0x0FFF;
            break;
        case 0x2000:
            // 2NNN: Call subroutine at address NNN
            if (sp < STACK_SIZE)
                stack[sp++] = pc + 2;
            else
                return -1;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000:
            // 3XNN: Skip the next instruction if VX equals NN
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;
        case 0x4000:
            // 4XNN: Skip the next instruction if VX does not equal NN
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;
        case 0x5000:
            switch (opcode & 0x000F) {
                case 0:
                    // 5XY0: Skip the next instruction if VX equals VY
                    if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
                        pc += 4;
                    else
                        pc += 2;
                    break;
                default:
                    return -1;
            }
            break;
        case 0x6000:
            // 6XNN: Set VX to NN
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;
        case 0x7000:
            // 7XNN: Add NN to VX, and store the result in VX (carry flag is not changed)
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;
        case 0x8000:
            switch(opcode & 0x000F) {
                case 0x0:
                    // 8XY0: Set VX to the value of VY
                    V[x] = V[y];
                    break;
                case 0x1:
                    // 8XY1: Set VX to VX OR VY
                    V[x] |= V[y];
                    break;
                case 0x2:
                    // 8XY2: Set VX to VX AND VY
                    V[x] &= V[y];
                    break;
                case 0x3:
                    // 8XY3: Set VX to VX XOR VY
                    V[x] ^= V[y];
                    break;
                case 0x4:
                    // 8XY4: Add VY to VX, and store the result in VX (on carry -> VF = 1, else -> VF = 0)
                    V[0xF] = ((int) V[x] + (int) V[y]) > 255 ? 1 : 0;
                    V[x] += V[y];
                    break;
                case 0x5:
                    // 8XY5: Substract VY from VX, and store the result in VX (on borrow -> VF = 0, else -> VF = 1)
                    V[0xF] = (V[x] > V[y]) ? 1 : 0;
                    V[x] -= V[y];
                    break;
                case 0x6:
                    // 8XY6: Store the LSB of VX in VF and then shift VX to the right by 1
                    V[0xF] = V[x] & 0x0001;
                    V[x] >>= 1;
                    break;
                case 0x7:
                    // 8XY7: Substract VX from VY, and store the result in VX ( on borrow -> VF = 0, else -> VF = 1)
                    V[0xF] = (V[y] > V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                    break;
                case 0xE:
                    // 8XYE: Store the MSB of VX in VF and then shift VX to the left by 1
                    V[0xF] = (V[x] & 0x8000) >> 15;
                    V[x] <<= 1;
                    break;
                default:
                    return -1;
            }
            pc += 2;
            break;
        case 0x9000:
            switch (opcode & 0x000F) {
                case 0x0:
                    // 9XY0: Skip the next instruction if VX does not equal VY
                    if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
                        pc += 4;
                    else
                        pc += 2;
                    break;
                default:
                    return -1;
            }
            break;
        case 0xA000:
            // ANNN: Set I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000:
            // BNNN: Jump to the address NNN plus V0
            pc = (opcode & 0x0FFF) + V[0x0];
            break;
        case 0xC000:
            // CXNN: Set VX to the result of a bitwise AND on a random number (0 to 255) and NN
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x0FF) & (rand() % 256);
            pc += 2;
            break;
        case 0xD000:
            // DXYN: Draw a sprite at coordinate (VX, VY) that is 8 pixels wide and N pixels high
            // Each row of 8 pixels is read as bit-coded starting from memory location I
            // VF is set to 1 if any of the pixels are flipped from set to unset; otherwise it is set to 0
            V[0xF] = draw_sprite(
                        V[((opcode & 0x0F00) >> 8)] % SCR_WIDTH,
                        V[((opcode & 0x00F0) >> 4)] % SCR_HEIGHT,
                        opcode & 0x000F
                    );
            chip8_draw_flag = true;
            pc += 2;
            break;
        case 0xE000:
            uint8_t key_index = V[x];
            switch (opcode & 0x00FF) {
                case 0x9E:
                    // EX9E: Skip the next instruction if the key stored in VX is pressed
                    if (key_index < NUM_KEYS && key[key_index])
                        pc += 4;
                    else
                        pc += 2;
                    break;
                case 0xA1:
                    // EXA1: Skip the next instruction if the key stored in VX is not pressed
                    if (key_index < NUM_KEYS && !key[key_index])
                        pc += 4;
                    else
                        pc += 2;
                    break;
                default:
                    return -1;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x07:
                    // FX07: Set VX to the value of the delay timer
                    V[x] = delay_timer; 
                    break;
                case 0x0A:
                    // FX0A: Await a key press, and store it on VX (blocking operation)
                    // Instruction halted until next key event
                    bool waiting = true;
                    while (waiting) {
                        for (uint8_t i = 0; i < NUM_KEYS; i++) {
                            if (key[i]) {
                                V[x] = i;
                                waiting = false;
                                break;
                            }
                        }

                    }
                    break;
                case 0x15:
                    // FX15: Set the delay timer to VX
                    delay_timer = V[x];
                    break;
                case 0x18:
                    // FX18: Set the sound timer to VX
                    sound_timer = V[x];
                    break;
                case 0x1E:
                    // FX1E: Add VX to I. VF is not affected
                    I += V[x];
                    break;
                case 0x29:
                    // FX29: Set I to the location of the sprite for the character in VX
                    // Characters 0-F (in hexadecimal) are represented by a 4x5 font
                    I = ((opcode & 0x0F00) >> 8) * 5;
                    break;
                case 0x33:
                    // FX33: Store the BCD representation of VX at I, with MSD at address I and LSD at address I+2
                    memory[I]   = V[x] / 100;
                    memory[I+1] = (V[x] % 100) / 10;
                    memory[I+2] = (V[x] % 10);
                    break;
                case 0x55:
                    // FX55: Store V0 to VX (both included) in memory starting at address I
                    for (int i = 0; i <= x; i++) {
                        memory[I+i] = V[i];
                    }
                    break;
                case 0x65:
                    // FX65: Fill V0 to VX (both included) with values from memory starting at address I
                    for (int i = 0; i <= x; i++) {
                        V[i] = memory[I+i];
                    }
                    break;
                default:
                    return -1;
            }
            pc += 2;
            break;
    }
    return 0;
}

void chip8_tick(void) {
    if (delay_timer > 0)
        delay_timer--;

    if (sound_timer > 0)
        sound_timer--;
}
