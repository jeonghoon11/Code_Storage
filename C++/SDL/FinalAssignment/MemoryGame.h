#include <SDL.h>

typedef struct {
    SDL_Rect rect;
    int dx, dy;
    SDL_Texture* texture;
    int cardValue;
} Card6Memory;

int runMemoryGame(SDL_Window* window, SDL_Renderer* renderer);