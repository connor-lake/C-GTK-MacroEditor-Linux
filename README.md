# C-GTK-MacroEditor-Linux

## Usage
- Select your event device from the dropdown
- Press create a macro
- Press the button you would like to be simulated
- Enter your delay in ms in the terminal

## To Added
- Editting of Macros, presets, etc. (Currently, the button does nothing.)
- Overall, more features

## How to build
```
gcc -I/usr/include/libevdev-1.0/ -Wall -Wextra -levdev `pkg-config --cflags gtk4` macros.c -o macros `pkg-config --libs gtk4
```
`./macros`
