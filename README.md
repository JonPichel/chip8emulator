# chip8emulator
Ncurses based Chip-8 emulator written in C. Features a debugging screen with full insight into memory, registers, stack and input status.

# Important

* Instruction 00E0 still not implemented.

# Installation and usage

```bash
# From the main directory
make

# Change directory to bin
cd bin

./main
```

Select a ROM and try it out. Instructions for each game are written on their corresponding .txt file.

# Input

* Numpad numbers from 0..9.
* A..F keys mapped to QWERTY keys.
