#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#define MAX_X 30
#define MAX_Y 15

typedef struct tail_t
{
    int x;
    int y;
} tail_t;

typedef struct snake_t {
    int x;
    int y;
    tail_t *tail;
    size_t tsize;
} snake_t;

typedef enum { UP, DOWN, LEFT, RIGHT, EXIT } direction;

// @***
snake_t initSnake(int x, int y, size_t tsize) {
    snake_t snake;
    snake.x = x;
    snake.y = y;
    snake.tsize = tsize;
    snake.tail = (tail_t*)malloc(sizeof(tail_t) * 100); // Grows up to 100 elements
    for(size_t i = 0; i < tsize; i++) {
        snake.tail[i].x = x + i + 1;
        snake.tail[i].y = y;
    }
    return snake;
}

void printSnake(snake_t snake) {
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

    // Print the matrix
    for(int i = 0; i < MAX_Y; i++) {
        for(int j = 0; j < MAX_X; j++) {
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }
}

// <-- @**
snake_t moveLeft(snake_t snake) {
    for(int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i].x = snake.tail[i-1].x;
        snake.tail[i].y = snake.tail[i-1].y;
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;
    snake.x -= 1;
    if(snake.x < 0) {
        snake.x = MAX_X - 1;
    }

    return snake;
}

snake_t moveRight(snake_t snake) {
    for(int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i].x = snake.tail[i-1].x;
        snake.tail[i].y = snake.tail[i-1].y;
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;
    snake.x += 1;
    
    if(snake.x >= MAX_X) {
        snake.x = 0;
    }

    return snake;
}

snake_t moveUp(snake_t snake) {
    for(int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i].x = snake.tail[i-1].x;
        snake.tail[i].y = snake.tail[i-1].y;
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;
    snake.y -= 1;
    
    if(snake.y < 0) {
        snake.y = MAX_Y - 1;
    }

    return snake;
}

snake_t moveDown(snake_t snake) {
    for(int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i].x = snake.tail[i-1].x;
        snake.tail[i].y = snake.tail[i-1].y;
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;
    snake.y += 1;
    
    if(snake.y >= MAX_Y) {
        snake.y = 0;
    }

    return snake;
}

direction get_direction(direction current_direction) {
    direction direct;
    char key = getch();
    if( (key == 'a' || key == 'A') && current_direction != RIGHT) {
            direct = LEFT;
        } else if( (key == 'd' || key == 'D') && current_direction != LEFT) {
            direct = RIGHT;
        } else if( (key == 'w' || key == 'W') && current_direction != DOWN) {
            direct = UP;
        } else if( (key == 's' || key == 'S') && current_direction != UP) {
            direct = DOWN;
        } else if(key == 'c' || key == 'C') {
            direct = EXIT;
        } else {
            direct = current_direction;
        }
    return direct;
}

// The clearScreen function uses ANSI escape codes to clear the screen, which is more portable.
// \033[H means move the cursor to the home position
// \033[J means clear the screen from the cursor's current position to the end
void clearScreen() {
    printf("\033[H\033[J");
}

int main(void) {
    snake_t snake = initSnake(10, 5, 4);
    printSnake(snake);

    int flag = 1;
    direction current_direction = EXIT;
    direction new_direction = get_direction(current_direction);
    while(flag) {
        if (kbhit()) { // Check if a key is pressed
            current_direction = new_direction;
            new_direction = get_direction(current_direction);
        }

        switch (new_direction)
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
            flag = 0;
            break;
        default:
            break;
        }

        Sleep(100);
        clearScreen();
        printSnake(snake);
    }

    free(snake.tail);
    return 0;
}
