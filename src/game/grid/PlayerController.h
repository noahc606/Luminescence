#pragma once
#include "Player.h"
#include "TileGridManaged.h"
#include "TileGridSidebars.h"

class PlayerController {
public:
    static bool isColliding(Player* pl, TileGridManaged* tgm);
    
    static void ctrlDrop(Player* pl, TileGridManaged* tgm, TileGridSidebars& tgs, uint64_t ingameTimeMS);
    static void ctrlRotateLeft(Player* pl, TileGridManaged* tgm);
    static void ctrlRotateRight(Player* pl, TileGridManaged* tgm);
    static void ctrlMoveLeft(Player* pl, TileGridManaged* tgm, int ticksHeldRight, int ticksHoldDelay);
    static void ctrlMoveRight(Player* pl, TileGridManaged* tgm, int ticksHeldRight, int ticksHoldDelay);
    static void idleFall(Player* pl, TileGridManaged* tgm, TileGridSidebars& tgs, uint64_t ingameTicks64, uint64_t fallTimeMS);
    static void reset(Player* pl, TileGridManaged* tgm, TileGridSidebars& tgs);

private:
    static void moveHorizontal(Player* pl, TileGridManaged* tgm, int ticksHeldRight, int ticksHoldDelay, int dX);
    static int getNeededSlideAmount(Player* pl, TileGridManaged* tgm);
};