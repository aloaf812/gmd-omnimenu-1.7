#pragma once

#include "constants.hpp"
#include <cocos2d.h>
#include "FLAlertLayer.hpp"
#include "GameObject.hpp"
#include "CCMenuItemSpriteExtra.hpp"

class CreateMenuItem : public CCMenuItemSpriteExtra {};

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
    void onDeleteSelected();
    void updateCreateMenu();
    CreateMenuItem* getCreateBtn(const char*, int);
#if GAME_VERSION < GV_1_5
    GameObject* duplicateObject(GameObject* obj, void* getSaveString);
    void onDuplicate();
#endif
#if GAME_VERSION < GV_1_6
    void onDeleteStartPos();
#endif
};
