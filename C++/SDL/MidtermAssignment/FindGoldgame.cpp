//주인공이 금을 찾으면 게임 종료. 중간마다 몬스터가 랜덤하게 움직인다. 몬스터를 피해 금을 찾아라!
#include <stdio.h>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Surface* imageSurface = NULL;
    SDL_Texture* texture = NULL;
    SDL_Rect destRect;

    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        return 1;
    }

    // 윈도우 창 생성
    window = SDL_CreateWindow("SDL2 Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_SHOWN);

    if (!window) {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }



    // 종료
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(imageSurface);

    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}