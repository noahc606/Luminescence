#pragma once
#include <cstdint>

class ScreenFader {
public:
    ScreenFader();
    ~ScreenFader();

    void tick();
private:

    uint64_t fadeLengthTotal = 0;
};