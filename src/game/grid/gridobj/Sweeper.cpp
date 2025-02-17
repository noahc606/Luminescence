#include "Sweeper.h"
#include <nch/cpp-utils/color.h>
#include <nch/cpp-utils/timer.h>
#include <nch/sdl-utils/texture-utils.h>

Sweeper::Sweeper(Skin* skin, uint64_t creationTimeMS)
{
    Sweeper::skin = skin;
    Sweeper::creationTimeMS = creationTimeMS;
}
Sweeper::~Sweeper(){}

void Sweeper::tick(int elapsedIngameTimeMS, int numCols)
{
    uint64_t elapsedTimeMS = elapsedIngameTimeMS-creationTimeMS;
    x = elapsedTimeMS*skin->getBPM()*32./(30.*1000.);

    if(x/32.>numCols+2) {
        offscreen = true;
    }

    //Control oscillation of light brightness from line
    counter += 1.0;
    if(counter>=360) {
        counter = 0;
    }
}

void drawVerticalLine(SDL_Renderer* rend, SDL_Rect* gridR, double scale, int len, double x)
{
    double s = scale;
    if(x<0) {
        len = len+x;
        if(len<=0) {
            return;
        }
        x = 0;
    }
    if(x*s>gridR->w) {
        return;
    }


    SDL_Rect lm;    //Line (Main)
    lm.x = gridR->x+s*x;    lm.y = gridR->y;
    lm.w = s*len;           lm.h = gridR->h;
    if(lm.w<1) lm.w = 1;

    SDL_RenderFillRect(rend, &lm);
}

void Sweeper::draw(SDL_Renderer* rend)
{
    if(offscreen) return;
    drawSweeperLine(rend, nch::Color(255, 255, 0), nch::Color(255, 170, 0));
    drawSweeperHead(rend);
}



void Sweeper::drawSweeperLine(SDL_Renderer* rend, nch::Color c1, nch::Color c2)
{
    //Graphical params
    nch::Color cm = c1;  //Main
    nch::Color cl = c2;  //Left vertical line
    nch::Color cr = c2;  //Right vertical line
    double rl = 13;         //right line length
    double llO1 = 50.*(std::abs(std::sin(counter*M_PI/180.0))); //Left line offset 1
    double ll = 51+llO1;    //left line length

    //Skin params
    SDL_Rect* gridR = skin->getGridRect();
    double s = skin->getGridScale();

    //Right line(s)
    cr.transpare(100);
    for(int i = 0; i<rl; i++) {
        cr.brighten(-1);
        cr.transpare(12);
        SDL_SetRenderDrawColor(rend, cr.r, cr.g, cr.b, cr.a);
        drawVerticalLine(rend, gridR, s, 1, x-rl+i+1);
    }
    
    //Main line(s)
    SDL_SetRenderDrawColor(rend, cm.r, cm.g, cm.b, 255);
    drawVerticalLine(rend, gridR, s, 2, x-rl);

    //Left line(s)
    cl.brighten(-20);
    double opacity = cl.a;
    double brightness = cl.getHSV()[2];
    for(int i = 0; i<ll-1; i++) {
        opacity -= (255./ll);
        cl.a = opacity;
        brightness -= (80./ll);
        cl.setBrightness(brightness);
        SDL_SetRenderDrawColor(rend, cl.r, cl.g, cl.b, cl.a);
        drawVerticalLine(rend, gridR, s, 1, x-rl-i);
    }
}



void Sweeper::drawSweeperHead(SDL_Renderer* rend)
{
    //Graphical params
    double rl = 13;         //right line length
    double llO1 = 50.*(std::abs(std::sin(counter*M_PI/180.0))); //Left line offset 1
    double ll = 51+llO1;    //left line length

    //Skin params
    SDL_Rect* gridR = skin->getGridRect();
    double s = skin->getGridScale();

    //Don't draw if orange line past right side
    if(x*s>gridR->w) {
        return;
    }
    
    //Main rectangle
    SDL_Rect r;
    r.x = gridR->x+s*x-48*s-rl*s+2*s;
    r.y = gridR->y-28*s;
    r.w = 48*s;
    r.h = 28*s;

    float borderSize = 1*s;
    if(borderSize<1) borderSize = 1;
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    nch::TexUtils::renderFillBorderedRect(rend, &r, borderSize);

    //Triangle
    int triW = r.x+r.w;
    SDL_Point p1; p1.x = triW;         p1.y = r.y;         //Top
    SDL_Point p2; p2.x = triW;         p2.y = r.y+r.h;     //Bottom
    SDL_Point p3; p3.x = triW+16*s;    p3.y = r.y+r.h/2;   //Middle
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    nch::TexUtils::renderFillTri(rend, p1, p2, p3);

    p1.y += borderSize;
    p2.y -= borderSize;
    p3.x -= borderSize;
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    nch::TexUtils::renderFillTri(rend, p1, p2, p3);
}

bool Sweeper::isOffscreen() { return offscreen; }
double Sweeper::getMainLineX() { return x-12; }

void Sweeper::setSkin(Skin* s) { skin = s; }