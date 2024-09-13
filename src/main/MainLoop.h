#pragma once
#include <nch/sdl-utils/Input.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "Game.h"

class MainLoop {
public:
	MainLoop(SDL_Renderer* rend);

private:
	bool running = true;
	uint64_t timer = 0;
	uint64_t secLast = 0;
	int currentTPS = 0;
	int currentFPS = 0;
	
	uint64_t getAvgNSPF();
	uint64_t getAvgNSPT();
	
	void tick();
	void draw(SDL_Renderer* rend);
	void event();
	
	bool loading = true;
	Game game;
	nch::Input input;

	nch::Text debugTxt;
	uint64_t secLastDebugged = 0;

	//uint64_t maxFPS = 300;
	//uint64_t fpsTarget = 0;
	uint64_t lastGameLoopMS = 0;
	std::vector<uint64_t> tickTimesNS;
	std::vector<uint64_t> frameTimesNS;
};