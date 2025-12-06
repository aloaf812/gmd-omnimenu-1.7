#pragma once

#include <cocos2d.h>

class CCScrollLayerExt : public cocos2d::CCLayer {
public:
    CCScrollLayerExt(CCRect rect);

    void registerWithTouchDispatcher();
};