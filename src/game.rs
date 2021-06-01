use sdl2::event::Event;
use sdl2::keyboard::{Keycode, Scancode};
use crate::tetromino::{Tetromino, Block, BlockType, TetrominoType};
use std::time::Instant;
use crate::tetromino::BlockType::*;
use gtk::glib::random_int_range;

pub struct Game
{
    screen_width: u32,
    screen_height: u32,

    sdl_context: sdl2::Sdl,
    sdl_events: sdl2::EventPump,
    sdl_canvas: sdl2::render::WindowCanvas,

    is_running: bool,

    ticks_count: u32,
    drop_speed: u128,
    last_time: Instant,
    current_time: Instant,
    delta_time: f32,
    map_width: i32,
    map_height: i32,
    block_size: i32,

    tetromino: Option<Tetromino>,
    game_map: Vec<Vec<Block>>
}

impl Game
{

    pub fn new(width: u32, height: u32) -> Result<Game, String>
    {
        let sdl = sdl2::init()?;
        let video = sdl.video()?;
        let events = sdl.event_pump()?;

        let window = video
        .window("JTetris", width, height)
        .position_centered()
        .build()
        .map_err(|e| e.to_string())?;

        let canvas = window
            .into_canvas()
            .accelerated()
            .build()
            .map_err(|e| e.to_string())?;

        let game = Ok(Game {
            screen_width: width,
            screen_height: height,

            sdl_context: sdl,
            sdl_events: events,
            sdl_canvas: canvas,

            is_running: true,
            
            ticks_count: 0,
            last_time: Instant::now(),
            current_time: Instant::now(),
            delta_time: 0f32,
            drop_speed: 500,
            map_width: 20,
            map_height: 30,
            block_size: 10,

            tetromino: None,
            game_map: vec![vec![Block { block_type: WALL, color: sdl2::pixels::Color::WHITE }; 30]; 20]
        });

        game
    }

    pub fn run_loop(&mut self)
    {
        while self.is_running
        {
            self.process_input();
            self.update_game();
            self.generate_output();
        }
    }

    fn process_input(&mut self)
    {
        for event in self.sdl_events.poll_iter() {
            match event {
                Event::Quit {..} |
                Event::KeyDown { keycode: Some(Keycode::Escape), .. } => {
                    self.is_running = false;
                },
                Event::KeyDown { keycode: Some(Keycode::Space), repeat: false, .. } => {
                    self.insert_tetromino();
                },
                Event::KeyDown { keycode: Some(Keycode::Left), repeat: false, .. } => {
                    self.update_position(-1, 0);
                },
                Event::KeyDown { keycode: Some(Keycode::Right), repeat: false, .. } => {
                    self.update_position(1, 0);
                },
                Event::KeyDown { keycode: Some(Keycode::Up), repeat: false, .. } => {
                    //self.rotate_tetromino();
                },
                Event::KeyDown { keycode: Some(Keycode::Down), repeat: false, .. } => {
                    self.update_position(0, 1);
                },
                _ => {}
            }
        }
    }

    fn update_game(&mut self)
    {
        let new_time = Instant::now();

        let frame_time = (new_time.duration_since(self.current_time).subsec_nanos()) as f32 / 1_000_000_000.0;
        self.current_time = new_time;
        self.delta_time += frame_time;

        // Clamp maximum delta value
        while self.delta_time >= 0.05 {
            self.delta_time -= 0.05;
        }

        if self.current_time.duration_since(self.last_time).as_millis() > self.drop_speed
        {
            self.update_blocks();
            self.last_time = self.current_time;
        }
    }

    fn generate_output(&mut self)
    {
        self.sdl_canvas.set_draw_color(sdl2::pixels::Color::RGB(0, 0, 0));
        self.sdl_canvas.clear();

        for x in 0..self.map_width as usize
        {
            for y in 0..self.map_height as usize
            {
                if matches!(self.game_map[x][y].block_type, WALL)
                    || matches!(self.game_map[x][y].block_type, MOVING)
                    || matches!(self.game_map[x][y].block_type, DROPPED)
                {
                    let mut map_rect = sdl2::rect::Rect::new(0, 0, 0, 0);
                    map_rect.x = 100 + x as i32 * (self.block_size + 2);
                    map_rect.y = 100 + y as i32 * (self.block_size + 2);
                    map_rect.h = self.block_size;
                    map_rect.w = self.block_size;
                    self.sdl_canvas.draw_rect(map_rect);
                    self.sdl_canvas.set_draw_color(sdl2::pixels::Color::RGB(self.game_map[x][y].color.r, self.game_map[x][y].color.g, self.game_map[x][y].color.b));
                }
            }
        }

        self.sdl_canvas.present();
    }

