## Summary

This is my rendition of the classic Snake game written in C and it is intended to be run in the terminal.

## Dependencies

This code relies upon the ncurses library for rendering the game in the terminal. The ncurses library must be properly installed on your system in order to compile the program.

## Note on Compiling

In order to compile the files into an executable, run the following command in the same directory as the source code:

    gcc main.c snake.c -o snake -lncurses -pthread
  
Note that this code makes use of threading (which is why we use the -pthread flag) and the ncurses library (which is why we use the -lncurses flag)
