#include "TileGridSidebars.h"
#include <nch/sdl-utils/timer.h>
#include <nch/sdl-utils/texture-utils.h>
#include "Main.h"
#include "Resources.h"
#include "TileGridManaged.h"

void TileGridSidebars::init(SDL_Renderer* rend, int difficulty)
{	
	levelTechnical = 40*difficulty;

	//Text displays
	std::string rsf = Resources::sidebarFont;
	lvlTxt.init(rend, Resources::getTTF(rsf));		lvlNTxt.init(rend, Resources::getTTF(rsf));
	timeTxt.init(rend, Resources::getTTF(rsf));		timeDTxt.init(rend, Resources::getTTF(rsf));
	scoreTxt.init(rend, Resources::getTTF(rsf));	scoreNTxt.init(rend, Resources::getTTF(rsf));
	erasedTxt.init(rend, Resources::getTTF(rsf));	erasedNTxt.init(rend, Resources::getTTF(rsf));
	comboTxt.init(rend, Resources::getTTF(rsf));	comboNTxt.init(rend, Resources::getTTF(rsf));
	timelineTxt.init(rend, Resources::getTTF(Resources::oswaldFont));
	
	sdlTickStarted = nch::Timer::getTicks64();
}

void TileGridSidebars::tick(uint64_t ingameTimeMS)
{
	/* Update time */
	//Get seconds
	timeSeconds = ingameTimeMS/1000;
	
	//Build time display using stringstream
	std::stringstream timeDisplaySS;
	//Hours 
	uint64_t hours = timeSeconds/3600;
	timeDisplaySS << hours << ":";
	//Minutes
	uint64_t minutes = (timeSeconds/60)%60;
	if(minutes<10) { timeDisplaySS << "0" << minutes << ":"; } else { timeDisplaySS << minutes << ":"; }
	//Seconds
	uint64_t seconds = timeSeconds%60;
	if(seconds<10) { timeDisplaySS << "0" << seconds; } else { timeDisplaySS << seconds; }

	//Update timeDisplay
	timeDisplay = timeDisplaySS.str();
}

void TileGridSidebars::drawSidebarUI(SDL_Renderer* rend, Skin* currSkin, nch::Text& t0, std::string t0Str, nch::Text& t1, std::string t1Str, int index)
{
	SDL_Rect bgr = Main::getBGRect();
	SDL_Rect gr = *currSkin->getGridRect();
	double heightScaleFactor = std::sqrt(188./t0.getUnscaledHeight());
	double s = 0.075*Main::getTextScale()*heightScaleFactor;

	//Statistic/Title
	t0.setText(t0Str); t0.setScale(s);
	int t0X = (bgr.x+bgr.w-bgr.w/16*0.4)-t0.getWidth(); if(t0X<=gr.x+gr.w) t0X = gr.x+gr.w;
	int t0Y = bgr.y+(bgr.h/9*(1.50+index*1.29));
	//Number/Desc.
	t1.setText(t1Str); t1.setScale(s);
	int t1X = (bgr.x+bgr.w-bgr.w/16*0.5)-t1.getWidth(); if(t1X<=gr.x+gr.w) t1X = gr.x+gr.w;
	int t1Y = bgr.y+(bgr.h/9*(1.50+index*1.29+0.68));

	//Draw 1: Score panel translucent background
	if(currSkin->areScorePanelsGeneric()) {
		SDL_Rect sptr;
		sptr.x = (bgr.x+bgr.w-bgr.w/16*2.5);
		sptr.y = t0Y;
		sptr.w = bgr.w/16*2.5;
		sptr.h = bgr.h/9*1.1;

		SDL_SetRenderDrawColor(rend, 0, 0, 0, 180);
		nch::TexUtils::renderFillBorderedRect(rend, &sptr, 2.*std::ceil(s), nch::Color(255, 255, 255, 80));
	}

	//Draw 2: t0 and t1
	t0.draw(t0X, t0Y);
	t1.draw(t1X, t1Y);
	
}

