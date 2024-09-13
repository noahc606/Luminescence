#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "Skin.h"

class Particle {
public:
    enum Types {
        COMPLETED_SQUARE_FLASH,
        COMPLETED_SQUARE_OUTLINE,
        SWEEP_COUNT_INDICATOR = 123,
        SCORE_BONUS,
    };

    Particle(double x, double y, int type, SDL_Renderer* rend, int info);
    Particle(int type, SDL_Renderer* rend, int info);
    Particle(double x, double y, int type, nch::Color c);
    Particle(double x, double y, int type);
    void init(double x, double y, int type, SDL_Renderer* rend, nch::Color c, int info);

    ~Particle();
    void tick();
    void draw(SDL_Renderer*, Skin*);

    int getAge(); int getMaxAge();

private:
    void drawCompletedSquareFlash(SDL_Renderer*, Skin*);
    void drawCompletedSquareOutline(SDL_Renderer*, Skin*);
    void drawSweepCountIndicator(SDL_Renderer*, Skin*);
    void drawScoreBonus(SDL_Renderer*, Skin*);

    double x = 0, y = 0;
    nch::Color color;
    int type = -1;
    int age = 0, maxAge = 40;
    nch::Text infoTxt;
};