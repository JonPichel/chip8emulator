
#ifndef __CHIP8VM__
#define __CHIP8VM__

#define SCR_HEIGHT  32
#define SCR_WIDTH   64
#define NUM_KEYS    16
#define NUM_REGS    16

#define MEM_SIZE    0x1000
#define STACK_SIZE  0x10
#define GFX_SIZE    (SCR_HEIGHT * SCR_WIDTH)

#define START_ROM_ADDR  0x200
#define MAX_ROM_SIZE    (MEM_SIZE - START_ROM_ADDR)

void chip8_init(void);
int chip8_loadrom(char *pathname);
int chip8_cycle(void);
void chip8_tick(void);

#endif /* __CHIP8VM__ */
