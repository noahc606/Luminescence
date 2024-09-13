#include "SkinChanger.h"

void SkinChanger::tick()
{
    if(changing) {
        progress += (1./400.);
    }

    if(progress>=1.0) {
        progress = 0;
        changing = false;
        finished = true;
    }
}

void SkinChanger::draw()
{

}

void SkinChanger::changeBetween(Skin* s1, Skin* s2)
{
    former = s1;
    latter = s2;
}

void SkinChanger::start()
{
    changing = true;
    progress = 0;
}

void SkinChanger::reset()
{
    changing = false;
    finished = false;
    progress = 0;

    former = latter;
    latter = nullptr;
}
std::string SkinChanger::getFormerSkinPD() { return former->getParentDir(); }
std::string SkinChanger::getLatterSkinPD() { return latter->getParentDir(); }
std::string SkinChanger::getFormerSkinID() { return former->getID(); }
std::string SkinChanger::getLatterSkinID() { return latter->getID(); }
bool SkinChanger::isChanging() { return changing; }
bool SkinChanger::hasFinished() { return finished; }
double SkinChanger::getProgress() { return progress; }