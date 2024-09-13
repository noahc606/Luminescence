#include "FallingTile.h"

FallingTile::FallingTile(double x, double y, Tile tile)
{
    FallingTile::x = x;
    FallingTile::y = y;
    FallingTile::tile = tile;
}

FallingTile::~FallingTile(){}

double FallingTile::getX() { return x; }
double FallingTile::getY() { return y; }
double FallingTile::getWeight() { return weight; }
Tile FallingTile::getTile() { return tile; }

void FallingTile::setX(double x) { FallingTile::x = x; }
void FallingTile::setY(double y) { FallingTile::y = y; }
void FallingTile::setWeight(double w) { FallingTile::weight = w; }