#include "Particle.h"
#include <sstream>
#include "Main.h"
#include "Resources.h"

Particle::Particle(double x, double y, int type, SDL_Renderer* rend, int info) { init(x, y, type, rend, nch::Color(255, 255, 255), info); }
Particle::Particle(int type, SDL_Renderer* rend, int info): Particle(0, 0, type, rend, info){}

Particle::Particle(double x, double y, int type, nch::Color c) { init(x, y, type, nullptr, c, 0); }
Particle::Particle(double x, double y, int type): Particle(x, y, type, nch::Color(255, 255, 255)){}

void Particle::init(double x, double y, int type, SDL_Renderer* rend, nch::Color c, int info)
{
    Particle::x = x;
    Particle::y = y;
    Particle::type = type;
    Particle::color = c;

    //Init infoTxt based on type
    if(rend!=nullptr) {
        if(type==SWEEP_COUNT_INDICATOR) {
            infoTxt.init(rend, Resources::getTTF(Resources::oswaldFont));
            std::stringstream ss; ss << info;
            infoTxt.setText(ss.str());
        }
        if(type==SCORE_BONUS) {
            std::string str = "???null???";
            infoTxt.init(rend, Resources::getTTF(Resources::primaryFont));
            if(info==0) { str = "SINGLE COLOR BONUS +1000pts"; }
            if(info==1) { str = "ALL CLEAR BONUS +10000pts"; }
            infoTxt.setText(str);
        }
    }

    //Set age and/or maxAge based on type
    if(type==SWEEP_COUNT_INDICATOR) {
        maxAge = 127;
    }
    if(type==SCORE_BONUS) {
        maxAge = 255;
    }
    if(type==COMPLETED_SQUARE_OUTLINE) {
        age = rand()%20;
    }
}

Particle::~Particle(){}

void Particle::tick()
{
    age++;
}

void Particle::draw(SDL_Renderer* rend, Skin* skin)
{
    switch(type) {
        case COMPLETED_SQUARE_FLASH:    { drawCompletedSquareFlash(rend, skin); } break;
        case COMPLETED_SQUARE_OUTLINE:  { drawCompletedSquareOutline(rend, skin); } break;
        case SWEEP_COUNT_INDICATOR:     { drawSweepCountIndicator(rend, skin); } break;
        case SCORE_BONUS:               { drawScoreBonus(rend, skin); } break;
    }
}

int Particle::getAge() { return age; }
int Particle::getMaxAge() { return maxAge; }

void Particle::drawCompletedSquareFlash(SDL_Renderer* rend, Skin* skin)
{
    SDL_Rect* gr = skin->getGridRect();
    double gs = skin->getGridScale();;

    SDL_Rect r;
    r.x = gr->x+x*32*gs;
    r.y = gr->y+y*32*gs;
    r.w = 64*gs;
    r.h = 64*gs;

    SDL_SetRenderDrawColor(rend, 0, 0, 0, 128-age*3);
    SDL_RenderFillRect(rend, &r);
}

void Particle::drawCompletedSquareOutline(SDL_Renderer* rend, Skin* skin)
{
    SDL_Rect* gr = skin->getGridRect();
    double gs = skin->getGridScale();;

    SDL_Rect r;
    r.x = gr->x+x*32*gs;
    r.y = gr->y+y*32*gs;
    r.w = 64*gs;
    r.h = 64*gs;

    double outdist = gs*32*((40-age)/40.);
    r.x -= outdist; r.w += (2*outdist);
    r.y -= outdist; r.h += (2*outdist);

    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, 200-age*5);
    SDL_RenderDrawRect(rend, &r);
}

void Particle::drawSweepCountIndicator(SDL_Renderer* rend, Skin* skin)
{
    SDL_Rect* gr = skin->getGridRect();
    double gs = skin->getGridScale();
    double ts = 0.15*gs;
    
    
    infoTxt.setScale(ts);
    infoTxt.setTextColor(nch::Color(255, 255, 255, 255-age*2));
	infoTxt.draw(
		gr->x+gr->w-infoTxt.getWidth()*1.5,
		gr->y-infoTxt.getHeight()-gs*age/2
	);
}

void Particle::drawScoreBonus(SDL_Renderer* rend, Skin* skin)
{
    SDL_Rect* gr = skin->getGridRect();
    double gs = skin->getGridScale();
    double ts = 0.15*gs;
    
    
    infoTxt.setScale(ts);
    infoTxt.setTextColor(nch::Color(255, 255, 255, 255-age));
	infoTxt.draw(
		gr->x+gr->w/2-infoTxt.getWidth()/2,
		gr->y+gr->h/2-infoTxt.getHeight()/2
	);
}