#include "Game.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <nch/cpp-utils/filepath.h>
#include <nch/cpp-utils/fs-utils.h>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/timer.h>
#include "GridImg.h"
#include "Main.h"

SkinChanger Game::sc;

Game::Game(){}
Game::~Game(){}

void Game::init(SDL_Renderer* rend)
{
    uint64_t t0 = nch::Timer::getTicks();
    
    Game::rend = rend;
    Game::guiHandler = std::make_unique<GUIHandler>(rend);

    fetchSkins("data/skins");
    fetchSkins("res/skins");
    loadConfigOptions("data/config");
    switchState(PREGAME);
    
    uint64_t t1 = nch::Timer::getTicks();
    printf("Game initialized in %dms.\n", t1-t0);
}

void Game::tick()
{
    guiHandler->tick(gamestate);
    
    processAction(guiHandler->getLastAction());
    if(guiHandler->getLastActionData()!=-1) {
        guiHandler->resetLastAction();
    }

    switch(gamestate) {
        case TESTING: {} break;
        case PREGAME: {} break;
        case TILE_GRID: {
            tg.tick();
            sc.tick();

            //Get info about skins
            int numLoaded = loadedSkinIDs.size();
            bool levelUp = false;
            if(tg.getLevelShown()>=(numSkinChanges+1)*4) {
                //Increment # of skin changes; get last & current skin ID from that
                numSkinChanges++;
                levelUp = true;
            }

            //Upon level up...
            if(levelUp) {
                int lastID = (numSkinChanges-1)%numLoaded; if(lastID<0) lastID += numLoaded;
                int currID = numSkinChanges%numLoaded;

                //If more than one skin loaded...
                if(numLoaded>1) {
                    //Change between last and current skin
                    tg.changeSkinTo(skins[loadedSkinIDs[currID]]);
                    sc.changeBetween(skins[loadedSkinIDs[lastID]], skins[loadedSkinIDs[currID]]);
                    sc.start();
                }
                
                
                //If the technical level is >=166 (Shown level: 41-1 on beginner)
                if(tg.getLevelTechnical()>=164) {
                    Skin::incLuminescenceModifier();
                }
            }

            //If the current skin transform is finished, deactivate the last skin
            if(sc.hasFinished()) {
                int lastID = (numSkinChanges-1)%numLoaded; if(lastID<0) lastID += numLoaded;
                skins[loadedSkinIDs[lastID]]->deactivate();
                sc.reset();
            }
        } break;
        ////////////////////////////////////////////////////////////////
        default: { printf("Tick for invalid gamestate \"%d\"!\n", gamestate); } break;
    }
}

void Game::draw()
{
    guiHandler->draw();

    switch(gamestate) {
        case TESTING: {} break;
        case PREGAME: {} break;
        case TILE_GRID: {
            //All black
            SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
            SDL_RenderFillRect(rend, NULL);

            tg.draw(rend);
        } break;
        default: { printf("Draw for invalid gamestate \"%d\"!\n", gamestate); } break;
    }
}

void Game::drawDebug(std::stringstream& ss)
{
    if(gamestate==TILE_GRID) {
        tg.drawDebug(ss);
    }
}

void Game::initTileGrid()
{
    printf("Starting game...\n");

    //Pre-Initialize tile grid.
    tg.preinit(gameSettings);

    //Load selected skins
    if(loadSelectedSkins()==true) {
        //Put first skin into TileGrid
        Skin* firstSkin = skins.at(loadedSkinIDs[0]);
        tg.init(rend, skins, loadedSkinIDs[0]);
        //Activate the first skin
        firstSkin->activate(tg.getIngameTicks64());
    }
}

void Game::switchState(int newGameState, int gameStateData)
{
    printf("Switching gamestate to %d...\n", newGameState);
    gamestate = newGameState;

    switch(gamestate) {
        case PREGAME: {
            guiHandler->switchUIs(GUIHandler::cid_pregame);
            guiHandler->setScreenID(0);
            GUI* temp = guiHandler->getGUI(GUIHandler::IDs::sksr_main);
            if(temp!=nullptr) {
                ss = (SkinSelector*)temp;
                ss->loadSkins(skins);    
            }
            
        } break;
        case TILE_GRID: {
            guiHandler->switchUIs(GUIHandler::cid_activegame);
            initTileGrid();
        } break;
        default: {
            printf("Init for invalid gamestate \"%d\"!\n", gamestate);
        } break;
    }
}

void Game::switchState(int newGameState) { switchState(newGameState, -1); }

SkinChanger Game::getSkinChanger() { return sc; }

