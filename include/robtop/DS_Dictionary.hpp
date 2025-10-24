#pragma once

#include <cocos2d.h>

class DS_Dictionary : public cocos2d::CCSprite {
public:
    DS_Dictionary();

    std::string saveRootSubDictToString();
    bool loadRootSubDictFromString(std::string);
};
