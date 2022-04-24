#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <stdbool.h>

#define SNAKE_CHAR '#'
#define INITIAL_SNAKE_LENGTH 1
#define SIDE_SIZE 40
#define STDIN 0     // INPUT FILE DESCRIPTOR
#define TIME_SEC 0
#define TIME_USEC 265 * 1000    //250

typedef struct snake_t {
    unsigned int pos_x, pos_y;
    struct snake_t *next_body_cell, *prev_body_cell;
} snake_t; 

snake_t *snake_head = NULL;
snake_t *snake_tail = NULL;

typedef struct fruit_t {
    bool is_on_board;
    unsigned int pos_x, pos_y;
} fruit_t;

fruit_t *fruit = NULL;


char board[SIDE_SIZE][SIDE_SIZE];

void new_cell (char);
void new_fruit ();
void initialize_board();
void update_snake(char, unsigned int *);
void updateBoard();
void keyboardListener(char *);
void printBoardOnScreen();
unsigned int genNumInBoard();

int main (void) {
    char snakeMoveDirection = 's';
    
    // ENABLE NON BLOCKING INPUT BY TURNING OFF ECHO AND ICANON 
    static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++) {
        new_cell(snakeMoveDirection);
    }
    fruit = (fruit_t*) malloc(sizeof(fruit_t));
    new_fruit();
    initialize_board();
    unsigned int fruits_eaten_count = 0;
    //new_fruit();
    //updateBoard(gameBoard); 
    while(true) {
        system("clear");
        printf("\t\t\t\t WHATEVER GAME\n\t\t\t\t FRUITS EATEN: %u\n", fruits_eaten_count);
        update_snake(snakeMoveDirection, &fruits_eaten_count);
        updateBoard();
        printBoardOnScreen();
        keyboardListener(&snakeMoveDirection); 
    }    

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // RETURN TO THE DEFAULT OPTIONS AFTER EXECUTION
    return 0;
}

void new_cell (char snakeMoveDirection) {
    
    if (snake_head == NULL) {
        snake_head = (snake_t*) malloc(sizeof(snake_t));
        snake_head->pos_y = 10;
        snake_head->pos_x = 10; 
        snake_tail = snake_head;
    } else { 
        
        snake_t *new_cell = (snake_t*) malloc(sizeof(snake_t));
        switch (snakeMoveDirection) {
            case 'w':
                new_cell->pos_x = snake_tail->pos_x;
                new_cell->pos_y = snake_tail->pos_y + 1;
                break;
            case 'd':
                new_cell->pos_x = snake_tail->pos_x - 1;
                new_cell->pos_y = snake_tail->pos_y;
                break;
            case 'a':
                new_cell->pos_x = snake_tail->pos_x + 1;
                new_cell->pos_y = snake_tail->pos_y;
                break;
            case 's':  
                new_cell->pos_x = snake_tail->pos_x;
                new_cell->pos_y = snake_tail->pos_y - 1;
                break;

        }
        snake_tail->next_body_cell = new_cell;
        new_cell->prev_body_cell = snake_tail;

        snake_tail = new_cell;
   }
}

void new_fruit () {
    fruit->pos_x = genNumInBoard();
    fruit->pos_y = genNumInBoard();
    fruit->is_on_board = false;

}

void initialize_board() {
   	for (unsigned int y = 0; y < SIDE_SIZE; y++){
		for (unsigned int x = 0; x < SIDE_SIZE; x++) {
			if (y == 0 || y == SIDE_SIZE - 1 || x == 0 || x == SIDE_SIZE - 1) {
				board[y][x] = '.';
		    }   else {
                    board[y][x] = ' ';
            }   
	    }
	}
}

void update_snake (char snakeMoveDirection, unsigned int *fruits_eaten_count) {
    snake_t *head_buff = snake_head;
    snake_t *tail_buff = snake_tail;
    
    board[snake_tail->pos_y][snake_tail->pos_x] = ' ';
    
    while(snake_tail->prev_body_cell != NULL) {
        snake_tail->pos_x = snake_tail->prev_body_cell->pos_x;
        snake_tail->pos_y = snake_tail->prev_body_cell->pos_y;
        snake_tail = snake_tail->prev_body_cell;
    
    }
    
    switch(snakeMoveDirection) {
        case 'w':
            snake_head->pos_y = (snake_head->pos_y-- > 1)
            ? snake_head->pos_y-- : SIDE_SIZE - 2;
            break;
        case 'd':
            snake_head->pos_x = (snake_head->pos_x++ < SIDE_SIZE - 2)
            ? snake_head->pos_x++ : 1;
            break;
        case 'a':
          snake_head->pos_x = (snake_head->pos_x-- > 1) 
          ? snake_head->pos_x-- : SIDE_SIZE - 2;
          break;
        case 's':
          snake_head->pos_y = (snake_head->pos_y++ < SIDE_SIZE - 2)
          ? snake_head->pos_y++ : 1;
          break;
      }   
    snake_head = head_buff;
    snake_tail = tail_buff;

    if (snake_head->pos_x == fruit->pos_x && snake_head->pos_y == fruit->pos_y) {
        new_fruit();
        new_cell(snakeMoveDirection);
        *fruits_eaten_count += 1;
    }
}

void updateBoard() {
    snake_t *head_buff = snake_head;
    
    while (snake_head != NULL) {
        board[snake_head->pos_y][snake_head->pos_x] = SNAKE_CHAR;
        snake_head = snake_head->next_body_cell;   
    }

    snake_head = head_buff;

    if (!fruit->is_on_board) {
        board[fruit->pos_y][fruit->pos_x] = '*';
        fruit->is_on_board = true;  
    }
} 

void keyboardListener (char *currentDirection) {
    fd_set input;
    int fds, ret_val, num;
    struct timeval tv;
    char ch;

    tv.tv_sec = TIME_SEC;
    tv.tv_usec = TIME_USEC;

    fds = STDIN + 1;

    FD_ZERO(&input);
    FD_SET(STDIN, &input);

    ret_val = select(fds, &input , NULL, NULL, &tv);

    if (ret_val == -1) {
        perror("select()");
    }else if (ret_val > 0) {
        if(FD_ISSET(0, &input))  {
            ch = getchar();
        }   
    } else {
        return;
    }
    //  Verify inputed key
    if ((ch == 'w' && *currentDirection != 's') || (ch == 's' && *currentDirection != 'w')
    || (ch == 'd' && *currentDirection != 'a') || (ch == 'a' && *currentDirection != 'd')) {
        *currentDirection = ch;
    }
    
}
unsigned int genNumInBoard() {
    unsigned int numValue = rand() % SIDE_SIZE;
    while (numValue == 0 || numValue == SIDE_SIZE - 1)
        numValue = rand() % SIDE_SIZE;
    
    return numValue;
}

void printBoardOnScreen() {
	for (unsigned int y = 0; y < SIDE_SIZE; y++) {
		for (unsigned int x = 0; x < SIDE_SIZE; x++) {
			printf("%c ", board[y][x]);
		}
		
		printf("\n");
	}	

    
}
