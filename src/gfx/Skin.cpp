#include "Skin.h"
#include <iostream>
#include <nch/cpp-utils/fs/FsUtils.h>
#include <nch/sdl-utils/gfx/Text.h>
#include <nch/sdl-utils/gfx/TexUtils.h>
#include <nch/sdl-utils/Timer.h>
#include "Game.h"
#include "GridImg.h"
#include "Main.h"
#include "Resources.h"
#include "SkinChanger.h"
#include "TileImg.h"

int Skin::colorSeed = 0;
bool Skin::musicStopped = false;

Skin::Skin(std::string parentDir, std::string id, SDL_Renderer* rend)
{
    Skin::parentDir = parentDir;
    Skin::id = id;
    Skin::rend = rend;
 
    skinNameTxt.init(rend, Resources::getTTF(Resources::primaryFont));
    skinBpmTxt.init(rend, Resources::getTTF(Resources::primaryFont));
}
Skin::~Skin()
{
    SDL_DestroyTexture(gridTex);
    SDL_DestroyTexture(tile1Tex);
    SDL_DestroyTexture(tile2Tex);
    SDL_DestroyTexture(square1Tex);
    SDL_DestroyTexture(square2Tex);
}

void Skin::load()
{
    //Get a single avg color from each of the textures of tile_a and tile_b
    tile1Color = TileImg::getTileCompositeColor(rend, parentDir+"/"+id+"/tile_a");
    tile2Color = TileImg::getTileCompositeColor(rend, parentDir+"/"+id+"/tile_b");

    //Build some textures
    TileImg::buildTileTex(rend, tile1Tex, this, 1);
    TileImg::buildTileTex(rend, tile2Tex, this, 2);
    TileImg::buildSquareTex(rend, square1Tex, this, 1);
    TileImg::buildSquareTex(rend, square2Tex, this, 2);
	GridImg::buildGridTex(rend, gridTex, getNumCols(), getNumRows());

    //Decode and play video
    std::string videoPath = nch::FsUtils::getPathWithInferredExtension(parentDir+"/"+id+"/bg");
    nch::FilePath vfp(videoPath);
    if(vfp.getExtension()!="png") {
        if(videoPath!="?null?") {
            mp = new nch::MediaPlayer(videoPath, rend);
            mp->decodeFull();
            noVideo = false;
        }
    } else {
        bgStaticImg = true;
    }
}

void Skin::activate(uint64_t ingameTimeMS)
{
    //Play video
    if(!noVideo) {
        mp->startPlayback(true);
    }
    //Play background music
	playMusic(this);
    musicStartTimeMS = ingameTimeMS;
    active = true;
}

void Skin::deactivate()
{
    active = false;
}

void Skin::draw()
{
    //Set bgColorMod's alpha component depending on skin changer state
    SkinChanger sc = Game::getSkinChanger();
    if(sc.isChanging()) {
        int dAlpha = sc.getProgress()*255;
        if(dAlpha<0) dAlpha = 0;
        if(dAlpha>255) dAlpha = 255;

        if(id==sc.getFormerSkinID()) {
            bgColorMod.a = 255-dAlpha;
        }
        if(id==sc.getLatterSkinID()) {
            bgColorMod.a = dAlpha;
        }
    } else {
        bgColorMod.a = 255;
    }

    //Create "intermediate" textures for tile1Tex and tile2Tex between former and latter skin.
    if(sc.isChanging()) {
	    SDL_Rect tileR;
	    tileR.x = 4;	tileR.w = 120;
	    tileR.y = 4;	tileR.h = 120;

        //Get needed textures
        SDL_Texture* formerTex1 = Resources::getTex(sc.getFormerSkinPD()+"/"+sc.getFormerSkinID()+"/tile_a");
        SDL_Texture* latterTex1 = Resources::getTex(sc.getLatterSkinPD()+"/"+sc.getLatterSkinID()+"/tile_a");
        SDL_Texture* formerTex2 = Resources::getTex(sc.getFormerSkinPD()+"/"+sc.getFormerSkinID()+"/tile_b");
        SDL_Texture* latterTex2 = Resources::getTex(sc.getLatterSkinPD()+"/"+sc.getLatterSkinID()+"/tile_b");

        
        //tile1Tex
        nch::TexUtils::clearTexture(rend, tile1Tex);
        SDL_SetRenderTarget(rend, tile1Tex);
        SDL_SetTextureBlendMode(formerTex1, SDL_BLENDMODE_BLEND);   SDL_SetTextureAlphaMod(formerTex1, 255-bgColorMod.a);   SDL_RenderCopy(rend, formerTex1, NULL, &tileR);
        SDL_SetTextureBlendMode(latterTex1, SDL_BLENDMODE_BLEND);   SDL_SetTextureAlphaMod(latterTex1, bgColorMod.a);       SDL_RenderCopy(rend, latterTex1, NULL, &tileR);
        
        //tile2Tex
        nch::TexUtils::clearTexture(rend, tile2Tex);
        SDL_SetRenderTarget(rend, tile2Tex);
        SDL_SetTextureBlendMode(formerTex2, SDL_BLENDMODE_BLEND);   SDL_SetTextureAlphaMod(formerTex2, 255-bgColorMod.a);   SDL_RenderCopy(rend, formerTex2, NULL, &tileR);
        SDL_SetTextureBlendMode(latterTex2, SDL_BLENDMODE_BLEND);   SDL_SetTextureAlphaMod(latterTex2, bgColorMod.a);       SDL_RenderCopy(rend, latterTex2, NULL, &tileR);

        SDL_SetRenderTarget(rend, NULL);
    }

    //Draw background
    if(active) {
        SDL_Rect bgDst = Main::getBGRect();
        
        SDL_SetRenderDrawColor(rend, bgAlphaColor.r, bgAlphaColor.g, bgAlphaColor.b, bgColorMod.a);
        SDL_RenderFillRect(rend, &bgDst);

        
        if(!bgStaticImg) {
            if(!noVideo) {
                mp->renderCurrentVidFrame(NULL, &bgDst, bgColorMod);
            }
        } else {
            SDL_Texture* bgTex = Resources::getTex(parentDir+"/"+id+"/bg");
            SDL_SetTextureColorMod(bgTex, bgColorMod.r, bgColorMod.g, bgColorMod.b);
            SDL_SetTextureAlphaMod(bgTex, bgColorMod.a);
            SDL_RenderCopy(rend, bgTex, NULL, &bgDst);
        }
    }
    
    updateScaling();
    updateGridRect();

	//Draw gridTex (background)
	SDL_RenderCopy(rend, gridTex, NULL, &gridRect);
    drawSkinInfoUI();
}

