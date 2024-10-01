#include "SkinSelect.h"
#include <nch/cpp-utils/log.h>
#include <nch/sdl-utils/input.h>
#include <nch/sdl-utils/text.h>
#include "Main.h"
#include "Resources.h"
#include "Button.h"

SkinSelect::SkinSelect(){}
SkinSelect::~SkinSelect(){}
void SkinSelect::init(SDL_Renderer* rend, std::vector<Skin*>& skins)
{
    SkinSelect::rend = rend;
    SkinSelect::skins = skins;

    for(int i = 0; i<100; i++) {
        SDL_Rect r;
        r.x = -1; r.y = -1; r.w = 0; r.h = 0;
        skinBoxes.push_back(r);
    }

    btnStart.init(rend);
    btnStart.setText("Start");
}

void SkinSelect::tick()
{
    int mx = nch::Input::getMouseX();
    int my = nch::Input::getMouseY();

    bool found = false;
    for(int i = 0; i<skinBoxes.size(); i++) {
        int sbX = skinBoxes[i].x;
        int sbY = skinBoxes[i].y;
        int sbW = skinBoxes[i].w;
        int sbH = skinBoxes[i].h;

        if(mx>sbX && mx<=sbX+sbW) {
            if(my>sbY && my<=sbY+sbH) {
                skinBoxHovered = i;
                found = true;
                break;
            }
        }
    }

    if(!found) {
        skinBoxHovered = -1;
    }

    //If we are clicking on a skin box
    if(nch::Input::mouseDownTime(1)==1 && skinBoxHovered!=-1) {
        int alreadyExistsAt = -1;
        for(int i = 0; i<selectedSkins.size(); i++) {
            if(selectedSkins[i]==skinBoxHovered) {
                alreadyExistsAt = i;
                break;
            }
        }

        //Modify skin collection if the skin exists
        if(getSkinIDByIndex(skinBoxHovered)!="?null?") {
            //If not selected: Add new skin to collection
            if(alreadyExistsAt==-1) {
                selectedSkins.push_back(skinBoxHovered);
                skinBoxLastSelected = skinBoxHovered;
            //If already selected: Remove this skin from collection
            } else {
                selectedSkins.erase(selectedSkins.begin()+alreadyExistsAt);
                skinBoxLastSelected = -1;
            }
        }
    }

    btnStart.tick();
    
    if(btnStart.wasClicked()) {
        gameStart = true;
        btnStart.unclick();
    }
}

