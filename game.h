#ifndef GAME_H
#define GAME_H

#include "config.h"



int InitSDL_2(GameContext* ctx);
int InitSDL(GameContext* ctx);
void CleanupSDL(GameContext* ctx);
void UpdateTime(double* worldTime, int* tick1);
void GenerateDot(Snake* snake, Dot* dot);
int CheckDotCollision(Snake* snake, Dot* dot);
void IncreaseSnake(Snake* snake);
void InitSnake(Snake* snake);
void LimitsSnake(Snake* snake);
void MoveSnake(Snake* snake);
int CheckCollision(Snake* snake);
void KeyOperation(Snake* snake, SDL_Event* event, int* running, double* worldTime);
void HandleGameOver(GameContext* ctx, Snake* snake, SDL_Event* event, int* running, double* worldTime, int* gameOver);
void CheckSpeedUp(int* gameSpeed, double* worldTime, int* lastSpeedUp);
void GenerateBonusDot(BonusDot* bonus, Snake* snake, double worldTime);
void CheckBonusDotCollision(Snake* snake, BonusDot* bonus, int* gameSpeed, int* points);
void UpdateBonus(BonusDot* bonus, double worldTime);
void IncreasePoints(int* points);
int MainLoop(GameContext* ctx, Snake* snake, Dot* dot, BonusDot* bonus);

#endif 