void Game::processAction(int lastAction)
{
    int action = lastAction;
    int data = guiHandler->getLastActionData();
    
    
    switch(action) {
        case GUIHandler::btn_selectMode_x: {
            gameSettings.mode = TileGrid::GameMode::CHALLENGE+data;
            nch::Log::log("Selected game mode %d.", data);
        } break;
        case GUIHandler::btn_selectLayout_x: {
            nch::Log::log("Selected layout type %d.", data);

            //Set selected skins depending on layout type 'data'
            switch(data) {
                case 0: case 1: case 2: {
                    ss->setSelectedSkins(ss->getDefaultSelection(data));
                } break;
                case 3: { ss->setSelectedSkins(ss->getRandomSelection(20)); } break;
                case 4: { ss->setSelectedSkins(ss->getRandomSelection(40)); } break;
                case 5: { ss->setSelectedSkins(ss->getRandomSelection(123456)); } break;
                default: { ss->setSelectedSkins({}); } break;
            }
        } break;
        case GUIHandler::btn_selectDifficulty_x: {

            if(data!=-100) {
                gameSettings.diff = TileGrid::Difficulty::BEGINNER+data;
                nch::Log::log("Selected difficulty %d.", gameSettings.diff); 

                if(gamestate!=TILE_GRID) {
                    switchState(TILE_GRID, 0);
                }
            }
        } break;
    }
}

void Game::fetchSkins(std::string parentDir)
{
    //parentDir should be either: "data/skins" or "res/skins".

    //Get info about skins
    nch::FilePath dataFile(nch::FsUtils::getPathWithInferredExtension(parentDir));
    if(dataFile.get()!="?null?") {
        printf("Loading \"%s\"...\n", dataFile.get().c_str());
        std::ifstream f(dataFile.get());
        nlohmann::json data = nlohmann::json::parse(f);
        f.close();
        printf("Found a total of %d skins within \"%s\".\n", data["skins"].size(), dataFile.get().c_str());

        auto skinlist = data["skins"];
        for(int i = 0; i<skinlist.size(); i++) {
            auto thisSkin = skinlist[i];

            //Set ID and create Skin
            std::string id = "[null]";
            try { id = thisSkin["id"]; } catch(...) {}
            Skin* elem = new Skin(parentDir, id, rend);

            //Set BPM
            elem->setBPM(60);
            try { elem->setBPM(thisSkin["bpm"]); } catch(...) {}
            
            //Set stylized name
            elem->setStylizedName(id);
            try { elem->setStylizedName(thisSkin["stylizedName"]); } catch(...) {}

            //Set background alpha color
            std::string bgac = "backgroundAlphaColor";
            elem->setBGAlphaColor(nch::Color(127, 127, 127));
            try { elem->setBGAlphaColor(nch::Color(thisSkin[bgac][0], thisSkin[bgac][1], thisSkin[bgac][2])); } catch(...) {}

            //Set whether score panels are translucent black (true) or invisible (false)
            elem->setScorePanelsGeneric(true);
            try { elem->setScorePanelsGeneric(thisSkin["scorePanelsGeneric"]); } catch(...) {}

            //Set music volume modifier
            elem->setMusicVolumeFactor(1.0);
            try { elem->setMusicVolumeFactor(thisSkin["musicVolumeFactor"]); } catch(...) {}

            //Set stylized ingame name
            elem->setStylizedIngameName("?null?");
            try { elem->setStylizedIngameName(thisSkin["stylizedIngameName"]); } catch(...) {}
            
            //Set background color mod (primary)
            std::string bgpc = "backgroundPrimaryColor";
            elem->setBGColorMod(nch::Color(255, 255, 255));
            try { elem->setBGColorMod(nch::Color(thisSkin[bgpc][0], thisSkin[bgpc][1], thisSkin[bgpc][2])); } catch(...) {}
            
            skins.push_back(elem);
        }
    }
}

void Game::loadConfigOptions(std::string configFile)
{
    //Get config info
    nch::FilePath dataFile(nch::FsUtils::getPathWithInferredExtension("config"));
    
    if(dataFile.get()!="?null?") {
        printf("Loading configured gameplay options...\n");
        std::ifstream f(dataFile.get());
        nlohmann::json data = nlohmann::json::parse(f);

        for(int i = 0; i<data.size(); i++) {
            try { configDemoStart = data["demoStart"]; } catch(...) {}
        }
    }
}

bool Game::loadSelectedSkins()
{
    //Get selection from the SkinSelect menu
    std::vector<int> skindexes;
    if(!configDemoStart) {
        skindexes = ss->getSelectedSkins();
    } else {
        std::vector<std::pair<std::string, std::string>> preset = {
            std::make_pair("res/skins", "jades"),
            std::make_pair("res/skins", "whirlworld")
        };
        for(int i = 0; i<preset.size(); i++) {
            int index = ss->getSkinIndexByRes(preset[i].first, preset[i].second);
            if(index!=-1) skindexes.push_back(index);
        }

    }

    if(skindexes.size()==0) {
        ss->cancelFinishedSelection();
        if(gamestate!=PREGAME) {
            switchState(PREGAME, 123);
        }
        return false;
    }
    
    //Go through all the skins selected, and load them
    uint64_t t0 = nch::Timer::getTicks();
    for(int i = 0; i<skindexes.size(); i++) {
        Skin* skin = skins.at(skindexes[i]);
        if(skin==nullptr) {
            printf("Skin is null!\n");
        } else {
            skin->load(tg.getNumCols(), tg.getNumRows());
        }
    }
    loadedSkinIDs = skindexes;

    uint64_t t1 = nch::Timer::getTicks();
    printf("Loaded %d skins in %dms\n", skindexes.size(), t1-t0);

    return true;
}