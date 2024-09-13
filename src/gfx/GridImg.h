#pragma once
#include <SDL2/SDL.h>

class GridImg {
public:
    GridImg();
    ~GridImg();

	static void buildGridTex(SDL_Renderer* rend, SDL_Texture*& gridTex, int gridCols, int gridRows);

private:

};