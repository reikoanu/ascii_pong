// aurbano
// main.cpp

#pragma once

void drawBlankField(wchar_t* screen);
void drawVerticalBars(wchar_t* screen);
void drawHorizontalBorders(wchar_t* screen);
void handlePaddleOneMovement(wchar_t* screen, const bool bKey[4]);
void handlePaddleTwoMovement(wchar_t* screen, const bool bKey[4]);
void drawPaddleOne(wchar_t* screen);
void drawPaddleTwo(wchar_t* screen);
void repositionBallVertically(unsigned int& ballYPos, int& ballYSpeed);
void repositionBallHorizontally(unsigned int& ballXPos, int& ballXSpeed, bool hitPaddle);
bool hitPaddle(const unsigned ballYPos,const unsigned ballXPos, wchar_t* screen);
void printPlayerScores(wchar_t* screen);
void checkWinningConditions(wchar_t* screen, bool& bGameOver);
void printPlayerOneWon(wchar_t* screen, const wchar_t* text, const unsigned int textLength);
void printPlayerTwoWon(wchar_t* screen, const wchar_t* text, const unsigned int textLength);