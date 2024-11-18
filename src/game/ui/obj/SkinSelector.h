#pragma once
#include "GUI.h"
#include <SDL2/SDL.h>
#include <string>
#include "Button.h"
#include "Skin.h"


class SkinSelector : public GUI {
public:
    SkinSelector(SDL_Renderer* rend, int id);
    ~SkinSelector();
    void loadSkins(std::vector<Skin*>& skins);

    void tick();
    void draw(int scrX);

    std::string getSkinIDByIndex(int index);
    std::string getSkinParentDirByIndex(int index);
    int getSkinIndexByRes(std::string parentDir, std::string id);
    bool hasFinishedSelection();
    std::vector<int> getSelectedSkins();
    std::vector<int> getRandomSelection(int grabSize);
    std::vector<int> getDefaultSelection(int type);

    void cancelFinishedSelection();
    void setSelectedSkins(std::vector<int> newSelectedSkins);

private:
    void drawSkinJacket(int index, SDL_Rect dst);

    std::vector<SDL_Rect> skinBoxes;
    int skinBoxHovered = -1;
    int skinBoxLastSelected = -1;

    std::vector<int> selectedSkins;
    std::vector<Skin*> skins;

    bool finishedSelection = false;
    SDL_Renderer* rend;

};