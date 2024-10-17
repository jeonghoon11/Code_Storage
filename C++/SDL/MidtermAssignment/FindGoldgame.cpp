//주인공이 금을 찾으면 게임 종료. 중간마다 몬스터가 랜덤하게 움직인다.
// w s a d를 이용하여 주인공을 컨트롤하여 몬스터를 피해 금을 찾아라!
#include <stdio.h> 
#include <iostream>
#include <SDL.h>
#include <time.h>
#include <SDL_image.h>
#include <SDL_messagebox.h>
#include <stdlib.h>

#define WIDTH 130*2
#define HEIGHT 130*3

typedef struct {    //여러개의 이미지를 표현할 때 구조체가 필요하다고 하심.
    SDL_Rect rect;
    int dx, dy;     //왼쪽 상단의 위치값
    SDL_Texture* texture;
} FindGold;

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Surface* imageSurface = NULL;
    SDL_Texture* texture = NULL;
    SDL_Rect destRect;

    int randomMove; //몬스터 상하좌우 어디로 움직일지 결정

    srand(time(NULL));
    
    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        return 1;
    }

    // 윈도우 창 생성
    window = SDL_CreateWindow("SDL2 Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN);
		
    if (!window) {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    FindGold findGold[4];

    SDL_Surface* imageSurface1 = IMG_Load("imgs/user.jpeg");
    SDL_Surface* imageSurface2 = IMG_Load("imgs/monster.png");
    SDL_Surface* imageSurface3 = IMG_Load("imgs/monster.png");
    SDL_Surface* imageSurface4 = IMG_Load("imgs/Gold.jpeg");

    //User
    findGold[0].texture = SDL_CreateTextureFromSurface(renderer, imageSurface1);
    findGold[0].rect = { rand() % 228, rand()%358, imageSurface1->w, imageSurface1->h };  //어디에 그릴지 위치 설정
    //Monster1
    findGold[1].texture = SDL_CreateTextureFromSurface(renderer, imageSurface2);
    findGold[1].rect = { rand() % 228, rand()%358, imageSurface1->w, imageSurface1->h };
    //Monster2
    findGold[2].texture = SDL_CreateTextureFromSurface(renderer, imageSurface3);
    findGold[2].rect = { rand() % 228, rand()%358, imageSurface1->w, imageSurface1->h };
    //Gold
    findGold[3].texture = SDL_CreateTextureFromSurface(renderer, imageSurface4);
    findGold[3].rect = { rand() % 228, rand()%358, imageSurface1->w, imageSurface1->h };

    while(SDL_HasIntersection(&findGold[0].rect, &findGold[1].rect) ||
           SDL_HasIntersection(&findGold[0].rect, &findGold[2].rect)) {
         findGold[1].rect = { rand() % 228, rand()%358, imageSurface1->w, imageSurface1->h };
         findGold[2].rect = { rand() % 228, rand()%358, imageSurface1->w, imageSurface1->h };
         }
    

    int lastMonsterMoveTime = 0;    // 몬스터가 마지막으로 움직인 시간 저장
    int monsterMoveInterval = 200;  // 몬스터가 200ms마다 움직이도록 설정


    // 메시지 루프  0
    SDL_Event event;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
            switch(event.type){
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym){
                        case SDLK_a:
                            findGold[0].rect.x -= 10;
                            break;
                        case SDLK_d:
                            findGold[0].rect.x += 10;
                            break;
                        case SDLK_w:
                            findGold[0].rect.y -= 10;
                            break;
                        case SDLK_s:
                            findGold[0].rect.y += 10; 
                            break;
                    }
                break;
            }
        }

        // 몬스터의 움직임을 별도의 시간 조건으로 처리
        int currentTime = SDL_GetTicks();   //SDL_GetTicks() : 현재 시간 체크
        if (currentTime - lastMonsterMoveTime > monsterMoveInterval) {
            lastMonsterMoveTime = currentTime;
            //Monster1
            randomMove = rand() % 4;
                switch (randomMove) {
                    case 0:
                        findGold[1].rect.x += 10;
                        break;
                    case 1:
                        findGold[1].rect.x -= 10;
                        break;
                    case 2:
                        findGold[1].rect.y += 10;
                        break;
                    case 3:
                        findGold[1].rect.y -= 10;
                        break;
                }
                
                //Monster2
                randomMove = rand() % 4;
                switch (randomMove) {
                    case 0:
                        findGold[2].rect.x += 10;
                        break;
                    case 1:
                        findGold[2].rect.x -= 10;
                        break;
                    case 2:
                        findGold[2].rect.y += 10;
                        break;
                    case 3:
                        findGold[2].rect.y -= 10;
                        break;
                }
        }


        //User와 Gold가 충돌했는지 확인 
        if (SDL_HasIntersection(&findGold[0].rect, &findGold[3].rect)) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, NULL , "너 재능있어.", window);
            quit = 1;
        }
        //User와 Monster가 충돌했는지 확인
        if (SDL_HasIntersection(&findGold[0].rect, &findGold[1].rect) ||
            SDL_HasIntersection(&findGold[0].rect, &findGold[2].rect)) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, NULL , "실력이 형편없네요...", window);
            quit = 1;
           }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderCopy(renderer, findGold[1].texture, NULL, &findGold[1].rect);
        SDL_RenderCopy(renderer, findGold[2].texture, NULL, &findGold[2].rect);
        SDL_RenderCopy(renderer, findGold[3].texture, NULL, &findGold[3].rect);
        SDL_RenderCopy(renderer, findGold[0].texture, NULL, &findGold[0].rect);
        SDL_RenderPresent(renderer);
    }

    // 종료

    SDL_DestroyTexture(findGold[0].texture);
    SDL_FreeSurface(imageSurface1);

    SDL_DestroyTexture(findGold[1].texture);
    SDL_FreeSurface(imageSurface2);

    SDL_DestroyTexture(findGold[2].texture);
    SDL_FreeSurface(imageSurface3);

    SDL_DestroyTexture(findGold[3].texture);
    SDL_FreeSurface(imageSurface4);

    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}