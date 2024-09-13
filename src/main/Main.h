#pragma once
#include <SDL2/SDL.h>

class Main {
public:
	static SDL_Window* getWindow();
	static SDL_PixelFormat* getWindowPixelFormat();
	static int getWidth();
	static int getHeight();
	static SDL_Rect getBGRect();
	static double getUIScaleAlt(bool accountForH);
	static double getUIScale();
	static double getTextScale();
};