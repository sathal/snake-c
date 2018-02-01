## Summary

//todo

## Note on Compiling

In order to compile snake.c into an executable, run the following command in the same directory as snake.c:

    gcc snake.c -o snake -lncurses -pthread
  
Note that this program makes use of threading (which is why we use the -pthread flag) and the ncurses library (which is why we use the -lncurses flag)
