#include "gui.h"

// GUI Implementation

void gui_text(Rectangle rectangle, const char* text, Color color)
{
    int text_width = MeasureText(text, FONT_SIZE);
    
    // Here we center the text inside the provided rectangle, and then draw it.

    int text_x = (int) (rectangle.x + (rectangle.width - text_width) / 2);
    int text_y = (int) (rectangle.y + (rectangle.height - FONT_SIZE) / 2);

    DrawText(text, text_x, text_y, FONT_SIZE, color);
}

bool gui_button(Rectangle rectangle, const char* text)
{
    bool clicked = false;
    bool hovered = false;

    // Detect if the mouse position is inside the button rectangle.
    // If it is then we would like to change the background color to a different one so that the user has some feedback.
    // If the user is over the button and _releases_ the left mouse button then we count that as a button press and return true.
    // This is by no means perfect but it's a good start.

    if(CheckCollisionPointRec(GetMousePosition(), rectangle))
    {
        hovered = true;

        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            clicked = true;
        }
    }

    // Here we initialize background_color to the normal background color, the reason why this and not the hovered background color
    // is because the other is a special case.

    Color background_color = BUTTON_BACKGROUND_COLOR;

    if(hovered)
    {
        background_color = BUTTON_HOVERED_BACKGROUND_COLOR;
    }
    
    // Draw the button, notice how we do both the logic for the button and the rendering. Based on how we decide to implement out ui
    // we might or might not want to split them. This kind of architecture is called Immediate Mode GUI. Or imgui. 

    DrawRectangleRec(rectangle, background_color);

    // Here we draw the text, centered in the button and over the background.
    gui_text(rectangle, text, BUTTON_TEXT_COLOR);

    return clicked;
}