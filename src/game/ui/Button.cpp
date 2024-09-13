#include "Button.h"
#include <nch/sdl-utils/gfx/TexUtils.h>
#include "Main.h"
#include "Resources.h"

Button::Button(SDL_Renderer* rend, std::string textStr)
{
    Button::rend = rend;

    text.init(rend, Resources::getTTF(Resources::carlitoFont));
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

    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    nch::TexUtils::renderFillBorderedRect(rend, &br, scale);
    
    text.setScale(Main::getTextScale()*0.125);
    text.draw(br.x+br.w/2-text.getWidth()/2, br.y+br.h/2-text.getHeight()/2);
}

double Button::getRealWidth() { return unscaledW*scale; }
double Button::getRealHeight() { return unscaledH*scale; }
SDL_Rect Button::getRect() { return btnRect; }

void Button::setText(std::string txt)
{
    text.setText(txt);
}