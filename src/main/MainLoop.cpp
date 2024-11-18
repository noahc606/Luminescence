#include "MainLoop.h"
#include <iostream>
#include <nch/cpp-utils/log.h>
#include <nch/sdl-utils/timer.h>
#include <nch/sdl-utils/text.h>
#include <SDL2/SDL_timer.h>
#include <sstream>
#include <stdio.h>
#include "Main.h"
#include "Resources.h"
#include "Tests.h"

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
	
	uint64_t targetTPS = 80;
	uint64_t maxFPS = 120;
	uint64_t nsPerTick = 1000000000/targetTPS;	int tps = 0;
	uint64_t nsPerFrame = 1000000000/maxFPS;	int fps = 0;
	uint64_t tickNextNS = 0;
	uint64_t frameNextNS = 0;

	//Run while game is running
	while(running) {
		SDL_Delay(1);
		for(int i = 0; i<1000; i++) {
			//If the game is ready to tick
			if(nch::Timer::getCurrentTimeNS()>=tickNextNS) {
				//Update when the next tick should happen
				tickNextNS = nch::Timer::getCurrentTimeNS()+nsPerTick;

				//Perform the tick, calculating how much time it takes.
				uint64_t tickT0 = nch::Timer::getCurrentTimeNS();
				nch::Input::tick();
				tick(); tps++;
				uint64_t tickT1 = nch::Timer::getCurrentTimeNS();
				uint64_t tickDeltaNS = tickT1-tickT0;

				/* Store how long the last 80 ticks have taken within 'tickTimesNS' */
				//If we have 80 elements, erase the first (and oldest) one.
				if(tickTimesNS.size()==targetTPS) { tickTimesNS.erase(tickTimesNS.begin()+0); }
				//Add the latest time.
				tickTimesNS.push_back(tickDeltaNS);
			}

			//If the game is ready to draw (new frame)
			if(nch::Timer::getCurrentTimeNS()>=frameNextNS) {
				//Update when the next frame should be drawn
				frameNextNS = nch::Timer::getCurrentTimeNS()+nsPerFrame;

				//Perform the draw, calculating how much time it takes.
				uint64_t frameT0 = nch::Timer::getCurrentTimeNS();
				draw(rend); fps++;
				uint64_t frameT1 = nch::Timer::getCurrentTimeNS();
				uint64_t frameDeltaNS = frameT1-frameT0;

				/* Store how long the last 300 frames have taken within 'frameTimesNS */
				//If we have 300 elements, erase the first (and oldest) one.
				if(frameTimesNS.size()==maxFPS) { frameTimesNS.erase(frameTimesNS.begin()+0); }
				//Add the latest time.
				frameTimesNS.push_back(frameDeltaNS);
			}

			//Modify maxFPS if the average FPS if a frame is taking too long...
			if(getAvgNSPF()>nsPerFrame) {
				//printf("Slowing down...\n");
				if(maxFPS>15) maxFPS--;
			} else {
				//printf("Speeding up...\n");
				maxFPS++;
				if(maxFPS>120) maxFPS = 120;
			}

			//Modify maxFPS if the average TPS is below 80 (MSPT).
			if(getAvgNSPT()>nsPerTick) {
				//printf("Slowing down...\n");
				if(maxFPS>15) maxFPS--;
			} else {
				//printf("Speeding up...\n");
				maxFPS++;
				if(maxFPS>120) maxFPS = 120;
			}


			//Run this block every second.
			if( nch::Timer::getTicks64()>=secLast ) {
				secLast = nch::Timer::getTicks64()+1000;
				currentTPS = tps;
				currentFPS = fps;

				tps = 0;
				fps = 0;
			}	

			event();
		}
	}
}

uint64_t MainLoop::getAvgNSPF()
{
	uint64_t res = 0;
	for(int i = 0; i<frameTimesNS.size(); i++) {
		res += frameTimesNS[i];
	}
	return res/frameTimesNS.size();
}
uint64_t MainLoop::getAvgNSPT()
{
	uint64_t res = 0;
	for(int i = 0; i<tickTimesNS.size(); i++) {
		res += tickTimesNS[i];
	}
	return res/tickTimesNS.size();
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

		if(secLastDebugged!=secLast) {
			secLastDebugged = secLast;

			//Build debug text
			std::stringstream ss;
			ss << "FPS=" << currentFPS << " (max " << "); TPS=" << currentTPS << ".\n";
			ss << "mspf=" << (getAvgNSPF()/1000000.) << "; mspt=" << (getAvgNSPT()/1000000.) << ".\n";
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

void MainLoop::event() {
	SDL_Event e;
	while( SDL_PollEvent(&e)!=0 ) {
		switch(e.type) {
			case SDL_QUIT: {
				nch::Log::log("Quitting MainLoopDriver...");
				running = false;
				SDL_Quit();
				std::exit(0);
			} break;
			
			case SDL_KEYDOWN: case SDL_KEYUP:
			case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
			case SDL_JOYBUTTONDOWN: case SDL_JOYBUTTONUP:
			case SDL_JOYHATMOTION:
			{
				input.inputEvents(e);
			} break;
		}
	}
}