void SkinSelect::draw()
{
    //Common values
    int w = Main::getWidth();
    int h = Main::getHeight();
    double scale = Main::getUIScaleAlt(false)*0.625;
    
    //Draw left-hand side's background (skin selection)
    SDL_Rect bg0;
    bg0.x = 24*scale; bg0.y = 24*scale;
    bg0.w = (68*10+12)*scale; bg0.h = (68*10+12)*scale;
    SDL_SetRenderDrawColor(rend, 100, 100, 100, 255);
    SDL_RenderFillRect(rend, &bg0);

    //Draw top-right box (most recent skin selected)
    if(skinBoxLastSelected!=-1) {
        SDL_Rect bg1;
        bg1.x = bg0.x+bg0.w+16*scale;
        bg1.y = bg0.y;
        bg1.w = 4*48*scale;
        bg1.h = 4*48*scale;

        drawSkinJacket(skinBoxLastSelected, bg1);
    }

    //Draw start button
    btnStart.bottomSideAnchorY = Main::getHeight()-80*scale;
    btnStart.bottomSideMaxY = bg0.y+bg0.h;
    btnStart.scale = Main::getUIScaleAlt(false)*0.625;
    btnStart.x = (Main::getWidth()+(bg0.x+bg0.w))/2-btnStart.getRealWidth()/2;   //X: In between left side of bg0 and right side of screen
    btnStart.draw();

    //Draw skin icons (may be filled or empty)
    for(int x = 0; x<10; x++) {
        for(int y = 0; y<10; y++) {
            int index = y*10+x;

            //Get whether this button is selected. If so, track its order within selection
            bool selected = false;
            int selectedOrder = -1;
            for(int i = 0; i<selectedSkins.size(); i++) {
                if(selectedSkins[i]==index) {
                    selected = true;
                    selectedOrder = i;
                }
            }

            //Create SDL_Rect representing this box, populate skinBoxes[index] with this object
            SDL_Rect dst;
            dst.x = (x*68+32)*scale;
            dst.y = (y*68+32)*scale;
            dst.w = 64*scale;
            dst.h = 64*scale;
            skinBoxes[index] = dst;
            
            //Draw skin jacket
            drawSkinJacket(index, dst);

            //If selected, draw selection overlay
            if(selected) {
                SDL_SetRenderDrawColor(rend, 0, 0, 0, 128);
                SDL_RenderFillRect(rend, &dst);

                std::stringstream ss; ss << (selectedOrder+1);
                nch::Text t;
                t.init(rend, Resources::getTTF(Resources::primaryFont));
                t.setText(ss.str());
                t.setScale(Main::getTextScale()*0.2);
                t.draw(dst.x+dst.w/2-t.getWidth()/2, dst.y+dst.h/2-t.getHeight()/2);
            }
        }
    }



    //Skin box hovered
    if(skinBoxHovered!=-1) {
        SDL_Rect hDst;
        hDst = skinBoxes[skinBoxHovered];
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 80);
        SDL_RenderFillRect(rend, &hDst);
    }

}

std::string SkinSelect::getSkinIDByIndex(int index)
{
    if(index>=0 && index<skins.size()) {
        return skins[index]->getID();
    } else {
        return "?null?";
    }
}
int SkinSelect::getSkinIndexByRes(std::string parentDir, std::string id)
{
    for(int i = 0; i<skins.size(); i++) {
        if(skins[i]->getID()==id && skins[i]->getParentDir()==parentDir) {
            printf("ID: %d\n", i);
            return i;
        }
    }

    nch::Log::warnv(__PRETTY_FUNCTION__, "returning -1", "Couldn't find any skins matching the resource \"%s/%s\"...", parentDir.c_str(), id.c_str());
    return -1;
}

std::string SkinSelect::getSkinParentDirByIndex(int index)
{
    if(index>=0 && index<skins.size()) {
        return skins[index]->getParentDir();
    } else {
        return "?null?";
    }
}
bool SkinSelect::hasGameStarted() { return gameStart; }
std::vector<int> SkinSelect::getSelectedSkins() { return selectedSkins; }

void SkinSelect::cancelGameStart() { gameStart = false; }

void SkinSelect::drawSkinJacket(int index, SDL_Rect dst)
{
    std::string skinID = getSkinIDByIndex(index);
    std::string skinParentDir = getSkinParentDirByIndex(index);

    if(skinID!="?null?") {
        Resources::getTexMakeException();
        SDL_Texture* jacketTex = Resources::getTex(skinParentDir+"/"+skinID+"/jacket");
        if(jacketTex!=nullptr) {
            SDL_RenderCopy(rend, jacketTex, NULL, &dst);
        } else {
            SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
            SDL_RenderFillRect(rend, &dst);

            std::string skinName = skinID;
            for(int si = 0; si<skins.size(); si++) {
                if(skins[si]->getID()==skinID) {
                    skinName = skins[si]->getStylizedName();
                    break;
                }
            }
            nch::Text t;
            t.init(rend, Resources::getTTF(Resources::primaryFont));
            t.setText(skinName);
            t.setScale(Main::getTextScale()*0.125/2);
            t.setWrapLength(680);

            int tx = dst.x;
            int ty = dst.y;
            if(ty<0) ty = 0;
            t.draw(tx, ty);
            
        }
    }
}