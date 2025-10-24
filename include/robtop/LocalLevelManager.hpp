#pragma once

#include <cocos2d.h>
#include "GJGameLevel.hpp"

class LocalLevelManager : public cocos2d::CCNode {
public:
    static LocalLevelManager* sharedState();
};
