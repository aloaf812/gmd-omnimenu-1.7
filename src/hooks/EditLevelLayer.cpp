#include "hook.hpp"
#include "EditLevelLayer.hpp"
#include <algorithm>
#include "Utils.hpp"

void EditLevelLayer::onViewLevelInfo() {
    GJGameLevel* level = getEditLayerLevel(this);
    std::string s = getLevelString(level);
    std::string::difference_type count = std::count(s.begin(), s.end(), ';');
    int objectCount = std::max(0, static_cast<int>(count) - 1);
    CCString* flAlertInsides = CCString::createWithFormat(
        "<cy>%s</c>\n<cg>Total Attempts</c>: %i\n<cr>Normal</c>: %i%%\n<co>Practice</c>: %i%%\n<cz>Object Count</c>: %i",
        getLevelName(level).c_str(),
        getLevelAttempts(level),
        getLevelNormalPercent(level),
        getLevelPracticePercent(level),
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
void EditLevelLayer::onExport() {
    GJGameLevel* level = getEditLayerLevel(this);
    std::string name = getLevelName(level);
    if (!strcmp(name.c_str(), "")) name = "Unnamed";
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
}
bool (*TRAM_EditLevelLayer_init)(EditLevelLayer* self, GJGameLevel* level);
bool EditLevelLayer_init(EditLevelLayer* self, GJGameLevel* level) {
    HaxManager& hax = HaxManager::sharedState();
#ifndef FORCE_AUTO_SAFE_MODE
    if (hax.getModuleEnabled("verify_bypass")) {
        setLevelVerified(level, true);
    }
#endif
    if (hax.getModuleEnabled("view_level_stats")) {
        CCMenu* infoMenu = CCMenu::create();
        CCSprite* infoSpr = createInfoSprite();
        CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoSpr, infoSpr, self, menu_selector(EditLevelLayer::onViewLevelInfo));

        infoBtn->setSizeMult(1.5f);

        self->addChild(infoMenu, 1001);
        infoMenu->addChild(infoBtn);
        infoMenu->setPosition(ccp(25.f, 25.f));
    }
    if (hax.getModuleEnabled("gdshare")) {
        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        
        CCMenu* shareMenu = CCMenu::create();
        CCSprite* exportSpr = cocos2d::CCSprite::create("gdshare_export.png");
        CCMenuItemSpriteExtra* exportBtn = CCMenuItemSpriteExtra::create(exportSpr, exportSpr, self, menu_selector(EditLevelLayer::onExport));

        self->addChild(shareMenu, 1001);
        shareMenu->addChild(exportBtn);
        shareMenu->setPosition(ccp(winSize.width - 29, winSize.height - 150.f));
    }
    return TRAM_EditLevelLayer_init(self, level);
}
void EditLevelLayer_om() {
    Omni::hook("_ZN14EditLevelLayer4initEP11GJGameLevel",
        reinterpret_cast<void*>(EditLevelLayer_init),
        reinterpret_cast<void**>(&TRAM_EditLevelLayer_init));
}