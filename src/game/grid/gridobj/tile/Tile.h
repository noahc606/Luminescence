#pragma once

class Tile
{
public:
    Tile();
    Tile(int type);
    ~Tile();

    void setComplete(bool complete);

    int type = -1;
    bool chainStart = false;
    bool chainPart = false;
    bool complete = false;
    bool faded = false;
};