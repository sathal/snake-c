#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>


#define DELAY 70000
#define END_GAME_PAUSE 5000000
#define TRUE 1
#define FALSE 0
#define MAX_LEN 101
#define WINNING_SCORE 5

int vertical_flag = 0;
int horizontal_flag = 1;
int directionX = 1;
int directionY = 1;
int tailCollision = FALSE;

// Global value for snake length
int length = 0;

short end_color = 4;

/* This thread monitors key presses */
void *key_monitor(void *arg) {

	while(1){
		switch(getch()){

			case KEY_UP :
				if(vertical_flag && length != 0) // Prevent snake from doubling back on itself
					break;
				directionY = -1;
				vertical_flag = 1;
				horizontal_flag = 0;
				break;
			case KEY_DOWN :
				if(vertical_flag && length != 0)
					break;
				directionY = 1;
				vertical_flag = 1;
				horizontal_flag = 0;
				break;
			case KEY_LEFT :
				if(horizontal_flag && length != 0)
					break;
				directionX = -1;
				vertical_flag = 0;
				horizontal_flag = 1;
				break;
			case KEY_RIGHT :
				if(horizontal_flag  && length != 0)
					break;
				directionX = 1;
				vertical_flag = 0;
				horizontal_flag = 1;
				break;
			default :
				break;
		}
	}
}

// Entity to represent a snake segment
typedef struct snake_segment {
   int x;
   int y;
   short exists;
}snake_segment;

/* Function Prototypes */
void endGameMessage(int y, int x, char* message);
int hasTailCollision(snake_segment* snake);
void drawBorder(int dimY, int dimX);

