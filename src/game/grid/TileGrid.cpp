#include "TileGrid.h"
#include <nch/cpp-utils/io/Log.h>
#include <nch/sdl-utils/Input.h>
#include <nch/sdl-utils/Timer.h>
#include <set>
#include <time.h>
#include "GridImg.h"
#include "Main.h"
#include "PlayerController.h"
#include "Resources.h"
#include "Text.h"
#include "TileImg.h"

void TileGrid::init(SDL_Renderer* rend, std::vector<Skin*> skins, int currSkinID, int difficulty)
{
	//Sanity check for skins
	if(skins.size()==0) {
		printf("Error: skins.size()==0 (TileGrid::init() at the wrong time?)\n");
		return;
	}

	//Skins and player
	TileGrid::skins = skins;
	tgm.currSkin = TileGrid::skins[currSkinID];
	players.clear();
	if(!true) {
		players.push_back(new Player(tgm.currSkin->getNumCols()/2-1, -2, true)); 
	} else {
		players.push_back(new Player(tgm.currSkin->getNumCols()/2-1, -2)); 
	}
	PlayerController::reset(players[0], tgm, tgs);

	//Init tile grid
	tgs.init(rend, difficulty);
	tgm.init();
}

void TileGrid::draw(SDL_Renderer* rend)
{
	/* Background */
	//Skin objects (grid, background movie)
	for(int i = 0; i<skins.size(); i++) {
		Skin* temp = skins[i];
		if(temp->isActive()) {
			temp->draw();
		}
	}

	/* Draw tiles */
	//Player
	for(int i = 0; i<players.size(); i++) players[i]->draw(rend, tgm.currSkin);
	//Static and moving tiles
	tgm.drawTiles(rend);

	/* Draw non-tile objects */
	//Sweepers
	for(int i = 0; i<sweepers.size(); i++) sweepers[i].draw(rend);
	//Particles
	for(int i = 0; i<particles.size(); i++) particles[i]->draw(rend, tgm.currSkin);
	//Draw sidebars/misc for MAIN PLAYER only.
	for(int i = 0; i<players.size(); i++) {
		Player* pl = players[i];
		if(pl->isMainPlayer()) {
			tgs.draw(rend, tgm.currSkin, pl, tgm.numSweepedThisCycle, getMainSweeperX());
		}
	}
	
	//Upon finished sweep
	if(tgm.justFinishedSweep) {
		tgm.justFinishedSweep = false;
		if(tgm.numSweepedLastCycle!=0) {
			addParticle(new Particle(0, 0, Particle::SWEEP_COUNT_INDICATOR, rend, tgm.numSweepedLastCycle));
		}
	}

	//Upon score bonus
	if(tgs.popSingleColorBonus()) 	{ addParticle(new Particle(Particle::SCORE_BONUS, rend, 0)); }
	if(tgs.popAllClearBonus()) 		{ addParticle(new Particle(Particle::SCORE_BONUS, rend, 1)); }

}

void TileGrid::drawDebug(std::stringstream& ss)
{
	uint64_t elapsedMusicTimeMS = getIngameTicks64()-tgm.currSkin->getMusicStartTimeMS();
	double sweepTimer = elapsedMusicTimeMS*tgm.currSkin->getBPM()/(30.*1000.);
	ss << "sweepTimer=" << sweepTimer << ";\n";
	tgs.drawDebug(ss, tgm.currSkin);
}

void TileGrid::tick()
{
	if(tgm.currSkin==nullptr) return;

	if(nch::Input::keyDownTime(SDLK_ESCAPE)==1 || nch::Input::joystickButtonDownTime(9)==1) {
		if(!tgm.paused) {
			Mix_PauseMusic();
			tgm.paused = true;
			tgm.lastPauseTimeMS = getIngameTicks64();
		} else {
			Mix_ResumeMusic();
			tgm.paused = false;
			tgm.pausedTimeMS += (getIngameTicks64()-tgm.lastPauseTimeMS);
		}
	}

	if(!tgm.paused) {
		/* Update game objects */
		//Physical grid objects
		tickSweepers();
		for(int i = 0; i<3; i++) tgm.tickFallingTiles();
		tickStaticTiles();
		//Graphical grid objects
		tickParticles();
		//Player(s)
		for(int i = 0; i<players.size(); i++) tickPlayer(players[i]);

		tgs.tick(getIngameTicks64());
	}

	tickCounter++;
}

void TileGrid::changeSkinTo(Skin* newSkin)
{
	//Reset sweep timer
	tgm.sweepTimer = 0;

	//Change skin of main sweeper
	int minID = 0;
	double minX = sweepers[0].getMainLineX();
	for(int i = 0; i<sweepers.size(); i++) {
		if(sweepers[i].getMainLineX()<minX) {
			minX = sweepers[i].getMainLineX();
			minID = i;
		}
	}
	sweepers[minID].setSkin(newSkin);

	//Activate the new skin
	tgm.currSkin = newSkin;
	tgm.currSkin->activate(getIngameTicks64());
}

