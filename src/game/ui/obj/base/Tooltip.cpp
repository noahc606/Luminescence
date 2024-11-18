#include "Tooltip.h"
#include "Main.h"
#include "Resources.h"

Tooltip::Tooltip(SDL_Renderer* rend, std::string text, int x, int y, int id)
{
    construct(GUI::TOOLTIP, id);

    Tooltip::text.init(rend, Resources::getTTF(Resources::primaryFont));
    Tooltip::text.setText(text);
    
    Tooltip::x = x;
    Tooltip::y = y;
    //setText(textStr);
}
Tooltip::Tooltip(SDL_Renderer* rend, std::string text, int id): Tooltip(rend, text, 0, 0, id){}

Tooltip::~Tooltip(){}

void Tooltip::draw(int scrX)
{
    text.setScale(Main::getTextScale()*0.0625*2);
    text.setShadowRelPos(-24, 24);
    text.setShadowFadeFactor(1.0);
    text.setShadowCustomColor(nch::Color(0, 0, 0));
    text.setShadowing(true);
    text.draw(x+scrX, y);
}

double Tooltip::getTextRealWidth() { return text.getWidth(); }
double Tooltip::getTextRealHeight() { return text.getHeight(); }