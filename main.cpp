/*
* ASCII Pong
*    Inspired by "Code-It-Yourself! Tetris - Programming from Scratch (Quick and Simple C++)": https://youtu.be/8OK8_tHeCIA
*/

#include <Windows.h>
#include <chrono>
#include <thread>
#include "main.h"

#include <iostream>
#include <string>

const int SCREEN_WIDTH = 120;    // Console Screen Size X (columns)
const int SCREEN_HEIGHT = 30;    // Console Screen Size Y (rows)
const int SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;       //SIZE OF SCREEN ARRAY

//Since we only use one character to display the score of each player, the winning score should be smaller than 10
const unsigned int WINNING_SCORE = 3;
const unsigned int SCORE_X_POSITION = 15;
const unsigned int SCORE_Y_POSITION = 2; 
unsigned int player1Score = 0;
unsigned int player2Score = 0;
//WINNER_X_POSITION is measured from the left border for player 1 and from the right border for player 2
const unsigned int WINNER_X_POSITION = SCREEN_WIDTH / 4;
const unsigned int WINNER_Y_POSITION = SCREEN_HEIGHT / 2;
const unsigned int FINAL_GAMEOVER_PAUSE = 5000; //milliseconds

const unsigned int BALL_SPEED = 20; //ORIGINALLY 5   

const unsigned int PADDLE_DISTANCE_FROM_SIDE_BORDERS = 2;
const unsigned int PADDLE_HEIGHT = 8;

//Paddle one XY position
unsigned int paddle1XPos = PADDLE_DISTANCE_FROM_SIDE_BORDERS;
int paddle1YPos = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;                          // CHANGED to signed to check bounds
const unsigned int PADDLE1_OG_Y_POS = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;       //original Y position of paddle 1

//Paddle two XY position
unsigned int paddle2XPos = (SCREEN_WIDTH - 1) - PADDLE_DISTANCE_FROM_SIDE_BORDERS;
int paddle2YPos = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;                          // CHANGED to signed ot check bounds
const unsigned int PADDLE2_OG_Y_POS = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;       //original Y position of paddle 2

int main()
{
    /*
    * Initialize data structures.
    */

    //Ball XY position
    unsigned int ballXPos = SCREEN_WIDTH / 2;
    unsigned int ballYPos = SCREEN_HEIGHT / 2;
  

    //Ball XY direction
    int ballXSpeed = 1; //go right
    int ballYSpeed = 1; //go down

    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    wchar_t* screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];

    // Main Loop
    bool bGameOver = false;
    while (!bGameOver)
    {
        // Draw a blank field of size nScreenWidth * nScreenHeight
        drawBlankField(screen);

        //Draw left, center and right vertical borders
        drawVerticalBars(screen);

        //Draw horizontal lines at the top and bottom of the screen
        drawHorizontalBorders(screen);

        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(BALL_SPEED)); // Small Step = 1 Game Tick

        // Read user input (pressing W/S or O/K keys)
        wchar_t controls[] = { L'W', L'S', L'O', L'K' };
        bool bKey[sizeof(controls) / sizeof(wchar_t)];
        for (int k = 0; k < sizeof(bKey) / sizeof(bool); k++)
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)(controls[k]))) != 0;

        // Handle paddle one movement
        handlePaddleOneMovement(screen, bKey);

        // Draw paddle one
        drawPaddleOne(screen);

        // Handle paddle two movement
        handlePaddleTwoMovement(screen, bKey);

        // Draw paddle two
        drawPaddleTwo(screen);

        // Reposition Ball vertically
        repositionBallVertically(ballYPos, ballYSpeed);

        // Reposition Ball horizontally
        repositionBallHorizontally(ballXPos, ballXSpeed, hitPaddle(ballYPos, ballXPos, screen));

        // Draw Player 1 and Player 2 Scores
        printPlayerScores(screen);

        //Draw ball
        screen[ballXPos + ballYPos * SCREEN_WIDTH] = L'O';

        checkWinningConditions(screen, bGameOver);

        // Display Frame
        WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwBytesWritten);
    }

    // Exiting
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(FINAL_GAMEOVER_PAUSE));
    CloseHandle(hConsole);
    return 0;
}

void drawBlankField(wchar_t* screen)
{
    for (size_t index = 0; index < SCREEN_HEIGHT * SCREEN_WIDTH; index++)
    {
        screen[index] = L' ';
    }
}

void drawVerticalBars(wchar_t* screen)
{
    for (size_t index = 0; index < SCREEN_HEIGHT * SCREEN_WIDTH; index += SCREEN_WIDTH)
    {
        screen[index] = L'|';
    }

    for (size_t index = SCREEN_WIDTH / 2; index < SCREEN_HEIGHT * SCREEN_WIDTH; index += SCREEN_WIDTH)
    {
        screen[index] = L'.';
    }

    for (size_t index = SCREEN_WIDTH - 1; index < SCREEN_HEIGHT * SCREEN_WIDTH; index += SCREEN_WIDTH)
    {
        screen[index] = L'|';
    }
    
   
}

