#pragma once

#include <cocos2d.h>

class DS_Dictionary : public cocos2d::CCSprite {
public:
    DS_Dictionary();

    std::string saveRootSubDictToString();
    bool loadRootSubDictFromString(std::string);

#ifdef NP4
    void setIntegerForKey(const char*, int);
    int getIntegerForKey(const char*);
#endif
};
