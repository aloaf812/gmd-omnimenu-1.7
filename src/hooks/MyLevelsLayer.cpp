#include "hook.hpp"
#include "GJGameLevel.hpp"
#include "MyLevelsLayer.hpp"
#include "EditLevelLayer.hpp"

void MyLevelsLayer::onImport() {
    JNIEnv* env = getEnv();
    if (!env || env == nullptr) {
        GDSHARE_FL("Error: could not get JNI Environment");
        return;
    }
    HaxManager& hax = HaxManager::sharedState();
    jclass activityClass = env->GetObjectClass(hax.activity);
    if (activityClass == nullptr) {
        GDSHARE_FL("Error: could not get the activity class");
        return;
    }
    jmethodID showPicker = env->GetMethodID(activityClass, "showOpenFilePicker", "()V");
    if (showPicker == nullptr) {
        GDSHARE_FL("Error: could not find showOpenFilePicker method. Are you sure you changed the smali?");
        return;
    }
    env->CallVoidMethod(hax.activity, showPicker);
}

void MyLevelsLayer::loadLevel(GJGameLevel* level) {
    if (level == nullptr) {
        GDSHARE_FL("Error: could not load level");
        return;
    }
    setLevelID(level, 0);
    setLevelType(level, GJLevelType::Editor);
    getLocalLevels()->addObject(level);
    // this delay is necessary. TODO come up with a better solution?
    runAction(CCSequence::create(
        CCDelayTime::create(0.2f),
        CCCallFunc::create(this, callfunc_selector(MyLevelsLayer::reload)),
        nullptr
    ));
    
}
void MyLevelsLayer::reload() {
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, MyLevelsLayer::scene()));
}

bool (*TRAM_MyLevelsLayer_init)(MyLevelsLayer* self);
bool MyLevelsLayer_init(MyLevelsLayer* self) {
    if (!TRAM_MyLevelsLayer_init(self)) return false;
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("gdshare")) {
        hax.myLevelsLayer = self;
        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        CCMenu* importMenu = CCMenu::create();
        CCSprite* importSpr = cocos2d::CCSprite::create("gdshare_import.png");
        CCMenuItemSpriteExtra* importBtn = CCMenuItemSpriteExtra::create(importSpr, importSpr, self, menu_selector(MyLevelsLayer::onImport));

        self->addChild(importMenu, 1000);
        importMenu->addChild(importBtn);
        importMenu->setPosition(ccp(winSize.width - 30.f, winSize.height - 90.f));
    }
    return true;
}

void MyLevelsLayer_om() {
    Omni::hook("_ZN13MyLevelsLayer4initEv",
        reinterpret_cast<void*>(MyLevelsLayer_init),
        reinterpret_cast<void**>(&TRAM_MyLevelsLayer_init));
}