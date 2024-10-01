#include "Button.h"
#include <nch/sdl-utils/texture-utils.h>
#include <nch/sdl-utils/input.h>
#include "Main.h"
#include "Resources.h"

Button::Button(){}
Button::Button(SDL_Renderer* rend, std::string textStr)
{
    init(rend);
    setText(textStr);
}
Button::~Button(){}

void Button::init(SDL_Renderer* rend)
{
    Button::rend = rend;
    text.init(rend, Resources::getTTF(Resources::oswaldFont));
}

void Button::draw()
{
    SDL_Rect br = getRect(); //Button rect

    //Make sure button doesn't go lower than it's supposed to
    if(br.y+br.h>bottomSideMaxY) {
        br.y = bottomSideMaxY-br.h;
    }

    rect = br;

    int cval;
    if(hovering) {
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

void Button::tick()
{
    int mx = nch::Input::getMouseX();
    int my = nch::Input::getMouseY();

    //Update rectangle object
    rect.w = getRealWidth();
    rect.h = getRealHeight();
    rect.x = x;
    rect.y = bottomSideAnchorY;

    //Update whether we are hovering over button
    if( mx>rect.x && mx<=rect.x+rect.w &&
        my>rect.y && my<=rect.y+rect.h
    ) {
        if(!hovering) Resources::playAudio("res/audio/button_hover");
        hovering = true;
    } else {
        hovering = false;
    }

    //Click button if hovering and mouse 1 is clicked
    if(hovering && nch::Input::mouseDownTime(1)==1) {
        if(!clicked) Resources::playAudio("res/audio/button_click");
        clicked = true;
    }
}

double Button::getRealWidth() { return unscaledW*scale; }
double Button::getRealHeight() { return unscaledH*scale; }
SDL_Rect Button::getRect() { return rect; }
bool Button::wasClicked() { return clicked; }

void Button::setText(std::string txt) { text.setText(txt); }
void Button::unclick() { clicked = false; }