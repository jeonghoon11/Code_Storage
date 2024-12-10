#include <SDL.h>

// 기본 클래스 Card6Memory
class Card8Memory {
protected: // protected로 멤버 변경
    SDL_Rect rect;     // 카드 위치 및 크기
    int dx, dy;        // 카드 이동 속도
    SDL_Texture* texture;  // 카드 이미지 텍스처
    int cardValue;     // 카드 값

public:
    // 생성자
    Card8Memory() : dx(0), dy(0), texture(nullptr), cardValue(0) {
        rect = {0, 0, 0, 0};
    }

    // 소멸자
    ~Card8Memory() {
        if (texture != nullptr) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }
};

// 파생 클래스 DerivedCard
class DerivedCard : protected Card8Memory {
public:
    // Card6Memory의 멤버를 public으로 노출
    using Card8Memory::rect;        //rect를 public으로 변경
    using Card8Memory::dx;
    using Card8Memory::dy;
    using Card8Memory::texture;
    using Card8Memory::cardValue;

    // 생성자
    DerivedCard() : Card8Memory() {}
};

// 메모리 게임 함수 선언
int runMemoryGame(SDL_Window* window, SDL_Renderer* renderer);
