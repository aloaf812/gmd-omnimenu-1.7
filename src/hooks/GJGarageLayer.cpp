#include "hook.hpp"
#include "GJGameLevel.hpp"
#include "GameStatsManager.hpp"

bool (*TRAM_GJGarageLayer_init)(CCLayer* self);
bool GJGarageLayer_init(CCLayer* self) {
    if (!TRAM_GJGarageLayer_init(self)) return false;
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("demons_in_garage")) {
        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        auto sprite = CCSprite::createWithSpriteFrameName("GJ_demonIcon_001.png");
        sprite->setPosition(ccp(winSize.width - 40, winSize.height - 65));
        sprite->setScale(0.8f);
        auto label = CCLabelBMFont::create(CCString::createWithFormat("%i", GameStatsManager::sharedState()->getStat("5"))->getCString(), "bigFont.fnt");
        label->setScale(0.5f);
        label->setAnchorPoint({1, 0.5});
        label->setPosition(ccp(winSize.width - 58, winSize.height - 65));
        self->addChild(sprite);
        self->addChild(label);
    }
    return true;
}

void GJGarageLayer_om() {
    Omni::hook("_ZN13GJGarageLayer4initEv",
        reinterpret_cast<void*>(GJGarageLayer_init),
        reinterpret_cast<void**>(&TRAM_GJGarageLayer_init));
}