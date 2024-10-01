#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "Skin.h"
#include "Button.h"

class SkinSelect {
public:
    SkinSelect();
    ~SkinSelect();
    void init(SDL_Renderer* rend, std::vector<Skin*>& skins);

    void tick();
    void draw();

    std::string getSkinIDByIndex(int index);
    int getSkinIndexByRes(std::string parentDir, std::string id);
    std::string getSkinParentDirByIndex(int index);
    bool hasGameStarted();
    std::vector<int> getSelectedSkins();

    void cancelGameStart();
private:
    void drawStartButton();
    void drawSkinJacket(int index, SDL_Rect dst);

    SDL_Renderer* rend;
    std::vector<Skin*> skins;

    Button btnStart;


    std::vector<SDL_Rect> skinBoxes;
    int skinBoxHovered = -1;
    int skinBoxLastSelected = -1;

    std::vector<int> selectedSkins;
    bool gameStart = false;
};