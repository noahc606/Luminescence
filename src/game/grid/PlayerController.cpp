#include "PlayerController.h"
#include "Resources.h"

bool PlayerController::isColliding(Player* pl, TileGridManaged* tgm)
{
	int px = pl->getX();
	int py = pl->getY();
	
	//Test for collision with a non-falling tile...
	for(int dx = 0; dx<2; dx++)
		for(int dy = 0; dy<2; dy++)
			if(tgm->getTile(px+dx, py+dy).type!=0) return true;
	
	//Test for collision with a falling tile...
	for(int i = 0; i<tgm->fallingTiles.size(); i++) {
		FallingTile ft = (tgm->fallingTiles[i]);

		if((int)ft.getX()==px || (int)ft.getX()==(px+1)) {
			//[py, py+2)
			if((int)ft.getY()>=py && (int)ft.getY()<py+2 ) {
				return true;
			}
		}
	}

	return false;
}

void PlayerController::ctrlDrop(Player* pl, TileGridManaged* tgm, TileGridSidebars& tgs, uint64_t ingameTimeMS)
{
	Resources::playAudio(tgm->currSkin->getParentDir()+"/"+tgm->currSkin->getID()+"/sfx/move_drop");
	tgs.incrementTotalScore();
	tgs.enableBonus();
	pl->updateLastFallMS(ingameTimeMS);

	//Get px, py, slide amount
	int px = pl->getX();
	int py = pl->getY();
	int psa = getNeededSlideAmount(pl, tgm);

	//Loss check
	bool willCollide = false;
	if(pl->getY()==-2) {
		pl->setY(py+1);
		if(isColliding(pl, tgm)) willCollide = true;
		if(willCollide && psa==0) {
			tgm->lose();
		}
		pl->setY(py);
	}

	//Shift by slide amount, place fast falling tiles
	px += psa;
	if(!willCollide) {
		tgm->setFallingTile(px+0, py+0, pl->getTile(0));
		tgm->setFallingTile(px+1, py+0, pl->getTile(1));
	}
	tgm->setFallingTile(px+0, py+1, pl->getTile(2));
	tgm->setFallingTile(px+1, py+1, pl->getTile(3));

	reset(pl, tgm, tgs);
}

void PlayerController::ctrlRotateLeft(Player* pl, TileGridManaged* tgm)
{ pl->rotate(false); Resources::playAudio(tgm->currSkin->getParentDir()+"/"+tgm->currSkin->getID()+"/sfx/rotate_left"); }

void PlayerController::ctrlRotateRight(Player* pl, TileGridManaged* tgm)
{ pl->rotate(true); Resources::playAudio(tgm->currSkin->getParentDir()+"/"+tgm->currSkin->getID()+"/sfx/rotate_right"); }

void PlayerController::ctrlMoveLeft(Player* pl, TileGridManaged* tgm, int ticksHeldRight, int ticksHoldDelay)
{ moveHorizontal(pl, tgm, ticksHeldRight, ticksHoldDelay, -1); }

void PlayerController::ctrlMoveRight(Player* pl, TileGridManaged* tgm, int ticksHeldRight, int ticksHoldDelay)
{ moveHorizontal(pl, tgm, ticksHeldRight, ticksHoldDelay, 1); }

void PlayerController::idleFall(Player* pl, TileGridManaged* tgm, TileGridSidebars& tgs, uint64_t ingameTicks64, uint64_t fallTimeMS)
{
	int px = pl->getX();
	int py = pl->getY();

	if(ingameTicks64>=pl->getLastFallMS()+fallTimeMS) {
		pl->updateLastFallMS(ingameTicks64);

		//"Test fall"
		bool willCollide = false;
		pl->setY(py+1);
		if(PlayerController::isColliding(pl, tgm)) {
			willCollide = true;
			pl->setY(py);
		}

		//If the player will collide, then cause tiles to fall
		if(willCollide) {
			int psa = PlayerController::getNeededSlideAmount(pl, tgm);
			px += psa;

			//Loss type 1
			if(psa==0) {
				if(pl->getY()==-2) {
					tgm->lose();
				}
			}

			if(pl->getY()>-1 || psa!=0) {
				tgm->setFallingTile(px+0, py+0, pl->getTile(0), 0.075);
				tgm->setFallingTile(px+1, py+0, pl->getTile(1), 0.075);
			}
			tgm->setFallingTile(px+0, py+1, pl->getTile(2), 0.075);
			tgm->setFallingTile(px+1, py+1, pl->getTile(3), 0.075);
			PlayerController::reset(pl, tgm, tgs);
			tgs.enableBonus();
			Resources::playAudio(tgm->currSkin->getParentDir()+"/"+tgm->currSkin->getID()+"/sfx/move_drop");
		}
	}
}

void PlayerController::reset(Player* pl, TileGridManaged* tgm, TileGridSidebars& tgs)
{
	int px = tgm->numCols/2-1;
	int py = -2;
	tgs.resetPlayerQueue(pl, px, py);
}

void PlayerController::moveHorizontal(Player* pl, TileGridManaged* tgm, int ticksHeldRight, int ticksHoldDelay, int dX)
{
	int px = pl->getX();
	pl->setX(px+dX);
	if(isColliding(pl, tgm)) {
		pl->setX(px);
	}

	//Sounds
	if(ticksHeldRight==1) { Resources::playAudio(tgm->currSkin->getParentDir()+"/"+tgm->currSkin->getID()+"/sfx/move"); }
	if(ticksHeldRight==ticksHoldDelay+1) { Resources::playAudio(tgm->currSkin->getParentDir()+"/"+tgm->currSkin->getID()+"/sfx/move_dash"); }
}





int PlayerController::getNeededSlideAmount(Player* pl, TileGridManaged* tgm)
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