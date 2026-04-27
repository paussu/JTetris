#pragma once

#include <SDL2/SDL_pixels.h>

enum BlockType
{
    EMPTY = 0,
    WALL,
    DROPPED,
    MOVING
};

struct Block
{
    BlockType type  = EMPTY;
    SDL_Color color = {};
};