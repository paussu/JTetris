//
// Created by jipe on 5/13/20.
//

#ifndef JTETRIS_GAME_H
#define JTETRIS_GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <bits/unique_ptr.h>
#include <random>
#include <cmath>

#include "Tetromino.h"

class Game
{
public:

    Game(int width, int height);

    bool Initialize();
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

    void RenderText(const char* text, int x, int y, int w, int h);
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* mFont;

    bool isRunning;
    bool isTetromino;
    bool gameRestarted;
    bool gameStopped;
    bool hideInfo;

    const int mMapWidth = 20;
    const int mMapHeight = 30;
    const int mBlockGap = 1;

    int mWidth, mHeight;
    int mBlockSize;
    int mDropSpeed;
    int mScore;
    int mLines;
    int mLevel;

    std::string ScoreText;
    std::string LevelText;
    std::string LinesText;
    Uint32 ticksCount;
    Uint32 currentTime;
    Uint32 lastTime;
    std::random_device mRandomDevice;
    std::mt19937 mRandomNumberGenerator;
    std::uniform_int_distribution<std::mt19937::result_type> numberDistribution;
    std::uniform_int_distribution<std::mt19937::result_type> colorDistribution;
    std::vector<std::vector<Block>> mGameMap;

    std::unique_ptr<Tetromino> mTetromino;
};

#endif //JTETRIS_GAME_H
