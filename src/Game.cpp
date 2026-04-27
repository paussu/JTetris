#include "Game.h"

Game::Game(int width, int height)
    : mGameMap(kMapWidth, std::vector<Block>(kMapHeight, {WALL, SDL_Color{255, 255, 255, 255}}))
    , mRandomNumberGenerator(mRandomDevice())
    , mNumberDistribution(0, 6)
    , mColorDistribution(50, 255)
{
    mWidth     = width;
    mHeight    = height;
    mBlockSize = mWidth / 64;
}

bool Game::Initialize()
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL! Error was: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("JTetris", 100, 100, mWidth, mHeight, 0);

    if (!mWindow)
    {
        SDL_Log("Unable to create window! Error was: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!mRenderer)
    {
        SDL_Log("Unable to create a renderer! Error was: %s", SDL_GetError());
        return false;
    }

    if (TTF_Init() != 0)
    {
        SDL_Log("Unable to initialize SDL_ttf! Error was: %s", TTF_GetError());
        return false;
    }

    for(int x = 2; x < kMapWidth - 2; x++)
    {
        for(int y = 0; y < kMapHeight - 2; y++)
        {
            mGameMap[x][y].type = EMPTY;
        }
    }

    mFont      = TTF_OpenFont(kFontPath, 14);
    mTitleFont = TTF_OpenFont(kFontPath, 22);
    mValueFont = TTF_OpenFont(kFontPath, 18);
    mSmallFont = TTF_OpenFont(kFontPath, 10);
    if(!mFont || !mTitleFont || !mValueFont || !mSmallFont)
    {
        SDL_Log("Failed to load font! Error was: %s", TTF_GetError());
        return false;
    }

    return true;
}

void Game::Run()
{
    while (mIsRunning)
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
    mFont      = nullptr;
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
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
                mIsRunning = false;
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
                        mGameStopped = !mGameStopped;
                        break;
                    case SDLK_SPACE:
                        for(int i = 0; i < kMapHeight - 2; i++)
                        {
                            if(!UpdatePosition(0, 1))
                                break;
                        }
                        UpdateBlocks();
                        break;
                    case SDLK_RETURN:
                        InsertTetromino();
                        if(mGameRestarted)
                            mGameRestarted = false;
                        break;
                }
                break;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(nullptr);

    if (state[SDL_SCANCODE_ESCAPE])
        mIsRunning = false;
}
void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    mCurrentTime = mTicksCount = SDL_GetTicks();

    if(mCurrentTime > mLastTime + static_cast<Uint32>(mDropSpeed))
    {
        UpdateBlocks();
        mLastTime = mCurrentTime;
    }
}

