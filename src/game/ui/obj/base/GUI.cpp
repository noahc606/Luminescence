#include "GUI.h"

GUI::GUI(){}
GUI::~GUI(){}

void GUI::construct(UIType type, int id, int eid)
{
    if(constructed) return;
    GUI::type = type;
    GUI::id = id;
    GUI::eid = eid;
    constructed = true;
}
void GUI::construct(UIType type, int id) { construct(type, id, 0); }

void GUI::draw(int scrX){}
void GUI::draw(){ draw(0); }
void GUI::tick(){}

int GUI::getEID() { return eid; }
int GUI::getID() { return id; }
int GUI::getScreenID() { return screenID; }
GUI::UIType GUI::getType() { return type; }

void GUI::setScreenID(int sid) { screenID = sid; }
