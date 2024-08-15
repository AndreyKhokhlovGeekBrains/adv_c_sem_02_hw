#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <math.h>

#define MAX_X 30
#define MAX_Y 15

// ANSI escape codes for text colors
#define RESET_COLOR "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"
#define YELLOW "\x1B[33m"

typedef enum { UP, DOWN, LEFT, RIGHT } direction;
typedef enum { SINGLE_PLAYER, TWO_PLAYERS } game_mode;

typedef struct {
    game_mode gameMode;
    char* snake1Color;
    char* snake2Color;
} settings;

char* getColorCode(int choice) {
    switch (choice) {
        case 1: return RED;
        case 2: return GREEN;
        case 3: return BLUE;
        case 4: return YELLOW;
        default: return RESET_COLOR;
    }
}

void resetColor() {
    printf("\033[0m");
}

void startMenu(settings *settings) {
    int choice;
    printf("********************************\n");
    printf("*      Welcome to Snake Game   *\n");
    printf("********************************\n");
    
    printf("\nSelect Game Mode:\n");
    printf("1. Single Player\n");
    printf("2. Two Players\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    
    if (choice == 1) {
        settings->gameMode = SINGLE_PLAYER;
    } else {
        settings->gameMode = TWO_PLAYERS;
    }

    printf("\nSelect Snake 1 Color:\n");
    printf("1. Red\n");
    printf("2. Green\n");
    printf("3. Blue\n");
    printf("4. Yellow\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    
    settings->snake1Color = getColorCode(choice);

    if (settings->gameMode == TWO_PLAYERS) {
        printf("\nSelect Snake 2 Color:\n");
        printf("1. Red\n");
        printf("2. Green\n");
        printf("3. Blue\n");
        printf("4. Yellow\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        settings->snake2Color = getColorCode(choice);
    }

    printf("\nSettings confirmed. Press any key to start the game...\n");
    getch();
}

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
    snake.speed = 200;
    snake.tsize = tsize;
    
    snake.tail = (tail_t*)malloc(sizeof(tail_t) * 100); // Grows up to 100 elements
    for(size_t i = 0; i < tsize; i++) {
        snake.tail[i].x = x + i + 1;
        snake.tail[i].y = y;
    }
    snake.direction = LEFT;

    return snake;
}

void printSnake(snake_t *snake, snake_t *snake2, food_container_t food_container, settings settings) {
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

    matrix[snake->y][snake->x] = '@';
    for(size_t i = 0; i < snake->tsize; i++) {
        matrix[snake->tail[i].y][snake->tail[i].x] = '*';
    }
    
    if (snake2 != NULL) {
        matrix[snake2->y][snake2->x] = '#';
        for(size_t i = 0; i < snake2->tsize; i++) {
        matrix[snake2->tail[i].y][snake2->tail[i].x] = 'o';
        }
    }

    // Place the snake's food
    for(size_t i = 0; i < food_container.fsize; i++) {
        matrix[food_container.food_arr[i].y][food_container.food_arr[i].x] = '$';
    }

    // Print the matrix with colors
    for(int i = 0; i < MAX_Y; i++) {
        for(int j = 0; j < MAX_X; j++) {
            if (matrix[i][j] == '@' || matrix[i][j] == '*') {
                printf("%s%c", settings.snake1Color, matrix[i][j]);
            } else if (matrix[i][j] == '#' || matrix[i][j] == 'o') {
                printf("%s%c", settings.snake2Color, matrix[i][j]);
            } else {
                printf("%s%c", RESET_COLOR, matrix[i][j]);
            }
        }
        printf("\n");
    }
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

_Bool check_head_position(snake_t *snake) {
    for(size_t i = 0; i < snake->tsize; i++) {
        if(snake->x == snake->tail[i].x && snake->y == snake->tail[i].y) {
            return TRUE;
        }
    }
    return FALSE;
}

void printExit(snake_t *snake, snake_t *snake2) {
    clearScreen();
    printf("Game over!\n");
    printf("You reached level: %d!\n", snake->level);

    if (snake2 != NULL) {
        printf("Snake 2 reached level: %d", snake2->level);
    }
    
    printf("\n");
    exit(0);
}

// <-- @**
void moveLeft(snake_t *snake) {
    snake->x -= 1;
    if(snake->x < 0) {
        snake->x = MAX_X - 1;
    }
}

void moveRight(snake_t *snake) {
    snake->x += 1;
    if(snake->x >= MAX_X) {
        snake->x = 0;
    }
}

void moveUp(snake_t *snake) {
    snake->y -= 1;
    if(snake->y < 0) {
        snake->y = MAX_Y - 1;
    }
}

void moveDown(snake_t *snake) {
    snake->y += 1;
    if(snake->y >= MAX_Y) {
        snake->y = 0;
    }
}

void move(snake_t *snake) {
    for(int i = snake->tsize - 1; i > 0; i--) {
        snake->tail[i].x = snake->tail[i-1].x;
        snake->tail[i].y = snake->tail[i-1].y;
    }
    snake->tail[0].x = snake->x;
    snake->tail[0].y = snake->y;

    switch (snake->direction)
        {
        case LEFT:
            moveLeft(snake);
            break;
        case RIGHT:
            moveRight(snake);
            break;
        case UP:
            moveUp(snake);
            break;
        case DOWN:
            moveDown(snake);
            break;
        default:
            break;
        }
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

void get_direction(snake_t *snake, snake_t *snake2) {
    char key = getch();
    if( (key == 'a' || key == 'A') && snake->direction != RIGHT) {
            snake->direction = LEFT;
        } else if( (key == 'd' || key == 'D') && snake->direction != LEFT) {
            snake->direction = RIGHT;
        } else if( (key == 'w' || key == 'W') && snake->direction != DOWN) {
            snake->direction = UP;
        } else if( (key == 's' || key == 'S') && snake->direction != UP) {
            snake->direction = DOWN;
        } else if(key == 'c' || key == 'C') {
            printExit(snake, snake2);
        } else if(key == 'p' || key == 'P') {
            pause_game();
    }
}

void generateSnakeDirection(snake_t *snake2, food_container_t food_container) {
    if (snake2 == NULL) {
        return;
    }
    food_t nearest_food_item = food_container.food_arr[0];
    // Euclidean distance between the snake's head and each food item: d = sqr( (x2 - x1)^2 + (y2 - y1)^2)
    // To make the comparison faster we will avoid computing the square root
    int min_distance = (snake2->x - nearest_food_item.x) * (snake2->x - nearest_food_item.x) + 
                       (snake2->y - nearest_food_item.y) * (snake2->y - nearest_food_item.y);
    for(size_t i = 0; i < food_container.fsize; i++) {
        int distance = (snake2->x - food_container.food_arr[i].x) * (snake2->x - food_container.food_arr[i].x) + 
                       (snake2->y - food_container.food_arr[i].y) * (snake2->y - food_container.food_arr[i].y);
        if( distance < min_distance) {
            nearest_food_item = food_container.food_arr[i];
            min_distance = distance;
        }
    }

    int x_diff = snake2->x - nearest_food_item.x; // negative = move right
    int y_diff = snake2->y - nearest_food_item.y; // negative = move down
    
    if(x_diff < 0 && snake2->direction == LEFT) { // case where snake needs to turn right and the current direction is LEFT
        if(y_diff < 0) { // decide where to move better: up or down
            snake2->direction = DOWN;
        } else if (y_diff > 0) {
            snake2->direction = UP;
        }
    } else if (x_diff > 0 && snake2->direction == RIGHT) { // case where snake needs to turn left and the current direction is RIGHT
        if(y_diff < 0) { // decide where to move better: up or down
            snake2->direction = DOWN;
        } else if (y_diff > 0) {
            snake2->direction = UP;
        }
    } else if (x_diff < 0) { // the case where the snake needs to turn right and the current direction is not left
        snake2->direction = RIGHT;
    } else if (x_diff > 0) { // the case where the snake needs to turn left and the current direction is not right
        snake2->direction = LEFT;
    } else if (x_diff == 0 && y_diff < 0) { // the case where the snake needs to turn up
        if(snake2->direction == UP) {
            snake2->direction = LEFT;
        } else {
            snake2->direction = DOWN;
        }
    } else if (x_diff == 0 && y_diff > 0) { // the case where the snake needs to turn down
        if(snake2->direction == DOWN) {
            snake2->direction = RIGHT;
        } else {
            snake2->direction = UP;
        }
    }
}

int main(void) {
    settings settings;
    startMenu(&settings);

    // setColor(settings.snake1Color);
    printf("Snake 1 color selected.\n");

    snake_t *snake;
    snake = (snake_t*)malloc(sizeof(snake_t));
    *snake = initSnake(10, 5, 3);
    snake_t *snake2 = NULL;

    if (settings.gameMode == TWO_PLAYERS) {
        // setColor(settings.snake2Color);
        printf("Snake 2 color selected.\n");

        snake2 = (snake_t*)malloc(sizeof(snake_t));
        *snake2 = initSnake(20, 10, 3);
    }
    
    food_container_t food_container = initFood(1);

    while(1) {
        if (kbhit()) { // Check if a key is pressed
            get_direction(snake, snake2);
        }
        
        if(snake2 != NULL && !check_head_position(snake2)) {
            generateSnakeDirection(snake2, food_container);
            move(snake2);
            eat_food(snake2, &food_container);
        }

        if (check_head_position(snake)) {
            printExit(snake, snake2);
        }
        move(snake);
        eat_food(snake, &food_container);
        Sleep(snake->speed);
        clearScreen();
        printSnake(snake, snake2, food_container, settings);
    }

    resetColor();
    free(food_container.food_arr);
    free(snake->tail);
    if (snake2 != NULL) {
        free(snake2->tail);
        free(snake2);
    }
    free(snake);
    return 0;
}

// Optimisation commands:
// gcc -O2 -o snake_game snake_game.c
// .\snake_game
// objdump -d snake_game.exe 1>61.asm