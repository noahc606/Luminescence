#pragma once
#include <nch/sdl-utils/gfx/Text.h>
#include <SDL2/SDL.h>

class Button {
public:
    enum AlignX {
        LEFT, CENTER, RIGHT
    };

    Button(SDL_Renderer* rend, std::string text);
    ~Button();

    void draw(int x);

    double getRealWidth();
    double getRealHeight();
    SDL_Rect getRect();

    void setText(std::string txt);


    int alignX = CENTER;
    int bottomSideAnchorY = 0;  //Unscaled Y value the bottom side of this button should "try" to be
    int bottomSideMaxY = 99999; //'bottomSideAnchorY' can't be greater than this no matter what
    int unscaledW = 240;
    int unscaledH = 40;
    double scale = 1;

private:
    SDL_Renderer* rend = nullptr;
    nch::Text text;
    SDL_Rect btnRect;
};