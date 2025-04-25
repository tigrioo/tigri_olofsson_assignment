// ---------------------------------------------------------------
// 
// Copyright © 2023 Filippo Crocchini
// 
// This file is licensed under the MIT license, see end of file.
// 
// ---------------------------------------------------------------

#include "raylib.h"

#include "resources.h"

#include <iostream>

#include "game.h"

constexpr int INITIAL_WINDOW_WIDTH  = 800;
constexpr int INITIAL_WINDOW_HEIGHT = 450;

int main(void)
{    
    InitWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Exercise 4");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    InitAudioDevice();

    PlaySound(res::get_or_load_sound("Assets/piano.wav"));

    SetTargetFPS(60);
    SetExitKey(0);

    Game::init();

    while (!Game::should_quit())
    {
        if(WindowShouldClose()) break;

        BeginDrawing();
        
        Game::draw_frame();
        
        EndDrawing();
    }
    
    Game::deinit();
    
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

/*
    MIT License (MIT)

    Copyright © 2023 Filippo Crocchini

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the “Software”), to deal in 
    the Software without restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
    Software, and to permit persons to whom the Software is furnished to do so, subject
    to the following conditions:

    The above copyright notice and this permission notice shall be included in all 
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
    PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/