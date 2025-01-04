#include "Resources.h"
#include <fstream>
#include <nch/cpp-utils/fs-utils.h>
#include <nch/sdl-utils/timer.h>
#include <nch/sdl-utils/texture-utils.h>
#include <nlohmann/json.hpp>
#include "Main.h"

const std::string Resources::primaryFont = "data/misc/FOT-MangiaCondBold";
const std::string Resources::sidebarFont = "data/misc/FOT-BullheadedBold";

const std::string Resources::orbitronFont = "res/fonts/Orbitron";
const std::string Resources::oswaldFont = "res/fonts/Oswald";
const std::string Resources::bteFont = "res/fonts/BackToEarth";

std::map<std::string, TTF_Font*> Resources::ttfResources;
std::map<std::string, SDL_Texture*> Resources::imgResources;
std::map<std::string, Mix_Chunk*> Resources::sfxResources;
std::map<std::string, Mix_Music*> Resources::musResources;
SDL_Texture* Resources::tileSheet = nullptr;
bool Resources::initialized = false;
bool Resources::doGetTexWarn = true;
bool Resources::configLowerQualityAudio = false;
bool Resources::configPixelatedFont = false;

Resources::Resources(){}
Resources::~Resources(){}

void Resources::init(SDL_Renderer* rend)
{
	uint64_t t0 = nch::Timer::getTicks64();
	//Quit if already initialized
    if(initialized) {
        printf("Resources already initialized.\n");
        return;
    }

    //Get config info
    nch::FilePath dataFile(nch::FsUtils::getPathWithInferredExtension("config"));
    
    if(dataFile.get()!="?null?") {
        printf("Loading configured gameplay options...\n");
        std::ifstream f(dataFile.get());
        nlohmann::json data = nlohmann::json::parse(f);

        for(int i = 0; i<data.size(); i++) {
            try { configLowerQualityAudio = data["lowerQualityAudio"]; } catch(...) {}
            try { configPixelatedFont = data["pixelatedFont"]; } catch(...) {}
        }
    }

	initTTFs();
	initImages(rend);
	
	int frequency = 48000;
	int channels = 2;
	if(configLowerQualityAudio) {
		frequency /= 4;
		channels = 1;
	}
	if( Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, channels, 1024)==-1 ) {
		printf("Warning: SDL_mixer failed to open.");
	}

	if(configLowerQualityAudio) {
		Mix_AllocateChannels(8);
	} else {
		Mix_AllocateChannels(32);
	}
	
	initSfxAndMusic(rend);

	if(!true) {
		for(auto it = ttfResources.begin(); it != ttfResources.end(); ++it) printf("TrueTypeFont loaded: %s as %p\n", it->first.c_str(), it->second);
		for(auto it = imgResources.begin(); it != imgResources.end(); ++it) printf("Image loaded: %s as %p\n", it->first.c_str(), it->second);
		for(auto it = sfxResources.begin(); it != sfxResources.end(); ++it) printf("Sound loaded: %s as %p\n", it->first.c_str(), it->second);
		for(auto it = musResources.begin(); it != musResources.end(); ++it) printf("Music loaded: %s as %p\n", it->first.c_str(), it->second);
	}

	//Set icon
	SDL_Surface* tempico = IMG_Load("res/icon.png");
	SDL_SetWindowIcon(Main::getWindow(), tempico);
	SDL_FreeSurface(tempico);

	uint64_t t1 = nch::Timer::getTicks64();
	printf("%d resources loaded in %dms.\n", imgResources.size()+sfxResources.size()+musResources.size(), t1-t0);
}

TTF_Font* Resources::getTTF(std::string asset, bool canFallback)
{
	TTF_Font* ttf = get(ttfResources, asset);
	if(ttf==nullptr) {
		if(configPixelatedFont) {
			return getTTF(bteFont);
		}
		if(canFallback) {
			//Fallback for specific fonts
			if(asset==primaryFont) { return getTTF(oswaldFont); }
			if(asset==sidebarFont) { return getTTF(oswaldFont); }

			//Fallback for everything else: FreeMono
			if(asset!="res/fonts/FreeMono") {
				return getTTF("res/fonts/FreeMono");
			} else {
				printf("Could not load default TrueTypeFont \"%s\" in ttfResources\n", asset.c_str());
				return nullptr;
			}
		} else {
			printf("Could not find TrueTypeFont \"%s\" in ttfResources\n", asset.c_str());
			return nullptr;
		}

	}
	return ttf;
}
TTF_Font* Resources::getTTF(std::string asset) { return getTTF(asset, true); }