void Game::GenerateOutput()
{
    const int boardOriginX = 72;
    const int boardOriginY = 64;
    const int cellStep = mBlockSize + kBlockGap;
    const int boardPixelWidth = kMapWidth * (mBlockSize + kBlockGap);
    const int boardPixelHeight = kMapHeight * (mBlockSize + kBlockGap);
    const int playfieldOriginX = boardOriginX + (2 * cellStep);
    const int playfieldOriginY = boardOriginY;
    const int playfieldWidth = ((kMapWidth - 4) * cellStep) - kBlockGap;
    const int playfieldHeight = ((kMapHeight - 2) * cellStep) - kBlockGap;
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

    SDL_SetRenderDrawColor(mRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(mRenderer);

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

    for(int x = 0; x < kMapWidth; x++)
    {
        for(int y = 0; y < kMapHeight; y++)
        {
            if(mGameMap[x][y].type == MOVING || mGameMap[x][y].type == DROPPED)
            {
                SDL_Rect mapRect;
                mapRect.x = boardOriginX + x * cellStep;
                mapRect.y = boardOriginY + y * cellStep;
                mapRect.h = mBlockSize;
                mapRect.w = mBlockSize;

                SDL_Color blockColor = mGameMap[x][y].color;
                DrawFilledRect(mapRect, blockColor);

                SDL_Rect highlightRect{mapRect.x + 2, mapRect.y + 2, mapRect.w - 4, mapRect.h / 3};
                DrawFilledRect(highlightRect, {255, 255, 255, 45});
                DrawRectOutline(mapRect, {18, 24, 36, 220});
            }
        }
    }

    const std::string scoreText = std::to_string(mScore);
    const std::string levelText = std::to_string(mLevel);
    const std::string linesText = std::to_string(mLines);

    RenderText(mTitleFont, "JTETRIS", {hudX + 20, hudY + 20, hudWidth - 40, 34}, accentBlue);

    const int statCardX     = hudX + 18;
    const int statCardWidth  = hudWidth - 36;
    const int statCardHeight = 80;
    const int statCardGap    = 12;

    SDL_Rect scoreCard{statCardX, hudY + 92, statCardWidth, statCardHeight};
    SDL_Rect levelCard{statCardX, scoreCard.y + statCardHeight + statCardGap, statCardWidth, statCardHeight};
    SDL_Rect linesCard{statCardX, levelCard.y + statCardHeight + statCardGap, statCardWidth, statCardHeight};

    DrawPanel(scoreCard, statFill, accentGold);
    DrawPanel(levelCard, statFill, accentBlue);
    DrawPanel(linesCard, statFill, accentPink);

    RenderText(mSmallFont, "SCORE",         {scoreCard.x + 16, scoreCard.y + 10, scoreCard.w - 32, 12}, mutedText);
    RenderText(mValueFont, scoreText.c_str(), {scoreCard.x + 16, scoreCard.y + 28, scoreCard.w - 32, 24}, accentGold);
    RenderText(mSmallFont, "POINTS EARNED", {scoreCard.x + 16, scoreCard.y + 58, scoreCard.w - 32, 12}, softText);

    RenderText(mSmallFont, "LEVEL",         {levelCard.x + 16, levelCard.y + 10, levelCard.w - 32, 12}, mutedText);
    RenderText(mValueFont, levelText.c_str(), {levelCard.x + 16, levelCard.y + 28, levelCard.w - 32, 24}, accentBlue);
    RenderText(mSmallFont, "DROP SPEED UP", {levelCard.x + 16, levelCard.y + 58, levelCard.w - 32, 12}, softText);

    RenderText(mSmallFont, "LINES",         {linesCard.x + 16, linesCard.y + 10, linesCard.w - 32, 12}, mutedText);
    RenderText(mValueFont, linesText.c_str(), {linesCard.x + 16, linesCard.y + 28, linesCard.w - 32, 24}, accentPink);
    RenderText(mSmallFont, "ROWS CLEARED", {linesCard.x + 16, linesCard.y + 58, linesCard.w - 32, 12}, softText);

    SDL_Rect statusPanel{statCardX, linesCard.y + statCardHeight + 16, statCardWidth, 64};
    DrawPanel(statusPanel, helpFill, accentGreen);
    RenderText(mSmallFont, "STATUS", {statusPanel.x + 16, statusPanel.y + 10, statusPanel.w - 32, 12}, mutedText);
    if(mGameRestarted)
    {
        RenderText(mFont, "GAME OVER",       {statusPanel.x + 16, statusPanel.y + 28, statusPanel.w - 32, 14}, accentPink);
        RenderText(mSmallFont, "ENTER TO RESTART", {statusPanel.x + 16, statusPanel.y + 44, statusPanel.w - 32, 12}, softText);
    }
    else if(mGameStopped)
    {
        RenderText(mFont, "PAUSED", {statusPanel.x + 16, statusPanel.y + 30, statusPanel.w - 32, 14}, accentGold);
    }
    else if(mTetromino == nullptr)
    {
        RenderText(mFont,      "PRESS ENTER", {statusPanel.x + 16, statusPanel.y + 26, statusPanel.w - 32, 14}, accentGreen);
        RenderText(mSmallFont, "TO START",    {statusPanel.x + 16, statusPanel.y + 44, statusPanel.w - 32, 12}, softText);
    }
    else
    {
        RenderText(mFont, "RUNNING", {statusPanel.x + 16, statusPanel.y + 30, statusPanel.w - 32, 14}, accentGreen);
    }

    SDL_Rect helpPanel{statCardX, statusPanel.y + statusPanel.h + 16, statCardWidth, 140};
    DrawPanel(helpPanel, helpFill, hudBorder);
    RenderText(mFont,      "CONTROLS",                   {helpPanel.x + 16, helpPanel.y + 12,  helpPanel.w - 32, 14}, softText);
    RenderText(mSmallFont, "MOVE      A D / LEFT RIGHT",  {helpPanel.x + 16, helpPanel.y + 42,  helpPanel.w - 32, 12}, mutedText);
    RenderText(mSmallFont, "DROP      S / DOWN",          {helpPanel.x + 16, helpPanel.y + 60,  helpPanel.w - 32, 12}, mutedText);
    RenderText(mSmallFont, "ROTATE    W / UP",            {helpPanel.x + 16, helpPanel.y + 78,  helpPanel.w - 32, 12}, mutedText);
    RenderText(mSmallFont, "HARD DROP SPACE",             {helpPanel.x + 16, helpPanel.y + 96,  helpPanel.w - 32, 12}, mutedText);
    RenderText(mSmallFont, "PAUSE     P",                 {helpPanel.x + 16, helpPanel.y + 114, helpPanel.w - 32, 12}, mutedText);

    SDL_RenderPresent(mRenderer);
}

void Game::RestartGame()
{
    mScore = mLines = 0;
    mLevel = 1;
    mGameRestarted = true;
    EmptyMap();
    mTetromino = nullptr;
    mIsTetromino = false;
}

void Game::InsertTetromino()
{
    if(mIsTetromino || mGameStopped || mGameRestarted)
        return;

    SDL_Color blockColor;
    blockColor.r = mColorDistribution(mRandomNumberGenerator);
    blockColor.g = mColorDistribution(mRandomNumberGenerator);
    blockColor.b = mColorDistribution(mRandomNumberGenerator);
    blockColor.a = 255;
    mTetromino = std::make_unique<Tetromino>(
        static_cast<TetrominoType>(mNumberDistribution(mRandomNumberGenerator)), blockColor);
    mTetromino->GetPosition().x = 10;
    mTetromino->GetPosition().y = 0;
    for(int x = 0; x < 4; x++)
    {
        for(int y = 0; y < 4; y++)
        {
            if(mTetromino->GetBlocks()[x][y].type == MOVING)
            {
                mGameMap[x + mTetromino->GetPosition().x][y + mTetromino->GetPosition().y].type  = MOVING;
                mGameMap[x + mTetromino->GetPosition().x][y + mTetromino->GetPosition().y].color = mTetromino->GetBlocks()[x][y].color;
            }
        }
    }

    mIsTetromino = true;
}

void Game::UpdateBlocks()
{
    if(mTetromino == nullptr || mGameStopped || mGameRestarted)
        return;

    bool lineFull;
    for(int y = 0; y < kMapHeight - 2; y++)
    {
        lineFull = true;
        for(int x = 2; x < kMapWidth - 2; x++)
        {
            if(mGameMap[x][2].type == DROPPED)
            {
                RestartGame();
                return;
            }
            if(mGameMap[x][y].type != DROPPED)
            {
                mGameMap[x][y].type = EMPTY;
                lineFull = false;
            }
        }
        if(lineFull)
        {
            AddScore();
            for(int my = y; my >= 1; my--)
            {
                for(int x = 2; x < kMapWidth - 2; x++)
                {
                    mGameMap[x][my].type  = mGameMap[x][my - 1].type;
                    mGameMap[x][my].color = mGameMap[x][my - 1].color;
                }
            }
        }
    }

    ApplyTetrominoToMap(MOVING);

    UpdatePosition(0, 1);
}

bool Game::UpdatePosition(int x, int y)
{
    if(mTetromino == nullptr || mGameStopped || mGameRestarted)
        return false;

    for (int tx = 0; tx < 4; tx++)
    {
        for (int ty = 0; ty < 4; ty++)
        {
            if (mTetromino->GetBlocks()[tx][ty].type == MOVING)
            {
                int xpos = tx + mTetromino->GetPosition().x + x;
                int ypos = ty + mTetromino->GetPosition().y + y;

                if(mGameMap[xpos][ypos].type == WALL || mGameMap[xpos][ypos].type == DROPPED)
                {
                    if(y > 0)
                    {
                        ApplyTetrominoToMap(DROPPED);
                        mTetromino   = nullptr;
                        mIsTetromino = false;
                        InsertTetromino();
                    }

                    return false;
                }
            }
        }
    }

    mTetromino->GetPosition().x += x;
    mTetromino->GetPosition().y += y;
    return true;
}

void Game::ApplyTetrominoToMap(BlockType valueToApply)
{
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            if (mTetromino->GetBlocks()[x][y].type == MOVING)
            {
                mGameMap[x + mTetromino->GetPosition().x][y + mTetromino->GetPosition().y].type  = valueToApply;
                mGameMap[x + mTetromino->GetPosition().x][y + mTetromino->GetPosition().y].color = mTetromino->GetBlocks()[x][y].color;
            }
        }
    }
}

