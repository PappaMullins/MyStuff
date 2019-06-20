#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <conio.h>
#include "Snake.h"


typedef int direction;

void displayGameBoard(direction head_direction, int score);
void displayGameBoard_Debug(int debug_mode);
void resetBoard(void);
int takeInput(void);
void main_game(void);
void pause(void);
void menu(int menu_mode, int score);
bool moveSnake(direction head_direction);
int nextSnakePart(int prev_snake_part);
void spawnMouse(void);
bool checkIfEaten(void);
int lengthOfSnake(void);
void growSnake(int last_snake_part);
bool checkIfOutOfBounds(void);

int game_board[BOARDSIZE_Y][BOARDSIZE_X] = {EMPTY};
int prev_location_xx, prev_location_yy, print_head_direction = DEFAULT;

int main(void)
{
    srand(time(NULL));
    menu(MENUMODE_MAIN, NO_SCORE);
    return 0;
}

//displays the board in character form
void displayGameBoard(direction head_direction, int score)
{
    int i,j;

    printf("SCORE %d\n\n", score);

    for(i = 0; i < BOARDSIZE_Y; i++)
        printf("-");

    printf("\n");

    for(i = 1; i < Y; i++)
    {
        printf("|");
        for(j = 1; j < X; j++)
        {
            if(game_board[i][j] == EMPTY)
            {
                printf(" ");
            }
            else if(game_board[i][j] == MOUSE)
            {
                printf("*");
            }
            else if(game_board[i][j] == BITTEN)
            {
                printf("X");
            }
            else if(game_board[i][j] == HEAD)
            {
                if(head_direction == UP)
                    printf("^");
                else if(head_direction == DOWN)
                    printf("v");
                else if(head_direction == RIGHT)
                    printf(">");
                else if(head_direction == LEFT)
                    printf("<");
            }
            else
            {
                printf("O");
            }
        }
        printf("|\n");
    }

    for(i = 0; i < BOARDSIZE_Y; i++)
        printf("-");
}

//Displays the game board in integer form for debugging
void displayGameBoard_Debug(int debug_mode)
{
    int i, j;

    //display with border
    if(debug_mode == DEBUGMODE_WITH_BORDER)
    {
        for(i = 0; i < BOARDSIZE_Y; i++)
        {
            for(j = 0; j < BOARDSIZE_X; j++)
            {
                printf("%d", game_board[i][j]);
            }
            printf("\n");
        }
    }
    //displays without border
    else
    {
        for(i = 1; i < Y; i++)
        {
            for(j = 1; j < X; j++)
            {
                printf("%d", game_board[i][j]);
            }
            printf("\n");
        }
    }
}

//makes every "tile" 0 and places the snake in its starting position
void resetBoard(void)
{
    int i,j;

    for(i = 0; i < BOARDSIZE_Y; i++)
    {
        for(j = 0; j < BOARDSIZE_X; j++)
        {
            game_board[i][j] = EMPTY;
        }
    }

    game_board[(Y - 1) / 2][1] = 7;
    game_board[(Y - 1) / 2][2] = 6;
    game_board[(Y - 1) / 2][3] = 5;
    game_board[(Y - 1) / 2][4] = 4;
    game_board[(Y - 1) / 2][5] = 3;
    game_board[(Y - 1) / 2][6] = 2;
    game_board[(Y - 1) / 2][7] = HEAD;
}

//takes input from the user and acts as a "delay" function.
//Change the how long the delay lasts by editing the MILLISECONDS macro(1 seconds = 1000 milliseconds)


int takeInput(void)
{
    char pressed_key;
    clock_t finish = MILLISECONDS + clock();

    while(finish > clock())
    {
        if(kbhit())
        {
            pressed_key = getch();

            if(pressed_key == ESC)
                return EXIT;
            else if(pressed_key == 'p')
                return PAUSE;
        }
    }

    if(pressed_key == 'w')
        return UP;
    else if(pressed_key == 's')
        return DOWN;
    else if(pressed_key == 'a')
        return LEFT;
    else if(pressed_key == 'd')
        return RIGHT;
    else
        return NO_RETURN;
}

//pauses until a key is pressed
void pause(void)
{
    printf("\n GAME PAUSED\nPress any key to resume");
    getch();
}

//main game loop
void main_game()
{
    bool win = false;
    direction head_direction = DEFAULT, temp;
    int last_snake_part, score = 0;

    resetBoard();

    displayGameBoard(DEFAULT, score);
    pause();

    while(win == false)
    {
        system("cls");

        if(checkIfEaten())
            spawnMouse();

        displayGameBoard(print_head_direction, score);

        temp = takeInput();

        if(temp != NO_RETURN && temp != EXIT && temp != PAUSE)
            head_direction = temp;

        if(temp == EXIT)
        {
            menu(MENUMODE_MAIN, NO_SCORE);
        }
        else if(temp == PAUSE)
        {
            pause();
        }
        else
        {
            last_snake_part = lengthOfSnake();
            win = moveSnake(head_direction);
        }

        if(checkIfOutOfBounds())
            win = true;

        if(checkIfEaten())
        {
            score++;
            growSnake(last_snake_part);
        }

    }
    system("cls");
    displayGameBoard(print_head_direction, score);
    menu(MENUMODE_END, score);
}

