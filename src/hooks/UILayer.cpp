#include "hook.hpp"
#include "UILayer.hpp"
#include "CCMenuItemSpriteExtra.hpp"
#include "ButtonSprite.hpp"
#include "PlayerObject.hpp"
#include <string>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

template <typename T>
std::string ToString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

void UILayer::createLabel() {
    HaxManager& hax = HaxManager::sharedState();
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    auto uiLabel = CCLabelBMFont::create("", "chatFont.fnt");
    uiLabel->setAnchorPoint({0, 1});
    uiLabel->setPosition(ccp(10, winSize.height - 23));
    uiLabel->setOpacity(127);
    uiLabel->setScale(0.5f);
    addChild(uiLabel);
    hax.uiLabel = uiLabel;
    updateLabel(1);
}
void UILayer::updateLabel(float dt) {
    HaxManager& hax = HaxManager::sharedState();
    std::string labelText = "";
    if (hax.getModuleEnabled("label_clock")) {
        const std::time_t clock = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        char buff[20];
        time_t now = time(NULL);
        tm* mytm = localtime(&now);
        std::stringstream strm;
        strm << std::setfill('0') << std::setw(2);
        strm << mytm->tm_hour;
        strm << ':';
        strm << std::setfill('0') << std::setw(2);
        strm << mytm->tm_min;
        strm << ':';
        strm << std::setfill('0') << std::setw(2);
        strm << mytm->tm_sec;
        labelText += strm.str();
        labelText += "\n";
    }
    if (hax.getModuleEnabled("label_fps")) {
        labelText += "FPS: ";
        labelText += ToString(hax.fps);
        labelText += "\n";
    }
    if (hax.getModuleEnabled("label_attempts_session")) {
        labelText += "Attempts: ";
        labelText += ToString(getCurrentAttempts());
        labelText += "\n";
    }
    if (hax.getModuleEnabled("label_attempts_total")) {
        labelText += "Total Attempts: ";
        labelText += ToString(getLevelAttempts(getPlayLayerLevel()));
        labelText += "\n";
    }
    if (hax.getModuleEnabled("label_best_run")) {
        labelText += "Best Run: ";
        if (hax.getModuleEnabled("show_percentage_decimal")) {
            labelText += ToString(hax.bestRun);
        } else {
            labelText += ToString(floorf(hax.bestRun));
        }
        labelText += "%\n";
    }
    if (hax.getModuleEnabled("label_jumps")) {
        labelText += "Jumps: ";
        labelText += ToString(getCurrentJumps());
        labelText += "\n";
    }
    if (hax.getModuleEnabled("label_clicks")) {
        labelText += "Clicks: ";
        labelText += ToString(hax.clicks);
        labelText += "\n";
    }
    if (hax.getModuleEnabled("label_time_spent")) {
        labelText += "Time Spent: ";
        labelText += ToString(getClkTimer());
        labelText += "\n";
    }
    if (hax.getModuleEnabled("label_frames")) {
        labelText += "Frames: ";
        labelText += ToString(hax.frameCount);
        labelText += "\n";
    }
    if (hax.getModuleEnabled("label_noclip_deaths") && hax.getModuleEnabled("noclip")) {
        labelText += "NoClip Deaths: ";
        labelText += ToString(hax.deaths);
        labelText += "\n";
    }
    auto player = getPlayer();
    if (hax.getModuleEnabled("label_player_position") && player) {
        labelText += "Player X: ";
        labelText += ToString(getPlayer()->getPositionX());
        labelText += "\nPlayer Y: ";
        labelText += ToString(getPlayer()->getPositionY());
        labelText += "\n";
    }
    if (hax.getModuleEnabled("label_pcommand") && player) {
        labelText += "X Velocity: ";
        labelText += ToString(getXVelocity(getPlayer()));
        labelText += "\nGravity: ";
        labelText += ToString(getGravity(getPlayer()));
        labelText += "\nY Velocity: ";
        labelText += ToString(getYStart(getPlayer()));
        labelText += "\n";
    }
    hax.uiLabel->setString(labelText.c_str());
}

