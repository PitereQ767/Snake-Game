#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH 720
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

#define SNAKE_INITIAL_LENGTH 6
#define SNAKE_MAX_LENGTH 100

#define SPEEDUP_TIME 15 // czas po którym nastêpuje przyœpieszenie w s
#define SPEEDUP_FACTOR 20 // wspo³czynnik o jaki przyœpiesza gra w %
#define GAME_SPEED 100 // pocz¹tkowa prêdkoœæ gry
#define MIN_GAME_SPEED 50


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
}BonusDot;


// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

void DrawGrid(GameContext* ctx) {
	for (int x = GRID_LEFT; x <= GRID_LEFT + GRID_HEIGHT; x += CELL_SIZE) {
		DrawLine(ctx->screen, x, GRID_TOP, GRID_HEIGHT, 0, 1, BIALY);
	}

	for (int y = GRID_TOP; y <= GRID_TOP + GRID_HEIGHT; y += CELL_SIZE) {
		DrawLine(ctx->screen, GRID_LEFT, y, GRID_WIDTH, 1, 0, BIALY); 
	}
}

void Rendering(GameContext* ctx) {
	SDL_UpdateTexture(ctx->scrtex, NULL, ctx->screen->pixels, ctx->screen->pitch);
	SDL_RenderClear(ctx->renderer);
	SDL_RenderCopy(ctx->renderer, ctx->scrtex, NULL, NULL);
	SDL_RenderPresent(ctx->renderer);
}


void ShowStat(GameContext* ctx, double worldTime) {
	char text[128];
	DrawRectangle(ctx->screen, 4, 4, SCREEN_WIDTH - 8, 36, BIALY, NIEBIESKI);
	sprintf(text, "Snake Game, game duration = %.1lf s", worldTime);
	DrawString(ctx->screen, ctx->screen->w / 2 - strlen(text) * 8 / 2, 8, text, ctx->charset);
	sprintf(text, "Implemented requirements: 1, 2, 3, 4, A, B");
	DrawString(ctx->screen, ctx->screen->w / 2 - strlen(text) * 8 / 2, 25, text, ctx->charset);

	DrawRectangle(ctx->screen, LEFT_BORDER, TOP_BORDER, GRID_WIDTH, GRID_HEIGHT, BIALY, CZARNY); // rysowanie pola gry
}

