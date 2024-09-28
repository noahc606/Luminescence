#include "PlayerAIController.h"
#include <nch/sdl-utils/input.h>
#include "PlayerController.h"

PlayerAIController::PlayerAIController(){}
PlayerAIController::~PlayerAIController(){}

void PlayerAIController::init(Player* player, TileGridManaged* tgm)
{
    initted = true;
    pl = player;
    PlayerAIController::tgm = tgm;
}

void PlayerAIController::tick(TileGridSidebars& tgs, uint64_t ingameTicks64)
{
    if(!initted) return;
    

	int holdingDbg0 = nch::Input::keyDownTime(SDLK_z);
	int holdingDbg1 = nch::Input::keyDownTime(SDLK_x);

    /*
    int preferredType = tgs.getPlayerQueueCommonType();
    if(pl->areAllTilesSame()) {
        preferredType = pl->getTile(0).type;
    }*/

    if(ingameTicks64>lastActionMS64+200) {
        lastActionMS64 = ingameTicks64;


        std::pair<int, int> ai = chooseDropCol(-1);
        chosenColumn = ai.first;
        for(int i = 0; i<ai.second; i++) { PlayerController::ctrlRotateLeft(pl, tgm); }
        for(int i = 0; i<20; i++) { tryMoveTowardCol(chosenColumn); }
        PlayerController::ctrlDrop(pl, tgm, tgs, ingameTicks64);
    }


}

int PlayerAIController::getChosenColumn() { return chosenColumn; }

/*
	Returns a pair (min, max) of traversable column indices according to the player's current position.
*/
std::pair<int, int> PlayerAIController::getTraversableColumns()
{
	std::vector<int> res;

	int ty = pl->getY()+1;

	//To the left
	int min = pl->getX();
	for(int ix = pl->getX(); ix>=0; ix--) {
		if(tgm->getTile(ix, ty).type!=0) { break; }
		min = ix;
	}

	//To the right
	int max = pl->getX();
	for(int ix = pl->getX(); ix<tgm->numCols-1; ix++) {
		if(tgm->getTile(ix+1, ty).type!=0) { break; }
		max = ix;
	}

	return std::make_pair(min, max);
}

std::vector<bool> PlayerAIController::getLossColumns()
{
	int py = -2;
	std::vector<bool> res;

	//If using a lower level ("dumber") AI, this block has a chance to be ignored.
	for(int ix = 0; ix<=tgm->numCols-2; ix++) {
		//Track whether player at x='ix' dropping will lose the game
		bool wouldLose = false;
		if(tgm->getTile(ix+0, py+2).type!=0) {
			if(tgm->getTile(ix+1, py+2).type!=0 || tgm->getTile(ix+2, py+2).type!=0) {
				wouldLose = true;
			}
		}
		if(tgm->getTile(ix+1, py+2).type!=0) {
			if(tgm->getTile(ix-1, py+2).type!=0 || tgm->getTile(ix+0, py+2).type!=0) {
				wouldLose = true;
			}
		}
		if(tgm->getTile(ix+0, py+2).type!=0 && tgm->getTile(ix+1, py+2).type!=0) {
			wouldLose = true;
		}

		//If there are falling blocks right underneath player, count as losing position
		for(int i = 0; i<tgm->fallingTiles.size(); i++) {
			FallingTile ft = tgm->fallingTiles[i];
			
			//If there is 'high' falling tile (touching top 2 rows) right underneath the player
			if((ft.getX()==ix || ft.getX()==ix+1) && ft.getY()<2.) {
				wouldLose = true;				
			}
		}

		//Track losing columns
		if(wouldLose) 	{ res.push_back(true); }
		else 			{ res.push_back(false); }
	}

	return res;
}

TileGridManaged::t_grid PlayerAIController::getPostDropGridCopy(int pos, int rotations)
{
	//Temporary rotation (we undo this at the end of the function)
	for(int i = 0; i<rotations; i++) {
		pl->rotate(false);
	}

	TileGridManaged::t_grid res = tgm->getGridCopy();
	int px = pos;
	int py = pl->getY();
	int slide = 0;

	//Simulate fall

	
	
	//Account for sliding
	//Slide right...
	if(tgm->getTile(px, py+2).type!=0) {
		if(tgm->getTile(px+1, py+2).type==0 && tgm->getTile(px+2, py+2).type==0) {
			slide = 1;	//Successful slide right
		} else {
			return res;	//Unsuccessful slide right
		}
	}
	//Slide left
	if(tgm->getTile(px+1, py+2).type!=0) {
		if(tgm->getTile(px, py+2).type==0 && tgm->getTile(px-1, py+2).type==0) {
			slide = -1; //Successful slide right
		} else {
			return res;	//Unsuccessful slide right
		}
	}

	//Translate 'px' by 'slide'
	px += slide;

	//For each of the player columns - Go down vertically, and if solid tile found, move the two player tiles vertically above it to be on top
	for(int iy = py+2; iy<=tgm->numRows; iy++) {
		if(tgm->getTile(px, iy).type!=0) {
			tgm->setTileWithin(res, px, iy-2, pl->getTile(0));
			tgm->setTileWithin(res, px, iy-1, pl->getTile(2));
			break;
		}
	}
	for(int iy = py+2; iy<=tgm->numRows; iy++) {
		if(tgm->getTile(px+1, iy).type!=0) {
			tgm->setTileWithin(res, px+1, iy-2, pl->getTile(1));
			tgm->setTileWithin(res, px+1, iy-1, pl->getTile(3));
			break;
		}
	}

	//Go back to original rotation
	for(int i = 0; i<rotations; i++) {
		pl->rotate(true);
	}

	return res;
}

