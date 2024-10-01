#pragma once
#include <nch/cpp-utils/color.h>
#include <nch/ffmpeg-utils/mediaplayer.h>
#include <nch/sdl-utils/text.h>
#include <SDL2/SDL.h>
#include <string>


class Skin {
public:
    Skin(std::string parentDir, std::string id, SDL_Renderer* rend);
    ~Skin();

    void load(int numCols, int numRows);
    void activate(uint64_t ingameTimeMS);
    void deactivate();
    void draw(int numCols, int numRows);

    nch::Color getColorFromTileType(int tileType);
    SDL_Texture* getTileTexByType(int type);
    SDL_Texture* getSquareTexByType(int type);
    std::string getParentDir();
    std::string getID();
    std::string getStylizedName();
    std::string getStylizedIngameName();
    double getBPM();
    double getMusicVolumeFactor();

    bool areScorePanelsGeneric();
    int64_t getMaxFallTimeMS();
    uint64_t getMusicStartTimeMS();
    SDL_Rect* getGridRect();
    double getGridScale();
    bool isActive();
    static bool hasMusicStopped();
    
    static void playMusic(Skin* s);
    void setStylizedName(std::string sn);
    void setStylizedIngameName(std::string sn);
    void setBPM(double bpm);
    void setMusicVolumeFactor(double mvf);
    void setBGColorMod(nch::Color bgColorMod);
    void setBGAlphaColor(nch::Color bgAlphaColor);
    void setScorePanelsGeneric(bool scorePanelsGeneric);
    static void incLuminescenceModifier();


private:
    static void onMusicFinished();
    void drawSkinInfoUI();
    void updateGridRect(int numCols, int numRows);
    void updateScaling(int numCols, int numRows);

    //Skin properties
    std::string parentDir = "?null?";
    std::string id = "unknown";
    std::string stylizedName = "null";
    std::string stylizedIngameName = "?null?";
	double bpm = 100.0;
    double musicVolumeFactor = 1.0;
    nch::Color bgAlphaColor = nch::Color(1, 2, 3);
    bool scorePanelsGeneric = true;

    //Game properties
	int64_t maxFallTimeMS = 1000000;
    uint64_t musicStartTimeMS = 0;
    static bool musicStopped;

    //Graphical - foreground
    nch::Color tile1Color; nch::Color tile2Color;
    SDL_Texture* tile1Tex; SDL_Texture* tile2Tex;
    SDL_Texture* square1Tex; SDL_Texture* square2Tex;
    nch::Color bgColorMod;
    SDL_Rect gridRect;
	SDL_Texture* gridTex = nullptr;
	double gridScale = 5;
 
    //Graphical - background
    SDL_Renderer* rend;
    nch::MediaPlayer* mp;
    SDL_Texture* videoTexture = nullptr;
    bool active = false;
    bool noVideo = true;
    bool bgStaticImg = false;
    nch::Text skinNameTxt;
    nch::Text skinBpmTxt;
    static int luminescenceModifier;
};