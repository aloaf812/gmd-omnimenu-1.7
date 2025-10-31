#pragma once

#include <cocos2d.h>
#include <Module.hpp>

class HaxMenu : public CCLayerColor {

public:
    static HaxMenu* create(CCLayer* referrer);
    static cocos2d::CCScene* scene();

protected:
    bool init(CCLayer* referrer);
    void onClose(CCObject*);
    void keyBackClicked();

    void registerWithTouchDispatcher();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

    void addButton(const char* label, float fontSize, float yOffset, CCObject* target, SEL_MenuHandler selector);
    // void addButtonRight(const char* label, float fontSize, float yOffset, CCObject* target, SEL_MenuHandler selector);
    void onCategory(ModuleCategory category);
    void onGameplay();
    void onEditor();
    void onBypass();
    void onInformational();
    void onUniversal();
    void onParticles();

    void toggler(CCObject*);
    float getDuration();
    void setColorAtIndex(int index);
    void onPih(CCObject*);

    CCNode* leftParent;
    CCNode* rightParent;
    CCLayer* referrer;
    CCArray* catButtons;
    CCMenu* catMenu;
    CCMenu* modMenu;
};