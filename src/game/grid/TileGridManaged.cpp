#include "TileGridManaged.h"
#include <nch/cpp-utils/log.h>
#include <nch/sdl-utils/timer.h>
#include "Resources.h"
#include "TileImg.h"

void TileGridManaged::preinit(std::pair<int, int> gridSize)
{
	numCols = gridSize.first;
	numRows = gridSize.second;
}

void TileGridManaged::init(Skin* currSkin)
{
	//Set current skin
	TileGridManaged::currSkin = currSkin;

	/* Build 2D grid array and fill with all zeros */
	for(int x = 0; x<numCols; x++) {
		//Build the current column and fill it with squares
		std::vector<Tile> thisCol;
		for(int y = numRows-1; y>=0; y--) {
			Tile s(0*(rand()%2+1));
			thisCol.push_back(s);
		}
		//Add this column to the grid
		grid.push_back(thisCol);
	}

	lastStartTimeMS = nch::Timer::getTicks64();
}


Tile TileGridManaged::getTileWithin(t_grid& grid, int x, int y)
{
	if( x>=0 && x<grid.size() && y<0 ) { return Tile(0); }	//Tiles above grid but not within are air (0)
	if( x<0 || x>=grid.size()) { return Tile(); }			//Tiles left or right of grid are solid (-1)
	if( y>=grid[x].size() ) { return Tile(); }				//Tiles bottom of grid are solid (-1)
	return grid[x][y];										//Return normal tile position which may be 0, 1, or 2
}

Tile TileGridManaged::getTile(int x, int y)
{
	return getTileWithin(grid, x, y);
}

bool TileGridManaged::isTilePartComplete(int x, int y)
{
	if(getTile(x, y).type<=0) return false;

	if(getTile(x, y).chainPart) return true;

	if(getTile(x-0, y-0).complete) return true;
	if(getTile(x-1, y-0).complete) return true;
	if(getTile(x-0, y-1).complete) return true;
	if(getTile(x-1, y-1).complete) return true;
	return false;
}

TileGridManaged::t_grid TileGridManaged::getGridCopy() { return grid; }

void TileGridManaged::tickFallingTiles()
{
	std::vector<int> deletedIDs;

	//Go through all falling tiles and apply physics
	for(int i = 0; i<fallingTiles.size(); i++) {
		/* Get info about this falling tile */
		FallingTile fti = fallingTiles[i];
		double ftx = fti.getX();
		double fty = fti.getY();
		double ftw = fti.getWeight();
		double fallspeed = ftw;
		//Reset onGround state.
		bool onGround = false;					
		//If the fallingTile finds a solid tile right below, set onGround to true.
		if(getTile(std::floor(ftx), std::floor(fty+1+fallspeed)).type!=0) onGround = true;

		/* Do one of two things depending on whether onGround */
		//If this fallingTile is onGround, set it to be a physical tile
		if(onGround) {
			int tx = std::floor(ftx);
			int ty = std::floor(fty+fallspeed);
			
			if(getTile(tx, ty).type==0 ) {
				setTile(tx, ty, fti.getTile());
				deletedIDs.push_back(i);
			} else {
				//Ideally, this siutation should not ever happen (or very rare)
				printf("FailedToFall!\n");
				fallingTiles[i].setY(ty-1.0);
			}
		//If this fallingTile is not onGround...
		} else {
			//See whether any fallingTiles with a lower weight are colliding with this one
			//If so, set this FallingTile's weight to be the lower value of the colliding FallingTile AND snap its height to be right above.
			//This prevents faster falling tiles from "falling through" other slower falling tiles (may happen during player's manual drops).
			for(int k = 0; k<2; k++)	//do two checks (dropped tiles come in packs of 2x2 so 2-high)
			for(int j = 0; j<fallingTiles.size(); j++) {
				FallingTile ftj = fallingTiles[j];
				if(ftj.getWeight()<fti.getWeight()) {
					bool intersects =
						(ftj.getX())==(fti.getX()) &&	//X
            			(ftj.getY())<(fti.getY()+1) && (fti.getY())<(ftj.getY()+1);	//Y
					if(intersects) {
						fallingTiles[i].setWeight(ftj.getWeight());	//Set weight
						fallingTiles[i].setY(ftj.getY()-1.0);		//Set height
					}
				}
			}

			//Finally,gGet height the tile will be after applying speed
			double nextY = fallingTiles[i].getY()+fallingTiles[i].getWeight();
			fallingTiles[i].setY(nextY);
		}
	}

	for(int i = deletedIDs.size()-1; i>=0; i--) {
		int elemID = deletedIDs[i];
		fallingTiles.erase(fallingTiles.begin()+elemID);
	}
}