//written by althea
//gets indices of top row and bottom row and fill them with '=' 
void drawHorizontalBorders(wchar_t* screen)
{
    for (size_t index = 0; index < SCREEN_WIDTH; index += 1)
    {
        if (screen[index] != L'|')
        {
            screen[index] = L'=';
        }
 
    }
    

    for (size_t index = SCREEN_SIZE -SCREEN_WIDTH; index < SCREEN_SIZE; index += 1)
    {
        if (screen[index] != L'|')
        {
            screen[index] = L'=';
        }

    }
}

//written by althea
void handlePaddleOneMovement(wchar_t* screen, const bool bKey[4])
{
    //W Up
    if (bKey[0]) {
        //check if paddle is at the top
        if (paddle1YPos <= 1) {
           paddle1YPos = 1;
        }
        else
        {
            paddle1YPos--;
        }
    }

    //S Down
    if (bKey[1])
    {
        //check if paddle is at the bottom
        if (paddle1YPos >= SCREEN_HEIGHT - 2 - PADDLE_HEIGHT)
        {
            paddle1YPos = SCREEN_HEIGHT - 1 - PADDLE_HEIGHT;
        }
        else
            {
            paddle1YPos++;
            }
    }
}

//written by althea
void handlePaddleTwoMovement(wchar_t* screen, const bool bKey[4])
{
    //O Up
    if (bKey[2]) {
        //check if paddle is at the top
        if (paddle2YPos <= 1) {
            paddle2YPos = 1;
        }
        else {
            paddle2YPos--;
        }
    }

    //L Down
    if (bKey[3])
    {
        //check if paddle is at bottom
        if ( paddle2YPos >= SCREEN_HEIGHT - 1 - PADDLE_HEIGHT)
        {
            paddle2YPos = SCREEN_HEIGHT - 1 - PADDLE_HEIGHT;
        }
        else
        {
            paddle2YPos++;
        }
    }
}

void drawPaddleOne(wchar_t* screen)
{
    for (size_t index = 0; index < PADDLE_HEIGHT; index++)
    {
        screen[(paddle1YPos + index) * SCREEN_WIDTH + paddle1XPos] = L'>';
    }
}

//written by althea
// similar to paddleOne but on the opposite side and represented by '<'
void drawPaddleTwo(wchar_t* screen)
{
    for (size_t index = 0; index < PADDLE_HEIGHT; index++)
    {
        screen[(paddle2YPos + index) * SCREEN_WIDTH + paddle2XPos] = L'<';
    }
}

void repositionBallVertically(unsigned int& ballYPos, int& ballYSpeed)
{
    if (ballYSpeed > 0) //ball going down
    {
        if (ballYPos >= (SCREEN_HEIGHT - 1) - 1)
        {
            ballYSpeed *= -1;
        }
        ballYPos += ballYSpeed;
    }
    else if (ballYSpeed < 0) //ball going up
    {
        if (ballYPos <= 1)
        {
            ballYSpeed *= -1;
        }
        ballYPos += ballYSpeed;
    }
}

//Written by althea
//NEEDS TO handle collision with brackets or if it goes past
//tick might be too fast/the order of operations isnt good for an accurate check
//might resolve by checking BEFORE hitting the barrier as well as check before hitting the paddle
void repositionBallHorizontally(unsigned int& ballXPos, int& ballXSpeed, bool hitPaddle)
{
    if (ballXSpeed > 0) //ball going RIGHT
    {
        if (ballXPos >= (SCREEN_WIDTH - 1) - 1 || hitPaddle)
        {
          /*  if (ballXPos >= SCREEN_WIDTH -1)
            {
                player1Score++;
                ballXPos = SCREEN_WIDTH/2;
            }
            ballXSpeed *= -1;
            //check if past edge, do else actions, then by default change direction*/

            
            //if hitPaddle, change direction
            if (hitPaddle)
            {
                ballXSpeed *= -1;
            }
            //else add score to one P1 and reposition ball to center;
            else
            {
                player1Score ++;
                ballXPos = SCREEN_WIDTH / 2;
                ballXSpeed *= -1;
            }
        }
        ballXPos += ballXSpeed;
    }

    
    else if (ballXSpeed < 0) //ball going LEFT
    {
        if (ballXPos <= 1 || hitPaddle)
        {

          //if hitpaddle, change direction
            if(hitPaddle)
            {
                ballXSpeed *= -1;
            }
            //else add score to P2
            else
            {
                player2Score ++;
                ballXPos = SCREEN_WIDTH/2;
                ballXSpeed *= -1;
            }
        }
        ballXPos += ballXSpeed;
    }
}

