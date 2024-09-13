#include "Tile.h"

Tile::Tile() {}
Tile::Tile(int type)
{
    Tile::type = type;
}

Tile::~Tile(){}

void Tile::setComplete(bool complete)
{
    Tile::complete = complete;
}