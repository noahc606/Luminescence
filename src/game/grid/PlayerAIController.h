#pragma once
#include <vector>
#include "Player.h"
#include "TileGridManaged.h"
#include "TileGridSidebars.h"

class PlayerAIController {
public:
    enum AIAction {
        MOVE_LEFT, MOVE_RIGHT,
        QUICK_MOVE_LEFT, QUICK_MOVE_RIGHT,
        ROTATE_LEFT, ROTATE_RIGHT,
        DROP,
    };

    PlayerAIController();
    ~PlayerAIController();

    void init(Player* player, TileGridManaged* tgm);
    void tick(TileGridSidebars& tgs, uint64_t ingameTicks64);
    int getChosenColumn();
    std::pair<int, int> getTraversableColumns();
    std::vector<bool> getLossColumns();
    TileGridManaged::t_grid getPostDropGridCopy(int pos, int rotations);
    TileGridManaged::t_grid getPostDropGridCopy();
    int getNumNewSquaresWithin(TileGridManaged::t_grid& grid, int gridColMin, int gridColMax, int requiredType);
    int getNumNewSquaresWithin(TileGridManaged::t_grid& grid, int gridColMin, int gridColMax);
    
    std::pair<int, int> chooseDropCol(int requiredType);
    void tryMoveTowardCol(int dCol);

private:
    Player* pl = nullptr;
    TileGridManaged* tgm = nullptr;
    bool initted = false;
    std::vector<int> scheduledActions;

	int chosenColumn = -1;
    uint64_t lastActionMS64 = 0;
};