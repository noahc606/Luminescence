#include "Game.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <nch/cpp-utils/fs/FilePath.h>
#include <nch/cpp-utils/fs/FsUtils.h>
#include <nch/sdl-utils/Timer.h>
#include "GridImg.h"
#include "Main.h"

SkinChanger Game::sc;

Game::Game(){}
Game::~Game(){}

void Game::init(SDL_Renderer* rend)
{
    uint64_t t0 = nch::Timer::getTicks64();
    
    Game::rend = rend;
    fetchSkins("data/skins");
    fetchSkins("res/skins");
    loadConfigOptions("data/config");
    ss.init(rend, skins);
    switchState(TILE_GRID, selectedDifficulty);
    
    uint64_t t1 = nch::Timer::getTicks64();
    printf("Game initialized in %dms.\n", t1-t0);
}

void Game::tick()
{
    switch(gamestate) {
        case TESTING: {
            //for(int i = 0; i<9000000; i++) {
                //int x = 12345;
            //}
        } break;
        case SKIN_SELECT: {
            ss.tick();
            if(ss.hasGameStarted()) {
                switchState(TILE_GRID, selectedDifficulty);
            }
        } break;
        case TILE_GRID: {
            tg.tick();
            sc.tick();

            //Get info about skins
            int numLoaded = loadedSkinIDs.size();

            //Change skin if more than one skin loaded
            if(numLoaded>1 && tg.getLevelShown()>=(numSkinChanges+1)*4) {
                //Increment # of skin changes; get last & current skin ID from that
                numSkinChanges++;
                int lastID = (numSkinChanges-1)%numLoaded; if(lastID<0) lastID += numLoaded;
                int currID = numSkinChanges%numLoaded;
                
                //Change between last and current skin
                tg.changeSkinTo(skins[loadedSkinIDs[currID]]);
                sc.changeBetween(skins[loadedSkinIDs[lastID]], skins[loadedSkinIDs[currID]]);
                sc.start();

                //Activate current skin
                activeSkinID = loadedSkinIDs[currID];
            }

            //If the current skin transform is finished
            if(sc.hasFinished()) {
                int lastID = (numSkinChanges-1)%numLoaded; if(lastID<0) lastID += numLoaded;
                skins[loadedSkinIDs[lastID]]->deactivate();
                sc.reset();
            }
        } break;
        default: { printf("Tick for invalid gamestate \"%d\"!\n", gamestate); } break;
    }
}

void Game::draw()
{
    switch(gamestate) {
        case TESTING: {
            
        } break;
        case MODE_SELECT: {
            
        } break;
        case SKIN_SELECT:   { ss.draw(); } break;
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

void Game::initTileGrid(int difficulty)
{
    printf("Starting game...\n");

    //Load selected skins
    if(loadSelectedSkins()==true) {
        //Activate the first skin and initialize tile grid.
        Skin* firstSkin = skins.at(loadedSkinIDs[0]);
        tg.init(rend, skins, loadedSkinIDs[0], difficulty);
        firstSkin->activate(tg.getIngameTicks64());
    }
}

void Game::switchState(int newGameState, int gameStateData)
{
    printf("Switching gamestate to %d...\n", newGameState);
    gamestate = newGameState;
    switch(gamestate) {
        case SKIN_SELECT: {
            
        } break;
        case TILE_GRID: {
            if(gameStateData==-1) {
                initTileGrid(Difficulty::BEGINNER);    
            } else {
                initTileGrid(Difficulty::BEGINNER+gameStateData);
            }
            
        } break;
        default: {
            printf("Init for invalid gamestate \"%d\"!\n", gamestate);
        } break;
    }
}

void Game::switchState(int newGameState) { switchState(newGameState, -1); }

SkinChanger Game::getSkinChanger() { return sc; }

void Game::fetchSkins(std::string parentDir)
{
    //parentDir should be either: "data/skins" or "res/skins".

    //Get info about skins
    nch::FilePath dataFile(nch::FsUtils::getPathWithInferredExtension(parentDir));
    if(dataFile.get()!="?null?") {
        printf("Loading \"%s\"...\n", dataFile.get().c_str());
        std::ifstream f(dataFile.get());
        nlohmann::json data = nlohmann::json::parse(f);
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
        skindexes = ss.getSelectedSkins();
    } else {
        using namespace std;
        std::vector<std::pair<std::string, std::string>> preset = {
            make_pair("res/skins", "neodymium"),
            make_pair("res/skins", "manager_class"),
            make_pair("res/skins", "mental_gymnastic_60sec"),
            make_pair("res/skins", "square_dance")
        };
        for(int i = 0; i<preset.size(); i++) {
            int index = ss.getSkinIndexByRes(preset[i].first, preset[i].second);
            if(index!=-1) skindexes.push_back(index);
        }

    }

    if(skindexes.size()==0) {
        ss.cancelGameStart();
        switchState(SKIN_SELECT);
        return false;
    }
    
    //Go through all the skins selected, and load them
    uint64_t t0 = nch::Timer::getTicks64();
    for(int i = 0; i<skindexes.size(); i++) {
        Skin* skin = skins.at(skindexes[i]);
        if(skin==nullptr) {
            printf("Skin is null!\n");
        } else {
            skin->load();
        }
    }
    loadedSkinIDs = skindexes;

    uint64_t t1 = nch::Timer::getTicks64();
    printf("Loaded %d skins in %dms\n", skindexes.size(), t1-t0);

    return true;
}