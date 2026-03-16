//
// Created by jipe on 5/13/20.
//

#ifndef JTETRIS_MENU_H
#define JTETRIS_MENU_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include "Game.h"

struct ResolutionChoice
{
    std::string title;
    int w, h;
};

class Menu
{
public:
    Menu();
    bool Initialize();

    void RunLoop();

    void Shutdown();
private:
    void ProcessInput();
    bool LoadMenu();
    void UnloadMenu();
    void DrawMenu();
    void DrawOptions();

    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    bool mIsRunning;
    bool mGameStart;
    bool mOptions;
    bool mNetworkOptions;

    Uint32 mTicksCount;
    int mWidth;
    int mHeight;

    bool debug = true;

    TTF_Font* menutFont;
    struct nk_context *ctx;
    SDL_GLContext glContext;
    int mButtonWidth;
    int mButtonHeight;

    int i, selected_resolution;
    std::vector<ResolutionChoice> resolutions;
};



#endif //JTETRIS_MENU_H
