#pragma once
#include <SDL2/SDL.h>
#include "Skin.h"
#include "Tile.h"


class TileImg {
public:
    TileImg();
    ~TileImg();

    static void buildSquareTex(SDL_Renderer* rend, SDL_Texture*& tex, Skin* skin, int type);
    static void buildTileTex(SDL_Renderer* rend, SDL_Texture*& tex, std::string skinPD, std::string skinID, int type);
    static void buildTileTex(SDL_Renderer* rend, SDL_Texture*& tex, Skin* skin, int type);

    static void drawChainTex(SDL_Renderer* rend, SDL_Rect tile, double gridScale);
    static void drawSquare(SDL_Renderer* rend, double scale, double dstX, double dstY, nch::Color c);
    static void drawSquare(SDL_Renderer* rend, Skin* skin, double gridX, double gridY, int type);
	static void drawTile(SDL_Renderer* rend, Skin* skin, double x, double y, Tile t);
    static nch::Color getTileCompositeColor(SDL_Renderer* rend, std::string tileAsset);
private:
    static void drawGradientSquare(SDL_Renderer* rend, SDL_Rect* r, nch::Color c);

};