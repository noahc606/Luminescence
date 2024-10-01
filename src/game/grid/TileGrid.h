#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <nch/cpp-utils/arraylist.h>
#include "FallingTile.h"
#include "Player.h"
#include "PlayerAIController.h"
#include "Particle.h"
#include "Skin.h"
#include "Sweeper.h"
#include "Text.h"
#include "Tile.h"
#include "TileGridManaged.h"
#include "TileGridSidebars.h"

class TileGrid {
public:
	void init(SDL_Renderer* rend, std::vector<Skin*> skins, int currSkinID, int difficulty);
	void destroy();

	void tick();
	void draw(SDL_Renderer* rend);
	void drawUIs(SDL_Renderer* rend);
	void drawDebug(std::stringstream& ss);

	double getMainSweeperX();
	int getLevelShown();
	int getLevelTechnical();
	uint64_t getIngameTicks64();
	int getNumCols();
	int getNumRows();

	void changeSkinTo(Skin* newSkin);

private:
	void tickSweepers();
	void tickStaticTiles();
	void tickParticles();
	void tickPlayer(Player* pl);
	void tickMainPlayerControls(Player* pl);

	void addParticle(Particle* p);
	void scoreBonusCheck();
	void onSweepCycleComplete();

	//Non-tile-stuff within the TILE_GRID gamestate
	std::vector<Sweeper> sweepers;
	std::vector<Particle*> particles;
	nch::ArrayList<Player*> players;
	std::vector<Skin*> skins;

	//Within the TILE_GRID gamestate, manage certain things
	TileGridManaged tgm;	//Manage the tiles within the grid
	TileGridSidebars tgs;	//Manage all sidebar objects (includes the sweeper's text)
	PlayerAIController pAIC;
	uint64_t tickCounter = 0;
};