bool hitPaddle(const unsigned int ballYPos, const unsigned int ballXPos, wchar_t* screen) {
    int index = SCREEN_WIDTH * ballYPos + ballXPos;
    // int RightUpIndex = SCREEN_WIDTH * (ballYPos + 1) + ballXPos -1;
    // int RightDownIndex = SCREEN_WIDTH * (ballYPos -1) * ballXPos -1;
    // int LeftUpIndex = SCREEN_WIDTH * (ballYPos + 1) + ballXPos + 1;
    // int LeftDownIndex = SCREEN_WIDTH * (ballYPos -1) * ballXPos + 1;
    //
    // if (screen[index] == L'<' || screen[RightUpIndex] == L'<' || screen[RightDownIndex] == L'<')
    // {
    //     return true;
    // }
    //
    // if(screen[index] == L'>' || screen[LeftDownIndex] == L'>'  || screen[LeftUpIndex] == L'>')
    // {
    //     return true;
    // }

    if (screen[index] == L'<' || screen[index] == L'>' )
    {
        return true;
    }
    return false;
}

void printPlayerScores(wchar_t* screen)
{
    //convert to wchar_t*
    std::string p1text = "P1   ";
    p1text[4] = '0' + player1Score; 
    const size_t newsize1 = (p1text.size() +1);
    size_t convertedChars1 = 0;
    wchar_t* p1w = new wchar_t[newsize1];
    mbstowcs_s(&convertedChars1,p1w, newsize1, p1text.c_str(), _TRUNCATE);

    std::string p2text = "P2   ";
    p2text[4] = '0' + player2Score;
    const size_t newsize2 = (p2text.size() +1);
    size_t convertedChars2 = 0;
    wchar_t* p2w = new wchar_t[newsize2];
    mbstowcs_s(&convertedChars2,p2w, newsize2, p2text.c_str(), _TRUNCATE);
    
    
    //Player One
    int p1StartIndex = SCORE_Y_POSITION * SCREEN_WIDTH + SCORE_X_POSITION;
    for (int i=0; i < 5; i++)
    {
        screen[p1StartIndex + i] = p1w[i];
    }
    //screen[SCORE_Y_POSITION * SCREEN_WIDTH + SCORE_X_POSITION - 1] = L'P';
    //screen[SCORE_Y_POSITION * SCREEN_WIDTH + SCORE_X_POSITION] = L'1';
    //*(screen + (SCORE_Y_POSITION) * SCREEN_WIDTH + SCORE_X_POSITION + 2) = ;
    

    //Player Two
    int p2StartIndex = (SCORE_Y_POSITION + 1) * SCREEN_WIDTH - SCORE_X_POSITION - 5;
   for (int i=0; i < 5; i++)
    {
       screen[p2StartIndex + i] = p2w[i];
    }
   // screen[(SCORE_Y_POSITION + 1) * SCREEN_WIDTH - SCORE_X_POSITION - 2 ] = p2Score;
    //screen[(SCORE_Y_POSITION + 1) * SCREEN_WIDTH - SCORE_X_POSITION] = L'P';
    //screen[(SCORE_Y_POSITION + 1) * SCREEN_WIDTH - SCORE_X_POSITION + 1] = L'2';
}

void checkWinningConditions(wchar_t* screen, bool& GameOver)
{

    std::string p1text = "P1 WINS!";
    const size_t newsize1 = (p1text.size() +1);
    size_t convertedChars1 = 0;
    wchar_t* p1wch = new wchar_t[newsize1];
    mbstowcs_s(&convertedChars1,p1wch, newsize1, p1text.c_str(), _TRUNCATE);

    std::string p2text = "P2 WINS!";
    const size_t newsize2 = (p2text.size() +1);
    size_t convertedChars2 = 0;
    wchar_t* p2wch = new wchar_t[newsize2];
    mbstowcs_s(&convertedChars2,p2wch, newsize2, p2text.c_str(), _TRUNCATE);


    if (player1Score >= WINNING_SCORE)
    {
        GameOver = true;
        printPlayerOneWon(screen, p1wch, std::wcslen(p1wch));
    }
    if (player2Score >= WINNING_SCORE)
    {
        GameOver = true;
        printPlayerTwoWon(screen, p2wch, 8);
    }
}

void printPlayerOneWon(wchar_t* screen, const wchar_t* text, const unsigned int textLength)
{
    int p1StartIndex = SCORE_Y_POSITION * SCREEN_WIDTH + SCORE_X_POSITION;
    for (int i=0; i < textLength; i++)
    {
        screen[p1StartIndex + i] = text[i];
    }

}

void printPlayerTwoWon(wchar_t* screen, const wchar_t* text, const unsigned int textLength)
{

    int p2StartIndex = SCORE_Y_POSITION * SCREEN_WIDTH - SCORE_X_POSITION - textLength;
    for (int i=0; i < textLength; i++)
    {
        screen[p2StartIndex + i] = text[i];
    }
}