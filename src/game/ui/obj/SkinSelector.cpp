#include "SkinSelector.h"
#include <nch/sdl-utils/input.h>
#include <nch/cpp-utils/log.h>
#include "GUIHandler.h"
#include "Main.h"
#include "Resources.h"


SkinSelector::SkinSelector(SDL_Renderer* rend, int id)
{
    construct(GUI::SKIN_SELECTOR, id);
    SkinSelector::rend = rend;
}
SkinSelector::~SkinSelector(){}

void SkinSelector::loadSkins(std::vector<Skin*>& skins)
{
    SkinSelector::skins = skins;

    for(int i = 0; i<100; i++) {
        SDL_Rect r;
        r.x = -1; r.y = -1; r.w = 0; r.h = 0;
        skinBoxes.push_back(r);
    }
}

void SkinSelector::tick()
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
}

void SkinSelector::draw(int scrX)
{
    //Common values
    int w = Main::getWidth();
    int h = Main::getHeight();
    double scale = Main::getUIScaleAlt(false)*0.625;
    
    //Draw left-hand side's background (skin selection)
    SDL_Rect bg0;
    bg0.x = 24*scale+scrX; bg0.y = 24*scale;
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
            dst.x = (x*68+32)*scale+scrX;
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

std::string SkinSelector::getSkinIDByIndex(int index)
{
    if(index>=0 && index<skins.size()) {
        return skins[index]->getID();
    } else {
        return "?null?";
    }
}

std::string SkinSelector::getSkinParentDirByIndex(int index)
{
    if(index>=0 && index<skins.size()) {
        return skins[index]->getParentDir();
    } else {
        return "?null?";
    }
}

int SkinSelector::getSkinIndexByRes(std::string parentDir, std::string id)
{
    for(int i = 0; i<skins.size(); i++) {
        if(skins[i]->getID()==id && skins[i]->getParentDir()==parentDir) {
            return i;
        }
    }

    nch::Log::warnv(__PRETTY_FUNCTION__, "returning -1", "Couldn't find any skins matching the resource \"%s/%s\"...", parentDir.c_str(), id.c_str());
    return -1;
}

bool SkinSelector::hasFinishedSelection() { return finishedSelection; }

std::vector<int> SkinSelector::getSelectedSkins() { return selectedSkins; }
std::vector<int> SkinSelector::getRandomSelection(int grabSize)
{
    std::srand(time(NULL));

    std::vector<int> res;
    int maxNumSkins = skins.size();

    //Build full list
    for(int i = 0; i<maxNumSkins; i++) res.push_back(i);
    //If bad grabSize, just return full list
    if(grabSize<1) return res;

    //Remove items from list at random until we have less than grabSize # of items
    while(res.size()>grabSize) res.erase(res.begin()+std::rand()%res.size());

    //Swap all items within the list at random (starting with the first, then second, etc.)
    for(int i = 0; i<res.size(); i++) {
        int swapIdx = std::rand()%res.size();

        int temp = res[i];
        res[i] = res[swapIdx];
        res[swapIdx] = temp;
    }

    return res;
}

std::vector<int> SkinSelector::getDefaultSelection(int type)
{
    //Build original skin list and free skin list
    bool originalSkinsExist = true;
    std::vector<std::string> originalSkins = {
        "circles", "urbanization", "inheritance", "square_dance", "day_dream", "strangers", "big_elpaso", "so_that_someone_may_visit_you", "block_the_sky",
        "xop", "automobile_industry",  "please_return_my_cd", "the_bird_singing_in_the_night", "the_speed_of_light", "mekong", "fanatic", "moon_beam", "machine_interface_normal"
    };
    std::vector<std::string> freeSkins = { "neon_sunset", "jades", "whirlworld", "starstruck", "string_theory" };

    //Check if original skins exist
    for(int i = 0; i<originalSkins.size(); i++) {
        if(getSkinIndexByRes("data/skins", originalSkins[i])==-1) {
            originalSkinsExist = false;
        }
    }

    //Build list of preferred skins
    std::vector<std::string> preferredSkins;
    switch(type) {
        case 0: {
            if(!originalSkinsExist) { preferredSkins = freeSkins; }
            else                    { preferredSkins = { "circles", "urbanization", "inheritance", "square_dance", "day_dream", "strangers", "big_elpaso", "so_that_someone_may_visit_you", "block_the_sky" }; }
        } break;
        case 1: {
            if(!originalSkinsExist) { preferredSkins = freeSkins; }
            else                    { preferredSkins = { "xop", "fanatic", "machine_interface_normal" }; }
        } break;
        case 2: {
            if(!originalSkinsExist) { preferredSkins = { "jades", "whirlworld", "neon_sunset" }; }
            else                    { preferredSkins = { "automobile_industry",  "please_return_my_cd", "the_bird_singing_in_the_night", "the_speed_of_light", "mekong", "moon_beam" }; }
        } break;
        default: {
            nch::Log::errorv(__PRETTY_FUNCTION__, "Invalid type (must be 0-2)", "Failed to build default selection for type '%d'", type);
        }
    }

    //Try to add all preferred skins to 'res'.
    std::vector<int> res;
    if(originalSkinsExist) {
        for(int i = 0; i<preferredSkins.size(); i++) {
            res.push_back(getSkinIndexByRes("data/skins", preferredSkins[i]));
        }
    } else {
        for(int i = 0; i<preferredSkins.size(); i++) {
            res.push_back(getSkinIndexByRes("res/skins", preferredSkins[i]));
        }
    }

    //Delete all "-1" from res and return.
    for(int i = res.size()-1; i>=0; i--) if(res[i]==-1) res.erase(res.begin()+i);
    if(res.size()==0) nch::Log::errorv(__PRETTY_FUNCTION__, "Empty list", "Failed to build default selection for type '%d'", type);
    return res;
}

void SkinSelector::cancelFinishedSelection() { finishedSelection = false; }

void SkinSelector::setSelectedSkins(std::vector<int> newSelectedSkins) { selectedSkins = newSelectedSkins; }


void SkinSelector::drawSkinJacket(int index, SDL_Rect dst)
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