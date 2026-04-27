#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <cmath>

#include "Tetromino.h"

class Game
{
public:

    Game(int width, int height);

    [[nodiscard]] bool Initialize();
    void Run();
    void Shutdown();

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();
    void RestartGame();

    void AddScore();
    void InsertTetromino();
    void UpdateBlocks();
    void ApplyTetrominoToMap(BlockType valueToApply);
    void RotateTetromino();
    void EmptyMap();
    bool UpdatePosition(int x, int y);

    void DrawFilledRect(const SDL_Rect& rect, SDL_Color color);
    void DrawRectOutline(const SDL_Rect& rect, SDL_Color color);
    void DrawPanel(const SDL_Rect& rect, SDL_Color fillColor, SDL_Color borderColor);
    void RenderText(TTF_Font* font, const char* text, const SDL_Rect& rect, SDL_Color color, bool centered = false);

    static constexpr int         kMapWidth  = 20;
    static constexpr int         kMapHeight = 30;
    static constexpr int         kBlockGap  = 1;
    static constexpr const char* kFontPath  = "Assets/PressStart2P-Regular.ttf";

    SDL_Window*   mWindow   = nullptr;
    SDL_Renderer* mRenderer = nullptr;
    TTF_Font*     mFont      = nullptr;
    TTF_Font*     mTitleFont = nullptr;
    TTF_Font*     mValueFont = nullptr;
    TTF_Font*     mSmallFont = nullptr;

    int    mWidth     = 0;
    int    mHeight    = 0;
    int    mBlockSize = 0;
    int    mDropSpeed = 500;
    int    mScore     = 0;
    int    mLines     = 0;
    int    mLevel     = 1;
    Uint32 mTicksCount  = 0;
    Uint32 mCurrentTime = 0;
    Uint32 mLastTime    = 0;

    bool mIsRunning     = true;
    bool mIsTetromino   = false;
    bool mGameRestarted = false;
    bool mGameStopped   = false;

    std::random_device mRandomDevice;
    std::mt19937 mRandomNumberGenerator;
    std::uniform_int_distribution<std::mt19937::result_type> mNumberDistribution;
    std::uniform_int_distribution<std::mt19937::result_type> mColorDistribution;
    std::vector<std::vector<Block>> mGameMap;
    std::unique_ptr<Tetromino>       mTetromino;
};