int main(void){

	//Setup
	initscr();
	noecho();
	curs_set(FALSE);
	cbreak();
	keypad(stdscr, TRUE);

	//Create the key monitor thread
	pthread_t pid;
	pthread_create(&pid, NULL, key_monitor, NULL);


	//Snake info
	int snake_next_x = 0;
	int snake_next_y = 0;
	int snake_prev_head_x = 0;
	int snake_prev_head_y = 0;
	short snake_head_color = 1;  //Identifier for snake color
	short snake_body_color = 2;
	int segment = 0;
	int new_tailX = 0;
	int new_tailY = 0;


	// Define the Snake
	snake_segment snake[MAX_LEN];
	int index = 0;
	for(index = 0; index < MAX_LEN; index++)
		snake[index].exists = FALSE;

	// Initialize the head of the snake
	snake[0].x = 1;
	snake[0].y = 1;
	snake[0].exists = TRUE;

	// The next snake segment does not exist yet
	snake[1].exists = 0;

	// Food info
	int foodX = 0;
	int foodY = 0;
	int food_gone = TRUE;
	short food_color = 3;

	// Boundaries of game
	int max_y = 0, max_x = 0;

	// Enable coloring
	has_colors();
	start_color();

	// Define snake color
	init_pair(snake_head_color, COLOR_RED, COLOR_RED);
	init_pair(snake_body_color, COLOR_GREEN, COLOR_GREEN);
	// Define food color
	init_pair(food_color, COLOR_BLUE, COLOR_BLUE);
	// Define end color
	init_pair(end_color, COLOR_YELLOW, COLOR_BLACK);



	//Game Loop
	while(1){

		//Seed the random number genertor
		srand(time(NULL));

		//Record game boundaries
		getmaxyx(stdscr, max_y, max_x);

		//Clear screen
		clear();

		//Print snake's head position to buffer
		attron(COLOR_PAIR(snake_head_color));
		segment = 1;
		mvprintw(snake[0].y, snake[0].x, "0");
		attroff(COLOR_PAIR(snake_head_color));

		//Print snake's body position to buffer
		attron(COLOR_PAIR(snake_body_color));
		while(snake[segment].exists){
			mvprintw(snake[segment].y, snake[segment].x, " ");
			segment++;
		}
		attroff(COLOR_PAIR(snake_body_color));

		//Calculate new food coordinates if da food is gone
		if(food_gone){

			// Take into account a 1-space boundary along all the sides of the screen
			foodX = rand() % (max_x - 2) + 1;
			foodY = rand() % (max_y - 2) + 1;

			//TODO: Make sure we don't place the food on the snake


			food_gone = FALSE;
		}


		// Draw the perimeter
		drawBorder(max_y, max_x);
		// Print snake position coordinates display to buffer
		mvprintw(max_y - 1, max_x - 13, "( %d, %d )", snake[0].x, snake[0].y);
		// Print the current score
		mvprintw(max_y - 1, 2, "[ SCORE: %d ]", length);

		//Print food position to buffer
		attron(COLOR_PAIR(food_color));
		mvprintw(foodY, foodX, " ");
		attroff(COLOR_PAIR(food_color));

		//Update the screen with the new material
		refresh();

		if(tailCollision)
		{
			endGameMessage(max_y - 1, (max_x / 2) - 5, " YOU LOSE! ");
			return 0;
		}

		// Pause briefly
		usleep(DELAY);

		// If the user has achieved the winning score then they have won!
		if(length == WINNING_SCORE)
		{
			endGameMessage(max_y - 1, (max_x / 2) - 5, " YOU WIN! ");
			return 0;
		}

		/* Setup snake's next position */

		// Save the previous position of the snake's head
		snake_prev_head_x = snake[0].x;
		snake_prev_head_y = snake[0].y;

		//Snake is moving horizontally
		if(horizontal_flag){

			snake_next_x = snake[0].x + directionX;

			// If snake runs into the left or right boundary with a tail, then game over
			if ((length != 0) && (snake_next_x >= (max_x - 1) || snake_next_x < 1))
			{
				endGameMessage(max_y - 1, (max_x / 2) - 5, " YOU LOSE! ");
				return 0;
			}

			if (snake_next_x >= (max_x - 1) || snake_next_x < 1) {
				directionX *= -1;
			}

			snake[0].x += directionX;
		}

		//Snake moving vertically
		else{

			snake_next_y = snake[0].y + directionY;

			// If snake runs into the top or bottom boundary with a tail, then game over
			if ((length != 0) && (snake_next_y >= (max_y - 1) || snake_next_y < 1))
			{
				endGameMessage(max_y - 1, (max_x / 2) - 5, " YOU LOSE! ");
				return 0;
			}

			if (snake_next_y >= (max_y - 1) || snake_next_y < 1) {
				directionY *= -1;
			}

			snake[0].y += directionY;

		}

		if(hasTailCollision(snake))
		{
			tailCollision = TRUE;
		}

		// This is where the new segment (end of tail) will be placed if a food is consumed
		new_tailX = snake[length].x;
		new_tailY = snake[length].y;

		// Update the segments of the snake
		for(segment = length; segment > 0; segment--){
			if(segment == 1)
			{
				// We need to reference the previous position of the snake's head here because index 0 has already been changed
				snake[segment].x = snake_prev_head_x;
				snake[segment].y = snake_prev_head_y;
			}
			else
			{
				snake[segment].x = snake[segment - 1].x;
				snake[segment].y = snake[segment - 1].y;
			}
		}


		//If the snake eats the food, make him grow!
		if((snake[0].x == foodX) && (snake[0].y == foodY)){
			food_gone = TRUE;

			length++;

			snake[length].x = new_tailX;
			snake[length].y = new_tailY;
			snake[length].exists = TRUE;

		}


	}


	endwin();


	return 0;
}

// Check to see if the head of the snake has collided with the tail
int hasTailCollision(snake_segment* snake)
{
	if(length < 4)
		return FALSE;

	int i;
	for(i = 3; i < length; i++)
	{
		if(((snake[0].x == snake[i].x) && (snake[0].y == snake[i].y)) && (snake[i+1].exists))
		{
			return TRUE;
		}
	}

	return FALSE;
}

void drawBorder(int dimY, int dimX)
{
	int i = 0;

	for(i = 0; i < dimX; i++)
	{
		mvprintw(0, i, "=");
		mvprintw(dimY - 1, i, "=");
	}

	for(i = 0; i < dimY; i++)
	{
		mvprintw(i, 0, "|");
		mvprintw(i, dimX - 1, "|");
	}
}

// Display end-of-game message
void endGameMessage(int y, int x, char* message)
{

	attron(COLOR_PAIR(end_color));
	mvprintw(y, x, message);
	attroff(COLOR_PAIR(end_color));
	refresh();
	usleep(END_GAME_PAUSE);
	endwin();
}
