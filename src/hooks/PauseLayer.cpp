#include "hook.hpp"
#include "GJGameLevel.hpp"
#include "PauseLayer.hpp"

void PauseLayer::onOpenMenu() {
    auto haxOverlay = HaxMenu::create(this);
    this->addChild(haxOverlay, 1000);
    this->setTouchEnabled(false);
}

void (*TRAM_PauseLayer_customSetup)(CCLayer* self);
void PauseLayer_customSetup(CCLayer* self) {
    HaxManager& hax = HaxManager::sharedState();
#ifndef FORCE_AUTO_SAFE_MODE
    if (hax.getModuleEnabled(ModuleID::LEVEL_EDIT)) {
        setEditButton(true);
    } else {
        setEditButton(false);
    }
#endif
    if (hax.getModuleEnabled(ModuleID::SHOW_RESTART_BUTTON)) {
        setRestartButton(true);
    } else {
        setRestartButton(false);
    }
    TRAM_PauseLayer_customSetup(self);
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();

    CCMenu* btnMenu = CCMenu::create();
    self->addChild(btnMenu, 999);
    btnMenu->setPosition(ccp(0, winSize.height));

    CCSprite* menuSpr = CCSprite::create("OMNImenu_btn.png");
    CCMenuItemSpriteExtra* menuBtn = CCMenuItemSpriteExtra::create(menuSpr, menuSpr, self, menu_selector(PauseLayer::onOpenMenu));
    btnMenu->addChild(menuBtn, 999);
    menuBtn->setPosition(ccp(50.f, -50.f));
}

void PauseLayer_om() {
    Omni::hook("_ZN10PauseLayer11customSetupEv",
        reinterpret_cast<void*>(PauseLayer_customSetup),
        reinterpret_cast<void**>(&TRAM_PauseLayer_customSetup));
}