#include <SDL.h>

class Card6Memory {
public:
    SDL_Rect rect;     // 카드 위치 및 크기
    int dx, dy;        // 카드 이동 속도
    SDL_Texture* texture;  // 카드 이미지 텍스처
    int cardValue;     // 카드 값

    // 생성자
    Card6Memory() : dx(0), dy(0), texture(nullptr), cardValue(0) {
        rect = {0, 0, 0, 0};
    }

    // 소멸자
    ~Card6Memory() {
        if (texture != nullptr) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }
};

// 메모리 게임 함수 선언
int runMemoryGame(SDL_Window* window, SDL_Renderer* renderer);