#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <math.h>

#define MAX_X 30
#define MAX_Y 15

typedef enum { UP, DOWN, LEFT, RIGHT, EXIT } direction;

// The clearScreen function uses ANSI escape codes to clear the screen, which is more portable.
// \033[H means move the cursor to the home position
// \033[J means clear the screen from the cursor's current position to the end
void clearScreen() {
    printf("\033[H\033[J");
}

typedef struct tail_t
{
    int x;
    int y;
} tail_t;

typedef struct food_t
{
    int x;
    int y;
} food_t;

typedef struct food_container_t {
    size_t fsize;
    food_t *food_arr;
} food_container_t;

food_container_t initFood(size_t fsize){
    food_container_t food_container;
    food_container.fsize = fsize;
    food_container.food_arr = (food_t*)malloc(sizeof(food_t) * 5); // Max food items cannot be greater than 5
    for(size_t i = 0; i < fsize; i++) {
        food_container.food_arr[i].x = rand() % MAX_X;
        food_container.food_arr[i].y = rand() % MAX_Y;
    }
    return food_container;
}

typedef struct snake_t {
    int x;
    int y;
    int level;
    int speed;
    tail_t *tail;
    size_t tsize;
    direction direction;
} snake_t;

// @***
snake_t initSnake(int x, int y, size_t tsize) {
    snake_t snake;
    snake.x = x;
    snake.y = y;
    snake.level = 0;
    snake.speed = 300;
    snake.tsize = tsize;
    
    snake.tail = (tail_t*)malloc(sizeof(tail_t) * 100); // Grows up to 100 elements
    for(size_t i = 0; i < tsize; i++) {
        snake.tail[i].x = x + i + 1;
        snake.tail[i].y = y;
    }
    snake.direction = LEFT;

    return snake;
}

void printSnake(snake_t snake, food_container_t food_container) {
    char matrix[MAX_Y][MAX_X];

    // Initialize the matrix
    for(int i = 0; i < MAX_Y; i++) {
        for(int j = 0; j < MAX_X; j++) {
            if(i == 0 || i == MAX_Y - 1) {
                matrix[i][j] = '-';
            } else if (j == 0 || j == MAX_X - 1) {
                matrix[i][j] = '|';
            } else {
                matrix[i][j] = ' ';
            }
            
        }
    }

    // Place the snake's head
    matrix[snake.y][snake.x] = '@';

    // Place the snake's tail
    for(size_t i = 0; i < snake.tsize; i++) {
        matrix[snake.tail[i].y][snake.tail[i].x] = '*';
    }

    // Place the snake's food
    for(size_t i = 0; i < food_container.fsize; i++) {
        matrix[food_container.food_arr[i].y][food_container.food_arr[i].x] = 'x';
    }

    // Print the matrix
    for(int i = 0; i < MAX_Y; i++) {
        for(int j = 0; j < MAX_X; j++) {
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }
}

snake_t general_move(snake_t snake) {
    for(int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i].x = snake.tail[i-1].x;
        snake.tail[i].y = snake.tail[i-1].y;
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;
    return snake;
}

void eat_food(snake_t *snake, food_container_t *food_container) {
    for(size_t i = 0; i < food_container->fsize; i++) {
        if(snake->x == food_container->food_arr[i].x && snake->y == food_container->food_arr[i].y) {
            snake->tsize++;
            snake->level++;
            snake->speed -= (int)(30 / log(snake->level + 1));
            food_container->food_arr[i].x = (rand() % (MAX_X - 2)) + 1;
            food_container->food_arr[i].y = (rand() % (MAX_Y - 2)) + 1;

            if(food_container->fsize < 5) {
                food_container->fsize++;
                // New food position
                food_container->food_arr[food_container->fsize - 1].x = (rand() % (MAX_X - 2)) + 1;
                food_container->food_arr[food_container->fsize - 1].y = (rand() % (MAX_Y - 2)) + 1;
            }
        }
    }
}

void check_head_position(snake_t *snake) {
    for(size_t i = 0; i < snake->tsize; i++) {
        if(snake->x == snake->tail[i].x && snake->y == snake->tail[i].y) {
            snake->direction = EXIT;
        }
    }
}

void printExit(snake_t snake) {
    clearScreen();
    printf("Game over! You reached level: %d!\n", snake.level);
    printf("\n");
    exit(0);
}

// <-- @**
snake_t moveLeft(snake_t snake) {
    snake = general_move(snake);
    snake.x -= 1;
    if(snake.x < 0) {
        snake.x = MAX_X - 1;
    }

    return snake;
}

snake_t moveRight(snake_t snake) {
    snake = general_move(snake);
    snake.x += 1;
    
    if(snake.x >= MAX_X) {
        snake.x = 0;
    }

    return snake;
}

snake_t moveUp(snake_t snake) {
    snake = general_move(snake);
    snake.y -= 1;
    
    if(snake.y < 0) {
        snake.y = MAX_Y - 1;
    }

    return snake;
}

snake_t moveDown(snake_t snake) {
    snake = general_move(snake);
    snake.y += 1;
    
    if(snake.y >= MAX_Y) {
        snake.y = 0;
    }

    return snake;
}

void pause_game() {
    while (1) {
        if (kbhit()) {
            char key = getch();
            if (key == 'p' || key == 'P') {
                break;
            }
        }
        Sleep(1000); // Sleep for a short time to prevent high CPU usage
    }
}

snake_t get_direction(snake_t snake) {
    char key = getch();
    if( (key == 'a' || key == 'A') && snake.direction != RIGHT) {
            snake.direction = LEFT;
        } else if( (key == 'd' || key == 'D') && snake.direction != LEFT) {
            snake.direction = RIGHT;
        } else if( (key == 'w' || key == 'W') && snake.direction != DOWN) {
            snake.direction = UP;
        } else if( (key == 's' || key == 'S') && snake.direction != UP) {
            snake.direction = DOWN;
        } else if(key == 'c' || key == 'C') {
            snake.direction = EXIT;
        } else if(key == 'p' || key == 'P') {
            pause_game();
    }
    return snake;
}

int main(void) {
    snake_t snake = initSnake(10, 5, 3);
    food_container_t food_container = initFood(1);
    printSnake(snake, food_container);

    int flag = 1;
    while(flag) {
        if (kbhit()) { // Check if a key is pressed
            snake = get_direction(snake);
        }

        switch (snake.direction)
        {
        case LEFT:
            snake = moveLeft(snake);
            break;
        case RIGHT:
            snake = moveRight(snake);
            break;
        case UP:
            snake = moveUp(snake);
            break;
        case DOWN:
            snake = moveDown(snake);
            break;
        case EXIT:
            printExit(snake);
            break;
        default:
            break;
        }

        check_head_position(&snake);
        eat_food(&snake, &food_container);
        Sleep(snake.speed);
        clearScreen();
        printSnake(snake, food_container);
    }

    free(food_container.food_arr);
    free(snake.tail);
    return 0;
}
