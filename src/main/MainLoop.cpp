#include "MainLoop.h"
#include <iostream>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/timer.h>
#include <nch/sdl-utils/main-loop-driver.h>
#include <nch/sdl-utils/text.h>
#include <SDL2/SDL_timer.h>
#include <sstream>
#include <stdio.h>
#include "Main.h"
#include "Resources.h"
#include "Tests.h"

bool MainLoop::loading = true;
Game MainLoop::game;

nch::Text MainLoop::debugTxt;
uint64_t MainLoop::secLast = 0;

MainLoop::MainLoop(SDL_Renderer* rend)
{
	/* Load screen */
	draw(rend);

	/* Game Resources */
	Resources::init(rend);
	debugTxt.init(rend, Resources::getTTF("res/FreeMono"), false);

	/* Testing? */
	if(!true) {
		Tests t(rend);
		return;
	}

	/* Game Objects */
	//Initialize Game
	game.init(rend);
	loading = false;
	//Main loop
	nch::MainLoopDriver mainLoop(rend, tick, 80, draw, 1000);
}

void MainLoop::tick()
{
	game.tick();
}

void MainLoop::draw(SDL_Renderer* rend)
{
	//Set render target to default and blendmode to blend
	SDL_SetRenderTarget(rend, NULL);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	
	//Gray background
	SDL_SetRenderDrawColor(rend, 127, 127, 127, 255);
	SDL_RenderFillRect(rend, NULL);

	if(loading) {
		SDL_Rect dst;
		dst.x = 10; dst.y = 10; dst.w = 500; dst.h = 500;
		SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
		SDL_RenderFillRect(rend, &dst);
	} else {
		//Draw and render game objects
		game.draw();

		if(nch::Timer::getTicks()/1000!=secLast) {
			secLast = nch::Timer::getTicks()/1000;

			//Build debug text
			std::stringstream ss;
			ss << "FPS=" << nch::MainLoopDriver::getCurrentFPS() << ", TPS=" << nch::MainLoopDriver::getCurrentTPS() << ".\n";
			game.drawDebug(ss);

			debugTxt.setText(ss.str());
			debugTxt.setScale(0.15);
		}

		if(true)
			debugTxt.draw(4, 4);
	
	
	}

	//Render present objects on screen
	SDL_RenderPresent(rend);
}