nch::Color Skin::getColorFromTileType(int tileType)
{
    switch(tileType) {
        case 0: { return nch::Color(0, 0, 0); }
        case 1: { return tile1Color; }
        case 2: { return tile2Color; }
    }
    return nch::Color(0, 0, 0);
}
SDL_Texture* Skin::getTileTexByType(int type)
{
    switch(type) {
        case 0: { return nullptr; }
        case 1: { return tile1Tex; }
        case 2: { return tile2Tex; }
    }
    return nullptr;
}
SDL_Texture* Skin::getSquareTexByType(int type)
{
    switch(type) {
        case 0: { return nullptr; }
        case 1: { return square1Tex; }
        case 2: { return square2Tex; }
    }
    return nullptr;
}

int Skin::getColorSeed() { return colorSeed; }

std::string Skin::getParentDir() { return parentDir; }
std::string Skin::getID() { return id; }
std::string Skin::getStylizedName() { return stylizedName; }
double Skin::getBPM() { return bpm; }
double Skin::getMusicVolumeFactor() { return musicVolumeFactor; }
bool Skin::areScorePanelsGeneric() { return scorePanelsGeneric; }
int Skin::getNumCols() { return numCols; }
int Skin::getNumRows() { return numRows; }
int64_t Skin::getMaxFallTimeMS() { return maxFallTimeMS; }
uint64_t Skin::getMusicStartTimeMS() { return musicStartTimeMS; }
SDL_Rect* Skin::getGridRect() { return &gridRect; }
double Skin::getGridScale() { return gridScale; }
bool Skin::isActive() { return active; }
bool Skin::hasMusicStopped() { return musicStopped; }

void Skin::playMusic(Skin* s)
{
    Mix_HookMusicFinished(onMusicFinished);
    Resources::playMusic(s->getParentDir()+"/"+s->getID()+"/mus/bgm", s->getMusicVolumeFactor());
    musicStopped = false;
}
void Skin::setStylizedName(std::string sn) { stylizedName = sn; }
void Skin::setStylizedIngameName(std::string sign) { stylizedIngameName = sign; }
void Skin::setBPM(double bpm) { Skin::bpm = bpm; }
void Skin::setMusicVolumeFactor(double mvf) { Skin::musicVolumeFactor = mvf; }
void Skin::setBGAlphaColor(nch::Color bgAlphaColor) { Skin::bgAlphaColor = bgAlphaColor; }
void Skin::setScorePanelsGeneric(bool scorePanelsGeneric) { Skin::scorePanelsGeneric = scorePanelsGeneric; }
void Skin::setBGColorMod(nch::Color bgColorMod) { Skin::bgColorMod = bgColorMod; }

void Skin::onMusicFinished()
{
    musicStopped = true;
}

void Skin::drawSkinInfoUI()
{
    double s = Main::getTextScale()*0.1;
    if(s<0.0625) s = 0.0625;
    double s2 = s/2;
    if(s2<0.0625) s2 = 0.0625;

    //Skin name
    std::string displayName = stylizedName;
    if(stylizedIngameName!="?null?") {
        displayName = stylizedIngameName;
    }

    SDL_Rect bgr = Main::getBGRect();

    skinNameTxt.setText(displayName);
    skinNameTxt.setScale(s);
    int t0x = bgr.x+bgr.w-skinNameTxt.getWidth()-80*s;
    int t0y = bgr.y+bgr.h-skinNameTxt.getHeight()*1.6-80*s;
    
    //Skin BPM
    std::stringstream ssBPM; ssBPM << bpm << " BPM";
    skinBpmTxt.setText(ssBPM.str()); 
    skinBpmTxt.setScale(s2);
    int t2x = bgr.x+bgr.w-skinBpmTxt.getWidth()-80*s;
    int t2y = bgr.y+bgr.h-skinBpmTxt.getHeight()-80*s;
    
    skinNameTxt.draw(t0x, t0y);
    skinBpmTxt.draw(t2x, t2y);
}

void Skin::updateScaling()
{
    //Grid scale
	gridScale = 10;
	int playWidth = Main::getWidth()*2/3;
	int playHeight = Main::getHeight()*2/3;
	
	while(getNumCols()*32*gridScale>playWidth) { gridScale -= 0.125; }
	while(getNumRows()*32*gridScale>playHeight) { gridScale -= 0.125; }
	if(gridScale<0.25) gridScale = 0.25;
}

void Skin::updateGridRect()
{
	gridRect.w = gridScale*32*getNumCols();
	gridRect.h = gridScale*32*getNumRows();
	gridRect.x = Main::getWidth()/2-gridRect.w/2;
	gridRect.y = Main::getHeight()/2-gridRect.h/2;
}