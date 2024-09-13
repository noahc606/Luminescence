#pragma once
#include <SDL2/SDL.h>
#include "TileGrid.h"
#include "SkinChanger.h"
#include "SkinSelect.h"

class Game {
public:
    enum States {
        TESTING, MODE_SELECT, SKIN_SELECT, TILE_GRID
    };

    enum Difficulty {
        BEGINNER = 0, INTERMEDIATE, ADVANCED, EXPERT, SUPER_EXPERT
    };

    Game();
    ~Game();
    void init(SDL_Renderer* rend);
    void tick();
    void draw();
    void drawDebug(std::stringstream& ss);

    void initTileGrid(int difficulty);
    void switchState(int newGameState, int gameStateData);
    void switchState(int newGameState);

    static SkinChanger getSkinChanger();

private:
    void fetchSkins(std::string parentDir);
    void loadConfigOptions(std::string configFile);
    bool loadSelectedSkins();

    bool configDemoStart = false;
    
    std::vector<Skin*> skins;
    std::vector<int> loadedSkinIDs;
    int activeSkinID = -1;
    SDL_Renderer* rend = nullptr;

    int gamestate = -1;
    TileGrid tg;
    SkinSelect ss;
    int selectedDifficulty = BEGINNER;
    static SkinChanger sc;
    int numSkinChanges = 0;
};