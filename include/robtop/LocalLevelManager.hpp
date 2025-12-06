#pragma once

#include <cocos2d.h>
#include "GJGameLevel.hpp"

#if GAME_VERSION > GV_1_0
class LocalLevelManager : public cocos2d::CCNode {
public:
    static LocalLevelManager* sharedState();
};
#endif