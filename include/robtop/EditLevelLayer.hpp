#pragma once

#include <cocos2d.h>
#include "GJGameLevel.hpp"

class EditLevelLayer : public cocos2d::CCLayer {
public:
    static EditLevelLayer* create(GJGameLevel*);
    static CCScene* scene(GJGameLevel*);
    void onViewLevelInfo();
};