//this is where I got stuck HARD.. and I'm sure it shows. I'm certain there is a better way to do this but honestly it works so for now i don't feel like torturing my self
bool moveSnake(direction head_direction)
{
    int snake_part, prev_snake_part = 0, current_location_y, current_location_x, prev_location_y, prev_location_x, i, j, break_from_loop = 0;
    static direction prev_head_direction;
    bool win = false;

    while(true)
    {
        break_from_loop = 0;
        snake_part = nextSnakePart(prev_snake_part);

        if(snake_part == NO_SNAKE_PART)
            break;

        for(i = 1; i < Y; i++)
        {
            for(j = 1; j < X; j++)
            {
                if(game_board[i][j] == snake_part)
                {
                    current_location_y = i;
                    current_location_x = j;
                    break_from_loop = 1;
                    break;
                }
            }
            if(break_from_loop == 1)
                break;
        }

        prev_snake_part = snake_part;

        if(snake_part == HEAD)
        {
            if(head_direction == UP && (game_board[current_location_y - 1][current_location_x] == EMPTY || game_board[current_location_y - 1][current_location_x] == MOUSE))
            {
                game_board[current_location_y - 1][current_location_x] = HEAD;
                game_board[current_location_y][current_location_x] = EMPTY;
                prev_head_direction = head_direction;
                print_head_direction = prev_head_direction;
            }
            else if(head_direction == DOWN && (game_board[current_location_y + 1][current_location_x] == EMPTY || game_board[current_location_y + 1][current_location_x] == MOUSE))
            {
                game_board[current_location_y + 1][current_location_x] = HEAD;
                game_board[current_location_y][current_location_x] = EMPTY;
                prev_head_direction = head_direction;
                print_head_direction = prev_head_direction;
            }
            else if(head_direction == LEFT && (game_board[current_location_y][current_location_x - 1] == EMPTY || game_board[current_location_y][current_location_x - 1] == MOUSE))
            {
                game_board[current_location_y][current_location_x - 1] = HEAD;
                game_board[current_location_y][current_location_x] = EMPTY;
                prev_head_direction = head_direction;
                print_head_direction = prev_head_direction;
            }
            else if(head_direction == RIGHT && (game_board[current_location_y][current_location_x + 1] == EMPTY || game_board[current_location_y][current_location_x + 1] == MOUSE))
            {
                game_board[current_location_y][current_location_x + 1] = HEAD;
                game_board[current_location_y][current_location_x] = EMPTY;
                prev_head_direction = head_direction;
                print_head_direction = prev_head_direction;
            }
            else if(head_direction == UP && game_board[current_location_y - 1][current_location_x] > 2 && game_board[current_location_y - 1][current_location_x] != MOUSE)
            {
                game_board[current_location_y - 1][current_location_x] = BITTEN;
                win = true;
            }
            else if(head_direction == DOWN && game_board[current_location_y + 1][current_location_x] > 2 && game_board[current_location_y + 1][current_location_x] != MOUSE)
            {
                game_board[current_location_y + 1][current_location_x] = BITTEN;
                win = true;
            }
            else if(head_direction == LEFT && game_board[current_location_y][current_location_x - 1] > 2 && game_board[current_location_y][current_location_x - 1] != MOUSE)
            {
                game_board[current_location_y][current_location_x - 1] = BITTEN;
                win = true;
            }
            else if(head_direction == RIGHT && game_board[current_location_y][current_location_x + 1] > 2 && game_board[current_location_y][current_location_x + 1] != MOUSE)
            {
                game_board[current_location_y][current_location_x + 1] = BITTEN;
                win = true;
            }
            else if(prev_head_direction == UP && (game_board[current_location_y - 1][current_location_x] == EMPTY || game_board[current_location_y - 1][current_location_x] == MOUSE))
            {
                game_board[current_location_y - 1][current_location_x] = HEAD;
                game_board[current_location_y][current_location_x] = EMPTY;
            }
            else if(prev_head_direction == DOWN && (game_board[current_location_y + 1][current_location_x] == EMPTY || game_board[current_location_y + 1][current_location_x] == MOUSE))
            {
                game_board[current_location_y + 1][current_location_x] = HEAD;
                game_board[current_location_y][current_location_x] = EMPTY;
            }
            else if(prev_head_direction == LEFT && (game_board[current_location_y][current_location_x - 1] == EMPTY || game_board[current_location_y][current_location_x - 1] == MOUSE))
            {
                game_board[current_location_y][current_location_x - 1] = HEAD;
                game_board[current_location_y][current_location_x] = EMPTY;
            }
            else if(prev_head_direction == RIGHT && (game_board[current_location_y][current_location_x + 1] == EMPTY || game_board[current_location_y][current_location_x + 1] == MOUSE))
            {
                game_board[current_location_y][current_location_x + 1] = HEAD;
                game_board[current_location_y][current_location_x] = EMPTY;
            }
        }
        else if(snake_part != HEAD && game_board[prev_location_y][prev_location_x] == EMPTY)
        {
            game_board[prev_location_y][prev_location_x] = snake_part;
            game_board[current_location_y][current_location_x] = EMPTY;
        }

        prev_location_y = current_location_y;
        prev_location_x = current_location_x;
        prev_location_yy = prev_location_y;
        prev_location_xx = prev_location_x;
    }
    return win;
}

