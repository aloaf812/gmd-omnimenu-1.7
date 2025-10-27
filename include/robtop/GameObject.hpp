#pragma once

#include <cocos2d.h>

class GameObject : public cocos2d::CCSprite {
public:
    static GameObject* objectFromString(std::string);
};
