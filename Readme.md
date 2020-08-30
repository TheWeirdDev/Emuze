# Emuze
My hobby project to learn about emulators.

Emuze currently only supports Chip8 games.

![Emuze Screenrecord](screenrecord/record.gif)

# Run
You need a C++20 compiler (GCC g++ 10.1.0 recommended)

Install `SFML`, `spdlog`, `fmt` and `pthread` libraries on your system before compiling.

```bash
mkdir build && cd build
cmake ..
make
./Emuze ../games/chip8_roms/INVADERS
```

# Key layout

Chip8 has 16 keys, from 0 to F. Keys are mapped to the following keys on your keyboard:

|     1 (1)     |     2 (2)     |     3 (3)     |     C (4)     | 
| ------------- | ------------- | ------------- | ------------- |
|     4 (Q)     |     5 (W)     |     6 (E)     |     D (R)     |
|     7 (A)     |     8 (S)     |     9 (D)     |     E (F)     |
|     A (Z)     |     0 (X)     |     B (C)     |     F (V)     |

# License
Emuze is free software and is licensed under GPL-3.0 or newer, for more information see [LICENSE](LICENSE).
