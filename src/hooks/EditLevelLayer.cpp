#include "hook.hpp"
#include "EditLevelLayer.hpp"
#include <algorithm>

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
bool (*TRAM_EditLevelLayer_init)(EditLevelLayer* self, GJGameLevel* level);
bool EditLevelLayer_init(EditLevelLayer* self, GJGameLevel* level) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("verify_bypass")) {
        setLevelVerified(level, true);
    }
    if (hax.getModuleEnabled("view_level_stats")) {
        CCMenu* infoMenu = CCMenu::create();
        CCSprite* infoSpr = cocos2d::CCSprite::create("GJ_infoIcon.png");
        CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoSpr, infoSpr, self, menu_selector(EditLevelLayer::onViewLevelInfo));

        infoBtn->setSizeMult(1.5f);

        self->addChild(infoMenu, 1001);
        infoMenu->addChild(infoBtn);
        infoMenu->setPosition(ccp(25.f, 25.f));
    }
    return TRAM_EditLevelLayer_init(self, level);
}
void EditLevelLayer_om() {
    Omni::hook("_ZN14EditLevelLayer4initEP11GJGameLevel",
        reinterpret_cast<void*>(EditLevelLayer_init),
        reinterpret_cast<void**>(&TRAM_EditLevelLayer_init));
}