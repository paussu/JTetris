//
// Created by jipe on 5/13/20.
//

#include "Tetromino.h"

Tetromino::Tetromino(TetrominoType type, SDL_Color color)
:
Blocks(4, std::vector<Block>(4, {EMPTY, color})),
Rotation(0),
Type(type)
{
    switch(type)
    {
        case I:
            Blocks[2][0].Type = MOVING;   //  #
            Blocks[2][1].Type = MOVING;   //  #
            Blocks[2][2].Type = MOVING;   //  #
            Blocks[2][3].Type = MOVING;   //  #
            break;
        case O:
            Blocks[1][0].Type = MOVING;   // ##
            Blocks[1][1].Type = MOVING;   // ##
            Blocks[2][0].Type = MOVING;
            Blocks[2][1].Type = MOVING;
            break;
        case T:
            Blocks[1][1].Type = MOVING;   //  #
            Blocks[2][0].Type = MOVING;   // ##
            Blocks[2][1].Type = MOVING;   //  #
            Blocks[2][2].Type = MOVING;
            break;
        case J:
            Blocks[2][0].Type = MOVING;   //  #
            Blocks[2][1].Type = MOVING;   //  #
            Blocks[2][2].Type = MOVING;   // ##
            Blocks[1][2].Type = MOVING;
            break;
        case L:
            Blocks[1][0].Type = MOVING;   // #
            Blocks[1][1].Type = MOVING;   // #
            Blocks[1][2].Type = MOVING;   // ##
            Blocks[2][2].Type = MOVING;
            break;
        case S:
            Blocks[1][0].Type = MOVING;   // #
            Blocks[1][1].Type = MOVING;   // ##
            Blocks[2][1].Type = MOVING;   //  #
            Blocks[2][2].Type = MOVING;
            break;
        case Z:
            Blocks[2][0].Type = MOVING;   //  #
            Blocks[2][1].Type = MOVING;   // ##
            Blocks[1][1].Type = MOVING;   // #
            Blocks[1][2].Type = MOVING;
            break;
        default:
            break;
    }
}

void Tetromino::AddRotation()
{
    Rotation += 90;
    if(Rotation >= 360) Rotation = 0;
}

int Tetromino::GetRotation()
{
    return Rotation;
}

TetrominoType Tetromino::GetType()
{
    return Type;
}
