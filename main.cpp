#include "config.h"
#include "game.h"
#include "render.h"



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

	BonusDot* bonus = (BonusDot*)malloc(sizeof(BonusDot));
	if (bonus == NULL) {
		printf("Memory allocation faild (BonusDot)");
		return 1;
	}

	
	MainLoop(ctx, snake, dot, bonus);

	//Sprz¹tanie po grze
	CleanupSDL(ctx);
	free(ctx);
	free(snake);
	free(dot);
	free(bonus);
	return 0;
}
