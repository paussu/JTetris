//
// Created by jipe on 5/13/20.
//

#include <iostream>
#include "Game.h"

Game::Game(int width, int height)
:
mGameMap(mMapWidth, std::vector<Block>(mMapHeight, {WALL, SDL_Color{255, 255, 255, 255}})),
mRandomNumberGenerator(mRandomDevice()),
numberDistribution(0, 6),
colorDistribution(50, 255)
{
    window = nullptr;
    renderer = nullptr;
    mFont = nullptr;
    ticksCount = 0;
    currentTime = 0;
    lastTime = 0;
    mWidth = width;
    mHeight = height;
    isRunning = true;
    isTetromino = false;
    gameRestarted = false;
    gameStopped = false;
    hideInfo = false;
    mTetromino = nullptr;
    mScore = 0;
    mLines = 0;
    mLevel = 1;
    mBlockSize = mWidth / 64;
    mDropSpeed = 500;
}

bool Game::Initialize()
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL! Error was: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("JTetris", 100, 100, mWidth, mHeight, 0);

    if (!window)
    {
        SDL_Log("Unable to create window! Error was: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        SDL_Log("Unable to create a renderer! Error was: %s", SDL_GetError());
        return false;
    }

    TTF_Init();

    for(int x = 2; x < mMapWidth - 2; x++)
    {
        for(int y = 0; y < mMapHeight - 2; y++)
        {
            mGameMap[x][y].Type = EMPTY;
        }
    }

    mFont = TTF_OpenFont("../Assets/PressStart2P-Regular.ttf", 24);
    if(!mFont) return false;

    ScoreText = "SCORE: ";

    return true;
}

void Game::Run()
{
    while (isRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::Shutdown()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_a:
                    case SDLK_LEFT:
                        UpdatePosition(-1, 0);
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        UpdatePosition(1, 0);
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        UpdatePosition(0, 1);
                        break;
                    case SDLK_w:
                    case SDLK_UP:
                        RotateTetromino();
                        break;
                    case SDLK_p:
                        gameStopped = !gameStopped;
                        break;
                    case SDLK_h:
                        hideInfo = !hideInfo;
                        break;
                    case SDLK_SPACE:
                        for(int i = 0; i < mMapHeight - 2; i++)
                        {
                            if(!UpdatePosition(0, 1)) break;
                        }
                        UpdateBlocks();
                        break;
                    case SDLK_RETURN:
                        InsertTetromino();
                        if(gameRestarted) gameRestarted = false;
                        hideInfo = true;
                        break;
                }
                break;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(nullptr);

    if (state[SDL_SCANCODE_ESCAPE])
    {
        isRunning = false;
    }


}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksCount + 16));

    float deltaTime = (SDL_GetTicks() - ticksCount) / 1000.0f;

    // Clamp maximum delta value
    if(deltaTime > 0.05f) deltaTime = 0.05f;

    currentTime = ticksCount = SDL_GetTicks();

    if(currentTime > lastTime + mDropSpeed)
    {
        UpdateBlocks();
        lastTime = currentTime;
    }
}

void Game::GenerateOutput()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for(int x = 0; x < mMapWidth; x++)
    {
        for(int y = 0; y < mMapHeight; y++)
        {
            if(mGameMap[x][y].Type == WALL || mGameMap[x][y].Type == MOVING || mGameMap[x][y].Type == DROPPED)
            {
                SDL_Rect mapRect;
                mapRect.x = 100 + x * (mBlockSize + mBlockGap);
                mapRect.y = 100 + y * (mBlockSize + mBlockGap);
                mapRect.h = mBlockSize;
                mapRect.w = mBlockSize;
                SDL_SetRenderDrawColor(renderer, mGameMap[x][y].Color.r, mGameMap[x][y].Color.g,
                        mGameMap[x][y].Color.b, mGameMap[x][y].Color.a);
                SDL_RenderFillRect(renderer, &mapRect);
            }
        }
    }
    if(!hideInfo)
    {
        RenderText("Use wasd or arrow keys for movement", mWidth / 2 + 40, mHeight - (mHeight / 2) ,
                mWidth / 3 + 120, mHeight / 16);
        RenderText("Press p to pause game", mWidth / 2 + 40, mHeight - (mHeight / 3) ,
                mWidth / 3 + 20, mHeight / 16);
        RenderText("Press enter to start game", mWidth / 2 + 40, mHeight - (mHeight / 4) ,
                mWidth / 3 + 20, mHeight / 16);
    }

    ScoreText = "SCORE: " + std::to_string(mScore);
    LevelText = "Level: " + std::to_string(mLevel);
    LinesText = "Lines: " + std::to_string(mLines);
    RenderText(ScoreText.c_str(), mWidth / 2 + 40, 100 , mWidth / 4, mHeight / 20);
    RenderText(LinesText.c_str(), mWidth / 2 + 40, 200 , mWidth / 4, mHeight / 20);
    RenderText(LevelText.c_str(), mWidth / 2 + 40, 300 , mWidth / 4, mHeight / 20);
    SDL_RenderPresent(renderer);
}

void Game::RestartGame()
{
    mScore = mLines = 0;
    mLevel = 1;
    gameRestarted = true;
    EmptyMap();
    mTetromino = nullptr;
    isTetromino = false;
}