//if there is a snake part bigger than the last it returns it else it returns NO_SNAKE_PART

int nextSnakePart(int prev_snake_part)
{
    int i,j;

    if(prev_snake_part == 0)
        return 1;

    for(i = 1; i < Y; i++)
    {
        for(j = 1; j < X; j++)
        {
            if(game_board[i][j] == prev_snake_part + 1)
                return (prev_snake_part + 1);
        }
    }

    return NO_SNAKE_PART;
}

//displays either the end menu or start menu


void menu(int menu_mode, int score)
{
    int user_input;

    if(menu_mode == MENUMODE_MAIN)
    {
        system("cls");
        printf("\t\t\t\t\t\t\tS N A K E\n\n\t\t\t\t\t\t\t1. START GAME\n\n\t\t\t\t\t\t\t2. CONTROLLS\n\n\t\t\t\t\t\t\t3. EXIT GAME\n\n\n\n");

        while(true)
        {
            scanf("%d", &user_input);
            if(user_input == 1 || user_input == 2 || user_input == 3)
            {
                break;
            }
            else
            {
                printf("\nInvalid input\n");
            }
        }

        if(user_input == 1)
        {
            main_game();
        }
        else if(user_input == 2)
        {
            system("cls");
            printf("W to go up\nS to go down\nA to go left\nD to go right\nP to pause\nESC to go back to main menu\n\n\nENTER 4 TO GO BACK\n");
            while(true)
            {
                scanf("%d", &user_input);

                if(user_input == 4)
                {
                    menu(MENUMODE_MAIN, NO_SCORE);
                }
                else
                {
                    printf("\nInvalid input\n");
                }
            }
        }
        else if(user_input == 3)
        {
            exit(0);
        }
    }
    else
    {
        printf("\n\n\nGAME\nOVER\n\nSCORE %d\n\n1. PLAY AGAIN\n\n2. MAIN MENU\n\n3. EXIT\n\n", score);

        while(true)
        {
            scanf("%d", &user_input);
            if(user_input == 1 || user_input == 2 || user_input == 3)
            {
                break;
            }
            else
            {
                printf("\nInvalid input\n");
            }
        }

        if(user_input == 1)
        {
            main_game();
        }
        else if(user_input == 2)
        {
            menu(MENUMODE_MAIN, NO_SCORE);
        }
        else
        {
            exit(0);
        }
    }
}

//places the mouse somewhere if that place is empty, if it isn't then it moves it along the array until an empty tile is found

void spawnMouse(void)
{
    int i, j;
    bool _break = false;
    int coords[2];

    for(;;)
    {
        coords[0] = rand()%(Y - 1);
        coords[1] = rand()%(X - 1);

        if(coords[0] == 0)
            coords[0]++;
        if(coords[1] == 0)
            coords[1]++;

        if(game_board[coords[0]][coords[1]] == EMPTY)
        {
            game_board[coords[0]][coords[1]] = MOUSE;
            break;
        }
        else
        {
            for(i = coords[0]; i < Y; i++)
            {
                for(j = coords[1]; j < X; j++)
                {
                    if(game_board[i][j] == EMPTY)
                    {
                        game_board[i][j] == MOUSE;
                        _break = true;
                        break;
                    }
                }
                if(_break == true)
                {
                    break;
                }
            }
        }
        if(_break == true)
        {
            break;
        }
    }

}

//looks if there is an mouse on the board if not then it returns true

bool checkIfEaten(void)
{
    int i, j;

    for(i = 1; i < Y; i++)
    {
        for(j = 1; j < X; j++)
        {
            if(game_board[i][j] == MOUSE)
            {
                return false;
            }
        }
    }

    return true;
}

//just counts how many parts the snake has

int lengthOfSnake(void)
{
    int i, j, length = 0;

    for(i = 1; i < Y; i++)
    {
        for(j = 1; j < X; j++)
        {
            if(game_board[i][j] != EMPTY && game_board[i][j] != MOUSE)
            {
                length++;
            }
        }
    }

    return length;
}

//adds a new snake part at the last location


void growSnake(int last_snake_part)
{
    game_board[prev_location_yy][prev_location_xx] = last_snake_part + 1;
}

//checks if the head is outside the board

bool checkIfOutOfBounds(void)
{
    int i, j;

    for(i = 0; i < BOARDSIZE_Y; i++)
    {
        for(j = 0; j < BOARDSIZE_X; j++)
        {
            if(game_board[i][j] == HEAD)
            {
                if(j == BOARDSIZE_X - 1 || j == 0 || i == BOARDSIZE_Y - 1 || i == 0)
                {
                    return true;
                }
            }
        }
    }
}