SDL_Texture* Resources::getTex(std::string asset)
{
	SDL_Texture* tex = get(imgResources, asset);
	if(tex==nullptr) {
		if(doGetTexWarn) {
			printf("Could not find SDL_Texture* \"%s\" in imgResources\n", asset.c_str());
		}
	}
	doGetTexWarn = true;
	return tex;
}

void Resources::playAudio(std::string asset)
{
	Mix_Chunk* mc = get(sfxResources, asset);
	if(mc==nullptr) {
		printf("Could not find Mix_Chunk* \"%s\" in sfxResources\n", asset.c_str());
	}
	Mix_VolumeChunk(mc, 0.25*MIX_MAX_VOLUME);
	Mix_PlayChannelTimed(-1, mc, 0, -1);
}

void Resources::playMusic(std::string asset, double volumeFactor)
{
	Mix_Music* mm = get(musResources, asset);
	if(mm==nullptr) {
		printf("Could not find Mix_Music* \"%s\" in musResources\n", asset.c_str());
	}
	Mix_VolumeMusic(0.15*MIX_MAX_VOLUME*volumeFactor);
	Mix_PlayMusic(mm, 0);
}

void Resources::playMusic(std::string asset) { playMusic(asset, 1.0); }

void Resources::getTexMakeException() { doGetTexWarn = false; }

