#pragma once

#include <cocos2d.h>

class GameSoundManager : public cocos2d::CCNode {
public:
    static GameSoundManager* sharedManager();

    void playUniqueEffect(const char*);
    void playEffect(const char*, float, float, float);
    void playEffect(const char*); // 1.0, 0.0, 1.0
};
