#include "hook.hpp"
#include "LevelInfoLayer.hpp"
#include "GJGameLevel.hpp"
#include "FLAlertLayer.hpp"
#include "CCMenuItemSpriteExtra.hpp"
#include <algorithm>

void LevelInfoLayer::onViewLevelInfo() {
    GJGameLevel* level = getInfoLayerLevel(this);
    std::string s = getLevelString(level);
    std::string::difference_type count = std::count(s.begin(), s.end(), ';');
    int objectCount = std::max(0, static_cast<int>(count) - 1);
    CCString* flAlertInsides = CCString::createWithFormat(
        "<cy>%s</c> by <cy>%s</c>\n<cg>Total Attempts</c>: %i\n<cr>Normal</c>: %i%%\n<co>Practice</c>: %i%%\n<cl>Level ID</c>: %i\n<cb>User ID</c>: %i\n<cz>Object Count</c>: %i",
        getLevelName(level).c_str(),
        getLevelUsername(level).c_str(),
        getLevelAttempts(level),
        getLevelNormalPercent(level),
        getLevelPracticePercent(level),
        getLevelID(level),
        getLevelUserID(level),
        objectCount
    );
    FLAlertLayer::create(
        nullptr,
        "Level Info",
        flAlertInsides->getCString(),
        "OK",
        nullptr,
        300.f
    )->show();
}
void LevelInfoLayer::onExport() {
    GJGameLevel* level = getInfoLayerLevel(this);
    std::string name = getLevelName(level);
    name += ".gmd";
    JNIEnv* env = getEnv();
    if (!env || env == nullptr) {
        GDSHARE_FL("Error: could not get JNI Environment");
        return;
    }
    HaxManager& hax = HaxManager::sharedState();
    hax.gdShareLevel = level;
    jclass activityClass = env->GetObjectClass(hax.activity);
    if (activityClass == nullptr) {
        GDSHARE_FL("Error: could not get the activity class");
        return;
    }
    jmethodID showPicker = env->GetMethodID(activityClass, "showSaveFilePicker", "(Ljava/lang/String;)V");
    if (showPicker == nullptr) {
        GDSHARE_FL("Error: could not find showSaveFilePicker method. Are you sure you changed the smali?");
        return;
    }
    jstring jname = env->NewStringUTF(name.c_str());
    env->CallVoidMethod(hax.activity, showPicker, jname);
    GDSHARE_FL("File exported successfully");
}
bool (*TRAM_LevelInfoLayer_init)(LevelInfoLayer* self, GJGameLevel* level);
bool LevelInfoLayer_init(LevelInfoLayer* self, GJGameLevel* level) {
    if (!TRAM_LevelInfoLayer_init(self, level)) return false;
    HaxManager& hax = HaxManager::sharedState();
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    if (hax.getLeftLILButtons()) {
        CCMenu* leftMenu = CCMenu::create();
        self->addChild(leftMenu, 1000);
        leftMenu->setPosition(ccp(30.f, winSize.height / 2));
        if (hax.getModuleEnabled("level_copy")) {
            CCSprite* cloneSpr = cocos2d::CCSprite::create("GJ_duplicateBtn.png");
            CCMenuItemSpriteExtra* cloneBtn = CCMenuItemSpriteExtra::create(cloneSpr, cloneSpr, self, menu_selector(LevelInfoLayer::onClone));

            leftMenu->addChild(cloneBtn);
            cloneBtn->setPosition(ccp(0.f, 25.f));
        }
        if (hax.getModuleEnabled("gdshare")) {
            CCSprite* exportSpr = cocos2d::CCSprite::create("gdshare_export.png");
            CCMenuItemSpriteExtra* exportBtn = CCMenuItemSpriteExtra::create(exportSpr, exportSpr, self, menu_selector(LevelInfoLayer::onExport));

            leftMenu->addChild(exportBtn);
            exportBtn->setPosition(ccp(0.f, -25.f));
        }
    }
    if (hax.getModuleEnabled("view_level_stats")) {
        CCMenu* infoMenu = CCMenu::create();
        CCSprite* infoSpr = createInfoSprite();
        CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoSpr, infoSpr, self, menu_selector(LevelInfoLayer::onViewLevelInfo));

        infoBtn->setSizeMult(1.5f);

        self->addChild(infoMenu, 1001);
        infoMenu->addChild(infoBtn);
        infoMenu->setPosition(ccp(25.f, 25.f));
    }
#if GAME_VERSION < 4
    if (hax.getModuleEnabled("show_difficulty")) {
        CCSprite* diffSpr = CCSprite::createWithSpriteFrameName(
            CCString::createWithFormat(
                "difficulty_%02d_btn_001.png", 
                level->getAverageDifficulty()
            )->getCString()
        );
        self->addChild(diffSpr);
        diffSpr->setPosition(ccp(winSize.width / 2 - 120, winSize.height / 2 + 30));
    }
#endif
    return true;
}

void LevelInfoLayer_om() {
    Omni::hook("_ZN14LevelInfoLayer4initEP11GJGameLevel",
        reinterpret_cast<void*>(LevelInfoLayer_init),
        reinterpret_cast<void**>(&TRAM_LevelInfoLayer_init));
}