//
// Created by jipe on 5/13/20.
//

#ifndef JTETRIS_TETROMINO_H
#define JTETRIS_TETROMINO_H

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
    std::vector<std::vector<Block>> Blocks;
    Vector2 Position;
    void AddRotation();
    int GetRotation();
    TetrominoType GetType();
private:
    int Rotation;
    TetrominoType Type;
};


#endif //JTETRIS_TETROMINO_H
