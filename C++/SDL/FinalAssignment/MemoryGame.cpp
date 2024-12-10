#include <stdio.h>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_messagebox.h>
#include <time.h>
#include <stdlib.h>
#include "MemoryGame.h"
using namespace std;
#include <string>

int runMemoryGame(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_Texture* texture = NULL;
    SDL_Rect destRect;

    // 기존 Card6Memory 대신 DerivedCard 사용
    DerivedCard leftCards[8];  // CPU 카드
    DerivedCard rightCards[8]; // User 카드

    int card[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    int selectedCard[8] = {0};
    int cardtemp[8];
    int i, j, temp1, temp2;

    int cpuSpeed;
    int diffiNum;
    string difficulty;

    cout << "난이도를 선택하시오. ( 장정훈 | 최상 | 상 | 중 | 하 ): ";
    cin >> difficulty;
    if (difficulty == "장정훈") diffiNum = 1;
    else if (difficulty == "최상") diffiNum = 2;
    else if (difficulty == "상") diffiNum = 3;
    else if (difficulty == "중") diffiNum = 4;
    else diffiNum = 5;

    switch (diffiNum) {
        case (1):
            cpuSpeed = 100;
            break;
        case (2):
            cpuSpeed = 500;
            break;
        case (3):
            cpuSpeed = 1000;
            break;
        case (4):
            cpuSpeed = 2000;
            break;
        case (5):
            cpuSpeed = 3000;
            break;
    }

    srand((unsigned int)time(NULL));

    for (i = 0; i < 8; i++) cardtemp[i] = rand();

    for (i = 7; i > 0; i--) {
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

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    char cardImage[8][23] = {
        "images/character_1.png", "images/character_2.png", "images/character_3.png", "images/character_4.png",
        "images/character_5.png", "images/character_6.png", "images/character_7.png", "images/character_8.png"};

    SDL_Surface* imageSurface[8];
    SDL_Surface* backSurface = IMG_Load("images/character_b.png");
    SDL_Surface* wrongSurface = IMG_Load("images/character_x.png");

    SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
    SDL_Texture* wrongTexture = SDL_CreateTextureFromSurface(renderer, wrongSurface);

    // 왼쪽 영역에 8개의 카드 랜덤 생성
    for (int i = 0; i < 8; i++) {
        imageSurface[i] = IMG_Load(cardImage[card[i]]);
        leftCards[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
        leftCards[i].cardValue = card[i];
        selectedCard[i] = 0;

        bool positionSet = false;
        while (!positionSet) {
            leftCards[i].rect = {rand() % 520, rand() % 620, imageSurface[i]->w, imageSurface[i]->h};

            bool collision = false;
            for (int j = 0; j < i; j++) {
                if (SDL_HasIntersection(&leftCards[i].rect, &leftCards[j].rect)) {
                    collision = true;
                    break;
                }
            }
            if (!collision) {
                positionSet = true;
            }
        }
    }

    // 오른쪽 영역에 8개의 카드 생성 (왼쪽 카드의 x 좌표 + 700)
    for (int i = 0; i < 8; i++) {
        rightCards[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
        rightCards[i].cardValue = card[i];
        rightCards[i].rect = {leftCards[i].rect.x + 700, leftCards[i].rect.y, imageSurface[i]->w, imageSurface[i]->h};
    }

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, NULL, "1초 뒤 글자가 가려집니다.", window);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // 세로 선 그리기
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 검은색 선
    SDL_RenderDrawLine(renderer, 700, 0, 700, 800); // x=700에서 위에서 아래로

    // 왼쪽 카드와 오른쪽 카드 모두 그리기
    for (int i = 0; i < 8; i++) {
        SDL_RenderCopy(renderer, leftCards[i].texture, NULL, &leftCards[i].rect);
        SDL_RenderCopy(renderer, rightCards[i].texture, NULL, &rightCards[i].rect);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(1000);  // 카드 가려지는 시간 설정.

    for (int i = 0; i < 8; i++) {
        // 왼쪽 카드 뒷면으로 변경
        SDL_DestroyTexture(leftCards[i].texture);
        SDL_FreeSurface(imageSurface[i]);
        imageSurface[i] = IMG_Load("images/character_b.png");
        leftCards[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);

        // 오른쪽 카드 뒷면으로 변경
        SDL_DestroyTexture(rightCards[i].texture);
        rightCards[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
    }

    SDL_Event event;
    int quit = 0;
    int currentTarget = 0;
    int leftRevealIndex = 0;
    int lastLeftRevealTime = 0;     // 왼쪽 카드가 마지막으로 열렸던 시간
    int leftRevealInterval = cpuSpeed;   // 왼쪽 카드가 1000ms마다 열리도록 설정

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = 1;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                for (int i = 0; i < 8; i++) {
                    if ((x >= rightCards[i].rect.x) && (x <= rightCards[i].rect.x + rightCards[i].rect.w) &&
                        (y >= rightCards[i].rect.y) && (y <= rightCards[i].rect.y + rightCards[i].rect.h)) {
                        if (!selectedCard[i]) {
                            if (rightCards[i].cardValue == currentTarget) {
                                selectedCard[i] = 1;
                                SDL_DestroyTexture(rightCards[i].texture);
                                SDL_FreeSurface(imageSurface[i]);
                                imageSurface[i] = IMG_Load(cardImage[rightCards[i].cardValue]);
                                rightCards[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
                                currentTarget++;
                                if (currentTarget == 8) {
                                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, NULL, "너 재능있어.", window);
                                    quit = 1;
                                }
                            } else {
                                SDL_DestroyTexture(rightCards[i].texture);
                                SDL_FreeSurface(imageSurface[i]);
                                imageSurface[i] = IMG_Load("images/character_x.png");
                                rightCards[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);

                                SDL_RenderClear(renderer);
                                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                                for (int j = 0; j < 8; j++) {
                                    SDL_RenderCopy(renderer, rightCards[j].texture, NULL, &rightCards[j].rect);
                                    SDL_RenderCopy(renderer, leftCards[j].texture, NULL, &leftCards[j].rect);
                                }
                                SDL_RenderPresent(renderer);
                                SDL_Delay(500);

                                SDL_DestroyTexture(rightCards[i].texture);
                                SDL_FreeSurface(imageSurface[i]);
                                imageSurface[i] = IMG_Load("images/character_b.png");
                                rightCards[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
                            }
                        }
                        break;
                    }
                }
            }
        }

        // 왼쪽 카드 자동 열기 (SDL_GetTicks()를 사용하여 일정한 간격으로 열림)
        int currentTime = SDL_GetTicks();
        if (leftRevealIndex < 8 && (currentTime - lastLeftRevealTime > leftRevealInterval)) {
            lastLeftRevealTime = currentTime;

            SDL_DestroyTexture(leftCards[leftRevealIndex].texture);
            SDL_FreeSurface(imageSurface[leftRevealIndex]);
            imageSurface[leftRevealIndex] = IMG_Load(cardImage[leftRevealIndex]);
            leftCards[leftRevealIndex].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[leftRevealIndex]);

            leftRevealIndex++;
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // 카드 그리기: 왼쪽 카드와 오른쪽 카드
        for (int i = 0; i < 8; i++) {
            SDL_RenderCopy(renderer, leftCards[i].texture, NULL, &leftCards[i].rect);
            SDL_RenderCopy(renderer, rightCards[i].texture, NULL, &rightCards[i].rect);
        }
        SDL_RenderPresent(renderer);

        if (leftRevealIndex == 8) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, NULL, "실력이 형편없군요", window);
            quit = 1;
        }
    }

    // 텍스처 및 서피스 정리
    for (int i = 0; i < 8; i++) {
        SDL_DestroyTexture(leftCards[i].texture);
        SDL_DestroyTexture(rightCards[i].texture);
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
