#pragma once
#include "Skin.h"

class SkinChanger {
public:

    void tick();
    void draw();

    void changeBetween(Skin* s1, Skin* s2);
    void start();
    void reset();

    std::string getFormerSkinPD();
    std::string getLatterSkinPD();
    std::string getFormerSkinID();
    std::string getLatterSkinID();
    bool isChanging();
    bool hasFinished();
    double getProgress();
private:

    Skin* former = nullptr;
    Skin* latter = nullptr;
    bool changing = false;
    bool finished = false;
    double progress = 0;

};