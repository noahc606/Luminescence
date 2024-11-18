#pragma once
#include "FallingTile.h"
#include "Particle.h"
#include "Player.h"
#include "Skin.h"
#include "Sweeper.h"
#include "Tile.h"
#include "TileGridSidebars.h"


class TileGridManaged {
public:
	typedef std::vector<std::vector<Tile>> t_grid;

	void preinit(std::pair<int, int> gridSize);
	void init(Skin* currSkin);
	
	static Tile getTileWithin(t_grid& grid, int x, int y);
    Tile getTile(int x, int y);
    bool isTilePartComplete(int x, int y);
	t_grid getGridCopy();
	static uint64_t getFallTimeMS(double level, Skin* currSkin);
	

	void tickFallingTiles();
	void drawTiles(SDL_Renderer* rend);

	bool tryEraseFadedTiles(bool all, TileGridSidebars& tgs);
    static void setTileWithin(t_grid& grid, int x, int y, Tile t);
    void setTile(int x, int y, Tile t);
	void setTileComplete(int x, int y, bool complete);
	void setFallingTile(int x, int y, Tile t, double weight);
	void setFallingTile(int x, int y, Tile t);
	void setTileToFalling(int x, int y);
	void lose();

	//Graphical
	Skin* currSkin;

	//Tiles
	t_grid grid;
	std::vector<FallingTile> fallingTiles;
	int numCols = (int)(16);
	int numRows = (int)(10);

	//Logic
    int64_t counter = 0;
	uint64_t sweepTimer = 999999;
	uint64_t lastIdealSweepPos = 999999;
	bool sweepedLastTick = false;
	int lastColSweeped = -1;
	int numSweepedThisCycle = 0;
	int numSweepedLastCol = 0;
	int numSweepedLastCycle = 0;
	bool justFinishedSweep = false;
	bool paused = false;
	uint64_t lastStartTimeMS = 0;
	uint64_t lastPauseTimeMS = 0;
	uint64_t pausedTimeMS = 0;
	
private:

};