#pragma once

#include "constants.hpp"
#include <cocos2d.h>
#include "FLAlertLayer.hpp"
#include "GameObject.hpp"

class EditorUI : public cocos2d::CCLayer
{
public:
    static EditorUI* create();
    bool init();

// #if GAME_VERSION < 7
//     void keyBackClicked();
// #endif
    void onPause();
    void deselectAll();
    void selectObject(GameObject*);
#if GAME_VERSION < GV_1_5
    void onDuplicate();
#endif
};
