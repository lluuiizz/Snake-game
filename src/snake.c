#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <stdbool.h>

#define SNAKE_CHAR '#'
#define SIDE_SIZE 40
#define STDIN 0     // INPUT FILE DESCRIPTOR
#define TIME_SEC 0
#define TIME_USEC 250 * 1000

typedef struct Snake_s {
    size_t pos_x, pos_y;
} Snake; 


void initializeBoard(char (*)[]);
void snakeFirstConditions(Snake *, char (*)[], size_t *, char *);
void moveSnake(Snake (*) [], char (*)[], size_t *, char *, bool *);
void keyboardListener(char *);
void printBoardOnScreen(char (*)[]);
size_t genNumInBoard();

int main (void) {
	char gameBoard[SIDE_SIZE][SIDE_SIZE];
    Snake snakeBody[SIDE_SIZE * SIDE_SIZE - SIDE_SIZE * 4];
    size_t snakeBodySize;
    char snakeMoveDirection;
    
    // ENABLE NON BLOCKING INPUT BY TURNING OFF ECHO AND ICANON 
    static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    


    initializeBoard(gameBoard);     // PUT THE CORNER CHARACTERS IN THE BOARD 
    snakeFirstConditions(&snakeBody[0], gameBoard, &snakeBodySize, &snakeMoveDirection);
    
    gameBoard[genNumInBoard()][genNumInBoard()] = '*'; // PUT A FRUIT IN THE BOARD
    bool isFruitOnBoard = true;     //  SET THAT IS AN FRUIT IN THE BOARD
	size_t fruitsEaten = 0;     //  COUNT THE TOTAL OF FRUITS EATEN 
    
    while(true) {
        system("clear");
        printf("\t\t\t\t WHATEVER GAME\n\t\t\t\t FRUITS EATEN: %lu\n", fruitsEaten);

        if (!isFruitOnBoard) {
            gameBoard[genNumInBoard()][genNumInBoard()] = '*';
            fruitsEaten++;
            isFruitOnBoard = true; 
        }
        
        moveSnake(&snakeBody, gameBoard, &snakeBodySize, &snakeMoveDirection, &isFruitOnBoard);
        printBoardOnScreen(gameBoard);
        keyboardListener(&snakeMoveDirection); 
        
    }    

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // RETURN TO THE DEFAULT OPTIONS AFTER EXECUTION
    return 0;
}

void initializeBoard(char (*board)[SIDE_SIZE]) {
	for (size_t y = 0; y < SIDE_SIZE; y++){
		for (size_t x = 0; x < SIDE_SIZE; x++) {
			if (y == 0 || y == SIDE_SIZE - 1 || x == 0 || x == SIDE_SIZE - 1) {
				board[y][x] = '.';
			}
			else {
				board[y][x] = ' ';
			}
		}
	}
}

void snakeFirstConditions(Snake *snakeHead, char (*board)[SIDE_SIZE], size_t **snakeSize, char *snakeDirection) {

    snakeHead->pos_x = SIDE_SIZE / 2;
    snakeHead->pos_y = SIDE_SIZE / 2;
    *snakeDirection = 'w';
    **snakeSize = 1;

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
    }
    else if (ret_val > 0) {
        if(FD_ISSET(0, &input))  {
            ch = getchar();
        }   
    }
    else {
        return;
    }

    if (ch == 'w' || ch == 's' || ch == 'd' || ch == 'a') {
        *currentDirection = ch;
    }
    else {
        ch = getchar();
        
    }
}

void moveSnake(Snake (*snake) [SIDE_SIZE * SIDE_SIZE - SIDE_SIZE * 4], 
char (*board)[SIDE_SIZE], size_t **snakeSize, char *direction, bool *isFruitOnBoard) {
    
        if (*direction == 'w') {
            board[snake[*snakeSize - 1]->pos_y][snake[snakeSize - 1]->pos_x] = ' ';
            snake[*snakeSize]->pos_y = (snake[snakeSize - 1]->pos_y-- > 1) ? snake[snakeSize - 1]->pos_y-- : SIDE_SIZE - 2;
            snake[*snakeSize]->pos_x = snake[snakeSize - 1]->pos_x;
        }
        else if (*direction == 's') {
            board[snake[*snakeSize - 1]->pos_y][snake[snakeSize - 1]->pos_x] = ' ';
            snake[*snakeSize]->pos_y = (snake[snakeSize - 1]->pos_y++ < SIDE_SIZE - 2) ? snake[snakeSize - 1]->pos_y++ : 1;
        }
        else if (*direction == 'd') {
            board[snake[*snakeSize - 1]->pos_y][snake[snakeSize - 1]->pos_x] = ' ';
            snake[*snakeSize]->pos_x = (snake[snakeSize - 1]->pos_x++ < SIDE_SIZE - 2) ? snake[snakeSize - 1]->pos_x++ : 1;
            
        }
        else if (*direction == 'a') {
            board[snake[*snakeSize - 1]->pos_y][snake[snakeSize - 1]->pos_x] = ' ';
            snake[*snakeSize]->pos_x = (snake[snakeSize - 1]->pos_x-- > 1) ? snake[snakeSize - 1]->pos_x-- : SIDE_SIZE - 2;
        
        }else {
            break;
        }
        *isFruitOnBoard = (board[snake[*snakeSize]->pos_y][snake[snakeSize]->pos_x] == '*') ? false : true;
        
        for (int i = 0; i < *snakeSize; i++) {
            snake[i]->pos_y = snake[i + 1]->pos_y;
            snake[i]->pos_x = snake[i + 1]->pos_x;
            
            board[snake[i]->pos_y][snake[i]->pos_x] = SNAKE_CHAR;
        }
     
}

size_t genNumInBoard() {
    size_t numValue = rand() % SIDE_SIZE;
    while (numValue == 0 || numValue == SIDE_SIZE - 1)
        numValue = rand() % SIDE_SIZE;
    
    return numValue;
}

void printBoardOnScreen(char (*board)[SIDE_SIZE]) {
	for (size_t y = 0; y < SIDE_SIZE; y++) {
		for (size_t x = 0; x < SIDE_SIZE; x++) {
			printf("%c ", board[y][x]);
		}
		
		printf("\n");
	}	


}
