#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>


#define DELAY 125000
#define TRUE 1
#define FALSE 0

int vertical_flag = 0;
int horizontal_flag = 1;
int directionX = 1;
int directionY = 1;

// Global value for snake length
int length = 0;



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
	short snake_head_color = 1;  //Identifier for snake color
	short snake_body_color = 2;
	int segment = 0;
	int new_tailX = 0;
	int new_tailY = 0;


	// Define the Snake
	snake_segment snake[101];

	// Initialize the head of the snake
	snake[0].x = 1;
	snake[0].y = 1;
	snake[0].exists = 1;

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
	init_pair(snake_body_color, COLOR_MAGENTA, COLOR_MAGENTA);
	// Define food color
	init_pair(food_color, COLOR_BLUE, COLOR_BLUE);



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

		// Print snake position coordinates display to buffer
		mvprintw(max_y - 1, max_x - 6, "%d, %d", snake[0].x, snake[0].y);
		// Print the current score
		mvprintw(max_y - 1, 0, "SCORE: %d", length);

		//Print food position to buffer
		attron(COLOR_PAIR(food_color));
		mvprintw(foodY, foodX, " ");
		attroff(COLOR_PAIR(food_color));

		//Update the screen with the new material
		refresh();

		//Pause briefly
		usleep(DELAY);

		/* Setup snake's next position */

		// This is where the new segment (end of tail) will be placed if a food is consumed
		new_tailX = snake[length].x;
		new_tailY = snake[length].y;

		for(segment = length; segment > 0; segment--){
			snake[segment].x = snake[segment - 1].x;
			snake[segment].y = snake[segment - 1].y;
		}



		//Snake is moving horizontally
		if(horizontal_flag){

			snake_next_x = snake[0].x + directionX;

			if (snake_next_x >= (max_x - 1) || snake_next_x < 1) {
				directionX *= -1;
			}

			snake[0].x += directionX;
		}

		//Snake moving vertically
		else{

			snake_next_y = snake[0].y + directionY;

			if (snake_next_y >= (max_y - 1) || snake_next_y < 1) {
				directionY *= -1;
			}

			snake[0].y += directionY;
		}


		//If the snake eats the food, make him grow!
		if((snake[0].x == foodX) && (snake[0].y == foodY)){
			food_gone = TRUE;

			length++;

			snake[length].x = new_tailX;
			snake[length].y = new_tailY;
			snake[length].exists = 1;
			snake[length + 1].exists = 0;

		}


	}


	endwin();


	return 0;
}