double TileGrid::getMainSweeperX()
{
	if(sweepers.size()<1) {
		return 0;
	}

	double minX = sweepers[0].getMainLineX();
	for(int i = 0; i<sweepers.size(); i++) {
		if(sweepers[i].getMainLineX()<minX) {
			minX = sweepers[i].getMainLineX();
		}
	}
	return minX;
}

int TileGrid::getLevelShown() { return tgs.getLevelShown(); }
uint64_t TileGrid::getIngameTicks64()
{
	return
		nch::Timer::getTicks64()-
		tgm.pausedTimeMS-
		tgm.lastStartTimeMS;
}

void TileGrid::tickSweepers()
{
	//Track number of squares sweeped and add it to the score
	//IMPORTANT: Running onSweepCycleComplete takes place JUST AFTER new sweeper added.
	if(tgm.sweepedLastTick) {
		onSweepCycleComplete();
		tgm.sweepedLastTick = false;
	}

	//Do some math with elapsed music time.
	//We need to have sweeper creation sync up with music and sound effects (the skin's BPM).
	uint64_t elapsedMusicTimeMS = getIngameTicks64()-tgm.currSkin->getMusicStartTimeMS();
	tgm.sweepTimer = elapsedMusicTimeMS*tgm.currSkin->getBPM()*32./(30.*1000.);
	uint64_t idealSweepPos = tgm.sweepTimer%(tgm.currSkin->getNumCols()*32);
	if(idealSweepPos<tgm.lastIdealSweepPos) {
		sweepers.push_back(Sweeper(tgm.currSkin, getIngameTicks64()));
		tgm.lastIdealSweepPos = idealSweepPos;
		tgm.sweepedLastTick = true;
	}

	tgm.lastIdealSweepPos = idealSweepPos;

	//Get the current column of the sweeper
	int colSweeped = std::floor(getMainSweeperX()/32.-0.5);
	//Every time the sweeper moves by a column
	if(tgm.lastColSweeped!=colSweeped) {
		tgm.lastColSweeped = colSweeped;
		
		bool foundComplete = false;
		int numCompletes = 0;
		for(int y = 0; y<tgm.currSkin->getNumRows(); y++) {
			int x = colSweeped;
			if(x==-1) x = tgm.currSkin->getNumCols()-1;

			if(tgm.isTilePartComplete(x, y)) {
				Tile t = tgm.getTile(x, y);
				t.faded = true;
				tgm.setTile(x, y, t);
				
				foundComplete = true;
				numCompletes++;
			}
			if(tgm.getTile(x, y).faded) {
				foundComplete = true;
			}
		}
	
		if(tgm.lastColSweeped==-1) {
			if(tgm.tryEraseFadedTiles(true, tgs)) {
				scoreBonusCheck();
			}
		} else {
			if(!foundComplete) {
				if(tgm.tryEraseFadedTiles(false, tgs)) {
					scoreBonusCheck();
				}
			}
		}

		if(numCompletes>0) {
			Resources::playAudio(tgm.currSkin->getParentDir()+"/"+tgm.currSkin->getID()+"/sfx/timeline_touch");
		}
	}

	//Delete offscreen sweepers
	for(int i = 0; i<sweepers.size(); i++) {
		if(sweepers[i].isOffscreen()) {
			sweepers.erase(sweepers.begin()+i);
			break;
		}
	}

	//Tick sweepers
	for(int i = 0; i<sweepers.size(); i++) {
		sweepers[i].tick(getIngameTicks64());
	}
}

