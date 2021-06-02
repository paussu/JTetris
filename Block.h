//
// Created by jipe on 5/21/20.
//

#ifndef JTETRIS_BLOCK_H
#define JTETRIS_BLOCK_H

#include <SDL2/SDL_pixels.h>

enum BlockType
{
    EMPTY = 0,
    WALL,
    DROPPED,
    MOVING
};

class Block
{
public:
    Block(BlockType type, SDL_Color color);

    BlockType Type;
    SDL_Color Color;
};


#endif //JTETRIS_BLOCK_H
