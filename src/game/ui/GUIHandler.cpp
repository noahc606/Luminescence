#include "GUIHandler.h"
#include <map>
#include <nch/sdl-utils/timer.h>
#include "Game.h"
#include "Main.h"
#include "Tooltip.h"


GUIHandler::GUIHandler(SDL_Renderer* rend)
{
    GUIHandler::rend = rend;
}
GUIHandler::~GUIHandler(){}

void GUIHandler::tick(int gamestate)
{
    //Tick all GUIs that should be ticked
    for(int i = 0; i<guis.size(); i++) {
        switch(guis[i].getType()) {
            case GUI::BUTTON: guis[i].tick(); break;
            case GUI::SKIN_SELECTOR: {
                if(screenID==2) {
                    guis[i].tick();
                }
            } break;
        }
    }

    //Cancel actions under certain circumstances
    if(action==btn_selectSkin_x && actionData==0) {
        GUI* temp = getGUI(sksr_main);
        if(temp==nullptr) {
            performAction(-1);
        } else {
            SkinSelector* skinSelector = (SkinSelector*)temp;
            if(skinSelector->getSelectedSkins().size()==0) {
                performAction(-1);
            }
        }
    }

    //Set destination depending on action
    int scrDestination = action;
    if(action==btn_selectLayout_x) {
        switch(actionData) {
            default: {
                scrDestination = action+1;
            } break;
            case 6: {
                scrDestination = action;
            } break;
        }
    }

    //Change screen and track action timer depending on action
    
    switch(action) {
        case 0: break;
        case btn_selectMode_x:
        case btn_selectLayout_x:
        case btn_selectSkin_x:
        case btn_selectDifficulty_x:
        {
            //If we clicked a back button, factor is -1.
            int factor = 1;
            if(actionData==-100) { factor = -1; }
            if(factor==-1) scrDestination = action-2;

            //Action timer increments
            actionTimer++;
            int speedFactor = 1;
            if(scrDestination-screenID==2) { speedFactor = 2; }
            screenX += (speedFactor*factor*(Main::getWidth())/40);
            
            //Action timer finishes
            if(actionTimer>=40) {
                lastAction = action;
                lastActionData = actionData;
                performAction(-1);
                screenID = scrDestination;
            }
        } break;
        default: {
            screenX = screenID*Main::getWidth();
        } break;
    }

    //Action in progress


    //Align UIs.
    alignUIs();

    //Track buttons being clicked
    for(int i = 0; i<guis.size(); i++) {
        if(guis[i].getType()==GUI::BUTTON) {
            Button* btn = (Button*)(&guis[i]);
            if(btn->wasClicked()) {
                if(actionTimer==0) {
                    performAction(btn->getID(), btn->getEID());
                }
                    
                btn->unclick();
            }
        }

    }


}

void GUIHandler::draw()
{
    for(int i = 0; i<guis.size(); i++) {;
        int dx = (guis[i].getScreenID())*Main::getWidth();

        guis[i].draw(-screenX+dx);
    }
}

std::vector<int> GUIHandler::getPossibleVariantsOfID(int id)
{
    std::vector<int> res;
    for(int i = 0; i<guis.size(); i++)
        if(guis[i].getID()==id)
            res.push_back(guis[i].getEID());
    return res;
}
GUI* GUIHandler::getGUI(int id, int eid)
{
    for(int i = 0; i<guis.size(); i++) {
        if(guis[i].getID()==id && guis[i].getEID()==eid) {
            return &guis[i];
        }
    }
    return nullptr;
}

GUI* GUIHandler::getGUI(int id)
{
    for(int i = 0; i<guis.size(); i++) {
        if(guis[i].getID()==id) {
            return &guis[i];
        }
    }
    return nullptr;
}

int GUIHandler::getLastAction() { return lastAction; }
int GUIHandler::getLastActionData() { return lastActionData; }

void GUIHandler::clearUIs()
{
    guis.clear();
}

void GUIHandler::switchUIs(int collectionID, int modifier)
{
    clearUIs();

    switch(collectionID) {
        case cid_activegame: {} break;
        case cid_pregame:
        {
            /* Screen 0: Select mode */
            std::vector<std::string> modes = { "Challenge", "Survival" };//, "Pacifist", "VS CPU", "Time Attack" };
            addGUI(new Tooltip(rend, "Select Mode", GUIHandler::ttp_selectMode_x), 0);
            for(int i = 0; i<modes.size(); i++) {
                addButton(new Button(rend, modes[i]+" Mode", GUIHandler::btn_selectMode_x, i), 0);
            }

            /* Screen 1: Select layout */
            std::map<int, std::string> layouts;
            if(modifier!=1) {
                layouts.insert(std::make_pair(0, "Default"));
            } else {
                layouts.insert(std::make_pair(1, "Preset I"));
                layouts.insert(std::make_pair(2, "Preset II"));
            }
            layouts.insert(std::make_pair(3, "Shuffle-20"));
            layouts.insert(std::make_pair(4, "Shuffle-40"));
            layouts.insert(std::make_pair(5, "Shuffle All"));
            layouts.insert(std::make_pair(6, "Custom"));

            addGUI(new Tooltip(rend, "Select Layout", GUIHandler::ttp_selectLayout_x), 1);
            for(auto elem : layouts) {
                addButton(new Button(rend, elem.second, GUIHandler::btn_selectLayout_x, elem.first), 1);
            }
            addButton(new Button(rend, "Go Back", GUIHandler::btn_selectLayout_x, -100), 1);

            /* Screen 2: Select skins (if we chose a "Custom" layout) */
            addGUI(new SkinSelector(rend, GUIHandler::sksr_main), 2);
            addButton(new Button(rend, "Start", GUIHandler::btn_selectSkin_x, 0), 2);
            addButton(new Button(rend, "Go Back", GUIHandler::btn_selectSkin_x, -100), 2);

            /* Screen 3: Select difficulty */
            addGUI(new Tooltip(rend, "Select Difficulty", GUIHandler::ttp_selectDifficulty_x), 3);
            std::vector<std::string> difficulties = { "Beginner", "Intermediate", "Advanced", "Expert", "Super Expert" };
            for(int i = 0; i<difficulties.size(); i++) {
                addButton(new Button(rend, difficulties[i], GUIHandler::btn_selectDifficulty_x, i), 3);
            }
            addButton(new Button(rend, "Go Back", GUIHandler::btn_selectDifficulty_x, -100), 3);
        
            /* Screen 4: Loading */
            addGUI(new Tooltip(rend, "Loading...", GUIHandler::ttp_loading), 4);

        } break;
    }
}
void GUIHandler::switchUIs(int collectionID) { switchUIs(collectionID, 0); }

