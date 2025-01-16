#ifndef CONFIG_H
#define CONFIG_H


#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 560

#define CELL_SIZE 20
#define GRID_WIDTH 400
#define GRID_HEIGHT 400
#define GRID_LEFT 160
#define GRID_TOP 80

#define CZARNY SDL_MapRGB(ctx->screen->format, 0x00, 0x00, 0x00)
#define ZIELONY SDL_MapRGB(ctx->screen->format, 0x00, 0xFF, 0x00)
#define CZERWONY SDL_MapRGB(ctx->screen->format, 0xFF, 0x00, 0x00)
#define NIEBIESKI SDL_MapRGB(ctx->screen->format, 0x11, 0x11, 0xCC)
#define BIALY SDL_MapRGB(ctx->screen->format, 0xFF, 0xFF, 0xFF)

#define LEFT_BORDER GRID_LEFT
#define RIGHT_BORDER (GRID_LEFT + GRID_WIDTH)
#define TOP_BORDER GRID_TOP
#define BOTTOM_BORDER (GRID_TOP + GRID_HEIGHT)

#define SNAKE_INITIAL_LENGTH 5
#define SNAKE_MAX_LENGTH 100
#define SNAKE_MIN_LENGTH 3 
#define EYE_SIZE 4
#define EYE_OFF_Y 4
#define EYE_OFF_X 4

#define SPEEDUP_TIME 15 // czas po którym nastêpuje przyœpieszenie w s
#define SPEEDUP_FACTOR 20 // wspo³czynnik o jaki przyœpiesza gra w s
#define GAME_SPEED 100 // pocz¹tkowa prêdkoœæ gry
#define MIN_GAME_SPEED 50

#define PROGRES_BAR_WIDTH 160
#define BONUS_DURATION 5
#define ACTIVE 1
#define INACTIVE 0
#define BONUS_TIME 10
#define SPEEDDOWN_FACTOR 10

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* screen;
	SDL_Surface* charset;
	SDL_Texture* scrtex;
} GameContext;

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT
}Direction;

typedef struct {
	int length;
	int x[SNAKE_MAX_LENGTH];
	int y[SNAKE_MAX_LENGTH];
	Direction direction;
}Snake;

typedef struct {
	int x;
	int y;
}Dot;

typedef struct {
	int x, y;
	int active;
	int timer;
	double startBonus;
}BonusDot;

#endif

