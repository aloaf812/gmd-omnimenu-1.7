#pragma once

#include <cocos2d.h>
#include "GJGameLevel.hpp"

class GJSearchObject : public cocos2d::CCNode {
public:
    const char* getKey();
};

class LevelBrowserLayer : public cocos2d::CCLayer {
public:
    static LevelBrowserLayer* create(GJSearchObject*);
    static CCScene* scene(GJSearchObject*);
    void loadPage(GJSearchObject*);

    void onImport();
    void onRefresh();
    void loadLevel(GJGameLevel* level);
    void reload();
    void FLAlert();
};
