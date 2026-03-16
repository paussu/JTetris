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
    mTitleFont = nullptr;
    mValueFont = nullptr;
    mSmallFont = nullptr;
    ticksCount = 0;
    currentTime = 0;
    lastTime = 0;
    mWidth = width;
    mHeight = height;
    isRunning = true;
    isTetromino = false;
    gameRestarted = false;
    gameStopped = false;
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

    mFont = TTF_OpenFont("Assets/PressStart2P-Regular.ttf", 14);
    mTitleFont = TTF_OpenFont("Assets/PressStart2P-Regular.ttf", 22);
    mValueFont = TTF_OpenFont("Assets/PressStart2P-Regular.ttf", 18);
    mSmallFont = TTF_OpenFont("Assets/PressStart2P-Regular.ttf", 10);
    if(!mFont || !mTitleFont || !mValueFont || !mSmallFont)
    {
        SDL_Log("Failed to load font! Error was: %s", TTF_GetError());
        return false;
    }

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
    TTF_CloseFont(mSmallFont);
    TTF_CloseFont(mValueFont);
    TTF_CloseFont(mTitleFont);
    TTF_CloseFont(mFont);
    mSmallFont = nullptr;
    mValueFont = nullptr;
    mTitleFont = nullptr;
    mFont = nullptr;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
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
                    case SDLK_SPACE:
                        for(int i = 0; i < mMapHeight - 2; i++)
                        {
                            if(!UpdatePosition(0, 1)) 
                                break;
                        }
                        UpdateBlocks();
                        break;
                    case SDLK_RETURN:
                        InsertTetromino();
                        if(gameRestarted) 
                            gameRestarted = false;
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
    const int boardOriginX = 72;
    const int boardOriginY = 64;
    const int cellStep = mBlockSize + mBlockGap;
    const int boardPixelWidth = mMapWidth * (mBlockSize + mBlockGap);
    const int boardPixelHeight = mMapHeight * (mBlockSize + mBlockGap);
    const int playfieldOriginX = boardOriginX + (2 * cellStep);
    const int playfieldOriginY = boardOriginY;
    const int playfieldWidth = ((mMapWidth - 4) * cellStep) - mBlockGap;
    const int playfieldHeight = ((mMapHeight - 2) * cellStep) - mBlockGap;
    const int panelGap = 28;
    const int hudX = boardOriginX + boardPixelWidth + panelGap;
    const int hudY = boardOriginY - 16;
    const int hudWidth = (mWidth - hudX - 32 > 260) ? (mWidth - hudX - 32) : 260;
    const int cardsCount = 5;
    const int hudHeight = (cardsCount * 80) + ((cardsCount - 1) * panelGap) + 32 + 128;

    const SDL_Color backgroundColor{10, 16, 28, 255};
    const SDL_Color boardFill{18, 28, 43, 255};
    const SDL_Color boardBorder{72, 114, 184, 255};
    const SDL_Color hudFill{14, 22, 36, 235};
    const SDL_Color hudBorder{62, 98, 168, 255};
    const SDL_Color statFill{24, 36, 58, 255};
    const SDL_Color helpFill{17, 26, 41, 245};
    const SDL_Color accentBlue{92, 170, 255, 255};
    const SDL_Color accentPink{255, 112, 196, 255};
    const SDL_Color accentGold{255, 196, 92, 255};
    const SDL_Color accentGreen{87, 231, 165, 255};
    const SDL_Color softText{210, 224, 255, 255};
    const SDL_Color mutedText{148, 167, 201, 255};
    const SDL_Color playfieldFill{6, 10, 18, 255};
    const SDL_Color playfieldBorder{235, 240, 255, 230};
    const SDL_Color playfieldGlow{92, 170, 255, 70};

    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);

    DrawPanel({boardOriginX - 18, boardOriginY - 18, boardPixelWidth + 36, boardPixelHeight + 36}, boardFill, boardBorder);
    DrawPanel({hudX, hudY, hudWidth, hudHeight}, hudFill, hudBorder);

    DrawFilledRect({boardOriginX, boardOriginY, boardPixelWidth, boardPixelHeight}, {8, 13, 24, 255});
    DrawRectOutline({boardOriginX, boardOriginY, boardPixelWidth, boardPixelHeight}, {40, 62, 96, 255});
    DrawFilledRect({playfieldOriginX, playfieldOriginY, playfieldWidth, playfieldHeight}, playfieldFill);

    DrawFilledRect({playfieldOriginX - 3, playfieldOriginY, 3, playfieldHeight}, playfieldGlow);
    DrawFilledRect({playfieldOriginX + playfieldWidth, playfieldOriginY, 3, playfieldHeight}, playfieldGlow);
    DrawFilledRect({playfieldOriginX, playfieldOriginY + playfieldHeight, playfieldWidth, 3}, playfieldGlow);

    DrawFilledRect({playfieldOriginX - 1, playfieldOriginY, 1, playfieldHeight}, playfieldBorder);
    DrawFilledRect({playfieldOriginX + playfieldWidth, playfieldOriginY, 1, playfieldHeight}, playfieldBorder);
    DrawFilledRect({playfieldOriginX - 1, playfieldOriginY + playfieldHeight, playfieldWidth + 2, 1}, playfieldBorder);

    for(int x = 0; x < mMapWidth; x++)
    {
        for(int y = 0; y < mMapHeight; y++)
        {
            if(mGameMap[x][y].Type == MOVING || mGameMap[x][y].Type == DROPPED)
            {
                SDL_Rect mapRect;
                mapRect.x = boardOriginX + x * cellStep;
                mapRect.y = boardOriginY + y * cellStep;
                mapRect.h = mBlockSize;
                mapRect.w = mBlockSize;

                SDL_Color blockColor = mGameMap[x][y].Color;
                DrawFilledRect(mapRect, blockColor);

                SDL_Rect highlightRect{mapRect.x + 2, mapRect.y + 2, mapRect.w - 4, mapRect.h / 3};
                DrawFilledRect(highlightRect, {255, 255, 255, 45});
                DrawRectOutline(mapRect, {18, 24, 36, 220});
            }
        }
    }

    ScoreText = std::to_string(mScore);
    LevelText = std::to_string(mLevel);
    LinesText = std::to_string(mLines);

    RenderText(mTitleFont, "JTETRIS", {hudX + 20, hudY + 20, hudWidth - 40, 34}, accentBlue);

    const int statCardX = hudX + 18;
    const int statCardWidth = hudWidth - 36;
    const int statCardHeight = 80;
    const int statCardGap = 12;

    SDL_Rect scoreCard{statCardX, hudY + 92, statCardWidth, statCardHeight};
    SDL_Rect levelCard{statCardX, scoreCard.y + statCardHeight + statCardGap, statCardWidth, statCardHeight};
    SDL_Rect linesCard{statCardX, levelCard.y + statCardHeight + statCardGap, statCardWidth, statCardHeight};

    DrawPanel(scoreCard, statFill, accentGold);
    DrawPanel(levelCard, statFill, accentBlue);
    DrawPanel(linesCard, statFill, accentPink);

    RenderText(mSmallFont, "SCORE", {scoreCard.x + 16, scoreCard.y + 10, scoreCard.w - 32, 12}, mutedText);
    RenderText(mValueFont, ScoreText.c_str(), {scoreCard.x + 16, scoreCard.y + 28, scoreCard.w - 32, 24}, accentGold);
    RenderText(mSmallFont, "POINTS EARNED", {scoreCard.x + 16, scoreCard.y + 58, scoreCard.w - 32, 12}, softText);

    RenderText(mSmallFont, "LEVEL", {levelCard.x + 16, levelCard.y + 10, levelCard.w - 32, 12}, mutedText);
    RenderText(mValueFont, LevelText.c_str(), {levelCard.x + 16, levelCard.y + 28, levelCard.w - 32, 24}, accentBlue);
    RenderText(mSmallFont, "DROP SPEED UP", {levelCard.x + 16, levelCard.y + 58, levelCard.w - 32, 12}, softText);

    RenderText(mSmallFont, "LINES", {linesCard.x + 16, linesCard.y + 10, linesCard.w - 32, 12}, mutedText);
    RenderText(mValueFont, LinesText.c_str(), {linesCard.x + 16, linesCard.y + 28, linesCard.w - 32, 24}, accentPink);
    RenderText(mSmallFont, "ROWS CLEARED", {linesCard.x + 16, linesCard.y + 58, linesCard.w - 32, 12}, softText);

    SDL_Rect statusPanel{statCardX, linesCard.y + statCardHeight + 16, statCardWidth, 64};
    DrawPanel(statusPanel, helpFill, accentGreen);
    RenderText(mSmallFont, "STATUS", {statusPanel.x + 16, statusPanel.y + 10, statusPanel.w - 32, 12}, mutedText);
    if(gameRestarted)
    {
        RenderText(mFont, "GAME OVER", {statusPanel.x + 16, statusPanel.y + 28, statusPanel.w - 32, 14}, accentPink);
        RenderText(mSmallFont, "ENTER TO RESTART", {statusPanel.x + 16, statusPanel.y + 44, statusPanel.w - 32, 12}, softText);
    }
    else if(gameStopped)
    {
        RenderText(mFont, "PAUSED", {statusPanel.x + 16, statusPanel.y + 30, statusPanel.w - 32, 14}, accentGold);
    }
    else if(mTetromino == nullptr)
    {
        RenderText(mFont, "PRESS ENTER", {statusPanel.x + 16, statusPanel.y + 26, statusPanel.w - 32, 14}, accentGreen);
        RenderText(mSmallFont, "TO START", {statusPanel.x + 16, statusPanel.y + 44, statusPanel.w - 32, 12}, softText);
    }
    else
    {
        RenderText(mFont, "RUNNING", {statusPanel.x + 16, statusPanel.y + 30, statusPanel.w - 32, 14}, accentGreen);
    }

    SDL_Rect helpPanel{statCardX, statusPanel.y + statusPanel.h + 16, statCardWidth, 140};
    DrawPanel(helpPanel, helpFill, hudBorder);
    RenderText(mFont, "CONTROLS", {helpPanel.x + 16, helpPanel.y + 12, helpPanel.w - 32, 14}, softText);
    RenderText(mSmallFont, "MOVE      A D / LEFT RIGHT", {helpPanel.x + 16, helpPanel.y + 42, helpPanel.w - 32, 12}, mutedText);
    RenderText(mSmallFont, "DROP      S / DOWN", {helpPanel.x + 16, helpPanel.y + 60, helpPanel.w - 32, 12}, mutedText);
    RenderText(mSmallFont, "ROTATE    W / UP", {helpPanel.x + 16, helpPanel.y + 78, helpPanel.w - 32, 12}, mutedText);
    RenderText(mSmallFont, "HARD DROP SPACE", {helpPanel.x + 16, helpPanel.y + 96, helpPanel.w - 32, 12}, mutedText);
    RenderText(mSmallFont, "PAUSE     P", {helpPanel.x + 16, helpPanel.y + 114, helpPanel.w - 32, 12}, mutedText);

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

