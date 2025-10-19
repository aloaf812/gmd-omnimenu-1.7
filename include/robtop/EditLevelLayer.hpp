#pragma once

#include <cocos2d.h>
#include "GJGameLevel.hpp"

class EditLevelLayer : public cocos2d::CCLayer {
public:
    static EditLevelLayer* create(GJGameLevel*);
    void onViewLevelInfo();
};