void Game::InsertTetromino()
{
    if(isTetromino || gameStopped || gameRestarted) return;

    SDL_Color blockColor;
    blockColor.r = colorDistribution(mRandomNumberGenerator);
    blockColor.g = colorDistribution(mRandomNumberGenerator);
    blockColor.b = colorDistribution(mRandomNumberGenerator);
    blockColor.a = 255;
    mTetromino = std::make_unique<Tetromino>((TetrominoType)numberDistribution(mRandomNumberGenerator), blockColor);
    mTetromino->Position.x = 10;
    mTetromino->Position.y = 0;
    for(int x = 0; x < 4; x++)
    {
        for(int y = 0; y < 4; y++)
        {
            if(mTetromino->Blocks[x][y].Type == MOVING)
            {
                mGameMap[x + mTetromino->Position.x][y + mTetromino->Position.y].Type = MOVING;
                mGameMap[x + mTetromino->Position.x][y + mTetromino->Position.y].Color = mTetromino->Blocks[x][y].Color;
            }
        }
    }

    isTetromino = true;
}

void Game::UpdateBlocks()
{
    if(mTetromino == nullptr || gameStopped || gameRestarted) return;
    bool lineFull;
    for(int y = 0; y < mMapHeight - 2; y++)
    {
        lineFull = true;
        for(int x = 2; x < mMapWidth - 2; x++)
        {
            if(mGameMap[x][2].Type == DROPPED)
            {
                RestartGame();
                return;
            }
            if(mGameMap[x][y].Type != DROPPED)
            {
                mGameMap[x][y].Type = EMPTY;
                lineFull = false;
            }
        }
        if(lineFull)
        {
            AddScore();
            for(int my = y; my >= 1; my--)
            {
                for(int x = 2; x < mMapWidth - 2; x++)
                {
                    mGameMap[x][my].Type = mGameMap[x][my - 1].Type;
                    mGameMap[x][my].Color = mGameMap[x][my - 1].Color;
                }
            }
        }
    }

    ApplyTetrominoToMap(MOVING);

    UpdatePosition(0, 1);
}

bool Game::UpdatePosition(int x, int y)
{
    if(mTetromino == nullptr || gameStopped || gameRestarted) return false;

    for (int tx = 0; tx < 4; tx++)
    {
        for (int ty = 0; ty < 4; ty++)
        {
            if (mTetromino->Blocks[tx][ty].Type == MOVING)
            {
                int xpos = tx + mTetromino->Position.x + x;
                int ypos = ty + mTetromino->Position.y + y;

                if(mGameMap[xpos][ypos].Type == WALL || mGameMap[xpos][ypos].Type == DROPPED)
                {
                    if(y > 0)
                    {
                        ApplyTetrominoToMap(DROPPED);
                        mTetromino = nullptr;
                        isTetromino = false;
                        InsertTetromino();
                    }

                    return false;
                }
            }
        }
    }

    mTetromino->Position.x += x;
    mTetromino->Position.y += y;
    return true;
}

void Game::ApplyTetrominoToMap(BlockType valueToApply)
{
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            if (mTetromino->Blocks[x][y].Type == MOVING)
            {
                mGameMap[x + mTetromino->Position.x][y + mTetromino->Position.y].Type = valueToApply;
                mGameMap[x + mTetromino->Position.x][y + mTetromino->Position.y].Color = mTetromino->Blocks[x][y].Color;
            }
        }
    }
}

void Game::RotateTetromino()
{
    if(mTetromino == nullptr || gameStopped || gameRestarted) return;
    if(mTetromino->GetType() == O) return;

    Vector2 pivot(2, 1);
    int newBlocks[4][4];

    if(mTetromino->GetType() == I && (mTetromino->GetRotation() == 90 || mTetromino->GetRotation() == 270))
    {
        newBlocks[2][0] = MOVING;   //  #
        newBlocks[2][1] = MOVING;   //  #
        newBlocks[2][2] = MOVING;   //  #
        newBlocks[2][3] = MOVING;   //  #
    }
    else
    {
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                if (mTetromino->Blocks[x][y].Type == MOVING)
                {
                    Vector2 relativeVector(x - pivot.x,  y - pivot.y);
                    Vector2 transformedVector;

                    transformedVector.x = 0 * relativeVector.x + (-1 * relativeVector.y);
                    transformedVector.y = 1 * relativeVector.x + 0 * relativeVector.y;

                    Vector2 positionVector(transformedVector.x + pivot.x, transformedVector.y + pivot.y);
                    newBlocks[positionVector.x][positionVector.y] = MOVING;
                }
                mTetromino->Blocks[x][y].Type = EMPTY;
            }
        }
    }

    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            if (newBlocks[x][y] == MOVING)
            {
                mTetromino->Blocks[x][y].Type = MOVING;
            }
            else
            {
                mTetromino->Blocks[x][y].Type = EMPTY;
            }
        }
    }
    mTetromino->AddRotation();
    if(mTetromino->Position.x < 1) mTetromino->Position.x++;
    if(mTetromino->Position.x > mMapWidth - 6) mTetromino->Position.x--;
}

void Game::RenderText(const char *text, int x, int y, int w, int h)
{
    SDL_Surface* scoreMessage = TTF_RenderText_Solid(mFont, text, {255,255,255,255});
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreMessage);

    SDL_Rect messageRect;
    messageRect.x = x;
    messageRect.y = y;
    messageRect.w = w;
    messageRect.h = h;

    SDL_RenderCopy(renderer, scoreTexture, nullptr, &messageRect);

    SDL_FreeSurface(scoreMessage);
    SDL_DestroyTexture(scoreTexture);
}

void Game::EmptyMap()
{
    for(int y = 0; y < mMapHeight - 2; y++)
    {
        for(int x = 2; x < mMapWidth - 2; x++)
        {
            mGameMap[x][y].Type = EMPTY;
        }
    }
}

void Game::AddScore()
{
    mScore += 10 * mLevel;
    mLines++;

    if(mLines % 10 == 0)
    {
        mLevel++;
        mDropSpeed -= 10;
    }
}
