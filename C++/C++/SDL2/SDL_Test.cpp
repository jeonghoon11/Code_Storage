#include <SDL.h>
#include <string>

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("windowName", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    //SDL_Event event;
    //SDL_Rect rect = { 50, 50, 200, 200 };

    SDL_MessageBoxButtonData buttons[3];
    buttons[0] = { 0, 0, "no" };
    buttons[1] = { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "ok" };
    buttons[2] = { 0, 2, "meow" };
    // { 버튼플래그, 누를시 반환할 id값, 표시될 문자열 }

    SDL_MessageBoxData messageBoxData = {SDL_MESSAGEBOX_WARNING,NULL,"Error","The Quick Brown Fox Jumps Over The Lazy Dog.",SDL_arraysize(buttons),buttons,NULL};
    //{ 메시지플래그, 부모윈도우, 제목, 내용, 버튼수, 버튼데이터, 색상 }

    int buttonid; // 사용자가 누른 버튼의 id가 저장될 변수
    SDL_ShowMessageBox(&messageBoxData, &buttonid);
    std::printf("%d\n", buttonid);

    //버튼에 따른 결과 구현
    if (buttonid == 1) printf("button : ok \n");
    else if (buttonid == 2)  printf("button : meow \n");
    else if (buttonid == -1) printf("error \n");
   
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