int InitSDL_2(GameContext* ctx) {
	ctx->scrtex = SDL_CreateTexture(ctx->renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (ctx->scrtex == NULL) {
		printf("SDL_CreateTexture error: %s\n", SDL_GetError());
		SDL_FreeSurface(ctx->screen);
		SDL_DestroyRenderer(ctx->renderer);
		SDL_DestroyWindow(ctx->window);
		SDL_Quit();
		return 1;
	}

	// Wy³¹czenie kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// Wczytanie charsetu
	ctx->charset = SDL_LoadBMP("./cs8x8.bmp");
	if (ctx->charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(ctx->screen);
		SDL_DestroyTexture(ctx->scrtex);
		SDL_DestroyRenderer(ctx->renderer);
		SDL_DestroyWindow(ctx->window);
		SDL_Quit();
		return 1;
	}

	SDL_SetColorKey(ctx->charset, true, 0x000000);

	return 0;
}

int InitSDL(GameContext *ctx) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &ctx->window, &ctx->renderer) != 0) {
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// Ustawienia renderera
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(ctx->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
	SDL_SetWindowTitle(ctx->window, "Gra Snake");

	// Tworzenie powierzchni i tekstury
	ctx->screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	if (ctx->screen == NULL) {
		printf("SDL_CreateRGBSurface error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(ctx->renderer);
		SDL_DestroyWindow(ctx->window);
		SDL_Quit();
		return 1;
	}

	if (InitSDL_2(ctx)) {
		return 1;
	}

	return 0;
}


void CleanupSDL(GameContext* ctx) {
	if (ctx->charset) SDL_FreeSurface(ctx->charset);
	if (ctx->screen) SDL_FreeSurface(ctx->screen);
	if (ctx->scrtex) SDL_DestroyTexture(ctx->scrtex);
	if (ctx->renderer) SDL_DestroyRenderer(ctx->renderer);
	if (ctx->window) SDL_DestroyWindow(ctx->window);
	SDL_Quit();
}

void UpdateTime(double* worldTime, int* tick1) {
	int tick2 = SDL_GetTicks();
	int delta = tick2 - *tick1;
	*worldTime += delta / 1000.0;
	*tick1 = tick2;
}

void GenerateDot(Snake* snake, Dot* dot) {
	int valid = 0;

	while (!valid) {
		dot->x = GRID_LEFT + (rand() % (GRID_WIDTH / CELL_SIZE)) * CELL_SIZE;
		dot->y = GRID_TOP + (rand() % (GRID_HEIGHT / CELL_SIZE)) * CELL_SIZE;

		valid = 1;
		for (int i = 0; i < snake->length; i++) {
			if (snake->x[i] == dot->x && snake->y[i] == dot->y) {
				valid = 0;
				break;
			}
		}
	}
}

void DrawDot(GameContext* ctx, Dot* dot) {
	DrawRectangle(ctx->screen, dot->x, dot->y, CELL_SIZE, CELL_SIZE, NIEBIESKI, NIEBIESKI);
}

int CheckDotCollision(Snake* snake, Dot* dot) {
	if (snake->x[0] == dot->x && snake->y[0] == dot->y) {
		return 1;
	}
	return 0;
}

void IncreaseSnake(Snake* snake) {
	if (snake->length < SNAKE_MAX_LENGTH) {
		snake->length++;

		snake->x[snake->length - 1] = snake->x[snake->length - 2];
		snake->y[snake->length - 1] = snake->y[snake->length - 2];
	}
}

void InitSnake(Snake* snake) {
	int startX = GRID_LEFT + (GRID_WIDTH / 2 / CELL_SIZE) * CELL_SIZE;
	int startY = GRID_TOP + (GRID_HEIGHT / 2 / CELL_SIZE) * CELL_SIZE;

	snake->length = SNAKE_INITIAL_LENGTH;
	snake->direction = RIGHT;

	for (int i = 0; i < snake->length; i++) {
		snake->x[i] = startX - i * CELL_SIZE;
		snake->y[i] = startY;
	}
}

void DrawSnake(GameContext* ctx, Snake* snake) {
	for (int i = 0; i < snake->length; i++) {
		DrawRectangle(ctx->screen, snake->x[i], snake->y[i], CELL_SIZE, CELL_SIZE, ZIELONY, ZIELONY);
	}
}

void LimitsSnake(Snake* snake) {
	if (snake->y[0] <= TOP_BORDER  && snake->direction == UP) {
		snake->direction = RIGHT;
	}
	else if (snake->x[0] >= RIGHT_BORDER - CELL_SIZE && snake->direction == RIGHT) {
		snake->direction = DOWN;
	}
	else if (snake->y[0] >= BOTTOM_BORDER - CELL_SIZE && snake->direction == DOWN) {
		snake->direction = LEFT;
	}
	else if (snake->x[0] <= LEFT_BORDER && snake->direction == LEFT) {
		snake->direction = UP;
	}
}

void MoveSnake(Snake* snake) {
	LimitsSnake(snake);

	for (int i = snake->length - 1; i > 0; i--) {
		snake->x[i] = snake->x[i - 1];
		snake->y[i] = snake->y[i - 1];
	}

	switch (snake->direction){
		case UP:
			snake->y[0] -= CELL_SIZE;
			break;
		case RIGHT:
			snake->x[0] += CELL_SIZE;
			break;
		case DOWN:
			snake->y[0] += CELL_SIZE;
			break;
		case LEFT:
			snake->x[0] -= CELL_SIZE;
			break;
	}

}

int CheckCollision(Snake* snake) {
	for (int i = 1; i < snake->length; i++) {
		if (snake->x[0] == snake->x[i] && snake->y[0] == snake->y[i]) {
			return 1;
		}
	}
	return 0;
}

void KeyOperation(Snake* snake, SDL_Event* event, int* running, double* worldTime) {

	while (SDL_PollEvent(event)) {
		if (event->type == SDL_QUIT) {
			*running = 0;
		}
		else if (event->type == SDL_KEYDOWN) {
			switch (event->key.keysym.sym) {
			case SDLK_ESCAPE:
				*running = 0;
				break;
			case SDLK_n:
				*worldTime = 0;
				InitSnake(snake);
				break;
			case SDLK_UP:
				if (snake->direction != DOWN) {
					snake->direction = UP;
				}
				break;
			case SDLK_RIGHT:
				if (snake->direction != LEFT) {
					snake->direction = RIGHT;
				}
				break;
			case SDLK_DOWN:
				if (snake->direction != UP) {
					snake->direction = DOWN;
				}
				break;
			case SDLK_LEFT:
				if (snake->direction != RIGHT) {
					snake->direction = LEFT;
				}
				break;
			}
		}

	}
}

void GameOverMessage(GameContext* ctx) {
	char text[128];
	DrawRectangle(ctx->screen, 4, 220, SCREEN_WIDTH - 8, 30, CZARNY, CZERWONY);
	sprintf(text, "Game over! Press 'n' to play again or 'Esc' to exit.");
	DrawString(ctx->screen, ctx->screen->w / 2 - strlen(text) * 8 / 2, 230, text, ctx->charset);
}

void HandleGameOver(GameContext* ctx, Snake* snake, SDL_Event* event, int* running, double* worldTime, int* gameOver) {
	GameOverMessage(ctx);

	while (SDL_PollEvent(event)) {
		if (event->type == SDL_QUIT) {
			*running = 0;
		}
		else if (event->type == SDL_KEYDOWN) {
			switch (event->key.keysym.sym) {
				case SDLK_ESCAPE:
					*running = 0;
					break;
				case SDLK_n:
					*worldTime = 0;
					*gameOver = 0;
					InitSnake(snake);
					break;
			}
		}
	}
}

void CheckSpeedUp(int* gameSpeed, double* worldTime, int* lastSpeedUp) {
	int currentTime = (int)(*worldTime);

	if (currentTime % SPEEDUP_TIME == 0 && *lastSpeedUp < currentTime) {
		*gameSpeed -= SPEEDUP_FACTOR;

		if (*gameSpeed < MIN_GAME_SPEED) {
			*gameSpeed = MIN_GAME_SPEED;
		}


		*lastSpeedUp = currentTime;
	}
}


int MainLoop(GameContext* ctx, Snake* snake, Dot* dot) {
	int running = 1;
	int tick1 = SDL_GetTicks();
	double worldTime = 0;
	int gameOver = 0;
	int gameSpeed = GAME_SPEED;
	int lastSpeedUp = 0;
	SDL_Event event;

	while (running) {
		SDL_FillRect(ctx->screen, NULL, CZARNY);
		UpdateTime(&worldTime, &tick1);
		CheckSpeedUp(&gameSpeed, &worldTime, &lastSpeedUp);

		if (!gameOver) {
			/*DrawGrid(ctx);*/ // siatka po której porusza siê w¹¿
			ShowStat(ctx, worldTime);
			KeyOperation(snake, &event, &running, &worldTime);
			MoveSnake(snake);
			
			if (CheckCollision(snake)) {
				gameOver = 1;
			}

			if (CheckDotCollision(snake, dot)) {
				IncreaseSnake(snake);
				GenerateDot(snake, dot);
			}
			DrawDot(ctx, dot);
			DrawSnake(ctx, snake);
		}
		else
		{
			HandleGameOver(ctx, snake, &event, &running, &worldTime, &gameOver);
			lastSpeedUp = 0;
			gameSpeed = GAME_SPEED;
		}

		
		
		Rendering(ctx);
		SDL_Delay(gameSpeed);
	}

	return 0;
}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	srand(time(NULL));
	GameContext* ctx = (GameContext*)malloc(sizeof(GameContext));
	if (ctx == NULL) {
		printf("Memory allocation faild (GameContext)\n");
		return 1;
	}

	if (InitSDL(ctx)) {
		printf("SDL initialization error\n");
		return 1;
	}

	Snake* snake = (Snake*)malloc(sizeof(Snake));
	if (snake == NULL) {
		printf("Memory allocation faild (Snake)\n");
		return 1;
	}
	InitSnake(snake);

	Dot* dot = (Dot*)malloc(sizeof(Dot));
	if (dot == NULL) {
		printf("Memory allocation faild (Dot)\n");
		return 1;
	}
	GenerateDot(snake, dot);

	
	MainLoop(ctx, snake, dot);


	CleanupSDL(ctx);
	free(ctx);
	free(snake);
	free(dot);
	return 0;
}
