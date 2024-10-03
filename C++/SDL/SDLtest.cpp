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

    renderer = SDL_CreateRenderer(window, -1, 0);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    imageSurface = IMG_Load("images/num_0.jpg");
    texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    destRect = { 0, 0, imageSurface->w, imageSurface->h };

    // 메시지 루프
    SDL_Event event;
    int quit = 0;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = 1;
                }
                break;
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_MOUSEBUTTONDOWN:
                //std::cout << "Mouse button pressed: " << static_cast<int>(event.button.button) << std::endl;
                //std::cout << "Mouse position: (" << event.button.x << ", " << event.button.y << ")" << std::endl;
                printf("Mouse position: (%d, %d)\n", event.button.x, event.button.y);
                break;
            case SDL_MOUSEBUTTONUP:
                //std::cout << "Mouse button released: " << static_cast<int>(event.button.button) << std::endl;
                //std::cout << "Mouse position: (" << event.button.x << ", " << event.button.y << ")" << std::endl;
                break;
            case SDL_MOUSEMOTION:
                //std::cout << "Mouse moved: (" << event.motion.x << ", " << event.motion.y << ")" << std::endl;
                break;
            default:
                break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        SDL_RenderCopy(renderer, texture, NULL, &destRect);
        SDL_RenderPresent(renderer);
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