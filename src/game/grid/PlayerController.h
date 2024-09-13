#pragma once
#include "Player.h"
#include "TileGridManaged.h"
#include "TileGridSidebars.h"

class PlayerController {
public:
    static bool isColliding(Player* pl, TileGridManaged& tgm);
    static int getSlideAmount(Player* pl, TileGridManaged& tgm);
    std::vector<std::vector<Tile>> getPostDropGridCopy(Player* pl, TileGridManaged& tgm);

    static void drop(Player* pl, TileGridManaged& tgm, TileGridSidebars& tgs, uint64_t ingameTimeMS);
    static void rotateLeft(Player* pl, TileGridManaged& tgm);
    static void rotateRight(Player* pl, TileGridManaged& tgm);
    static void moveLeft(Player* pl, TileGridManaged& tgm, int ticksHeldRight, int ticksHoldDelay);
    static void moveRight(Player* pl, TileGridManaged& tgm, int ticksHeldRight, int ticksHoldDelay);
    static void reset(Player* pl, TileGridManaged& tgm, TileGridSidebars& tgs);

private:
    static void moveHorizontal(Player* pl, TileGridManaged& tgm, int ticksHeldRight, int ticksHoldDelay, int dX);
};