void GUIHandler::addGUI(GUI* ui, int screenID)
{
    ui->setScreenID(screenID);
    guis.pushBack(ui);
}


void GUIHandler::addButton(Button* btn, int screenID) { addGUI(btn, screenID); }
void GUIHandler::resetLastAction() { lastAction = -1; }
void GUIHandler::setScreenID(int sid) { screenID = sid; }

void GUIHandler::alignUIs()
{
    //Modify button locations so they are all centered and stacked vertically
    std::vector<int> alignType1IDs = { btn_selectMode_x, btn_selectLayout_x, btn_selectDifficulty_x, -1 };
    std::vector<int> alignType1TTPs = { ttp_selectMode_x, ttp_selectLayout_x, ttp_selectDifficulty_x, ttp_loading };
    for(int i = 0; i<alignType1IDs.size(); i++) {
        
        int highestButtonHeight = -10000;
        std::vector<int> variants = getPossibleVariantsOfID(alignType1IDs[i]);
        if(variants.size()!=0) {
            //Set minimum button-group area height
            bool backButtonExists = false;
            for(int v : variants) { if(v==-100) backButtonExists = true; }
            int btnHeight = ((Button*)getGUI(alignType1IDs[i], variants[0]))->getRealHeight();
            int minAllowedHeight = variants.size()*btnHeight;
            if(backButtonExists) minAllowedHeight = (variants.size()+1)*btnHeight;

            //Set button-group area height
            int allowedHeight = Main::getHeight()/2;
            if(allowedHeight<minAllowedHeight) allowedHeight = minAllowedHeight;

            //Align buttons
            
            int cnt = 0;
            for(int j = 0; j<variants.size(); j++) {
                cnt++;
                GUI* temp = getGUI(alignType1IDs[i], variants[j]);
                if(temp==nullptr) break;

                Button* sm = (Button*)temp;
                sm->x = Main::getWidth()/2-sm->getRealWidth()/2;
                int tHeight = Main::getHeight()/2-allowedHeight/2;

                //If this is a back button, make it appear lower
                if(variants[j]==-100) { cnt++; }

                sm->bottomSideAnchorY = cnt*allowedHeight/(variants.size()+1)-sm->getRealHeight()/2+tHeight;
                if(highestButtonHeight==-10000) {
                    highestButtonHeight = sm->getRect().y;
                }
            }
        }

        //Align tooltip(s)
        GUI* temp = getGUI(alignType1TTPs[i]);
        if(temp==nullptr) continue;
        Tooltip* ttp = (Tooltip*)temp;
        ttp->x = Main::getWidth()/2-ttp->getTextRealWidth()/2;

        if(highestButtonHeight!=-10000) {
            ttp->y = highestButtonHeight-ttp->getTextRealHeight()-8*Main::getTextScale();    
        } else {
            ttp->y = Main::getHeight()/2-ttp->getTextRealHeight()/2;
        }        
    }


    //Modify button locations so they are all stacked near the bottom right of the screen
    std::vector<int> alignType2IDs = { btn_selectSkin_x };
    for(int at2ID : alignType2IDs) {
        std::vector<int> variants = getPossibleVariantsOfID(at2ID);
        if(variants.size()==0) continue;

        //Align buttons
        int currentBtnHeight = 0;
        for(int i = 0; i<variants.size(); i++) {
            GUI* temp = getGUI(at2ID, variants[i]);
            if(temp==nullptr) break;

            //Get alignment info
            double scale = Main::getUIScaleAlt(false)*0.625;
            SDL_Rect bg0;
            bg0.x = 24*scale; bg0.y = 24*scale;
            bg0.w = (68*10+12)*scale; bg0.h = (68*10+12)*scale;

            //Align buttons
            Button* sm = (Button*)temp;
            sm->bottomSideAnchorY = Main::getHeight()-80*scale+currentBtnHeight;
            sm->bottomSideMaxY = bg0.y+bg0.h+currentBtnHeight;
            sm->x = (Main::getWidth()+(bg0.x+bg0.w))/2-sm->getRealWidth()/2;   //X: In between left side of bg0 and right side of screen

            currentBtnHeight += sm->getRealHeight();
        }
    }
}

void GUIHandler::performAction(int newAction, int newActionData)
{
    action = newAction;
    actionData = newActionData;
    
    actionTimer = 0;
}

void GUIHandler::performAction(int newAction)
{
    performAction(newAction, 0);
}