void TileGridSidebars::draw(SDL_Renderer* rend, Skin* currSkin, Player* player, int numSweepedThisCycle, double mainSweeperX)
{
	//Level
	std::stringstream lvlNss; lvlNss << (level/4+1) << "-" << (level%4+1);
	drawSidebarUI(rend, currSkin, lvlTxt, "LEVEL", lvlNTxt, lvlNss.str(), 0);
	//Time
	drawSidebarUI(rend, currSkin, timeTxt, "TIME", timeDTxt, timeDisplay, 1);
	//Score
	std::stringstream scoreNss; scoreNss << totalScore;
	drawSidebarUI(rend, currSkin, scoreTxt, "SCORE", scoreNTxt, scoreNss.str(), 2);
	/* Erased */
	std::stringstream erasedNss; erasedNss << totalErased;
	drawSidebarUI(rend, currSkin, erasedTxt, "ERASED", erasedNTxt, erasedNss.str(), 3);
	/* Combo */
	std::stringstream comboNss; comboNss << comboCount;
	drawSidebarUI(rend, currSkin, scoreTxt, "COMBO", comboNTxt, comboNss.str(), 4);

	//Time Line: Draw number of squares sweeped this cycle
	std::stringstream ss; ss << numSweepedThisCycle;
	timelineTxt.setText(ss.str());
	

	double heightScaleFactor = std::sqrt(188./timelineTxt.getUnscaledHeight());
	
	timelineTxt.setScale(0.15*currSkin->getGridScale()*heightScaleFactor);
	timelineTxt.draw(
		currSkin->getGridRect()->x+mainSweeperX*currSkin->getGridScale()-timelineTxt.getWidth()*1.5,
		currSkin->getGridRect()->y-timelineTxt.getHeight()+4*currSkin->getGridScale()
	);

	//Player queue
	if(playerQueue.size()==4) {
		double tileDY = -((double)(nch::Timer::getTicks64()-lastPlayerReset))/500.;
		if(tileDY<-1) tileDY = -1;

		if(tileDY!=-1) {
			player->drawQueuePiece(rend, currSkin, 0, (tileDY-1)*2.25);
		}

		int draws = playerQueue.size();
		for(int i = 0; i<draws; i++) {
			playerQueue[i].drawQueuePiece(rend, currSkin, 0, (i+tileDY)*2.25);
		}
	}
}

void TileGridSidebars::drawDebug(std::stringstream& ss, Skin* currSkin)
{
	ss << "levelTechnical=" << levelTechnical << "; fallTimeMS=" << TileGridManaged::getFallTimeMS(levelTechnical, currSkin) << ".\n";
}

int TileGridSidebars::getLevelShown() { return level; }
int TileGridSidebars::getLevelTechnical() { return levelTechnical; }
int TileGridSidebars::getTotalErasedShown() { return totalErased; }
int TileGridSidebars::getTotalErasedTechnical() { return totalErasedTechnical; }
int TileGridSidebars::getPlayerQueueCommonType()
{
	int type1Count = 0;
	int type2Count = 0;
	for(int i = 0; i<playerQueue.size(); i++) {
		Player tpl = playerQueue[i];
		for(int j = 0; j<4; j++) {
			if(tpl.getTile(j).type==1) { type1Count++; } else { type2Count++; }
		}
	}

	if(type1Count>type2Count) return 1;
	if(type2Count>type1Count) return 2;
	return -1;
}

void TileGridSidebars::resetPlayerQueue(Player* player, int px, int py)
{
	lastPlayerReset = nch::Timer::getTicks64();

	//Re-create player queue if it doesn't have 3 players in it
	if(playerQueue.size()!=4) {
		playerQueue.clear();
		for(int i = 0; i<4; i++) {
			playerQueue.push_back(Player(px, py));
		}
	}

	//Replace player with queue[0], remove queue[0], add queue[3]
	player->reset(px, py, playerQueue[0]);
	playerQueue.erase(playerQueue.begin());
	playerQueue.push_back(Player(px, py));
}
void TileGridSidebars::incrementLevel()
{
	level++;
	levelTechnical++;
}
void TileGridSidebars::incrementCombos() { comboCount++; }
void TileGridSidebars::resetCombos() { comboCount = 0; }
void TileGridSidebars::incrementTotalScore(int by) { totalScore += by; }
void TileGridSidebars::incrementTotalScore() { incrementTotalScore(1); }
void TileGridSidebars::incrementTotalErased(int by)
{
	totalErasedTechnical += by;
	totalErased += by;
}

void TileGridSidebars::giveSingleColorBonus()
{
	if(canGetSCBonus) {
		canGetSCBonus = false;
		incrementTotalScore(1000);
		scoreBonusSingleColor = true;
	}
}

void TileGridSidebars::giveAllClearBonus()
{
	if(canGetACBonus) {
		canGetACBonus = false;
		incrementTotalScore(10000);
		scoreBonusAllClear = true;
	}
}

void TileGridSidebars::enableBonus() {
	canGetSCBonus = true;
	canGetACBonus = true;
}

bool TileGridSidebars::popSingleColorBonus()
{
	if(scoreBonusSingleColor) {
		scoreBonusSingleColor = false;
		return true;
	}
	return false;
}

bool TileGridSidebars::popAllClearBonus()
{
	if(scoreBonusAllClear) {
		scoreBonusAllClear = false;
		return true;
	}
	return false;
}