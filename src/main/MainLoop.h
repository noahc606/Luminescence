#pragma once
#include <nch/sdl-utils/input.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "Game.h"

class MainLoop {
public:
	MainLoop(SDL_Renderer* rend);

private:
	bool running = true;
	uint64_t timer = 0;
	
	
	static void tick();
	static void draw(SDL_Renderer* rend);
	
	static bool loading;
	static Game game;

	static nch::Text debugTxt;
	static uint64_t secLast;
};