#pragma once
#include <nch/sdl-utils/gfx/Text.h>
#include <sstream>
#include "Player.h"
#include "Skin.h"


class TileGridSidebars {
public:
	void init(SDL_Renderer* rend, int difficulty);
    void tick(uint64_t ingameTimeMS);
	void drawSidebarUI(SDL_Renderer* rend, Skin* currSkin, nch::Text& t0, std::string t0Str, nch::Text& t1, std::string t1Str, int index);
    void draw(SDL_Renderer* rend, Skin* currSkin, Player* player, int numSweepedThisCycle, double mainSweeperX);
	void drawDebug(std::stringstream& ss, Skin* currSkin);

	int getLevelShown();
	int getLevelTechnical();
	int getTotalErasedShown();
	int getTotalErasedTechnical();

	void resetPlayerQueue(Player* player, int px, int py);
	void incrementLevel();
	void incrementCombos();
	void resetCombos();
	void incrementTotalScore(int by);
	void incrementTotalScore();
	void incrementTotalErased(int by);
	void giveSingleColorBonus();
	void giveAllClearBonus();
	void enableBonus();
	bool popSingleColorBonus();
	bool popAllClearBonus();

private:
    std::vector<Player> playerQueue;
	uint64_t lastPlayerReset = 0;

	//Text displays
	nch::Text lvlTxt; nch::Text lvlNTxt;
	nch::Text timeTxt; nch::Text timeDTxt;
	nch::Text scoreTxt; nch::Text scoreNTxt;
	nch::Text erasedTxt; nch::Text erasedNTxt;
	nch::Text comboTxt; nch::Text comboNTxt;
	nch::Text timelineTxt;

	bool canGetBonus = false;
	bool scoreBonusSingleColor = false;
	bool scoreBonusAllClear = false;

	//Level, score, erased, combos
    int level = 0;
    int levelTechnical = 0; //Starting: Beginner (0), Intermediate (40), Advanced (80), Expert (120), Super Expert (160)
	int totalScore = 0;
	int totalErased = 0;
	int totalErasedTechnical = levelTechnical*20;
	int comboCount = 0;

	//Time
	uint64_t timeSeconds = 0;
	uint64_t timeSecondsStarted = 0;
	uint64_t sdlTickStarted = 0;
	std::string timeDisplay = "0:00:00";
	int timeHours = 0;
};