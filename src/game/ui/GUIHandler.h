#pragma once
#include <nch/cpp-utils/arraylist.h>
#include <SDL2/SDL.h>
#include "Button.h"
#include "GUI.h"


class GUIHandler {
public:
    enum IDs {
        btn_selectMode_x = 1,
        btn_selectLayout_x,
        btn_selectSkin_x,
        btn_selectDifficulty_x,

        ttp_selectMode_x,
        ttp_selectLayout_x,
        ttp_selectSkin_x,
        ttp_selectDifficulty_x,
        ttp_loading,


        sksr_main,
    };
    enum CollectionIDs {
        cid_pregame,
        cid_activegame,
    };

    GUIHandler(SDL_Renderer* rend);
    ~GUIHandler();
    void tick(int gamestate);
    void draw();

    std::vector<int> getPossibleVariantsOfID(int id);
    GUI* getGUI(int id, int eid);
    GUI* getGUI(int id);
    int getLastAction();
    int getLastActionData();

    void clearUIs();
    void switchUIs(int collectionID, int modifier);
    void switchUIs(int collectionID);
    void addGUI(GUI* gui, int screenID);
    void addButton(Button* btn, int screenID);
    void resetLastAction();
    void setScreenID(int sid);

private:
    void alignUIs();
    void performAction(int newAction, int newActionData);
    void performAction(int newAction);

    SDL_Renderer* rend;
    nch::ArrayList<GUI> guis;


    int action = -1; int actionData = 0;
    int lastAction = -1; int lastActionData = 0;

    int actionTimer = 0;
    int screenX = 0;
    int screenID = 0;
};