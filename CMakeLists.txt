cmake_minimum_required(VERSION 3.15)
project(JTetris)

set(CMAKE_CXX_STANDARD 14)

include_directories(OPENGL_INCLUDE_DIR)
include_directories(GLEW_INCLUDE_DIR)
include_directories(SDL2_INCLUDE_DIR)

add_executable(JTetris main.cpp Game.cpp Game.h Menu.cpp Menu.h nuklear.h nuklear_sdl_gl3.h Tetromino.cpp Tetromino.h Vector2.cpp Vector2.h Block.cpp Block.h)

target_link_libraries(JTetris SDL2 SDL2_ttf GL GLU GLEW)