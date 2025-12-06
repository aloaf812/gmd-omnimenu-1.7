#include "hook.hpp"
#include "GJGameLevel.hpp"
#include "LevelBrowserLayer.hpp"

#if GAME_VERSION >= GV_1_4

void LevelBrowserLayer::onImport() {
    JNIEnv* env = getEnv();
    if (!env || env == nullptr) {
        GDSHARE_FL("Error: could not get JNI Environment");
        return;
    }
    jclass activityClass = env->FindClass(JAVA_PATH_MAIN "/GeometryJump");
    if (activityClass == nullptr) {
        GDSHARE_FL("Error: could not get the activity class");
        return;
    }
    jmethodID showPicker = env->GetMethodID(activityClass, "showOpenFilePicker", "()V");
    if (showPicker == nullptr) {
        GDSHARE_FL("Error: could not find showOpenFilePicker method. Are you sure you changed the smali?");
        return;
    }
    jmethodID getActivity = env->GetStaticMethodID(activityClass, "getInstance", "()L" JAVA_PATH_MAIN "/GeometryJump;");
    if (getActivity == nullptr) {
        GDSHARE_FL("Error: could not find getInstance method. Are you sure you changed the smali?");
        return;
    }
    jobject activity = env->CallStaticObjectMethod(activityClass, getActivity);
    env->CallVoidMethod(activity, showPicker);
}

void LevelBrowserLayer::loadLevel(GJGameLevel* level) {
    if (level == nullptr) {
        GDSHARE_FL("Error: could not load level");
        return;
    }
    level->m_nLevelID = 0;
    level->m_eLevelType = GJLevelType::Editor;
    getLocalLevels()->insertObject(level, 0);
    // this delay is necessary. TODO come up with a better solution?
    runAction(CCSequence::create(
        CCDelayTime::create(0.2f),
        CCCallFunc::create(this, callfunc_selector(LevelBrowserLayer::reload)),
        nullptr
    ));
    
}
void LevelBrowserLayer::reload() {
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, LevelBrowserLayer::scene(getSearchObject(this))));
}

bool (*TRAM_LevelBrowserLayer_init)(LevelBrowserLayer* self, GJSearchObject* searcher);
bool LevelBrowserLayer_init(LevelBrowserLayer* self, GJSearchObject* searcher) {
    if (!TRAM_LevelBrowserLayer_init(self, searcher)) return false;
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("gdshare") && getSearchType(searcher) == 98) {
        hax.levelBrowserLayer = self;
        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        CCMenu* importMenu = CCMenu::create();
        CCSprite* importSpr = cocos2d::CCSprite::create("gdshare_import.png");
        CCMenuItemSpriteExtra* importBtn = CCMenuItemSpriteExtra::create(importSpr, importSpr, self, menu_selector(LevelBrowserLayer::onImport));

        self->addChild(importMenu, 1000);
        importMenu->addChild(importBtn);
        importMenu->setPosition(ccp(winSize.width - 30.f, 90.f));
    }
    return true;
}

void LevelBrowserLayer_om() {
    Omni::hook("_ZN17LevelBrowserLayer4initEP14GJSearchObject",
        reinterpret_cast<void*>(LevelBrowserLayer_init),
        reinterpret_cast<void**>(&TRAM_LevelBrowserLayer_init));
}

#endif