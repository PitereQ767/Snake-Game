#ifndef RENDER_H
#define RENDER_H

#include "config.h"


void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset);
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color);
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
void DrawGrid(GameContext* ctx);
void Rendering(GameContext* ctx);
void ShowStat(GameContext* ctx, double worldTime);


void DrawDot(GameContext* ctx, Dot* dot);
void DrawSnake(GameContext* ctx, Snake* snake);
void GameOverMessage(GameContext* ctx);
void DrawBonusDot(GameContext* ctx, BonusDot* bonus);
void DrawProgressBar(GameContext* ctx, BonusDot* bonus);
void DrawPoints(GameContext* ctx, int points);

#endif 