TileGridManaged::t_grid PlayerAIController::getPostDropGridCopy()
{
	return getPostDropGridCopy(pl->getX(), 0);
}

/*
	Count the number of NEW complete squares in a grid without changing the grid.
	We are assuming the 'grid' is a "slightly newer copy" of the main TileGridManaged::grid.

	This is used to help the AI player's logic.
*/
int PlayerAIController::getNumNewSquaresWithin(TileGridManaged::t_grid& gCopy, int gridColMin, int gridColMax, int requiredType)
{
	//Validate bounds
	if(gridColMin<0) gridColMin = 0;
	if(gridColMin>=gCopy.size()-1) gridColMin = gCopy.size()-2;
	if(gridColMax<0) gridColMax = 0;
	if(gridColMax>=gCopy.size()-1) gridColMax = gCopy.size()-2;

	//Go thru grid and count to find 'res'
	int res = 0;
	for(int ix = gridColMin; ix<=gridColMax; ix++) {
		for(int iy = 0; iy<gCopy[0].size()-1; iy++) {
			//Check the top left of this 2x2 area...
			Tile tracked = tgm->getTileWithin(gCopy, ix, iy);
			int t = tracked.type;
			if(t<=0 || (requiredType>0 && t!=requiredType)) continue;
			
			//Do the next 3 tiles within this 2x2 match?
			if(	tgm->getTileWithin(gCopy, ix+0, iy+1).type==t && !tgm->getTileWithin(gCopy, ix+0, iy+1).faded &&
				tgm->getTileWithin(gCopy, ix+1, iy+0).type==t && !tgm->getTileWithin(gCopy, ix+1, iy+0).faded &&
				tgm->getTileWithin(gCopy, ix+1, iy+1).type==t && !tgm->getTileWithin(gCopy, ix+1, iy+1).faded
			) {
				//If this completed square doesn't already exist within the main TileGridManaged::grid...
				if(!tgm->getTile(ix, iy).complete && !tgm->getTileWithin(gCopy, ix, iy).faded) {
					res++;	//Add to count
				}
			}
		}
	}

	//Return result
	return res;
}

int PlayerAIController::getNumNewSquaresWithin(TileGridManaged::t_grid& grid, int gridColMin, int gridColMax)
{ return getNumNewSquaresWithin(grid, gridColMin, gridColMax, -1); }

std::pair<int, int> PlayerAIController::chooseDropCol(int requiredType)
{
	int px = pl->getX();
	int py = pl->getY();

	
	
	std::vector<bool> fallCols;                             //Track columns which have falling tiles
	std::vector<bool> lossCols = getLossColumns();          //Track columns where dropping will lose the game.
	std::pair<int, int> tCols = getTraversableColumns();    //Track columns which are traversable to


	//Scan each grid column and find the move making the most squares
	int max = 0;
	int maxIX = -1;
	int rotationsTodo = 0;
	for(int r = 0; r<4; r++) {
		for(int ix = tCols.first; ix<=tCols.second; ix++) {
			TileGridManaged::t_grid grid = getPostDropGridCopy(ix, r);
			int count = getNumNewSquaresWithin(grid, ix-2, ix+2, requiredType);
			if(count>max) {
				//If this column is safe to drop in and exceeds the previous max, consider it.
				if(lossCols[ix]==false) {
					max = count;
					maxIX = ix;
					rotationsTodo = r;
				}
			}
		}
	}


	if(maxIX!=-1 && !lossCols[maxIX]) {
		//If optimal drop column found, return that column.
		printf("Chose optimal column %d with %d potential squares...\n", maxIX, max);
		return std::make_pair(maxIX, rotationsTodo);
	}

	//If no "good columns" found, return a random non-losing column
	int numSearches = 0;
	int rc = 0;
	do {
		rc = std::rand()%(tgm->numCols-1);
		numSearches++;
	}
	while( (lossCols[rc] || (tCols.first>rc || rc>tCols.second)) && numSearches<1000);
	
	printf("Chose random column %d...\n", rc);
	return std::make_pair(rc, 0);
}

void PlayerAIController::tryMoveTowardCol(int dCol)
{
	if(pl->getX()<dCol) {
		PlayerController::ctrlMoveRight(pl, tgm, 1, 20);
	}
	if(pl->getX()>dCol) {
		PlayerController::ctrlMoveLeft(pl, tgm, 1, 20);
	}
}