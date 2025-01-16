
#include "config.h"
#include "render.h"

// narysowanie napisu txt na powierzchni screen, zaczynajπc od punktu (x, y)
// charset to bitmapa 128x128 zawierajπca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
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
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};

// rysowanie linii o d≥ugoúci l w pionie (gdy dx = 0, dy = 1) 
// bπdü poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};
// rysowanie prostokπta o d≥ugoúci bokÛw l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
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
	sprintf(text, "Implemented requirements: 1, 2, 3, 4, A, B, C, D");
	DrawString(ctx->screen, ctx->screen->w / 2 - strlen(text) * 8 / 2, 25, text, ctx->charset);

	DrawRectangle(ctx->screen, LEFT_BORDER, TOP_BORDER, GRID_WIDTH, GRID_HEIGHT, BIALY, CZARNY); // rysowanie pola gry
}

void DrawDot(GameContext* ctx, Dot* dot) {
	DrawRectangle(ctx->screen, dot->x, dot->y, CELL_SIZE, CELL_SIZE, NIEBIESKI, NIEBIESKI);
}

void DrawSnake(GameContext* ctx, Snake* snake) {
	for (int i = 0; i < snake->length; i++) {
		DrawRectangle(ctx->screen, snake->x[i], snake->y[i], CELL_SIZE, CELL_SIZE, ZIELONY, ZIELONY);
	}
}

void GameOverMessage(GameContext* ctx) {
	char text[128];
	DrawRectangle(ctx->screen, 4, 220, SCREEN_WIDTH - 8, 30, CZARNY, CZERWONY);
	sprintf(text, "Game over! Press 'n' to play again or 'Esc' to exit.");
	DrawString(ctx->screen, ctx->screen->w / 2 - strlen(text) * 8 / 2, 230, text, ctx->charset);
}

void DrawBonusDot(GameContext* ctx, BonusDot* bonus) {
	DrawRectangle(ctx->screen, bonus->x, bonus->y, CELL_SIZE, CELL_SIZE, CZERWONY, CZERWONY);
}

void DrawProgressBar(GameContext* ctx, BonusDot* bonus) {
	char text[128];
	DrawRectangle(ctx->screen, RIGHT_BORDER + 40, TOP_BORDER, PROGRES_BAR_WIDTH, 20, BIALY, CZARNY);
	sprintf(text, "Bonus active time");
	DrawString(ctx->screen, RIGHT_BORDER + 52, TOP_BORDER + 6, text, ctx->charset);
	if (bonus->active) {
		double progress = (BONUS_DURATION - bonus->timer) / (double)BONUS_DURATION;
		DrawBonusDot(ctx, bonus);
		int fillWidth = (int)(progress * PROGRES_BAR_WIDTH);
		if (fillWidth > 0) {
			DrawRectangle(ctx->screen, RIGHT_BORDER + 40, TOP_BORDER + 21, fillWidth, CELL_SIZE, CZERWONY, CZERWONY);
		}

	}
}

void DrawPoints(GameContext* ctx, int points) {
	char text[128];
	DrawRectangle(ctx->screen, RIGHT_BORDER + 40, TOP_BORDER + 50, PROGRES_BAR_WIDTH, CELL_SIZE, BIALY, CZARNY);
	sprintf(text, "Points: %d", points);
	DrawString(ctx->screen, RIGHT_BORDER + 85, TOP_BORDER + 56, text, ctx->charset);
}