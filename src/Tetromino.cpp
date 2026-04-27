#include "Tetromino.h"

Tetromino::Tetromino(TetrominoType type, SDL_Color color)
    : mBlocks(4, std::vector<Block>(4, {EMPTY, color}))
    , mRotation(0)
    , mType(type)
{
    switch(type)
    {
        case I:
            mBlocks[2][0].type = MOVING;   //  #
            mBlocks[2][1].type = MOVING;   //  #
            mBlocks[2][2].type = MOVING;   //  #
            mBlocks[2][3].type = MOVING;   //  #
            break;
        case O:
            mBlocks[1][0].type = MOVING;   // ##
            mBlocks[1][1].type = MOVING;   // ##
            mBlocks[2][0].type = MOVING;
            mBlocks[2][1].type = MOVING;
            break;
        case T:
            mBlocks[1][1].type = MOVING;   //  #
            mBlocks[2][0].type = MOVING;   // ##
            mBlocks[2][1].type = MOVING;   //  #
            mBlocks[2][2].type = MOVING;
            break;
        case J:
            mBlocks[2][0].type = MOVING;   //  #
            mBlocks[2][1].type = MOVING;   //  #
            mBlocks[2][2].type = MOVING;   // ##
            mBlocks[1][2].type = MOVING;
            break;
        case L:
            mBlocks[1][0].type = MOVING;   // #
            mBlocks[1][1].type = MOVING;   // #
            mBlocks[1][2].type = MOVING;   // ##
            mBlocks[2][2].type = MOVING;
            break;
        case S:
            mBlocks[1][0].type = MOVING;   // #
            mBlocks[1][1].type = MOVING;   // ##
            mBlocks[2][1].type = MOVING;   //  #
            mBlocks[2][2].type = MOVING;
            break;
        case Z:
            mBlocks[2][0].type = MOVING;   //  #
            mBlocks[2][1].type = MOVING;   // ##
            mBlocks[1][1].type = MOVING;   // #
            mBlocks[1][2].type = MOVING;
            break;
        default:
            break;
    }
}
