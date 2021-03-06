#include "snake.h"

int length = 0; // Length of the snake tail (does not include the head)
int vertical_flag = 0;
int horizontal_flag = 1;
int directionX = 1; // 1 = right, -1 = left
int directionY = 1; // 1 = up, -1 = down
int tailCollision = FALSE;
int wallCollision = FALSE;
short snake_head_color = 1;  // Color identifiers
short snake_body_color = 2;
short food_color = 3;
short end_color = 4;
int max_y = 0, max_x = 0; // Boundaries of game

// Food info
int foodX = 0;
int foodY = 0;
int food_gone = TRUE;

// Snake info
int snake_next_x = 0;
int snake_next_y = 0;
int snake_prev_head_x = 0;
int snake_prev_head_y = 0;
int segment = 0; // 0 designates the head, 1 designates the first tail segment, and so on so forth
int new_tailX = 0;
int new_tailY = 0;

// Define the Snake
snake_segment snake[MAX_LEN];

pthread_t pid;

// Initializations for the program
void init()
{
	// Configure ncurses
	initNcurses();

	// Create the key monitor thread
	pthread_create(&pid, NULL, key_monitor, NULL);

	initSnake(snake);

	// Seed the random number genertor (used to randomly place the food)
	srand(time(NULL));
}

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

// Update the direction of the snake
void updateSnakeDirection(snake_segment* snake, int max_x, int max_y, int snake_next_x, int snake_next_y, int* wallCollision)
{
	//Snake is moving horizontally
	if(horizontal_flag){

		snake_next_x = snake[0].x + directionX;

		// If snake runs into the left or right boundary with a tail, then game over
		if ((length != 0) && (snake_next_x >= (max_x - 1) || snake_next_x < 1))
		{
			*wallCollision = TRUE;
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
			*wallCollision = TRUE;
		}

		if (snake_next_y >= (max_y - 1) || snake_next_y < 1) {
			directionY *= -1;
		}

		snake[0].y += directionY;

	}
}

//If the snake eats the food, make him grow!
void updateSnakeLength(snake_segment* snake, int foodX, int foodY, int* food_gone, int new_tailX, int new_tailY)
{
	if((snake[0].x == foodX) && (snake[0].y == foodY)){
		*food_gone = TRUE;

		length++;

		snake[length].x = new_tailX;
		snake[length].y = new_tailY;
		snake[length].exists = TRUE;

	}
}


// Update the segments of the snake
void updateSnakeSegments(snake_segment* snake, int snake_prev_head_x, int snake_prev_head_y)
{
	int index = 0;

	for(index = length; index > 0; index--){
		if(index == 1)
		{
			// We need to reference the previous position of the snake's head here because index 0 has already been changed
			snake[index].x = snake_prev_head_x;
			snake[index].y = snake_prev_head_y;
		}
		else
		{
			snake[index].x = snake[index - 1].x;
			snake[index].y = snake[index - 1].y;
		}
	}
}



//Calculate new food coordinates if da food is gone
void handleFood(int* max_x,int* max_y, int* foodX, int* foodY, int* food_gone)
{
	if(*food_gone)
	{
		*foodX = rand() % (*max_x - 2) + 1;
		*foodY = rand() % (*max_y - 2) + 1;
		*food_gone = FALSE;
	}
}

// Record the game boundaries - this allows for the dynamic resizing of the game view screen
void recordGameBoundaries(int* y, int* x)
{
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);

	*y = max_y;
	*x = max_x;
}

// Clear the view screen
void clearScreen()
{
	clear();
}

// Print the game border, the current score, and the coordinates of the snake head
void printGameBorderAndStats(int max_y, int max_x, snake_segment* snake)
{
	// Draw the perimeter
	drawBorder(max_y, max_x);
	// Print snake position coordinates display to buffer
	mvprintw(max_y - 1, max_x - 13, "( %d, %d )", snake[0].x, snake[0].y);
	// Print the current score
	mvprintw(max_y - 1, 2, "[ SCORE: %d ]", length);
}

//Print food position to buffer
void printFood(int foodY, int foodX, int food_color)
{
	attron(COLOR_PAIR(food_color));
	mvprintw(foodY, foodX, " ");
	attroff(COLOR_PAIR(food_color));
}

// Print snake's body position to buffer
void printSnakeBody(int* segment, snake_segment* snake)
{
	attron(COLOR_PAIR(snake_body_color));
	while(snake[*segment].exists){
		mvprintw(snake[*segment].y, snake[*segment].x, " ");
		*segment += 1;
	}
	attroff(COLOR_PAIR(snake_body_color));
}

// Print snake's head position to buffer
void printSnakeHead(int* segment, snake_segment* snake)
{
	attron(COLOR_PAIR(snake_head_color));
	*segment = 1;
	mvprintw(snake[0].y, snake[0].x, "0");
	attroff(COLOR_PAIR(snake_head_color));
}

// Configure the colors to be used in the game
void initColorScheme()
{
	// Enable coloring
	has_colors();
	start_color();

	// Define snake head color
	init_pair(snake_head_color, COLOR_RED, COLOR_RED);
	// Define snake body color
	init_pair(snake_body_color, COLOR_GREEN, COLOR_GREEN);
	// Define food color
	init_pair(food_color, COLOR_BLUE, COLOR_BLUE);
	// Define end color
	init_pair(end_color, COLOR_YELLOW, COLOR_BLACK);
}

// Initialize the snake
void initSnake(snake_segment* snake)
{
	int index = 0;
	for(index = 0; index < MAX_LEN; index++)
		snake[index].exists = FALSE;

	// Initialize the head of the snake
	snake[0].x = 1;
	snake[0].y = 1;
	snake[0].exists = TRUE;

	// The next snake segment does not exist yet
	snake[1].exists = 0;
}

// Initialize ncurses
void initNcurses()
{
	// ncurses setup
	initscr();
	noecho();
	curs_set(FALSE);
	cbreak();
	keypad(stdscr, TRUE);

	initColorScheme();
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

// Draw perimeter around the screen
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
