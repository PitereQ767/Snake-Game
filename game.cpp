
#include "config.h"
#include "game.h"
#include "render.h"

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

int InitSDL(GameContext* ctx) {
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

void LimitsSnake(Snake* snake) {
	if (snake->y[0] <= TOP_BORDER && snake->direction == UP) {
		if (snake->x[0] >= RIGHT_BORDER - CELL_SIZE) {
			snake->direction = LEFT;
		}
		else
		{
			snake->direction = RIGHT;
		}
	}
	else if (snake->x[0] >= RIGHT_BORDER - CELL_SIZE && snake->direction == RIGHT) {
		;
		if (snake->y[0] >= BOTTOM_BORDER - CELL_SIZE) {
			snake->direction = UP;
		}
		else
		{
			snake->direction = DOWN;
		}
	}
	else if (snake->y[0] >= BOTTOM_BORDER - CELL_SIZE && snake->direction == DOWN) {
		if (snake->x[0] <= LEFT_BORDER) {
			snake->direction = RIGHT;
		}
		else
		{
			snake->direction = LEFT;
		}

	}
	else if (snake->x[0] <= LEFT_BORDER && snake->direction == LEFT) {
		if (snake->y[0] <= TOP_BORDER) {

			snake->direction = DOWN;
		}
		else
		{
			snake->direction = UP;
		}

	}
}

void MoveSnake(Snake* snake) {
	LimitsSnake(snake);

	for (int i = snake->length - 1; i > 0; i--) {
		snake->x[i] = snake->x[i - 1];
		snake->y[i] = snake->y[i - 1];
	}

	switch (snake->direction) {
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

	if (SDL_PollEvent(event)) {
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
				if (snake->direction != DOWN && snake->y[0] != TOP_BORDER) {
					snake->direction = UP;
				}
				break;
			case SDLK_RIGHT:
				if (snake->direction != LEFT && snake->x[0] != RIGHT_BORDER - CELL_SIZE) {
					snake->direction = RIGHT;
				}
				break;
			case SDLK_DOWN:
				if (snake->direction != UP && snake->y[0] != BOTTOM_BORDER - CELL_SIZE) {
					snake->direction = DOWN;
				}
				break;
			case SDLK_LEFT:
				if (snake->direction != RIGHT && snake->x[0] != LEFT_BORDER) {
					snake->direction = LEFT;
				}
				break;
			}
		}

	}
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

void GenerateBonusDot(BonusDot* bonus, Snake* snake, double worldTime) {
	int valid = 0;
	bonus->active = ACTIVE;
	bonus->timer = BONUS_DURATION;
	bonus->startBonus = worldTime;

	while (!valid) {
		bonus->x = GRID_LEFT + (rand() % (GRID_WIDTH / CELL_SIZE)) * CELL_SIZE;
		bonus->y = GRID_TOP + (rand() % (GRID_HEIGHT / CELL_SIZE)) * CELL_SIZE;

		valid = 1;
		for (int i = 0; i < snake->length; i++) {
			if (snake->x[i] == bonus->x && snake->y[i] == bonus->y) {
				valid = 0;
				break;
			}
		}
	}
}

void CheckBonusDotCollision(Snake* snake, BonusDot* bonus, int* gameSpeed, int* points) {
	if (snake->x[0] == bonus->x && snake->y[0] == bonus->y) {
		bonus->active = INACTIVE;
		*points += 1;

		if (rand() % 2 == 0) {
			if (snake->length >= SNAKE_MIN_LENGTH) {
				snake->length -= 2;
			}
		}
		else
		{
			*gameSpeed += SPEEDDOWN_FACTOR;
		}
	}
}

void UpdateBonus(BonusDot* bonus, double worldTime) {
	if (bonus->active) {
		double deltaTime = worldTime - bonus->startBonus;
		if (deltaTime >= BONUS_DURATION) {
			bonus->active = INACTIVE;
		}
		bonus->timer = BONUS_DURATION - deltaTime;
	}
}

void IncreasePoints(int* points) {
	*points += 1;
}

int MainLoop(GameContext* ctx, Snake* snake, Dot* dot, BonusDot* bonus) {
	int running = 1;
	int tick1 = SDL_GetTicks();
	double worldTime = 0;
	int gameOver = 0;
	int gameSpeed = GAME_SPEED;
	int lastSpeedUp = 0;
	SDL_Event event;
	int points = 0;

	while (running) {
		SDL_FillRect(ctx->screen, NULL, CZARNY);
		UpdateTime(&worldTime, &tick1);
		CheckSpeedUp(&gameSpeed, &worldTime, &lastSpeedUp);
		UpdateBonus(bonus, worldTime);

		if (!gameOver) {
			/*DrawGrid(ctx);*/ // siatka po której porusza siê w¹¿
			ShowStat(ctx, worldTime);
			if (!bonus->active && (int)worldTime % BONUS_TIME == 0) {
				GenerateBonusDot(bonus, snake, worldTime);
			}

			KeyOperation(snake, &event, &running, &worldTime);
			MoveSnake(snake);


			if (CheckCollision(snake)) {
				gameOver = 1;
			}

			if (CheckDotCollision(snake, dot)) {
				IncreaseSnake(snake);
				IncreasePoints(&points);
				GenerateDot(snake, dot);
			}

			CheckBonusDotCollision(snake, bonus, &gameSpeed, &points);

			DrawProgressBar(ctx, bonus);
			DrawPoints(ctx, points);
			DrawDot(ctx, dot);
			DrawSnake(ctx, snake);
		}
		else
		{
			HandleGameOver(ctx, snake, &event, &running, &worldTime, &gameOver);
			lastSpeedUp = 0;
			gameSpeed = GAME_SPEED;
			points = 0;
		}

		Rendering(ctx);
		SDL_Delay(gameSpeed);
	}

	return 0;
}