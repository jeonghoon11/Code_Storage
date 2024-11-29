#include <stdio.h> 
#include <iostream>
#include <SDL.h> 
#include <SDL_image.h>
#include <SDL_messagebox.h>
#include <time.h>
#include <stdlib.h>
#include "MemoryGame.h"

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;
    SDL_Rect destRect;

    Card6Memory card6Game[6];

    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        return 1;
    }

    // 윈도우 창 생성
    window = SDL_CreateWindow("SDL2 Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1200, 800,
        SDL_WINDOW_SHOWN);
		
    if (!window) {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }   //여기까지 SDL 윈도우 생성 코드

    int card[6] = { 0, 1, 2, 3, 4, 5};
    int selectedCard[6] = { 0 };   // 이미 맞힌 카드 여부 저장.
    int cardtemp[6];
    int i, j, temp1, temp2;

    srand((unsigned int)time(NULL));

    for (i = 0; i < 6; i++) cardtemp[i] = rand();

    for (i = 5; i > 0; i--) {
        for (j = 0; j < i; j++) {
            if (cardtemp[j] > cardtemp[j + 1]) {
                temp1 = cardtemp[j];
                cardtemp[j] = cardtemp[j + 1];
                cardtemp[j + 1] = temp1;

                temp2 = card[j];
                card[j] = card[j + 1];
                card[j + 1] = temp2;
            }
        }
    }

    //window : 렌더링할 창, -1 : SDL 드라이버 최적 선택, SDL_REN~ : 렌더링 성능 향상
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);  //이미지 초기화

    char cardImage[6][23] = {
        "images/character_1.png",
        "images/character_2.png",
        "images/character_3.png",
        "images/character_4.png",
        "images/character_5.png",
        "images/character_6.png"
    };

    SDL_Surface* imageSurface[6];
    SDL_Surface* backSurface = IMG_Load("images/character_b.png");
    SDL_Surface* wrongSurface = IMG_Load("images/character_x.png");

    SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
    SDL_Texture* wrongTexture = SDL_CreateTextureFromSurface(renderer, wrongSurface);

    int currentTarget = 0;

    for (int i = 0; i < 6; i++) {   //카드 위치 윈도우창에 랜덤으로 정하기
        imageSurface[i] = IMG_Load(cardImage[card[i]]);
        card6Game[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
        card6Game[i].cardValue = card[i]; // 카드의 실제 값 저장
        selectedCard[i] = 0;
        
        bool positionSet = false;   //  겹치는 부분이 있는지 확인하기 위함
        while (!positionSet) {
            card6Game[i].rect = { rand() % 1020, rand() % 620, imageSurface[i] -> w, imageSurface[i] -> h };    //위치 랜덤 생성
            
            //이전에 생성된 카드들과 겹치는지 확인
            bool collision = false; 
            for (int j = 0; j < i; j++) {
                if (SDL_HasIntersection(&card6Game[i].rect, &card6Game[j].rect)) {
                    collision = true;   //카드 겹침
                    break;
                }
            }
            if (!collision) {   //카드가 겹쳐지지 않았으면
                positionSet = true;
            }
        }
    }

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, NULL , "1초 뒤 글자가 가려집니다.", window);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer); // 렌더링 버퍼 초기화

    for(int i = 0; i < 6; i++) {
        SDL_RenderCopy(renderer, card6Game[i].texture, NULL, &card6Game[i].rect);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(1000);

    for( int i = 0; i < 6; i++) {
        SDL_DestroyTexture(card6Game[i].texture);
        SDL_FreeSurface(imageSurface[i]);
        imageSurface[i] = IMG_Load("images/character_b.png");
        card6Game[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
    }

    // 메시지 루프
    SDL_Event event;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
            }
            
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                for (int i = 0; i < 6; i++) {
                    if ((x >= card6Game[i].rect.x) && (x <= card6Game[i].rect.x + card6Game[i].rect.w) &&
                        (y >= card6Game[i].rect.y) && (y <= card6Game[i].rect.y + card6Game[i].rect.h)) {
                        // 카드를 선택함
                        if (!selectedCard[i]) {
                            if (card6Game[i].cardValue == currentTarget) {
                                // 올바른 카드 선택
                                selectedCard[i] = 1;
                                SDL_DestroyTexture(card6Game[i].texture);
                                SDL_FreeSurface(imageSurface[i]);
                                imageSurface[i] = IMG_Load(cardImage[card6Game[i].cardValue]); // 실제 이미지 로드
                                card6Game[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
                                currentTarget++;
                                // 모든 카드를 찾았는지 확인
                                if (currentTarget == 6) {
                                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, NULL, "너 재능있어.", window);
                                    quit = true;
                                }
                            } else {
                                // 잘못된 카드 선택
                                SDL_DestroyTexture(card6Game[i].texture);
                                SDL_FreeSurface(imageSurface[i]);
                                imageSurface[i] = IMG_Load("images/character_x.png");
                                card6Game[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
                                // 화면 업데이트
                                SDL_RenderClear(renderer);
                                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                                for (int j = 0; j < 6; j++) {
                                    SDL_RenderCopy(renderer, card6Game[j].texture, NULL, &card6Game[j].rect);
                                }
                                SDL_RenderPresent(renderer);
                                SDL_Delay(1000);
                                // 다시 뒷면 이미지로 변경
                                SDL_DestroyTexture(card6Game[i].texture);
                                SDL_FreeSurface(imageSurface[i]);
                                imageSurface[i] = IMG_Load("images/character_b.png");
                                card6Game[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
                            }
                        }
                        break; // 한 번의 클릭에 하나의 카드만 처리 break을 안 걸면 틀렸을 때 다른걸 클릭하면 딜레이가 발생해도 여러개가 클릭됩니다.
                    }
                }
            }
        }


        
        //화면 렌더링
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        for(int i = 0; i < 6; i++) {
            SDL_RenderCopy(renderer, card6Game[i].texture, NULL, &card6Game[i].rect);
        }
        SDL_RenderPresent(renderer);
    }

    // 종료
    for (int i = 0; i < 6; i++) {
        SDL_DestroyTexture(card6Game[i].texture);
        SDL_FreeSurface(imageSurface[i]);
    }
    SDL_DestroyTexture(backTexture);
    SDL_DestroyTexture(wrongTexture);
    SDL_FreeSurface(backSurface);
    SDL_FreeSurface(wrongSurface);


    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}