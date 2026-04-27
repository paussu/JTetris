#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstdarg>
#include <cstring>
#include <cmath>
#include <cassert>
#include <climits>
#include <ctime>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_gl3.h"

#include "Menu.h"
#include "Game.h"

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

bool Menu::Initialize()
{
    mWidth  = 1024;
    mHeight = 768;

    if(!LoadMenu())
        return false;

    mIsRunning  = true;
    mGameStart  = false;

    mButtonWidth  = 320;
    mButtonHeight = 120;
    mSelectedResolution = 2;

    mResolutions.push_back({"640x480",    640,  480});
    mResolutions.push_back({"800x600",    800,  600});
    mResolutions.push_back({"1024x768",  1024,  768});
    mResolutions.push_back({"1280x960",  1280,  960});
    mResolutions.push_back({"1440x900",  1440,  900});
    mResolutions.push_back({"1600x1200", 1600, 1200});
    mResolutions.push_back({"1920x1080", 1920, 1080});

    return true;
}

void Menu::Shutdown()
{
    UnloadMenu();
}

void Menu::RunLoop()
{
    while(mIsRunning)
    {
        ProcessInput();

        if(mGameStart)
        {
            UnloadMenu();
            auto game = std::make_unique<Game>(
                mResolutions[mSelectedResolution].width,
                mResolutions[mSelectedResolution].height);

            if(game->Initialize())
                game->Run();

            game->Shutdown();
            mGameStart = false;

            if (!LoadMenu())
                mIsRunning = false;
        }

        DrawMenu();
    }
}

bool Menu::LoadMenu()
{
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");

    int sdlResult = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS);
    if(sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL! Error was: %s", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    mWindow = SDL_CreateWindow("JTetris", 100, 100, mWidth, mHeight,
                               SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);

    if(!mWindow)
    {
        SDL_Log("Unable to create window! Error was %s", SDL_GetError());
        return false;
    }

    mGlContext = SDL_GL_CreateContext(mWindow);
    SDL_GetWindowSize(mWindow, &mWidth, &mHeight);

    glViewport(0, 0, mWidth, mHeight);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to setup GLEW\n");
        return false;
    }

    mCtx = nk_sdl_init(mWindow);

    {
        struct nk_font_atlas* atlas;
        nk_sdl_font_stash_begin(&atlas);
        nk_sdl_font_stash_end();
    }

    mOptions = false;
    return true;
}

void Menu::UnloadMenu()
{
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(mGlContext);
    mGlContext = nullptr;
    SDL_DestroyWindow(mWindow);
    mWindow = nullptr;
    SDL_Quit();
}

void Menu::ProcessInput()
{
    nk_input_begin(mCtx);

    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                mIsRunning = false;
                break;
        }
        nk_sdl_handle_event(&event);
    }
    nk_input_end(mCtx);

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if(state[SDL_SCANCODE_ESCAPE])
        mIsRunning = false;
}

void Menu::DrawMenu()
{
    if (nk_begin(mCtx, "Main menu", nk_rect(10, 10, mWidth - 10, mHeight - 10),
                 NK_WINDOW_BORDER|NK_WINDOW_NO_INPUT|NK_WINDOW_SCALABLE|
                 NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        nk_layout_space_begin(mCtx, NK_STATIC, 0, 1);
        {
            nk_layout_space_push(mCtx,
                                 nk_rect(mWidth / 2 - (mButtonWidth / 2), mHeight - mButtonHeight * 6, mButtonWidth, mButtonHeight));
            if (nk_button_label(mCtx, "Start Game"))
                mGameStart = true;

            nk_layout_space_push(mCtx,
                                 nk_rect(mWidth / 2 - (mButtonWidth / 2), mHeight - mButtonHeight * 5, mButtonWidth, mButtonHeight));
            if (nk_button_label(mCtx, "Options"))
                mOptions = true;

            nk_layout_space_push(mCtx,
                                 nk_rect(mWidth / 2 - (mButtonWidth / 2), mHeight - mButtonHeight * 4, mButtonWidth, mButtonHeight));

            if (nk_button_label(mCtx, "Exit"))
                mIsRunning = false;
        }

        nk_layout_space_end(mCtx);
    }
    nk_end(mCtx);

    if(mOptions)
        DrawOptions();

    SDL_GetWindowSize(mWindow, &mWidth, &mHeight);
    glViewport(0, 0, mWidth, mHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
     * with blending, scissor, face culling, depth test and viewport and
     * defaults everything back into a default state.
     * Make sure to either a.) save and restore or b.) reset your own state after
     * rendering the UI. */
    nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
    SDL_GL_SwapWindow(mWindow);
}

void Menu::DrawOptions()
{
    if (nk_begin(mCtx, "Options", nk_rect(mWidth / 4, mHeight / 4, mWidth / 2, mHeight / 2),
                 NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|
                 NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_dynamic(mCtx, 30, 2);

        nk_label(mCtx, "Game resolution:", NK_TEXT_RIGHT);
        if (nk_combo_begin_label(mCtx, mResolutions[mSelectedResolution].title.c_str(), nk_vec2(nk_widget_width(mCtx), 200)))
        {
            nk_layout_row_dynamic(mCtx, 25, 1);
            for (int i = 0; i < static_cast<int>(mResolutions.size()); ++i)
            {
                if (nk_combo_item_label(mCtx, mResolutions[i].title.c_str(), NK_TEXT_LEFT))
                    mSelectedResolution = i;
            }
            nk_combo_end(mCtx);
        }
        nk_layout_space_begin(mCtx, NK_STATIC, 0, 1);
        {
            nk_layout_space_push(mCtx, nk_rect(mWidth / 3 + 60, mHeight / 3, 80, 40));
            if (nk_button_label(mCtx, "Back"))
                mOptions = false;
        }

        nk_layout_space_end(mCtx);
    }
    nk_end(mCtx);
}