void TileGrid::tickStaticTiles()
{
	//Mark the top left tile of any same-color 2x2 as "complete"
	for(int x = 0; x<tgm.currSkin->getNumCols(); x++)
	for(int y = 0; y<tgm.currSkin->getNumRows(); y++) {
		bool complete2x2 = true;
		int thisType = tgm.getTile(x, y).type;
		if(thisType<=0) {
			complete2x2 = false;
		} else {
			for(int i = 0; i<2; i++)
			for(int j = 0; j<2; j++) {
				if(tgm.getTile(x+i, y+j).type!=thisType) {
					complete2x2 = false;
				}
			}
		}

		if(complete2x2) {
			if(!tgm.getTile(x, y).complete) {
				
				//Add particles
				addParticle(new Particle(x, y, Particle::COMPLETED_SQUARE_FLASH));
				for(int p = 0; p<5; p++)
				addParticle(new Particle(x, y, Particle::COMPLETED_SQUARE_OUTLINE, tgm.currSkin->getColorFromTileType(tgm.getTile(x, y).type) ));
				
				//Play audio
				if(tgm.getTile(x, y).type%2==0) { Resources::playAudio(tgm.currSkin->getParentDir()+"/"+tgm.currSkin->getID()+"/sfx/create_square_a"); }
				else 							{ Resources::playAudio(tgm.currSkin->getParentDir()+"/"+tgm.currSkin->getID()+"/sfx/create_square_b"); } 
			}
			tgm.setTileComplete(x, y, true);
		} else {
			tgm.setTileComplete(x, y, false);
		}
	}

	//All tiles next to a tile that is a chain tile will have its chainPart flag enabled.
	for(int x = 0; x<tgm.currSkin->getNumCols(); x++)
	for(int y = 0; y<tgm.currSkin->getNumRows(); y++) {
		Tile t1 = tgm.getTile(x, y);
		int msx = std::floor(getMainSweeperX()/32.-0.5);
		if( (t1.chainStart || t1.chainPart) && !t1.faded && x!=msx-1 && x!=msx) {
			int dx[] = {-1, 0, 0, 1};
			int dy[] = {0, -1, 1, 0};
			for(int i = 0; i<4; i++) {
				Tile t2 = tgm.getTile(x+dx[i], y+dy[i]);
				if(t1.type==t2.type) {
					Tile t3 = t2; t3.chainPart = true;
					tgm.setTile(x+dx[i], y+dy[i], t3);
				}
			}
		}
	}

	//Cause floating tiles to fall (go from bottom to top, so all tiles in a column fall at the same time)
	for(int x = 0; x<tgm.currSkin->getNumCols(); x++)
	for(int y = tgm.currSkin->getNumRows()-1; y>=0; y--) {
		if(tgm.getTile(x, y).type>0 && tgm.getTile(x, y+1).type==0) {
			tgm.setTileToFalling(x, y);
		}
	}
}

void TileGrid::tickParticles()
{
	std::vector<int> deletedIDs;

	//Tick all particles
	for(int i = 0; i<particles.size(); i++) {
		Particle* part = particles[i];
		part->tick();

		//If particle age reaches max age
		if(part->getAge()>=part->getMaxAge()) {
			deletedIDs.push_back(i);
		}
	}

	//Remove particles that should be deleted
	for(int i = deletedIDs.size()-1; i>=0; i--) {
		int elemID = deletedIDs[i];
		delete particles[elemID];
		particles.erase(particles.begin()+elemID);
	}
}

void TileGrid::tickPlayer(Player* pl)
{
	int px = pl->getX();
	int py = pl->getY();

	//Calculate fall time based on skin and level
	int64_t fallTimeMS = tgm.getFallTimeMS(tgs.getLevelTechnical(), tgm.currSkin);
	
	//Fall every 'fallTimeMS' millseconds
	if(pl->getCooldown()>=0) {
		if(getIngameTicks64()>=pl->getLastFallMS()+fallTimeMS) {
			pl->updateLastFallMS(getIngameTicks64());

			//"Test fall"
			bool willCollide = false;
			pl->setY(py+1);
			if(PlayerController::isColliding(pl, tgm)) {
				willCollide = true;
				pl->setY(py);
			}

			//If the player will collide, then cause tiles to fall
			if(willCollide) {
				int psa = PlayerController::getSlideAmount(pl, tgm);
				px += psa;

				//Loss type 1
				if(psa==0) {
					if(pl->getY()==-2) {
						tgm.lose();
					}
				}

				if(pl->getY()>-1 || psa!=0) {
					tgm.setFallingTile(px+0, py+0, pl->getTile(0), 0.075);
					tgm.setFallingTile(px+1, py+0, pl->getTile(1), 0.075);
				}
				tgm.setFallingTile(px+0, py+1, pl->getTile(2), 0.075);
				tgm.setFallingTile(px+1, py+1, pl->getTile(3), 0.075);
				PlayerController::reset(pl, tgm, tgs);
				tgs.enableBonus();
				Resources::playAudio(tgm.currSkin->getParentDir()+"/"+tgm.currSkin->getID()+"/sfx/move_drop");
			}
		}
	}

	pl->tick();

	if(pl->isMainPlayer()) { tickMainPlayerControls(pl); }
	if(pl->isAIControlled()) { tickAIPlayerControls(pl); }
}

