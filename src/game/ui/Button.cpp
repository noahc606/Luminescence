#include "Button.h"
#include <nch/sdl-utils/texture-utils.h>
#include "Main.h"
#include "Resources.h"

Button::Button(SDL_Renderer* rend, std::string textStr)
{
    Button::rend = rend;

    text.init(rend, Resources::getTTF(Resources::oswaldFont));
    setText(textStr);
}
Button::~Button(){}

void Button::draw(int x)
{
    SDL_Rect br;
    br.w = getRealWidth();
    br.h = getRealHeight();
    br.x = x;
    br.y = bottomSideAnchorY;

    //Make sure button doesn't go lower than it's supposed to
    if(br.y+br.h>bottomSideMaxY) {
        br.y = bottomSideMaxY-br.h;
    }

    btnRect = br;

    bool active = false;
    int cval;
    if(active) {
        //White button
        cval = 255-96;
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        text.setTextColor(nch::Color(0, 0, 0));
    } else {
        //Black button
        cval = 255;
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        text.setTextColor(nch::Color(255, 255, 255));
    }

    nch::TexUtils::renderFillBorderedRect(rend, &br, std::ceil(scale*3.), nch::Color(cval, cval, cval));
    
    text.setScale(Main::getTextScale()*0.0625);
    text.setShadowRelPos(-24, 24);
    text.setShadowFadeFactor(1.0);
    text.setShadowCustomColor(nch::Color(128, 128, 128));
    text.draw(br.x+Main::getTextScale()*8, br.y+br.h/2-text.getHeight()/2);
}

double Button::getRealWidth() { return unscaledW*scale; }
double Button::getRealHeight() { return unscaledH*scale; }
SDL_Rect Button::getRect() { return btnRect; }

void Button::setText(std::string txt)
{
    text.setText(txt);
}