#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "Tile.h"
#include "Skin.h"

class Player {
public:
	enum IDs { AI_PLAYER = -1, PLAYER1 = 0, PLAYER2 = 1 };

	Player();
	Player(int x, int y, bool isAIControlled);
	Player(int x, int y);
	void reset(int x, int y, Player p);
	void reset(int x, int y);

	void tick();
	void drawQueuePiece(SDL_Renderer* rend, Skin* skin, double x, double y);
	void draw(SDL_Renderer* rend, Skin* skin, double tx, double ty);
	void draw(SDL_Renderer* rend, Skin* skin);

	int getX();
	int getY();
	Tile getTile(int index);
	int getCooldown();
	bool isAIControlled();
	bool isMainPlayer();
	uint64_t getLastFallMS();

	void setX(int x);
	void setY(int y);
	void rotate(bool clockwise);
	void setTilesRandom();
	void updateLastFallMS(uint64_t ingameTimeMS);
private:
	int x = 0;
	int y = -2;
	int cooldown = 0;
	std::vector<Tile> tiles;
	int id = PLAYER1;
	uint64_t lastFallMS = 0;
	
	SDL_Texture* tex = nullptr;
};