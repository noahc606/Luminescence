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
	nch::FsUtils fsu;
	std::vector<std::string> resDirs = {"data", "res"};
    for(std::string s : fsu.getDirContents(resDirs, fsu.ALL, true)) {
        nch::FilePath fp(s);
        printf("%s\n", fp.get().c_str());
    }

    std::vector<int> vec = {0, 1, 2, 3, 4, 5};
    uint64_t t0 = nch::Timer::getTicks64();
    for(int i = 0; i<100000; i++) {
        Resources::getTex("res/urbanization/tile_a");
    }
    uint64_t t1 = nch::Timer::getTicks64();
    printf("Time elapsed: %dms.\n", t1-t0);
}
Tests::~Tests()
{
    
}