void UILayer::createCheatIndicator() {
    HaxManager& hax = HaxManager::sharedState();
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    auto cheatIndicatorLabel = CCLabelBMFont::create(".", "bigFont.fnt");
    cheatIndicatorLabel->setPosition(ccp(15, winSize.height - 10));
    switch (hax.getCheatIndicatorColor()) {
        case CheatIndicatorColor::Green:
            cheatIndicatorLabel->setColor(ccGREEN);
            break;
        case CheatIndicatorColor::Yellow:
            cheatIndicatorLabel->setColor(ccYELLOW);
            break;
        case CheatIndicatorColor::Orange:
            cheatIndicatorLabel->setColor(ccORANGE);
            break;
        case CheatIndicatorColor::Red:
            cheatIndicatorLabel->setColor(ccRED);
            break;
        default:
            cheatIndicatorLabel->setColor(ccWHITE);
            break;
    };
    hax.cheatIndicatorLabel = cheatIndicatorLabel;
    addChild(cheatIndicatorLabel, 10000);
}

void UILayer::createPercentageLabel() {
    HaxManager& hax = HaxManager::sharedState();
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    auto percentageLabel = CCLabelBMFont::create("0%", "bigFont.fnt");
    percentageLabel->setPosition(ccp(winSize.width / 2, winSize.height - 10));
    percentageLabel->setScale(0.5f);
    hax.percentageLabel = percentageLabel;
    addChild(percentageLabel, 10000);
}

CCMenuItemSpriteExtra* createPCButton(const char* label, float y, SEL_MenuHandler selector, UILayer* self, CCMenu* menu) {
    auto btnSpr1 = ButtonSprite::create(label, 20);
    btnSpr1->setScale(1.0f);

    auto btn1 = CCMenuItemSpriteExtra::create(btnSpr1, btnSpr1, self, selector);
    btn1->setPosition(ccp(0, y));
    menu->addChild(btn1);
    return btn1;
}
void UILayer::createPCommand() {
    HaxManager& hax = HaxManager::sharedState();
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    if (!hax.pMenu || hax.pMenu == nullptr) {
        auto menu = CCMenu::create();
        menu->setPosition(ccp(winSize.width - 30, winSize.height - 40));
        this->addChild(menu, 10000);
        hax.pMenu = menu;
    }
    if (!hax.pButton1 || hax.pButton1 == nullptr) 
        hax.pButton1 = createPCButton("S+", -55, menu_selector(UILayer::speedUp), this, hax.pMenu);
    if (!hax.pButton2 || hax.pButton2 == nullptr) 
        hax.pButton2 = createPCButton("S-", -85, menu_selector(UILayer::speedDown), this, hax.pMenu);
    if (!hax.pButton3 || hax.pButton3 == nullptr) 
        hax.pButton3 = createPCButton("G+", -115, menu_selector(UILayer::gravityUp), this, hax.pMenu);
    if (!hax.pButton4 || hax.pButton4 == nullptr) 
        hax.pButton4 = createPCButton("G-", -145, menu_selector(UILayer::gravityDown), this, hax.pMenu);
    if (!hax.pButton5 || hax.pButton5 == nullptr) 
        hax.pButton5 = createPCButton("Y+", -175, menu_selector(UILayer::yStartUp), this, hax.pMenu);
    if (!hax.pButton6 || hax.pButton6 == nullptr) 
        hax.pButton6 = createPCButton("Y-", -205, menu_selector(UILayer::yStartDown), this, hax.pMenu);
}

