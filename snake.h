#ifndef SNAKE_H
#define SNAKE_H

#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

// Constants
#define DELAY 70000
#define END_GAME_PAUSE 5000000
#define TRUE 1
#define FALSE 0
#define MAX_LEN 101
#define WINNING_SCORE 30

// Globals
extern int length;
extern int vertical_flag;
extern int horizontal_flag;
extern int directionX;
extern int directionY;
extern int tailCollision;
extern int wallCollision;
extern short snake_head_color;  // Color identifiers
extern short snake_body_color;
extern short food_color;
extern short end_color;
extern int max_y, max_x;
extern int foodX;
extern int foodY;
extern int food_gone;
extern int snake_next_x;
extern int snake_next_y;
extern int snake_prev_head_x;
extern int snake_prev_head_y;
extern int segment; // 0 designates the head, 1 designates the first tail segment, and so on so forth
extern int new_tailX;
extern int new_tailY;

// Entity to represent a snake segment
typedef struct snake_segment {
   int x;
   int y;
   short exists;
}snake_segment;

extern snake_segment snake[MAX_LEN];

/* Function Prototypes */
void init();
void *key_monitor(void *arg); // this is a thread
void endGameMessage(int y, int x, char* message);
void drawBorder(int dimY, int dimX);
void initNcurses();
void initColorScheme();
void clearScreen();
void recordGameBoundaries(int* y, int* x);
void printGameBorderAndStats(int max_y, int max_x, snake_segment* snake);
void handleFood(int* max_x,int* max_y, int* foodX, int* foodY, int* food_gone);
void printFood(int foodY, int foodX, int food_color);
void updateSnakeSegments(snake_segment* snake, int snake_prev_head_x, int snake_prev_head_y);
void updateSnakeLength(snake_segment* snake, int foodX, int foodY, int* food_gone, int new_tailX, int new_tailY);
void updateSnakeDirection(snake_segment* snake, int max_x, int max_y, int snake_next_x, int snake_next_y, int* wallCollision);
int hasTailCollision(snake_segment* snake);
void initSnake(snake_segment* snake);
void printSnakeHead(int* segment, snake_segment* snake);
void printSnakeBody(int* segment, snake_segment* snake);


#endif