void Game::RotateTetromino()
{
    if (mTetromino == nullptr || mGameStopped || mGameRestarted)
        return;
    if (mTetromino->GetType() == O)
        return;

    const Vector2 pivot(2, 1);
    int newBlocks[4][4] = {};

    const bool isIVertical = mTetromino->GetType() == I
                          && (mTetromino->GetRotation() == 90 || mTetromino->GetRotation() == 270);
    if (isIVertical)
    {
        newBlocks[2][0] = newBlocks[2][1] = newBlocks[2][2] = newBlocks[2][3] = MOVING;
    }
    else
    {
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                if (mTetromino->GetBlocks()[x][y].type != MOVING)
                    continue;

                const Vector2 rel(x - pivot.x, y - pivot.y);
                newBlocks[-rel.y + pivot.x][rel.x + pivot.y] = MOVING;
            }
        }
    }

    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
            mTetromino->GetBlocks()[x][y].type = (newBlocks[x][y] == MOVING) ? MOVING : EMPTY;
    }

    mTetromino->AddRotation();

    auto& posX = mTetromino->GetPosition().x;

    if (posX < 1)
        posX++;
    if (posX > kMapWidth - 6)
        posX--;
}

void Game::DrawFilledRect(const SDL_Rect& rect, SDL_Color color)
{
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(mRenderer, &rect);
}

