#pragma once
#include <nch/sdl-utils/text.h>
#include <SDL2/SDL.h>

class Button {
public:
    enum AlignX {
        LEFT, CENTER, RIGHT
    };

    Button();
    Button(SDL_Renderer* rend, std::string text);
    ~Button();
    void init(SDL_Renderer* rend);

    void draw();
    void tick();

    double getRealWidth();
    double getRealHeight();
    SDL_Rect getRect();
    bool wasClicked();

    void setText(std::string txt);
    void unclick();

    int x = 0;
    int bottomSideAnchorY = 0;  //Unscaled Y value the bottom side of this button should "try" to be
    int bottomSideMaxY = 99999; //'bottomSideAnchorY' can't be greater than this no matter what
    int unscaledW = 240;
    int unscaledH = 40;
    double scale = 1;

private:
    SDL_Renderer* rend = nullptr;
    SDL_Rect rect;
    nch::Text text;

    bool hovering = false;
    bool clicked = false;
};