#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>
#include <vector>
#include "Game.h"

struct ResolutionChoice
{
    std::string title;
    int         width;
    int         height;
};

class Menu
{
public:
    Menu() = default;
    [[nodiscard]] bool Initialize();

    void RunLoop();

    void Shutdown();
private:
    void ProcessInput();
    [[nodiscard]] bool LoadMenu();
    void UnloadMenu();
    void DrawMenu();
    void DrawOptions();

    SDL_Window*        mWindow    = nullptr;
    struct nk_context* mCtx       = nullptr;
    SDL_GLContext      mGlContext = nullptr;

    int mWidth              = 0;
    int mHeight             = 0;
    int mButtonWidth        = 0;
    int mButtonHeight       = 0;
    int mSelectedResolution = 0;

    bool mIsRunning  = false;
    bool mGameStart  = false;
    bool mOptions    = false;

    std::vector<ResolutionChoice> mResolutions;
};
