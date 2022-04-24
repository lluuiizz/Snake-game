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
#define TIME_USEC 250 * 1000    //250

typedef struct snake_t {
    unsigned int pos_x, pos_y;
    struct snake_t *next_body_cell;
} snake_t; 

snake_t *snake_head = NULL;

void new_cell ();
void update_snake(bool, char );
void updateBoard(char (*)[]);
void keyboardListener(char *);
void printBoardOnScreen(char (*)[]);
unsigned int genNumInBoard();

int main (void) {
	char gameBoard[SIDE_SIZE][SIDE_SIZE];
    char snakeMoveDirection = 'w';
    
    // ENABLE NON BLOCKING INPUT BY TURNING OFF ECHO AND ICANON 
    static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    

    new_cell();
    new_cell();
    new_cell();
    updateBoard(gameBoard);     // PUT THE CORNER CHARACTERS IN THE BOARD     

    while(true) {
        system("clear");
        printf("\t\t\t\t WHATEVER GAME\n\t\t\t\t FRUITS EATEN: \n");
        update_snake(true ,snakeMoveDirection);
        updateBoard(gameBoard);
        printBoardOnScreen(gameBoard);
        keyboardListener(&snakeMoveDirection); 
    }    

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // RETURN TO THE DEFAULT OPTIONS AFTER EXECUTION
    return 0;
}

void new_cell () {
    
    if (snake_head == NULL) {
        snake_head = (snake_t*) malloc(sizeof(snake_t));
        snake_head->pos_y = 10;
        snake_head->pos_x = 10; 
    } else { 
        snake_t *first_cell = snake_head;
        snake_t *last_cell = NULL;
        
        while (snake_head->next_body_cell != NULL) {
            snake_head = snake_head->next_body_cell;
        }


        last_cell = snake_head;
        snake_t *new_cell = (snake_t*) malloc(sizeof(snake_t));
        new_cell->pos_x = last_cell->pos_x;
        new_cell->pos_y = last_cell->pos_y + 1;
        last_cell->next_body_cell = new_cell;
        snake_head = first_cell;
   }
}
void update_snake (bool head_is_first_cell , char moveDirection) {
    static snake_t *snake_first_cell = NULL;
    if (snake_head->next_body_cell != NULL) {
        snake_t *next_cell = snake_head->next_body_cell;
        next_cell->pos_x = snake_head->pos_x;
        next_cell->pos_y = snake_head->pos_y;
        
       
    }
    
    if (head_is_first_cell == true) {
          switch(moveDirection) {
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
          snake_first_cell = snake_head;
          
    }

    
    if (snake_head->next_body_cell != NULL) {
        snake_head = snake_head->next_body_cell;
        update_snake(false ,moveDirection);
    }
    snake_head = snake_first_cell;
}

void updateBoard(char (*board)[SIDE_SIZE]) {
   	for (unsigned int y = 0; y < SIDE_SIZE; y++){
		for (unsigned int x = 0; x < SIDE_SIZE; x++) {
			if (y == 0 || y == SIDE_SIZE - 1 || x == 0 || x == SIDE_SIZE - 1) {
				board[y][x] = '.';
			}
			else {
				board[y][x] = ' ';
			}
		}
	}

    snake_t *first_cell = snake_head;
    
    while (snake_head != NULL) {
        board[snake_head->pos_y][snake_head->pos_x] = SNAKE_CHAR;
        snake_head = snake_head->next_body_cell;   
    }

    snake_head = first_cell;

} void keyboardListener (char *currentDirection) {
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

void printBoardOnScreen(char (*board)[SIDE_SIZE]) {
	for (unsigned int y = 0; y < SIDE_SIZE; y++) {
		for (unsigned int x = 0; x < SIDE_SIZE; x++) {
			printf("%c ", board[y][x]);
		}
		
		printf("\n");
	}	

}
