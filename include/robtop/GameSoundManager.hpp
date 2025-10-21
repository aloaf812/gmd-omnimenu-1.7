#pragma once

#include <cocos2d.h>

class GameSoundManager : public cocos2d::CCNode {
public:
    static GameSoundManager* sharedManager();

    void playUniqueEffect(const char*);
};