void TileGridManaged::drawTiles(SDL_Renderer* rend)
{
	//Falling/moving tiles within 's'
	for(int i = 0; i<fallingTiles.size(); i++) {
		FallingTile ft = fallingTiles[i];
		
		//For fast-falling tiles (player drops), use special drawing rules.
		if(ft.getWeight()>0.1) {
			int groundTileY = std::floor(ft.getY()+1.+ft.getWeight()*3);
			if(getTile(ft.getX(), groundTileY).type==0 ) {
				//Draw at usual position
				TileImg::drawTile(rend, currSkin, ft.getX(), ft.getY(), ft.getTile());
			}
		} else {
			TileImg::drawTile(rend, currSkin, ft.getX(), ft.getY(), ft.getTile());
		}
	}

	//Solid, unmoving tiles within 'grid'
	for(int x = 0; x<numCols; x++) {
		for(int y = 0; y<numRows; y++) {

			bool shouldDraw = true;
			if(getTile(x-1, y-1).complete && !getTile(x, y).faded) shouldDraw = false;
			if(getTile(x-1, y-0).complete && !getTile(x, y).faded) shouldDraw = false;
			if(getTile(x-0, y-1).complete && !getTile(x, y).faded) shouldDraw = false;
			if(getTile(x-0, y-0).complete) shouldDraw = true;
			
			if(shouldDraw) {
				TileImg::drawTile(rend, currSkin, x, y, getTile(x, y));
			}
		}
	}
}

uint64_t TileGridManaged::getFallTimeMS(double level, Skin* currSkin)
{
	double lvl = level;

	//Logarithmic difficulty curve
	int64_t fallTimeMS = std::ceil(1000.-1256.*std::log10((lvl+30)/30));
	
	//Some skins have a maximum fall time MS
	if(fallTimeMS>currSkin->getMaxFallTimeMS()) {
		fallTimeMS = currSkin->getMaxFallTimeMS();
	}

	if(fallTimeMS<0) {
		fallTimeMS = 0;
	}

	return (uint64_t)fallTimeMS;
}

bool TileGridManaged::tryEraseFadedTiles(bool all, TileGridSidebars& tgs)
{
	//Track how many squares sweeped last column
	numSweepedLastCol = numSweepedThisCycle;

	//Erase faded tiles (tiles that were just sweeped) and increment numSweepedThisCycle.
	int maxCol = lastColSweeped;
	if(all) maxCol = 999999;
	for(int x = 0; x<numCols && x<=maxCol; x++)
	for(int y = 0; y<numRows; y++) {
		Tile t = getTile(x, y);
		if(t.faded) {
			setTile(x, y, Tile(0));
			if(t.complete) {
				numSweepedThisCycle++;
			}
		}
	}
     
	//Based on how many sweeped since last, play one of the following sounds
	int sweepdiff = numSweepedThisCycle-numSweepedLastCol;
	if(sweepdiff>3) 		{ Resources::playAudio(currSkin->getParentDir()+"/"+currSkin->getID()+"/sfx/timeline_erase_special");
	} else if(sweepdiff>2) 	{ Resources::playAudio(currSkin->getParentDir()+"/"+currSkin->getID()+"/sfx/timeline_erase_3");
	} else if(sweepdiff>1) 	{ Resources::playAudio(currSkin->getParentDir()+"/"+currSkin->getID()+"/sfx/timeline_erase_2");
	} else if(sweepdiff>0) 	{ Resources::playAudio(currSkin->getParentDir()+"/"+currSkin->getID()+"/sfx/timeline_erase_1");
	}
	
	if(sweepdiff>0) return true;
	return false;
}

void TileGridManaged::setTileWithin(t_grid& grid, int x, int y, Tile t)
{
	if( x<0 || y<0 ) { return; }
	if( x>=grid.size() ) { return; }
	if( y>=grid[x].size() ) { return; }
	grid[x][y] = t;
}
void TileGridManaged::setTile(int x, int y, Tile t) { setTileWithin(grid, x, y, t); }

void TileGridManaged::setTileComplete(int x, int y, bool complete)
{
	Tile t = getTile(x, y);
	t.setComplete(complete);
	setTile(x, y, t);
}

void TileGridManaged::setFallingTile(int x, int y, Tile t, double weight)
{
	setTile(x, y, Tile(0));
	
	t.chainPart = false;
	FallingTile ft(x, y, t);
	
	ft.setWeight(weight);
	fallingTiles.push_back(ft);
}

void TileGridManaged::setFallingTile(int x, int y, Tile t)
{
	FallingTile ft(x, y, t);
	setFallingTile(x, y, t, ft.getWeight());
}

void TileGridManaged::setTileToFalling(int x, int y)
{
	Tile origTile = getTile(x, y);
	if(origTile.type>0) {
		origTile.complete = false;
		origTile.faded = false;
		setFallingTile(x, y, origTile, 0.075);
	}
}

void TileGridManaged::lose()
{
	nch::Log::log("You just lost.");
	paused = true;
	Mix_PauseMusic();
	//std::exit(0);
}