#pragma once
#include <map>
#include <nch/cpp-utils/fs/FilePath.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <string>
#include <vector>

class Resources {
public:
    Resources();
    ~Resources();

    static void init(SDL_Renderer* rend);
    
    template<typename T> static T* get(std::map<std::string, T*>& resourceMap, std::string asset) {
        auto fres = resourceMap.find(asset); if(fres!=resourceMap.end()) {
            return fres->second;
        }
        return nullptr;
    }

    static TTF_Font* getTTF(std::string asset, bool canFallback);
    static TTF_Font* getTTF(std::string asset);
    static SDL_Texture* getTex(std::string asset);
    static void playAudio(std::string asset);
    static void playMusic(std::string asset, double volumeFactor);
    static void playMusic(std::string asset);

    static void getTexMakeException();

    static const std::string primaryFont;
    static const std::string sidebarFont;
    static const std::string orbitronFont;
    static const std::string carlitoFont;
    static const std::string bteFont;
    
private:
    static void loadTTFRes(nch::FilePath fp, int ptsize);
    static void loadImgRes(nch::FilePath fp, SDL_Renderer* rend);
    static void loadSfxRes(nch::FilePath fp);
    static void loadMusRes(nch::FilePath fp);

    static void initTTFs();
    static void initImages(SDL_Renderer* rend);
    static void initSfxAndMusic(SDL_Renderer* rend);
    

	static std::map<std::string, TTF_Font*> ttfResources;
    static std::map<std::string, SDL_Texture*> imgResources;
	static std::map<std::string, Mix_Chunk*> sfxResources;
	static std::map<std::string, Mix_Music*> musResources;
	static SDL_Texture* tileSheet;

    static bool initialized;
    static bool doGetTexWarn;
    static bool configLowerQualityAudio;
    static bool configPixelatedFont;
};