void UILayer::speedUp() {
    PlayerObject* player = getPlayer();
    addXVelocity(player, 0.5d);
    HaxManager& hax = HaxManager::sharedState();
    hax.pSpeedModified += 1;
    hax.setCheating(true);
    player->logValues();
}
void UILayer::speedDown() {
    PlayerObject* player = getPlayer();
    addXVelocity(player, -0.5d);
    HaxManager& hax = HaxManager::sharedState();
    hax.pSpeedModified -= 1;
    hax.setCheating(true);
    player->logValues();
}
void UILayer::gravityUp() {
    PlayerObject* player = getPlayer();
    addGravity(player, 0.1d);
    HaxManager& hax = HaxManager::sharedState();
    hax.pGravityModified += 1;
    hax.setCheating(true);
    player->logValues();
}
void UILayer::gravityDown() {
    PlayerObject* player = getPlayer();
    addGravity(player, -0.1d);
    HaxManager& hax = HaxManager::sharedState();
    hax.pGravityModified -= 1;
    hax.setCheating(true);
    player->logValues();
}
void UILayer::yStartUp() {
    PlayerObject* player = getPlayer();
    addYStart(player, 0.5d);
    HaxManager& hax = HaxManager::sharedState();
    hax.pYStartModified += 1;
    hax.setCheating(true);
    player->logValues();
}
void UILayer::yStartDown() {
    PlayerObject* player = getPlayer();
    addYStart(player, -0.5d);
    HaxManager& hax = HaxManager::sharedState();
    hax.pYStartModified -= 1;
    hax.setCheating(true);
    player->logValues();
}
bool (*TRAM_UILayer_init)(UILayer* self);
bool UILayer_init(UILayer* self) {
    if (!TRAM_UILayer_init(self)) return false;
    HaxManager& hax = HaxManager::sharedState();
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    if (hax.getModuleEnabled("cheat_indicator")) {
        self->createCheatIndicator();
    }
    if (hax.getModuleEnabled("show_percentage")) {
        self->createPercentageLabel();
    }
    if (hax.getModuleEnabled("pcommand")) {
        self->createPCommand();
    }
    if (hax.getShowLabel()) {
        self->createLabel();
    }
    return true;
}
void (*TRAM_UILayer_destructor)(UILayer* self);
void UILayer_destructor(UILayer* self) {
    HaxManager& hax = HaxManager::sharedState();
    TRAM_UILayer_destructor(self);
    hax.cheatIndicatorLabel = nullptr;
    hax.percentageLabel = nullptr;
    hax.pButton1 = nullptr;
    hax.pButton2 = nullptr;
    hax.pButton3 = nullptr;
    hax.pButton4 = nullptr;
    hax.pButton5 = nullptr;
    hax.pButton6 = nullptr;
    hax.uiLabel = nullptr;
    hax.pMenu = nullptr;
    hax.pSpeedModified = 0;
    hax.pGravityModified = 0;
    hax.pYStartModified = 0;
    hax.bestRun = 0;
    hax.clicks = 0;
    hax.deaths = 0;
    hax.frameCount = 0;
    hax.lastDeadFrame = -1;
}
void (*TRAM_UILayer_ccTouchBegan)(UILayer* self, CCTouch* touch, CCEvent* event);
void UILayer_ccTouchBegan(UILayer* self, CCTouch* touch, CCEvent* event) {
    TRAM_UILayer_ccTouchBegan(self, touch, event);
    HaxManager& hax = HaxManager::sharedState();
    hax.clicks++;
}


void UILayer_om() {
    Omni::hook("_ZN7UILayer4initEv",
        reinterpret_cast<void*>(UILayer_init),
        reinterpret_cast<void**>(&TRAM_UILayer_init));
    Omni::hook("_ZN7UILayerD0Ev",
        reinterpret_cast<void*>(UILayer_destructor),
        reinterpret_cast<void**>(&TRAM_UILayer_destructor));
    Omni::hook("_ZN7UILayer12ccTouchBeganEPN7cocos2d7CCTouchEPNS0_7CCEventE",
        reinterpret_cast<void*>(UILayer_ccTouchBegan),
        reinterpret_cast<void**>(&TRAM_UILayer_ccTouchBegan));
}