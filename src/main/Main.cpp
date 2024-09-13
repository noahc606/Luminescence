#include "Main.h"
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>
#include "MainLoop.h"

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static SDL_PixelFormat* windowPixelFormat = nullptr;

int main()
{
	/* 1 - SDL initialization */
	//Create flags
	Uint32 flags = SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_JOYSTICK|SDL_INIT_GAMECONTROLLER;
	//Init SDL w/ flags, checking if successful
	if( SDL_Init(flags)!=0 ) {
		printf("Failed to SDL_Init()! %s\n", SDL_GetError());
	}
	if (TTF_Init()!=0) {
		printf("Failed to TTF_Init()! %s\n", TTF_GetError());
	}
		
	/* 2 - Window */
	//Create window, checking if successful
	window = SDL_CreateWindow("Luminescence", SDL_WINDOWPOS_UNDEFINED_DISPLAY(0), SDL_WINDOWPOS_UNDEFINED_DISPLAY(0), 640, 480, 0);
	if(window==NULL) {
		printf("Window is null! %s\n", SDL_GetError());
	}
	//Set window to be visible and resizable
	SDL_ShowWindow(window);
	SDL_SetWindowResizable(window, SDL_TRUE);
	//Store pixel format of the window
	windowPixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

	/* 3 - Renderer */
	//Create flags
	Uint32 rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
	//Create renderer with flags, checking if successful
	//SDL_SetHintWithPriority(SDL_HINT_RENDER_VSYNC, "0", SDL_HINT_OVERRIDE);
	renderer = SDL_CreateRenderer(window, -1, rendererFlags);
	if(renderer==NULL) {
		printf("Renderer is null! %s\n", SDL_GetError());
	}
	
	/* 4 - Game Loop */
	//Create MainLoop (infinite game loop within MainLoop's constructor)
	MainLoop mainLoop(renderer);
	//After the mainLoop has stopped running, destroy window and end application.
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

SDL_Window* Main::getWindow() { return window; }
SDL_PixelFormat* Main::getWindowPixelFormat() { return windowPixelFormat; } 

int Main::getWidth()
{
	int w;
	SDL_GetWindowSize(window, &w, NULL);
	return w;
}

int Main::getHeight()
{
	int h;
	SDL_GetWindowSize(window, NULL, &h);
	return h;
}

SDL_Rect Main::getBGRect()
{
	SDL_Rect wr;

	if(getHeight()*16/9>getWidth()) {
		wr.w = getWidth(); wr.h = wr.w*9/16;
		wr.x = 0; wr.y = (getHeight()-wr.h)/2;
	} else {
		wr.h = getHeight(); wr.w = wr.h*16/9;
		wr.y = 0; wr.x = (getWidth()-wr.w)/2;
	}

	return wr;
}

double Main::getUIScaleAlt(bool accountForH)
{
    //UI scale
	double inc = 1./16.;
    double uiScale = 10;
	while(640*uiScale>Main::getWidth()) { uiScale -= inc; }
	if(accountForH)
	while(480*uiScale>Main::getHeight()) { uiScale -= inc; }

	if(uiScale<inc) uiScale = inc;
	return uiScale;
}

double Main::getUIScale() { return getUIScaleAlt(true); }

double Main::getTextScale()
{
	double ts = getUIScaleAlt(false);
	if(ts<0.75) ts = 0.75;
	return ts;
}