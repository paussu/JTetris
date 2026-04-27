#pragma once

#include <vector>
#include <SDL2/SDL_stdinc.h>
#include "Vector2.h"
#include "Block.h"

enum TetrominoType
{
    I = 0,
    O,
    T,
    J,
    L,
    S,
    Z
};

class Tetromino
{
public:
    Tetromino(TetrominoType type, SDL_Color color);

    std::vector<std::vector<Block>>& GetBlocks() { return mBlocks; }
    const std::vector<std::vector<Block>>& GetBlocks() const { return mBlocks; }
    Vector2& GetPosition() { return mPosition; }
    const Vector2& GetPosition() const { return mPosition; }

    void          AddRotation()  { mRotation = (mRotation + 90) % 360; }
    int           GetRotation() const { return mRotation; }
    TetrominoType GetType()     const { return mType; }

private:
    Vector2       mPosition;
    int           mRotation = 0;
    TetrominoType mType     = I;
    std::vector<std::vector<Block>> mBlocks;
};