    fn insert_tetromino(&mut self)
    {
        if self.tetromino.is_some()
        {
            return;
        }

        let mut block_color = sdl2::pixels::Color::RGBA(0, 0, 0, 0 );
        block_color.r = random_int_range(0, 255) as u8;
        block_color.g = random_int_range(0, 255) as u8;
        block_color.b = random_int_range(0, 255) as u8;
        block_color.a = 255;
        self.tetromino = Some(Tetromino::new(rand::random(), block_color));
        self.tetromino.as_mut().unwrap().position.x = 10;
        self.tetromino.as_mut().unwrap().position.y = 0;

        for x in 0..4
        {
            for y in 0..4
            {
                if matches!(self.tetromino.as_mut().unwrap().blocks[x][y].block_type, MOVING)
                {
                    self.game_map[x + self.tetromino.as_mut().unwrap().position.x as usize][y + self.tetromino.as_mut().unwrap().position.y as usize].block_type = MOVING;
                    self.game_map[x + self.tetromino.as_mut().unwrap().position.x as usize][y + self.tetromino.as_mut().unwrap().position.y as usize].color = self.tetromino.as_mut().unwrap().blocks[x][y].color;
                }
            }
        }
    }

    fn update_blocks(&mut self)
    {
        let mut line_full = true;
        for y in 0..self.map_height - 2
        {
            line_full = true;
            for x in 2..self.map_width - 2
            {
                if matches!(self.game_map[x as usize][2 as usize].block_type, DROPPED)
                {
                    //RestartGame();
                    return;
                }
                if !matches!(self.game_map[x as usize][y as usize].block_type, DROPPED)
                {
                    self.game_map[x as usize][y as usize].block_type = EMPTY;
                    line_full = false;
                }
            }
            if line_full
            {
                //AddScore();
                for my in y as usize..1
                {
                    for x in 2 as usize..self.map_width as usize - 2
                    {
                        self.game_map[x][my].block_type = self.game_map[x][my - 1].block_type;
                        self.game_map[x][my].color = self.game_map[x][my - 1].color;
                    }
                }
            }
        }
        self.apply_tetromino_to_map(MOVING);

        self.update_position(0, 1);
    }

    fn apply_tetromino_to_map(&mut self, value_to_apply: BlockType)
    {
        if self.tetromino.is_none()
        {
            return;
        }

        for x in 0..4
        {
            for y in 0..4
            {
                if matches!(self.tetromino.as_mut().unwrap().blocks[x][y].block_type, MOVING)
                {
                    self.game_map[x + self.tetromino.as_mut().unwrap().position.x as usize][y + self.tetromino.as_mut().unwrap().position.y as usize].block_type = value_to_apply;
                    self.game_map[x + self.tetromino.as_mut().unwrap().position.x as usize][y + self.tetromino.as_mut().unwrap().position.y as usize].color = self.tetromino.as_mut().unwrap().blocks[x][y].color;
                }
            }
        }
    }

    fn rotate_tetromino()
    {

    }

    fn update_position(&mut self, x: i32, y: i32) -> bool
    {
        if self.tetromino.is_none()
        {
            return false;
        }

        for tx in 0..4
        {
            for ty in 0..4
            {
                if matches!(self.tetromino.as_mut().unwrap().blocks[tx][ty].block_type, MOVING)
                {
                    let xpos = (tx as i32 + self.tetromino.as_mut().unwrap().position.x + x) as usize;
                    let ypos = (ty as i32 + self.tetromino.as_mut().unwrap().position.y + y) as usize;

                    if matches!(self.game_map[xpos][ypos].block_type, WALL) || matches!(self.game_map[xpos][ypos].block_type, DROPPED)
                    {
                        if y > 0
                        {
                            self.apply_tetromino_to_map(DROPPED);
                            self.tetromino = None;
                            self.insert_tetromino();
                        }

                        return false;
                    }
                }
            }
        }

        self.tetromino.as_mut().unwrap().position.x += x;
        self.tetromino.as_mut().unwrap().position.y += y;
        return true;
    }

    pub fn key_pressed(&self, k: Keycode) -> bool {
        let scancode = Scancode::from_keycode(k);
        if let Some(s) = scancode {
            return self.sdl_events.keyboard_state().is_scancode_pressed(s);
        } else {
            panic!("Keycode does not exist: {:?}", scancode)
        }
    }
}