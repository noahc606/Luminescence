#pragma once
#include <memory>
#include <SDL2/SDL.h>
#include "GUIHandler.h"
#include "TileGrid.h"
#include "SkinChanger.h"
#include "SkinSelector.h"

class Game {
public:
    enum States {
        TESTING,
        PREGAME,
        TILE_GRID
    };

    Game();
    ~Game();
    void init(SDL_Renderer* rend);
    void tick();
    void draw();
    void drawDebug(std::stringstream& ss);

    void initTileGrid();
    void switchState(int newGameState, int gameStateData);
    void switchState(int newGameState);

    static SkinChanger getSkinChanger();

private:
    void processAction(int lastAction);
    void fetchSkins(std::string parentDir);
    void loadConfigOptions(std::string configFile);
    bool loadSelectedSkins();

    bool configDemoStart = false;
    
    std::vector<Skin*> skins;
    std::vector<int> loadedSkinIDs;
    int activeSkinID = -1;
    SDL_Renderer* rend = nullptr;
    std::unique_ptr<GUIHandler> guiHandler = nullptr;

    int gamestate = -1;
    TileGrid tg;
    SkinSelector* ss = nullptr;

    TileGrid::GameSettings gameSettings;
    static SkinChanger sc;
    int numSkinChanges = 0;
};