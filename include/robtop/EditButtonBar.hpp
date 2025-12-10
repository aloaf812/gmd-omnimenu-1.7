#pragma once

#include <cocos2d.h>

class EditButtonBar : public cocos2d::CCNode {
public:
#if GAME_VERSION < GV_1_6
    static EditButtonBar* create(CCArray*, CCPoint);
#else
    static EditButtonBar* create(CCArray*, CCPoint, bool);
#endif
};
