#pragma once

#include "raylib.h"

// GUI Settings

constexpr int   FONT_SIZE                       = 24;
constexpr Color BUTTON_TEXT_COLOR               = BLACK;
constexpr Color BUTTON_BACKGROUND_COLOR         = LIGHTGRAY;
constexpr Color BUTTON_HOVERED_BACKGROUND_COLOR = GRAY;

void gui_text(Rectangle rectangle, const char* text, Color color);
bool gui_button(Rectangle rectangle, const char* text);