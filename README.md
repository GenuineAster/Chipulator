Chipulator
==========

Chipulator is a Chip-8 emulator written in C++ by Mischa Aster Alff. It is currently in heavy development.
Things that remain TBI are:

* Sound


To build Chipulator, you'll need CMake and SFML2.
```bash
$ cmake . && make
```

To run a CHIP-8 ROM, such as `MAZE` just run
```bash
$ ./Chipulator run/roms/MAZE
```