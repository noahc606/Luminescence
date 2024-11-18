#pragma once

class GUI {
public:

    enum UIType {
        UNSET,
        BUTTON, TOOLTIP, SKIN_SELECTOR,
    };

    GUI();
    ~GUI();
    void construct(UIType type, int id, int eid);
    void construct(UIType type, int id);

    virtual void tick();
    virtual void draw(int scrX);
    virtual void draw();
    
    int getEID();
    int getID();
    int getScreenID();
    UIType getType();

    void setScreenID(int sid);

private:
    int id = -1;
    int eid = 0;
    UIType type = UIType::UNSET;
    bool constructed = false;

protected:

int screenID = 0;
};