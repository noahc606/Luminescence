#pragma once
#include <SDL2/SDL.h>
#include "Skin.h"

class Sweeper {
public:
    Sweeper(Skin* skin, uint64_t creationTimeMS);
    ~Sweeper();

    void tick(int elapsedIngameTimeMS, int numCols);
    void draw(SDL_Renderer* rend);

    bool isOffscreen();
    double getMainLineX();

    void setSkin(Skin* s);

private:
    void drawSweeperHead(SDL_Renderer* rend);
    void drawSweeperLine(SDL_Renderer* rend, nch::Color cm, nch::Color cr);

    uint64_t creationTimeMS;
    Skin* skin;
    bool offscreen = false;
    double x = 0;
    double counter = 0.0;
};