void TileGrid::tickMainPlayerControls(Player* pl)
{
	int holdDelay = 20-tgs.getLevelTechnical()/16;
	if(holdDelay<15) holdDelay = 15;

	int holdingRotR = nch::Input::keyDownTime(SDLK_w) + nch::Input::joystickHatDirTime(nch::Input::UP);		//W or joystick hat UP to rotate right
	int holdingRotL = nch::Input::keyDownTime(SDLK_s) + nch::Input::joystickHatDirTime(nch::Input::DOWN);	//S or joystick hat DOWN to rotate left
	int holdingLeft = nch::Input::keyDownTime(SDLK_a) + nch::Input::joystickHatDirTime(nch::Input::LEFT);	//A or joystick hat LEFT to move left
	int holdingRight = nch::Input::keyDownTime(SDLK_d) + nch::Input::joystickHatDirTime(nch::Input::RIGHT);	//D or joystick hat RIGHT to move right
	int holdingDrop = nch::Input::mouseDownTime(1) + nch::Input::joystickButtonDownTime(1);					//LEFT MOUSE or joystick hat BUTTON 1 to hard drop

	//ESCAPE or joystick button 9 to pause

	//Rotate left, right
	if(holdingRotR==1 && holdingRotL!=1) { PlayerController::rotateRight(pl, tgm); }
	if(holdingRotL==1 && holdingRotR!=1) { PlayerController::rotateLeft(pl, tgm); }

	//Moving left and right
	if( holdingRight==0 && (holdingLeft==1 || (holdingLeft>holdDelay && pl->getCooldown()>=-60+holdDelay)) ) {
		PlayerController::moveLeft(pl, tgm, holdingLeft, holdDelay);
	}
	if( holdingLeft==0 && (holdingRight==1 || (holdingRight>holdDelay && pl->getCooldown()>=-60+holdDelay)) ) {
		PlayerController::moveRight(pl, tgm, holdingRight, holdDelay);
	}

	//Hard dropping
	if(holdingDrop==1 || holdingDrop>holdDelay && holdingDrop%20==0) {
		PlayerController::drop(pl, tgm, tgs, getIngameTicks64());
	}
}

void TileGrid::tickAIPlayerControls(Player* pl)
{
	if(tickCounter%100==0) {
		


		PlayerController::rotateLeft(pl, tgm);
	}

}

void TileGrid::addParticle(Particle* p) { particles.push_back(p); }

void TileGrid::scoreBonusCheck()
{
	//See if the grid has a single color or is all clear
	int tilesFound = 0;
	int firstTileTypeFound = 0;
	for(int x = 0; x<tgm.currSkin->getNumCols(); x++)
	for(int y = 0; y<tgm.currSkin->getNumRows(); y++) {
		int tt = tgm.getTile(x, y).type;
		if(tt!=0) {
			tilesFound++;

			if(firstTileTypeFound==0)	{ firstTileTypeFound = tt; }
			if(tt!=firstTileTypeFound)	{ firstTileTypeFound = -1000; }
		}
	}

	for(int i = 0; i<tgm.fallingTiles.size(); i++) {
		Tile ft = tgm.fallingTiles[i].getTile();
		int ftt = ft.type;
		if(ftt!=0) {
			tilesFound++;

			if(firstTileTypeFound==0)	{ firstTileTypeFound = ftt; }
			if(ftt!=firstTileTypeFound)	{ firstTileTypeFound = -1000; }
		}
	}

	//Give score bonuses for single color or all clear	
	if(tilesFound==0) {
		tgs.giveAllClearBonus();
	} else if(firstTileTypeFound>0) {
		tgs.giveSingleColorBonus();
	}
}

void TileGrid::onSweepCycleComplete()
{
	//Get rid of all complete tiles on the rightmost column


	//Don't lose combos during skin level up
	uint64_t skinElapsedTimeMS = getIngameTicks64()-tgm.currSkin->getMusicStartTimeMS();
	if(skinElapsedTimeMS<100) return;

	//When having a high sweep count: play sound, increment combo count	
	if(tgm.numSweepedThisCycle>=4) {
		tgs.incrementCombos();
		Resources::playAudio(tgm.currSkin->getParentDir()+"/"+tgm.currSkin->getID()+"/sfx/score");
	} else {
		tgs.resetCombos();
	}

	//Increment squares and level
	tgs.incrementTotalErased(tgm.numSweepedThisCycle);
	if(tgs.getTotalErasedShown()>=(getLevelShown()+1)*20) {
		tgs.incrementLevel();
	}

	//Increment score based on numSweepedThisCycle
	int dScore = 0;
	switch(tgm.numSweepedThisCycle) {
		case 0: {} break;
		case 1: { dScore = 40; } break;
		case 2: { dScore = 80; } break;
		case 3: { dScore = 120; } break;
		default: { dScore = (480+160*(tgm.numSweepedThisCycle-3)); } break;
	}
	tgs.incrementTotalScore(dScore);

	//Reset count for this cycle
	tgm.numSweepedLastCycle = tgm.numSweepedThisCycle;
	tgm.numSweepedThisCycle = 0;
	tgm.justFinishedSweep = true;

	//Start playing music again if it has stopped (keeps music in sync w/ sweeper)
	if(tgm.currSkin->hasMusicStopped()) {
		tgm.currSkin->playMusic(tgm.currSkin);
	}
}

