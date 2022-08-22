use sdl2::rect::Point;
use sdl2::pixels::Color;

use rand::{
    distributions::{Distribution, Standard},
    Rng,
};
use crate::tetromino::BlockType::*;

#[derive(Clone, Copy)]
pub enum BlockType
{
    EMPTY = 0,
    WALL,
    DROPPED,
    MOVING
}

#[derive(Clone, Copy)]
pub enum TetrominoType
{
    I = 0,
    O,
    T,
    J,
    L,
    S,
    Z
}

impl Distribution<TetrominoType> for Standard {
    fn sample<R: Rng + ?Sized>(&self, rng: &mut R) -> TetrominoType {
        match rng.gen_range(0..=6) {
            0 => TetrominoType::I,
            1 => TetrominoType::O,
            2 => TetrominoType::T,
            3 => TetrominoType::J,
            4 => TetrominoType::L,
            5 => TetrominoType::S,
            _ => TetrominoType::Z,
        }
    }
}

#[derive(Clone, Copy)]
pub struct Block
{
    pub(crate) block_type: BlockType,
    pub(crate) color: Color
}

pub struct Tetromino
{
    pub(crate) rotation: i32,
    pub(crate) tetromino_type: TetrominoType,
    color: Color,
    pub(crate) position: Point,
    pub(crate) blocks: Vec<Vec<Block>>
}

impl Tetromino
{
    pub fn new(tetromino_type: TetrominoType, color: Color) -> Tetromino
    {
        let mut tetro_blocks: Vec<Vec<Block>>;
        tetro_blocks = vec![vec![Block { block_type: EMPTY, color: color }; 4]; 4];

        if matches!(tetromino_type, TetrominoType::I)
        {
            tetro_blocks[2][0].block_type = MOVING;   //  #
            tetro_blocks[2][1].block_type = MOVING;   //  #
            tetro_blocks[2][2].block_type = MOVING;   //  #
            tetro_blocks[2][3].block_type = MOVING;   //  #
        }
        else if matches!(tetromino_type, TetrominoType::O)
        {
            tetro_blocks[1][0].block_type = MOVING;   // ##
            tetro_blocks[1][1].block_type = MOVING;   // ##
            tetro_blocks[2][0].block_type = MOVING;
            tetro_blocks[2][1].block_type = MOVING;
        }
        else if matches!(tetromino_type, TetrominoType::T)
        {
            tetro_blocks[1][1].block_type = MOVING;   //  #
            tetro_blocks[2][0].block_type = MOVING;   // ##
            tetro_blocks[2][1].block_type = MOVING;   //  #
            tetro_blocks[2][2].block_type = MOVING;
        }
        else if matches!(tetromino_type, TetrominoType::J)
        {
            tetro_blocks[2][0].block_type = MOVING;   //  #
            tetro_blocks[2][1].block_type = MOVING;   //  #
            tetro_blocks[2][2].block_type = MOVING;   // ##
            tetro_blocks[1][2].block_type = MOVING;
        }
        else if matches!(tetromino_type, TetrominoType::L)
        {
            tetro_blocks[1][0].block_type = MOVING;   // #
            tetro_blocks[1][1].block_type = MOVING;   // #
            tetro_blocks[1][2].block_type = MOVING;   // ##
            tetro_blocks[2][2].block_type = MOVING;
        }
        else if matches!(tetromino_type, TetrominoType::S)
        {
            tetro_blocks[1][0].block_type = MOVING;   // #
            tetro_blocks[1][1].block_type = MOVING;   // ##
            tetro_blocks[2][1].block_type = MOVING;   //  #
            tetro_blocks[2][2].block_type = MOVING;
        }
        else if matches!(tetromino_type, TetrominoType::Z)
        {
            tetro_blocks[2][0].block_type = MOVING;   //  #
            tetro_blocks[2][1].block_type = MOVING;   // ##
            tetro_blocks[1][1].block_type = MOVING;   // #
            tetro_blocks[1][2].block_type = MOVING;
        }
        Tetromino
        {
            tetromino_type: tetromino_type,
            color,
            rotation: 0,
            position: Point::new(0, 0),
            blocks: tetro_blocks
        }
    }

    pub fn add_rotation(&mut self)
    {
        self.rotation += 90;

        if self.rotation >= 360
        {
            self.rotation = 0;
        }
    }
}