#include "PlayerController.h"
#include "Resources.h"

bool PlayerController::isColliding(Player* pl, TileGridManaged& tgm)
{
	int px = pl->getX();
	int py = pl->getY();
	
	for(int dx = 0; dx<2; dx++)
		for(int dy = 0; dy<2; dy++)
			if(tgm.getTile(px+dx, py+dy).type!=0) return true;
	
	return false;
}

int PlayerController::getSlideAmount(Player* pl, TileGridManaged& tgm)
{
	if(pl->getY()!=-2) {
		return 0;
	}
	int px = pl->getX();
	int py = pl->getY();

	//Slide 1 tile to the left or right if there is an empty gap there (forgives mistakes)
	pl->setY(py+1);
	int shouldSlide = 0;
	if(isColliding(pl, tgm)) {
		pl->setX(px-1); if(!isColliding(pl, tgm)) shouldSlide--;
		pl->setX(px+1); if(!isColliding(pl, tgm)) shouldSlide++;
		pl->setX(px);
	}
	pl->setY(py);

	return shouldSlide;
}

std::vector<std::vector<Tile>> PlayerController::getPostDropGridCopy(Player* pl, TileGridManaged& tgm)
{
	std::vector<std::vector<Tile>> res = tgm.getGridCopy();
	int px = pl->getX();
	int py = pl->getY();

	//Drop bottom-left tile - pl->getTile(2) which is @ (px, py+1)
	pl->getTile(2);
	for(int iy = py+1; iy<tgm.currSkin->getNumRows(); iy++) {
		//If we find a solid tile whose y>1
		if( tgm.getTile(px, iy).type>0 ) {

		}
	}

	return res;
}


void PlayerController::drop(Player* pl, TileGridManaged& tgm, TileGridSidebars& tgs, uint64_t ingameTimeMS)
{
	Resources::playAudio(tgm.currSkin->getParentDir()+"/"+tgm.currSkin->getID()+"/sfx/move_drop");
	tgs.incrementTotalScore();
	tgs.enableBonus();
	pl->updateLastFallMS(ingameTimeMS);

	//Get px, py, slide amount
	int px = pl->getX();
	int py = pl->getY();
	int psa = getSlideAmount(pl, tgm);

	//Loss check
	bool willCollide = false;
	if(pl->getY()==-2) {
		pl->setY(py+1);
		if(isColliding(pl, tgm)) willCollide = true;
		if(willCollide && psa==0) {
			tgm.lose();
		}
		pl->setY(py);
	}

	//Shift by slide amount, place fast falling tiles
	px += psa;
	if(!willCollide) {
		tgm.setFallingTile(px+0, py+0, pl->getTile(0));
		tgm.setFallingTile(px+1, py+0, pl->getTile(1));
	}
	tgm.setFallingTile(px+0, py+1, pl->getTile(2));
	tgm.setFallingTile(px+1, py+1, pl->getTile(3));

	reset(pl, tgm, tgs);
}

void PlayerController::rotateLeft(Player* pl, TileGridManaged& tgm)
{ pl->rotate(false); Resources::playAudio(tgm.currSkin->getParentDir()+"/"+tgm.currSkin->getID()+"/sfx/rotate_left"); }

void PlayerController::rotateRight(Player* pl, TileGridManaged& tgm)
{ pl->rotate(true); Resources::playAudio(tgm.currSkin->getParentDir()+"/"+tgm.currSkin->getID()+"/sfx/rotate_right"); }

void PlayerController::moveLeft(Player* pl, TileGridManaged& tgm, int ticksHeldRight, int ticksHoldDelay)
{ moveHorizontal(pl, tgm, ticksHeldRight, ticksHoldDelay, -1); }

void PlayerController::moveRight(Player* pl, TileGridManaged& tgm, int ticksHeldRight, int ticksHoldDelay)
{ moveHorizontal(pl, tgm, ticksHeldRight, ticksHoldDelay, 1); }

void PlayerController::reset(Player* pl, TileGridManaged& tgm, TileGridSidebars& tgs)
{
	int px = tgm.currSkin->getNumCols()/2-1;
	int py = -2;
	tgs.resetPlayerQueue(pl, px, py);
}

void PlayerController::moveHorizontal(Player* pl, TileGridManaged& tgm, int ticksHeldRight, int ticksHoldDelay, int dX)
{
	int px = pl->getX();
	pl->setX(px+dX);
	if(isColliding(pl, tgm)) {
		pl->setX(px);
	}

	//Sounds
	if(ticksHeldRight==1) { Resources::playAudio(tgm.currSkin->getParentDir()+"/"+tgm.currSkin->getID()+"/sfx/move"); }
	if(ticksHeldRight==ticksHoldDelay+1) { Resources::playAudio(tgm.currSkin->getParentDir()+"/"+tgm.currSkin->getID()+"/sfx/move_dash"); }
}