void Resources::loadTTFRes(nch::FilePath fp, int ptsize)
{
	TTF_Font* ttf = TTF_OpenFont(fp.get().c_str(), ptsize);
	if(ttf!=nullptr) {
		ttfResources.insert(std::make_pair(fp.getWithoutExtension(), ttf));
	} else {
		printf("Failed to load TrueTypeFont \"%s\" from \"%s\": %s\n", fp.getWithoutExtension().c_str(), fp.get().c_str(), TTF_GetError());
	}
}
void Resources::loadImgRes(nch::FilePath fp, SDL_Renderer* rend)
{
	SDL_Texture* temp = IMG_LoadTexture(rend, fp.get().c_str());
	if(temp!=NULL) {
		int w, h;
		SDL_QueryTexture(temp, NULL, NULL, &w, &h);
		SDL_Texture* tex = SDL_CreateTexture(rend, Main::getWindowPixelFormat()->format, SDL_TEXTUREACCESS_TARGET, w, h);
		
		

		SDL_Texture* oldTarget = SDL_GetRenderTarget(rend);
		SDL_SetRenderTarget(rend, tex);
		SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
		SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
		SDL_RenderFillRect(rend, NULL);
		SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
		SDL_RenderCopy(rend, temp, NULL, NULL);
		SDL_SetRenderTarget(rend, oldTarget);

		imgResources.insert(std::make_pair(fp.getWithoutExtension(), tex));
	} else {
		printf("Failed to load image \"%s\" from \"%s\": %s\n", fp.getWithoutExtension().c_str(), fp.get().c_str(), IMG_GetError());
	}
	
	SDL_DestroyTexture(temp);
}
void Resources::loadSfxRes(nch::FilePath fp)
{
	Mix_Chunk* res = Mix_LoadWAV(fp.get().c_str());
	if(res==0) {
		printf("Failed to load audio chunk \"%s\" from \"%s\": %s\n", fp.getWithoutExtension().c_str(), fp.get().c_str(), Mix_GetError());
	} else {
		sfxResources.insert( std::make_pair(fp.getWithoutExtension(), res) );
	}
}
void Resources::loadMusRes(nch::FilePath fp)
{
	Mix_Music* res = Mix_LoadMUS(fp.get().c_str());
	if(res==0) {
		printf("Failed to load music chunk \"%s\" from \"%s\": %s\n", fp.getWithoutExtension().c_str(), fp.get().c_str(), Mix_GetError());
	} else {
		musResources.insert( std::make_pair(fp.getWithoutExtension(), res) );
	}
}
void Resources::initTTFs()
{
	std::vector<std::string> resDirs = {"data", "res"};

	nch::FsUtils::ListSettings lise;
	nch::FsUtils::RecursionSettings rese; rese.recursiveSearch = true;
	std::vector<std::string> resList = nch::FsUtils::getManyDirContents(resDirs, lise, rese);	
	
	std::vector<std::string> ttfResList;
	for(std::string s : resList) {
		nch::FilePath fp(s);
		std::string ext = fp.getExtension();
		
		if(ext=="ttf" || ext=="otf") {
			ttfResList.push_back(fp.get());
		}
	}

	//Load TTFs into raw 'resources' list
	for(int i = 0; i<ttfResList.size(); i++) {
		nch::FilePath fp(ttfResList[i]);
		loadTTFRes(fp, 200);
	}
}
void Resources::initImages(SDL_Renderer* rend)
{
	std::vector<std::string> resDirs = {"data", "res"};

	nch::FsUtils::ListSettings lise;
	nch::FsUtils::RecursionSettings rese; rese.recursiveSearch = true;
	std::vector<std::string> resList = nch::FsUtils::getManyDirContents(resDirs, lise, rese);
	
	std::vector<std::string> imgResList;
	for(std::string s : resList) {
		nch::FilePath fp(s);
		std::string ext = fp.getExtension();
		
		if(ext=="png") {
			imgResList.push_back(fp.get());
		}
	}

	//Load images into raw 'resources' list
	for(int i = 0; i<imgResList.size(); i++) {
		nch::FilePath fp(imgResList[i]);
		loadImgRes(fp, rend);
	}
}
void Resources::initSfxAndMusic(SDL_Renderer* rend)
{
	std::vector<std::string> assetDirs = {"data", "res"};

	nch::FsUtils::ListSettings lise;
	nch::FsUtils::RecursionSettings rese; rese.recursiveSearch = true;
	std::vector<std::string> assetList = nch::FsUtils::getManyDirContents(assetDirs, lise, rese);
	
	std::vector<std::string> musAssetList;
	for(std::string s : assetList) {
		nch::FilePath fp(s);
		
		//Add music tracks (any file fitting the pattern "['data' or 'res']/skins/[skin name]/mus/bgm")
		if(	fp.getNumDirsDown()==5 &&
			(fp.getGrandparentDir(4)=="data" || fp.getGrandparentDir(4)=="res") &&
			fp.getGrandparentDir(3)=="skins" &&
			fp.getParentDir()=="mus" )
		{
			musAssetList.push_back(fp.get());
		}
	}
	std::vector<std::string> sfxAssetList;
	for(std::string s : assetList) {
		nch::FilePath fp(s);
		std::string ext = fp.getExtension();

		//Add sfx files (any file fitting the pattern "['data' or 'res']/skins/[skin name]/sfx/*")
		if(	fp.getNumDirsDown()==5 &&
			(fp.getGrandparentDir(4)=="data" || fp.getGrandparentDir(4)=="res") &&
			fp.getGrandparentDir(3)=="skins" &&
			fp.getParentDir()=="sfx")
		{
			sfxAssetList.push_back(fp.get());
		}

		//Add sfx within res/audio
		if(fp.getNumDirsDown()==3) {
			if(fp.getGrandparentDir(2)=="res" && fp.getParentDir()=="audio") {
				sfxAssetList.push_back(fp.get());
			}
		}
	}
	
	//Load sfx
	for(int i = 0; i<sfxAssetList.size(); i++) {
		nch::FilePath fp(sfxAssetList[i]);
		loadSfxRes(fp);
	}
	//Load music
	for(int i = 0; i<musAssetList.size(); i++) {
		nch::FilePath fp(musAssetList[i]);
		loadMusRes(fp);
	}
}

