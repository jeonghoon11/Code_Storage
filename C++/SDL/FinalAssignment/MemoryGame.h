#include <stdio.h> 
#include <iostream>
#include <SDL.h> 
#include <SDL_image.h>
#include <SDL_messagebox.h>
#include <time.h>
#include <stdlib.h>

typedef struct {
    SDL_Rect rect;
    int dx, dy;
    SDL_Texture* texture;
    int cardValue;
} Card6Memory;