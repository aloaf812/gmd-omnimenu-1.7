// massive thanks to adelfa for helping me figure out this touch dispatcher bullshit

#include "../layers/HaxOverlay.hpp"
#include "FLAlertLayer.hpp"
#include "HaxManager.hpp"
#include "CCMenuItemToggler.hpp"
#include "CCMenuItemSpriteExtra.hpp"
#include "Utils.hpp"
#include "GameSoundManager.hpp"

using namespace cocos2d;

HaxOverlay* HaxOverlay::create(CCLayer* referrer) {
    auto ret = new HaxOverlay();
    if (ret->init(referrer)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

// CCScene* HaxOverlay::scene() {
//     auto layer = HaxOverlay::create();
//     auto scene = CCScene::create();
//     scene->addChild(layer);
//     return scene;
// }

float HaxOverlay::getDuration() {
    auto& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled(ModuleID::FAST_MENU)) return 0.f;
    return 0.25f;
}

ccColor3B color = ccc3(127, 255, 255);

bool HaxOverlay::init(CCLayer* referrer) {
    if (!CCLayerColor::initWithColor(ccc4(0, 0, 0, 180)))
        return false;

    auto& hax = HaxManager::sharedState();

    this->referrer = referrer;
    CCDirector* director = CCDirector::sharedDirector();
    CCSize winSize = director->getWinSize();

    CCTouchDispatcher* touchDispatch = director->getTouchDispatcher();
    touchDispatch->setForcePrio(true);
    touchDispatch->setTargetPrio(0x80000002);

    CCNode* parent = CCNode::create();
    addChild(parent);
    parent->setPosition(winSize.width, 0);
    this->mainParent = parent;
    
    CCNode* categoryParent = CCNode::create();
    parent->addChild(categoryParent, 1003);
    this->categoryParent = categoryParent;

    auto categoryMenu = CCMenu::create();
    parent->addChild(categoryMenu, 1003);
    // parentMenu->setPosition(ccp(winSize.width / 2, winSize.height / 2));
    this->categoryMenu = categoryMenu;

    CCSprite* menuBackground = CCSprite::create("menubackground2.png");
    parent->addChild(menuBackground, 1000);
    menuBackground->setPosition(ccp(winSize.width / 2, winSize.height / 2));

    auto omniLabel = CCLabelTTF::create(" OMNI ", "Helvetica-Oblique.ttf", scaleFontSize(14));
    parent->addChild(omniLabel, 1001);
    omniLabel->setAnchorPoint({0.5, 0});
    omniLabel->setPosition(ccp(winSize.width / 2 - 21, winSize.height / 2 + 90));

    auto menuLabel = CCLabelTTF::create(" menu ", "Helvetica-Oblique.ttf", scaleFontSize(10));
    parent->addChild(menuLabel, 1001);
    menuLabel->setAnchorPoint({0.5, 0});
    menuLabel->setPosition(ccp(winSize.width / 2 + 21, winSize.height / 2 + 92));

    auto btnMenu = CCMenu::create();
    auto closeSpr = CCSprite::create("closebtn.png");
    closeSpr->setScale(0.75f);
    auto closeBtn = CCMenuItemSpriteExtra::create(closeSpr, closeSpr, this, menu_selector(HaxOverlay::onClose));
    closeBtn->setPosition(ccp(winSize.width / 2 - 300 + 185, winSize.height / 2 - 225 + 185));
    btnMenu->addChild(closeBtn);
    parent->addChild(btnMenu, 1002);
    btnMenu->setPosition(ccp(winSize.width / 2, winSize.height / 2));
    this->btnMenu = btnMenu;

    this->buttons = CCArray::create();
    this->labels = CCArray::create();
    this->buttons->retain();
    this->labels->retain();

    addButton(" Gameplay ", 12, 80, this, menu_selector(HaxOverlay::onGameplay));
    addButton(" Editor ", 12, 40, this, menu_selector(HaxOverlay::onEditor));
    addButton(" Bypass ", 12, 0, this, menu_selector(HaxOverlay::onBypass));
    addButton(" Informational ", 9.5, -40, this, menu_selector(HaxOverlay::onInformational));
    addButton(" Universal ", 12, -80, this, menu_selector(HaxOverlay::onUniversal));

    addButtonRight(" Particles ", 12, 80, this, menu_selector(HaxOverlay::onParticles));

    setTouchEnabled(true);
    setKeypadEnabled(true);
    referrer->setTouchEnabled(false);

    parent->runAction(CCEaseOut::create(
        CCMoveTo::create(getDuration(), ccp(0.0f, 0.0f)), 3
    ));

    mainParent = parent;

    onCategory(hax.lastCategory);

    return true;
}

void HaxOverlay::addButton(const char* label, float fontSize, float yOffset, CCObject* target, SEL_MenuHandler selector) {
    CCDirector* director = CCDirector::sharedDirector();
    CCSize winSize = director->getWinSize();

    auto btn1Sprite = CCSprite::create("menubtn.png");
    auto btn1 = CCMenuItemSprite::create(btn1Sprite, btn1Sprite, btn1Sprite, target, selector);
    this->buttons->addObject(btn1);
    btnMenu->addChild(btn1, 999);
    btn1->setAnchorPoint({0, 0.5});
    btn1->setPosition(ccp(-105-189, yOffset));

    auto btn1Label = CCLabelTTF::create(label, "Helvetica-Oblique.ttf", scaleFontSize(fontSize));
    this->labels->addObject(btn1Label);
    mainParent->addChild(btn1Label, 1003);
    btn1Label->setAnchorPoint({0, 0.5});
    btn1Label->setPosition(ccp(winSize.width / 2 - 90 - 189, winSize.height / 2 + yOffset));
}
void HaxOverlay::addButtonRight(const char* label, float fontSize, float yOffset, CCObject* target, SEL_MenuHandler selector) {
    CCDirector* director = CCDirector::sharedDirector();
    CCSize winSize = director->getWinSize();

    auto btn1Sprite = CCSprite::create("menubtn.png");
    auto btn1 = CCMenuItemSprite::create(btn1Sprite, btn1Sprite, btn1Sprite, target, selector);
    this->buttons->addObject(btn1);
    btnMenu->addChild(btn1, 999);
    btn1->setAnchorPoint({0, 0.5});
    btn1->setScaleX(-1);
    btn1->setPosition(ccp(105+189, yOffset));

    auto btn1Label = CCLabelTTF::create(label, "Helvetica-Oblique.ttf", scaleFontSize(fontSize));
    this->labels->addObject(btn1Label);
    mainParent->addChild(btn1Label, 1003);
    btn1Label->setAnchorPoint({1, 0.5});
    btn1Label->setPosition(ccp(winSize.width / 2 + 90 + 189, winSize.height / 2 + yOffset));
}

void HaxOverlay::setColorAtIndex(int index) {
    static_cast<CCMenuItemSprite*>(this->buttons->objectAtIndex(index))->setColor(color);
    static_cast<CCLabelTTF*>(this->labels->objectAtIndex(index))->setColor(color);
    for (int i = 0; i < this->buttons->count(); i++) {
        if (i == index) continue;
        static_cast<CCMenuItemSprite*>(this->buttons->objectAtIndex(i))->setColor(ccWHITE);
        static_cast<CCLabelTTF*>(this->labels->objectAtIndex(i))->setColor(ccWHITE);
    }
}
void HaxOverlay::onGameplay() {
    onCategory(ModuleCategory::Gameplay);
}
void HaxOverlay::onEditor() {
    onCategory(ModuleCategory::Editor);
}
void HaxOverlay::onBypass() {
    onCategory(ModuleCategory::Bypass);
}
void HaxOverlay::onInformational() {
    onCategory(ModuleCategory::Informational);
}
void HaxOverlay::onUniversal() {
    onCategory(ModuleCategory::Universal);
}
void HaxOverlay::onParticles() {
    onCategory(ModuleCategory::Particles);
}

void HaxOverlay::onCategory(ModuleCategory category) {
    CCDirector* director = CCDirector::sharedDirector();
    CCSize winSize = director->getWinSize();

    setColorAtIndex(static_cast<int>(category));

    while (categoryParent->getChildrenCount() > 0) {
        CCNode* node = static_cast<CCNode*>(categoryParent->getChildren()->objectAtIndex(0));
        // node->removeFromParentAndCleanup(true);
        categoryParent->removeChild(node, true);
    }
    while (categoryMenu->getChildrenCount() > 0) {
        CCNode* node = static_cast<CCNode*>(categoryMenu->getChildren()->objectAtIndex(0));
        // node->removeFromParentAndCleanup(true);
        std::string* userData = static_cast<std::string*>(node->getUserData());
        delete userData;
        userData = nullptr;
        categoryMenu->removeChild(node, true);
    }

    auto& hax = HaxManager::sharedState();
    hax.lastCategory = category;
    std::map<std::string, Module*>::iterator it;

    int y = 80;
    int x = 0;

    for (it = hax.modules.begin(); it != hax.modules.end(); it++)
    {
        if (y < -100) {
            y = 80;
            x += 155;
        }
        std::string* key = new std::string; // heap allocation
        *key = it->first;
        Module* mod = it->second;
        if (mod->category != category) continue;

        auto toggleOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        auto toggleOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

        auto checkbox = CCMenuItemToggler::create(toggleOn, toggleOff, this, menu_selector(HaxOverlay::toggler));
        checkbox->toggle(!mod->enabled);
        checkbox->setPosition(ccp(-159 + x, y));
        checkbox->setScale(0.6f);
        checkbox->setUserData(key);
        categoryMenu->addChild(checkbox, 1003);

        std::string labelValue = mod->name;
        labelValue += " "; // italics font gets cut off grrrr
        auto label = CCLabelTTF::create(labelValue.c_str(), "Helvetica-Oblique.ttf", scaleFontSize(10));
        categoryParent->addChild(label, 1003);
        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition(ccp(winSize.width / 2 - 145 + x, winSize.height / 2 + y));
        y -= 20;
    }
}

void HaxOverlay::toggler(CCObject* sender) {
    CCMenuItem* menuItem = (CCMenuItem *)(sender);
    std::string* userData = static_cast<std::string*>(menuItem->getUserData());
    auto& hax = HaxManager::sharedState();
    hax.getModule(userData->c_str())->toggle();
    if (!strcmp(userData->c_str(), "ping_spoofing")) {
        GameSoundManager::sharedManager()->playUniqueEffect("pih.mp3");
        hax.getModule(userData->c_str())->toggle();
        static_cast<CCMenuItemToggler*>(sender)->toggle(true);
        this->runAction(CCSequence::create(
            CCDelayTime::create(0.2f),
            CCCallFunc::create(this, callfunc_selector(HaxOverlay::onPih)),
            nullptr
        ));
    }
}
void HaxOverlay::onPih(CCObject* sender) {
    CCDirector* director = CCDirector::sharedDirector();
    CCSize winSize = director->getWinSize();
    CCSprite* pih = CCSprite::create("pih.png");
    addChild(pih, 1010);
    pih->setPosition(ccp(winSize.width / 2, winSize.height / 2));
    pih->setScale(2.f);
    pih->runAction(CCSequence::create(
        CCFadeOut::create(1.0f),
        CCCallFunc::create(pih, callfunc_selector(CCNode::removeFromParentAndCleanup)),
        nullptr
    ));
}

void HaxOverlay::keyBackClicked() {
    onClose(nullptr);
}

void HaxOverlay::onClose(CCObject* sender) {
    auto& hax = HaxManager::sharedState();
    hax.saveSettingsToFile();

    CCDirector* director = CCDirector::sharedDirector();
    CCSize winSize = director->getWinSize();
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    //referrer->setTouchEnabled(true);
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);

    mainParent->runAction(CCEaseIn::create(
        CCMoveTo::create(getDuration(), ccp(winSize.width, 0.0f)), 3
    ));
    if (sender) this->retain();
    this->runAction(CCSequence::create(
        CCDelayTime::create(getDuration()),
        CCCallFunc::create(this, callfunc_selector(CCNode::removeFromParentAndCleanup)), // CCRemoveSelf does not exist in old cocos
        nullptr
    ));
}

bool HaxOverlay::ccTouchBegan(cocos2d::CCTouch* t, cocos2d::CCEvent*)
{ 
    return true;
}

// from starry sky. never in a million years would i have figured ts out without it
void HaxOverlay::registerWithTouchDispatcher()
{
    CCTouchDispatcher* t = CCDirector::sharedDirector()->getTouchDispatcher();   
    t->addTargetedDelegate(this, 0x80000003, true);
}