void Game::DrawFilledRect(const SDL_Rect& rect, SDL_Color color)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void Game::DrawRectOutline(const SDL_Rect& rect, SDL_Color color)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &rect);
}

void Game::DrawPanel(const SDL_Rect& rect, SDL_Color fillColor, SDL_Color borderColor)
{
    SDL_Rect shadowRect{rect.x + 6, rect.y + 8, rect.w, rect.h};
    DrawFilledRect(shadowRect, {0, 0, 0, 80});
    DrawFilledRect(rect, fillColor);
    DrawRectOutline(rect, borderColor);

    SDL_Rect innerBorder{rect.x + 3, rect.y + 3, rect.w - 6, rect.h - 6};
    if(innerBorder.w > 0 && innerBorder.h > 0)
    {
        DrawRectOutline(innerBorder, {255, 255, 255, 20});
    }
}

void Game::RenderText(TTF_Font* font, const char *text, const SDL_Rect& rect, SDL_Color color, bool centered)
{
    if(font == nullptr)
    {
        return;
    }

    SDL_Surface* scoreMessage = TTF_RenderText_Solid(font, text, color);
    if(scoreMessage == nullptr)
    {
        return;
    }

    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreMessage);
    if(scoreTexture == nullptr)
    {
        SDL_FreeSurface(scoreMessage);
        return;
    }

    SDL_Rect messageRect = rect;

    if(scoreMessage->w > 0 && scoreMessage->h > 0)
    {
        const float widthScale = static_cast<float>(rect.w) / static_cast<float>(scoreMessage->w);
        const float heightScale = static_cast<float>(rect.h) / static_cast<float>(scoreMessage->h);
        const float scale = std::min(widthScale, heightScale);

        messageRect.w = std::max(1, static_cast<int>(scoreMessage->w * scale));
        messageRect.h = std::max(1, static_cast<int>(scoreMessage->h * scale));
        messageRect.y = rect.y + (rect.h - messageRect.h) / 2;

        if(centered)
        {
            messageRect.x = rect.x + (rect.w - messageRect.w) / 2;
        }
    }

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
