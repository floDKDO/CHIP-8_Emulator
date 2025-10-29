# CHIP-8-Emulator

CHIP-8 emulator written in C using SDL2 for graphics and SDL2 Mixer for sounds. Many thanks to [Tobias](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) for his excellent guide.

## Usage

Compile and execute the executable. You can optionally choose between *Legacy* (0, by default) and *Modern* (1) mode depending on the desired quirks : 

***./CHIP-8_Emulator [0=LEGACY,default ; 1=MODERN]***

## Tetris
![](https://github.com/floDKDO/CHIP-8_Emulator/blob/main/showcase/playing_tetris.gif)

## Timendus' tests

To test the accuracy of my emulator's behavior, I used various tests written by [Timendus](https://github.com/Timendus/chip8-test-suite?tab=readme-ov-file), ranging from graphics display to opcode and flag accuracy.

### CHIP-8 splash screen
<p align="center">
  <img src="https://github.com/floDKDO/CHIP-8_Emulator/blob/main/showcase/test1.png" alt="Test 1" width="500"/>
</p>

### IBM logo
<p align="center">
  <img src="https://github.com/floDKDO/CHIP-8_Emulator/blob/main/showcase/test2.png" alt="Test 2" width="500"/>
</p>

### Corax+ opcode test
<p align="center">
  <img src="https://github.com/floDKDO/CHIP-8_Emulator/blob/main/showcase/test3.png" alt="Test 3" width="500"/>
</p>

### Flags test
<p align="center">
  <img src="https://github.com/floDKDO/CHIP-8_Emulator/blob/main/showcase/test4.png" alt="Test 4" width="500"/>
</p>

### Quirks test
<p align="center">
  <img src="https://github.com/floDKDO/CHIP-8_Emulator/blob/main/showcase/test5.png" alt="Test 5" width="500"/>
</p>

### Keypad test

#### Key down
<p align="center">
  <img src="https://github.com/floDKDO/CHIP-8_Emulator/blob/main/showcase/test6_1.png" alt="Test 6_1" width="500"/>
</p>

The "4" and "F" keys are pressed.

#### Key up
<p align="center">
  <img src="https://github.com/floDKDO/CHIP-8_Emulator/blob/main/showcase/test6_2.png" alt="Test 6_2" width="500"/>
</p>

The "4" and "F" keys are pressed.

#### Get key
<p align="center">
  <img src="https://github.com/floDKDO/CHIP-8_Emulator/blob/main/showcase/test6_3.png" alt="Test 6_3" width="500"/>
</p>
