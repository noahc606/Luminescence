#pragma once
#include "GUI.h"
#include <string>
#include <nch/sdl-utils/text.h>

class Tooltip : public GUI {
public:
    Tooltip(SDL_Renderer* rend, std::string text, int x, int y, int id);
    Tooltip(SDL_Renderer* rend, std::string text, int id);
    ~Tooltip();
    
    void draw(int scrX);

    double getTextRealWidth();
    double getTextRealHeight();

    int x;
    int y;
private:
    nch::Text text;
};