void Game::DrawRectOutline(const SDL_Rect& rect, SDL_Color color)
{
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(mRenderer, &rect);
}

void Game::DrawPanel(const SDL_Rect& rect, SDL_Color fillColor, SDL_Color borderColor)
{
    SDL_Rect shadowRect{rect.x + 6, rect.y + 8, rect.w, rect.h};
    DrawFilledRect(shadowRect, {0, 0, 0, 80});
    DrawFilledRect(rect, fillColor);
    DrawRectOutline(rect, borderColor);

    SDL_Rect innerBorder{rect.x + 3, rect.y + 3, rect.w - 6, rect.h - 6};
    if(innerBorder.w > 0 && innerBorder.h > 0)
        DrawRectOutline(innerBorder, {255, 255, 255, 20});
}

void Game::RenderText(TTF_Font* font, const char *text, const SDL_Rect& rect, SDL_Color color, bool centered)
{
    if(font == nullptr)
        return;

    SDL_Surface* scoreMessage = TTF_RenderText_Solid(font, text, color);
    if(scoreMessage == nullptr)
        return;

    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(mRenderer, scoreMessage);
    if(scoreTexture == nullptr)
    {
        SDL_FreeSurface(scoreMessage);
        return;
    }

    SDL_Rect messageRect = rect;

    if(scoreMessage->w > 0 && scoreMessage->h > 0)
    {
        const float widthScale  = static_cast<float>(rect.w) / static_cast<float>(scoreMessage->w);
        const float heightScale = static_cast<float>(rect.h) / static_cast<float>(scoreMessage->h);
        const float scale = std::min(widthScale, heightScale);

        messageRect.w = std::max(1, static_cast<int>(scoreMessage->w * scale));
        messageRect.h = std::max(1, static_cast<int>(scoreMessage->h * scale));
        messageRect.y = rect.y + (rect.h - messageRect.h) / 2;

        if(centered)
            messageRect.x = rect.x + (rect.w - messageRect.w) / 2;
    }

    SDL_RenderCopy(mRenderer, scoreTexture, nullptr, &messageRect);

    SDL_FreeSurface(scoreMessage);
    SDL_DestroyTexture(scoreTexture);
}

void Game::EmptyMap()
{
    for(int y = 0; y < kMapHeight - 2; y++)
    {
        for(int x = 2; x < kMapWidth - 2; x++)
            mGameMap[x][y].type = EMPTY;
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
