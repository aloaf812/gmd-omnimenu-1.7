#pragma once

#include <cocos2d.h>
#include "GameObject.hpp"

class DrawGridLayer : public cocos2d::CCLayer {
public:
    static DrawGridLayer* create(CCNode*);
    void addToEffects(GameObject*);
};
