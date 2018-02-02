#include "snake.h"

int main(void){

  init();

	// Game Loop
	while(1){

		recordGameBoundaries(&max_y, &max_x);
		handleFood(&max_x, &max_y, &foodX, &foodY, &food_gone);
		printSnakeHead(&segment, snake);
		printSnakeBody(&segment, snake);
		printGameBorderAndStats(max_y, max_x, snake);
		printFood(foodY, foodX, food_color);

		//Update the screen with new material
		refresh();

		// If there has been a collision between the head and the tail, then the user lost
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

		updateSnakeDirection(snake, max_x, max_y, snake_next_x, snake_next_y, &wallCollision);

		// Check to see if the head has collided with the tail
		if(hasTailCollision(snake))
		{
			tailCollision = TRUE;
		}

		// If the snake has collided with a wall, then the user has lost
		if(wallCollision)
		{
			endGameMessage(max_y - 1, (max_x / 2) - 5, " YOU LOSE! ");
			return 0;
		}

		// This is where the new segment (end of tail) will be placed if a food is consumed
		new_tailX = snake[length].x;
		new_tailY = snake[length].y;

		updateSnakeSegments(snake, snake_prev_head_x, snake_prev_head_y);
		updateSnakeLength(snake, foodX, foodY, &food_gone, new_tailX, new_tailY);

		clearScreen();

	}

}
