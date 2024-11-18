#include "Tests.h"
#include <iostream>
#include <nch/cpp-utils/filepath.h>
#include <nch/cpp-utils/fs-utils.h>
#include <nch/sdl-utils/timer.h>
#include <SDL2/SDL.h>
#include "Color.h"
#include "Resources.h"


Tests::Tests(SDL_Renderer* rend)
{

	double lvl = 12;

	//Logarithmic difficulty curve
	int64_t fallTimeMS = std::ceil( 1000.-1256.*std::log10((lvl+30)/30) );
	//int64_t fallTimeMS = std::ceil(-1256.*std::log10((lvl+30)));
	
	//Some skins have a maximum fall time MS
	if(fallTimeMS>1000) {
		fallTimeMS = 1000;
	}

	printf("falltime: %dms\n", fallTimeMS);


    return;
	
}
Tests::~Tests()
{
    
}