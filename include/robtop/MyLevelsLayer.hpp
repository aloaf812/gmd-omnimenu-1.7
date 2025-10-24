#pragma once

#include <cocos2d.h>
#include "GJGameLevel.hpp"

class MyLevelsLayer : public cocos2d::CCLayer {
public:
    static LevelInfoLayer* create();
    static CCScene* scene();

    void onImport();
    void loadLevel(GJGameLevel* level);
    void reload();
};
