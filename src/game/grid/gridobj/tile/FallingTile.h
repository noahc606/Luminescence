#pragma once
#include "Tile.h"

class FallingTile {
public:
    FallingTile(double x, double y, Tile tile);
    ~FallingTile();

    double getX(); double getY();
    double getWeight();
    Tile getTile();

    void setX(double x); void setY(double y);
    void setWeight(double w);

private:
    double x = 0, y = 0;
    double weight = 0.45;
    Tile tile;
};