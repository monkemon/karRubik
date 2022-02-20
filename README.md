# karRubik

This is a utility to visualise a rubiks cube in 2d.
You can perform moves on it by typing special move commands into terminal.

### Prerequisites
1. SDL2 installed
2. SDL2_image installed (*i believe you can get it via* `apt install libsdl2-image-dev`)
In my case `pacman -S sdl2_image`
3. make installed
4. cmake installed
5. C++ compiler (*in my case * `g++`)

### Compilation and building
Suppose you're in the main folder
1. `mkdir build`
2. `cd build`
3. `cmake ..`
4. `make .`

### Usage
- Run it by `./bin/karRubik-main`
- a prompt opens up
- you can type `help` to see the list of commands

### basic workflow
- `./bin/karRubik-main`
- `nc 3`
- turn on the visualisation by `gui`
- `nc 3` (to clear the graphics buffer so it doesn't look glitchy, i haven't fixed that and probably won't anytime soon)
- perform some moves like `R`, `1-2R'`, `F`, `2B` and so on...
- `sc` to save the cube
- `ls` to see move history (in formalised form)
- `goto x` to step back to xth step in history
- `cls` to close the gui
- `exit` to quit the program

### Notes
- **always turn off the gui before exitting**
- after exit there will be some memory leak warnings caused by sdl2
