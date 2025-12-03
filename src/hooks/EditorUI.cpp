#include "hook.hpp"
#include "EditorUI.hpp"
#include "FLAlertLayer.hpp"
#include "LevelEditorLayer.hpp"
#include "CCMenuItemSpriteExtra.hpp"
#include "GameObject.hpp"
#include "DrawGridLayer.hpp"
#include "UndoObject.hpp"
#include "ButtonSprite.hpp"
#include "EditButtonBar.hpp"

void (*TRAM_EditorUI_showMaxError)(void* self);
void EditorUI_showMaxError(void* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("object_hack")) {
        FLAlertLayer::create(
            nullptr,
            "Max Objects",
            CCString::createWithFormat("You cannot create more than <cy>%i</c> <cg>objects</c> in a single level.", INCREASED_OBJECT_LIMIT)->getCString(),
            "OK",
            nullptr,
            300.f
        )->show();
        return;
    }
    TRAM_EditorUI_showMaxError(self);
}
void (*TRAM_EditorUI_constrainGameLayerPosition)(void* self);
void EditorUI_constrainGameLayerPosition(void* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("free_scroll")) return;
    TRAM_EditorUI_constrainGameLayerPosition(self);
}

void (*TRAM_EditorUI_zoomOut)(EditorUI* self);
void EditorUI_zoomOut(EditorUI* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("zoom_bypass")) {
        cocos2d::CCLayer* gameLayer = getEditorGameLayer(getUIEditorLayer(self));
        if (gameLayer->getScale() > 0.1f) TRAM_EditorUI_zoomOut(self);
    } else {
        TRAM_EditorUI_zoomOut(self);
    }
}

#if GAME_VERSION < GV_1_5
const auto cyan = ccc3(0, 255, 255);
GameObject* EditorUI::duplicateObject(GameObject* obj, void* getSaveString) {
    LevelEditorLayer* editLayer = getUIEditorLayer(this);
    DrawGridLayer* gridLayer = getGridLayer(editLayer);
    CCSpriteBatchNode* batchNode = getEditorBatchNode(editLayer);

    std::string saveString = ((std::string(*)(GameObject*))getSaveString)(obj);
    GameObject* newObj = GameObject::objectFromString(saveString);

    obj->setColor(ccWHITE);
    editLayer->addToSection(newObj);
    if (getObjectType(newObj) == 7 && getShouldSpawn(newObj)) { // trigger effect line
        gridLayer->addToEffects(newObj);
    }
    newObj->setColor(cyan);

    batchNode->addChild(newObj);
    UndoObject* undo = UndoObject::create(newObj, UndoCommand::Placement);
    editLayer->addToUndoList(undo);

    return newObj;
}
void EditorUI::onDuplicate() {
    GameObject* selObj = getSelectedObject(this);
    CCArray* selectedObjects = getSelectedObjects(this);
        
    if (selObj == nullptr && selectedObjects->count() < 0) return;

    void* getSaveString = DobbySymbolResolver(MAIN_LIBRARY, "_ZN10GameObject13getSaveStringEv");
    LevelEditorLayer* editLayer = getUIEditorLayer(this);
    getRedoArray(editLayer)->removeAllObjects();

    if (selObj != nullptr) {
        auto newObj = duplicateObject(selObj, getSaveString);
        this->selectObject(newObj);
        newObj->setColor(cyan);
    } else {
        for (int i = 0; i < selectedObjects->count(); i++) {
            auto currObj = static_cast<GameObject*>(selectedObjects->objectAtIndex(i));
            auto newObj = duplicateObject(currObj, getSaveString);
            selectedObjects->replaceObjectAtIndex(i, newObj, false);
        }
    }
}

bool (*TRAM_EditorUI_init)(EditorUI* self, LevelEditorLayer* lel);
bool EditorUI_init(EditorUI* self, LevelEditorLayer* lel) {
    if (!TRAM_EditorUI_init(self, lel)) return false;
    HaxManager& hax = HaxManager::sharedState();
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    if (hax.getModuleEnabled("delete_selected")) {
        CCMenu* delMenu = CCMenu::create();
        self->addChild(delMenu);

        delMenu->setPosition(ccp(125, winSize.height - 20));
        CCSprite* delSelSpr = CCSprite::create("GJ_trashBtn.png");
        CCMenuItemSpriteExtra* delSelBtn = CCMenuItemSpriteExtra::create(delSelSpr, delSelSpr, self, menu_selector(EditorUI::onDeleteSelected));

        delMenu->addChild(delSelBtn);
    }
    if (hax.getModuleEnabled("copy_paste")) {

        CCMenu* btnMenu = CCMenu::create();
        self->addChild(btnMenu);

#if GAME_VERSION == GV_1_4
        auto y = winSize.height - 120;
#else
        auto y = winSize.height - 70;
#endif
        btnMenu->setPosition(ccp(winSize.width - 27, y));
        CCSprite* copySpr = CCSprite::create("GJ_copyBtn.png");
        CCMenuItemSpriteExtra* copyBtn = CCMenuItemSpriteExtra::create(copySpr, copySpr, self, menu_selector(EditorUI::onDuplicate));

        btnMenu->addChild(copyBtn);
    }
    return true;
}

void EditorUI::onDeleteSelected() {
    GameObject* selObj = getSelectedObject(this);
    CCArray* selectedObjects = getSelectedObjects(this);
    LevelEditorLayer* editLayer = getUIEditorLayer(this);
    if (selObj == nullptr && selectedObjects->count() < 0) return;

    if (selObj != nullptr) {
        editLayer->removeObject(selObj);
    } else {
        for (int i = 0; i < selectedObjects->count(); i++) {
            auto currObj = static_cast<GameObject*>(selectedObjects->objectAtIndex(i));
            editLayer->removeObject(currObj);
        }
        this->deselectAll();
    }
}
#endif // GAME_VERSION < GV_1_5

#if GAME_VERSION < GV_1_6
    void (*TRAM_EditorUI_setupDeleteMenu)(EditorUI* self);
    void EditorUI_setupDeleteMenu(EditorUI* self) {
        TRAM_EditorUI_setupDeleteMenu(self);
        HaxManager& hax = HaxManager::sharedState();
        if (hax.getModuleEnabled("delete_start_pos")) {
            CCMenu* menu = getEditorUIButtonMenu(self);

            if (!menu || menu == nullptr) { // useless failsafe but i don't like removing those
                TRAM_EditorUI_setupDeleteMenu(self);
                menu = getEditorUIButtonMenu(self);
            }

            auto delSPSpr = ButtonSprite::create("Delete Start Pos", 85, 0, 0.6, false, "bigFont.fnt", "GJ_button_04.png");
            // delSelSpr->setScale(0.8f);

            auto delSPBtn = CCMenuItemSpriteExtra::create(delSPSpr, delSPSpr, self, menu_selector(EditorUI::onDeleteStartPos));
            menu->addChild(delSPBtn);
        }
    }
    void EditorUI::onDeleteStartPos() {
        LevelEditorLayer* editLayer = getUIEditorLayer(this);
        CCArray* sections = getEditorSections(editLayer);
        for (int i = 0; i < sections->count(); i++) {
            auto section = static_cast<CCArray*>(sections->objectAtIndex(i));
            for (int j = 0; j < section->count(); j++) {
                auto currObj = static_cast<GameObject*>(section->objectAtIndex(j));
                CCLog("sidx: %i, idx: %i, id: %i", i, j, getObjectKey(currObj));
                if (getObjectKey(currObj) == 31) {// start position object ID
                    editLayer->removeObject(currObj);
                    j--;
                }
            }
        }
    }
#endif

void (*TRAM_EditorUI_setupCreateMenu)(EditorUI* self);
void EditorUI_setupCreateMenu(EditorUI* self) {
    TRAM_EditorUI_setupCreateMenu(self);
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("unlisted_objects")) {
        CCArray* createBtns = getCreateButtons(self);
        CCArray* fuckingArray = CCArray::create();
        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();

        for (int i = 0; i < createBtns->count(); i++) {
            CCObject* btn = createBtns->objectAtIndex(i);
            static_cast<CCNode*>(btn)->removeFromParentAndCleanup(false);
            fuckingArray->addObject(btn);
        }
        CCNode* separator = CCNode::create();
        separator->setTag(0);

        EditButtonBar* bar = getCreateButtonBar(self);
        bar->removeFromParentAndCleanup(false);
        bar->release();

        CCNode* sep1 = CCNode::create();
        sep1->setTag(0);
#if GAME_VERSION > GV_1_2
        CCNode* sep2 = CCNode::create();
        sep2->setTag(0);
#endif
#if GAME_VERSION == GV_1_0
        fuckingArray->insertObject(sep1, 25);
#elif GAME_VERSION == GV_1_1
        fuckingArray->insertObject(sep1, 27);
#elif GAME_VERSION == GV_1_2
        fuckingArray->insertObject(sep1, 28);
#elif GAME_VERSION == GV_1_3
        fuckingArray->insertObject(sep1, 51);
        fuckingArray->insertObject(sep2, 63);
#elif GAME_VERSION == GV_1_4
        fuckingArray->insertObject(sep1, 74);
        fuckingArray->insertObject(sep2, 86);
#endif

        CCNode* unlistedSeparator = CCNode::create();
        unlistedSeparator->setTag(0);
        fuckingArray->addObject(unlistedSeparator);
#if GAME_VERSION < GV_1_3
        fuckingArray->addObject(self->getCreateBtn("edit_eeSDBtn_001.png", 4)); // a transition trigger that only got listed in 1.3
#endif
        fuckingArray->addObject(self->getCreateBtn("edit_eLevelEndBtn_001.png", 4)); // level end
        fuckingArray->addObject(self->getCreateBtn("edit_eBGEOn_001.png", 4)); // bg effect on
        fuckingArray->addObject(self->getCreateBtn("edit_eBGEOff_001.png", 4)); // bg effect off
#if GAME_VERSION == GV_1_0 // 1.02 ONLY!
        fuckingArray->addObject(self->getCreateBtn("portal_05_front_001.png", 4)); // orange mirror portal
        fuckingArray->addObject(self->getCreateBtn("portal_06_front_001.png", 4)); // blue mirror portal
#elif GAME_VERSION == GV_1_1 // 1.11 ONLY!
        fuckingArray->addObject(self->getCreateBtn("portal_07_front_001.png", 4)); // ball portal
#elif GAME_VERSION >= GV_1_3
        fuckingArray->addObject(self->getCreateBtn("edit_eeFABtn_001.png", 4)); // scatter transition trigger
    #if GAME_VERSION < GV_1_6 // they are removed in 1.6 and reintroduced in 1.8
        fuckingArray->addObject(self->getCreateBtn("square_b_02_001.png", 4)); // wavy slab outer corner thing
        fuckingArray->addObject(self->getCreateBtn("square_b_03_001.png", 4)); // wavy slab inner corner thing
    #endif
    #if GAME_VERSION == GV_1_3
        fuckingArray->addObject(self->getCreateBtn("square_d_01_001.png", 4)); // x blocks
        fuckingArray->addObject(self->getCreateBtn("square_d_02_001.png", 4));
        fuckingArray->addObject(self->getCreateBtn("square_d_03_001.png", 4));
        fuckingArray->addObject(self->getCreateBtn("square_d_04_001.png", 4)); // this one will crash on 1.4+
        fuckingArray->addObject(self->getCreateBtn("square_d_05_001.png", 4));
        fuckingArray->addObject(self->getCreateBtn("square_d_06_001.png", 4));
        fuckingArray->addObject(self->getCreateBtn("square_d_07_001.png", 4));
    #elif GAME_VERSION == GV_1_4
        fuckingArray->addObject(self->getCreateBtn("edit_eTintObjBtn_001.png", 4)); // obj trigger
    #endif
#endif

        EditButtonBar* newBar = EditButtonBar::create(fuckingArray, ccp(winSize.width * 0.5 - 5, getScreenBottom() + getUnkFloat(self) - 6.f));
        setCreateButtonBar(self, newBar);

        self->addChild(newBar, 11);
        self->updateCreateMenu();
    }
}
// credit to akqanile/adelfa
CCMenuItemSpriteExtra* EditorUI::getSpriteButton2(const char* name, SEL_MenuHandler callback, CCMenu* menu, float scale)
{
    auto spr = CCSprite::create(name);
    auto btnSpr = ButtonSprite::create(spr, 32, 0, 32, 1.0, true, "GJ_button_01.png");
    auto btn = CCMenuItemSpriteExtra::create(btnSpr, 0, this, callback);
    btn->setScale(scale);
    setOriginalScale(btn, scale);

    if (menu) menu->addChild(btn);

    return btn;
}
CCMenuItemSpriteExtra* EditorUI::getSpriteButton3(const char* name, SEL_MenuHandler callback, CCMenu* menu, float scale, float sprScale)
{
    auto spr = CCSprite::createWithSpriteFrameName(name);
    spr->setScale(sprScale);
    auto btnSpr = ButtonSprite::create(spr, 32, 0, 32, 1.0, true, "GJ_button_01.png");
    auto btn = CCMenuItemSpriteExtra::create(btnSpr, 0, this, callback);
    btn->setScale(scale);
    setOriginalScale(btn, scale);

    if (menu) menu->addChild(btn);

    return btn;
}
void EditorUI::moveObjectCall2(CCNode* sender) {
    if (!getSelectedObject(this) && getSelectedObjects(this)->count() <= 0) return;

    auto transform = ccp(0, 0);

    switch (sender->getTag()) {

        case 100001:
            transform.y = 150;
            break;
        case 100002:
            transform.y = -150;
            break;
        case 100003:
            transform.x = -150;
            break;
        case 100004:
            transform.x = 150;
            break;

        case 100005:
            transform.y = 1;
            break;
        case 100006:
            transform.y = -1;
            break;
        case 100007:
            transform.x = -1;
            break;
        case 100008:
            transform.x = 1;
            break;

        case 100009:
            transform.y = 15;
            break;
        case 100010:
            transform.y = -15;
            break;
        case 100011:
            transform.x = -15;
            break;
        case 100012:
            transform.x = 15;
            break;

        default:
            break;
    }
    if (getSelectedObjects(this)->count() > 0) {
        for (int i = 0; i < getSelectedObjects(this)->count(); i++) {
            this->moveObject(static_cast<GameObject*>(getSelectedObjects(this)->objectAtIndex(i)), transform);
        }
    } else {
        this->moveObject(getSelectedObject(this), transform);
    }
}
void EditorUI::transformObjectCall2(CCNode* sender) {
    if (!getSelectedObject(this) && getSelectedObjects(this)->count() <= 0) return;
    auto selectedObjects = getSelectedObjects(this);

    int tag = sender->getTag();
    float rot = 0;

    switch (tag) {

        case 100013:
            rot = -45;
            break;
        case 100014:
            rot = 45;
            break;
        case 100015:
            rot = -22.5;
            break;
        case 100016:
            rot = 22.5;
            break;
        case 100017:
            rot = -11.25;
            break;
        case 100018:
            rot = 11.25;
            break;
        case 100019:
            rot = -5;
            break;
        case 100020:
            rot = 5;
            break;
        case 100021:
            rot = -1;
            break;
        case 100022:
            rot = 1;
            break;

        default:
            break;
    }
    if (selectedObjects->count() > 0) {
        for (int i = 0; i < selectedObjects->count(); i++) {
            GameObject* obj = static_cast<GameObject*>(selectedObjects->objectAtIndex(i));
            if (tag == 100023) obj->setRotation(0);
            else obj->setRotation(obj->getRotation() + rot);
        }
    } else {
        auto obj = getSelectedObject(this);
        if (tag == 100023) obj->setRotation(0);
        else obj->setRotation(obj->getRotation() + rot);
    }
}

void (*TRAM_EditorUI_createMoveMenu)(EditorUI* self);
void EditorUI_createMoveMenu(EditorUI* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("extra_edit_buttons")) {
        CCArray* buttons = CCArray::create();

        CCMenuItemSpriteExtra* btn;

        btn = self->getSpriteButton("edit_upBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9);
        btn->setTag(3);
        buttons->addObject(btn);
        btn = self->getSpriteButton("edit_downBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9);
        btn->setTag(4);
        buttons->addObject(btn);
        btn = self->getSpriteButton("edit_leftBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9);
        btn->setTag(1);
        buttons->addObject(btn);
        btn = self->getSpriteButton("edit_rightBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9);
        btn->setTag(2);
        buttons->addObject(btn);
        btn = self->getSpriteButton("edit_upBtn2_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9);
        btn->setTag(7);
        buttons->addObject(btn);
        btn = self->getSpriteButton("edit_downBtn2_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9);
        btn->setTag(8);
        buttons->addObject(btn);
        btn = self->getSpriteButton("edit_leftBtn2_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9);
        btn->setTag(5);
        buttons->addObject(btn);
        btn = self->getSpriteButton("edit_rightBtn2_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9);
        btn->setTag(6);
        buttons->addObject(btn);



        btn = self->getSpriteButton2("edit_upBtn3_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9);
        btn->setTag(100001);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_downBtn3_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9);
        btn->setTag(100002);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_leftBtn3_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9);
        btn->setTag(100003);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_rightBtn3_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9);
        btn->setTag(100004);
        buttons->addObject(btn);



        btn = self->getSpriteButton3("edit_upBtn_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9, 0.8);
        btn->setTag(100005);
        buttons->addObject(btn);
        btn = self->getSpriteButton3("edit_downBtn_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9, 0.8);
        btn->setTag(100006);
        buttons->addObject(btn);
        btn = self->getSpriteButton3("edit_leftBtn_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9, 0.8);
        btn->setTag(100007);
        buttons->addObject(btn);
        btn = self->getSpriteButton3("edit_rightBtn_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9, 0.8);
        btn->setTag(100008);
        buttons->addObject(btn);
        


        btn = self->getSpriteButton2("edit_upBtn5_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9);
        btn->setTag(100009);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_downBtn5_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9);
        btn->setTag(100010);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_leftBtn5_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9);
        btn->setTag(100011);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_rightBtn5_001.png", menu_selector(EditorUI::moveObjectCall2), nullptr, 0.9);
        btn->setTag(100012);
        buttons->addObject(btn);



        btn = self->getSpriteButton("edit_flipXBtn_001.png", menu_selector(EditorUI::transformObjectCall), nullptr, 0.9);
        btn->setTag(9);
        buttons->addObject(btn);
        btn = self->getSpriteButton("edit_flipYBtn_001.png", menu_selector(EditorUI::transformObjectCall), nullptr, 0.9);
        btn->setTag(10);
        buttons->addObject(btn);
        btn = self->getSpriteButton("edit_cwBtn_001.png", menu_selector(EditorUI::transformObjectCall), nullptr, 0.9);
        btn->setTag(11);
        buttons->addObject(btn);
        btn = self->getSpriteButton("edit_ccwBtn_001.png", menu_selector(EditorUI::transformObjectCall), nullptr, 0.9);
        btn->setTag(12);
        buttons->addObject(btn);



        btn = self->getSpriteButton2("edit_rotate45rBtn_001.png", menu_selector(EditorUI::transformObjectCall2), nullptr, 0.9);
        btn->setTag(100014);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_rotate45lBtn_001.png", menu_selector(EditorUI::transformObjectCall2), nullptr, 0.9);
        btn->setTag(100013);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_rotate22rBtn_001.png", menu_selector(EditorUI::transformObjectCall2), nullptr, 0.9);
        btn->setTag(100016);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_rotate22lBtn_001.png", menu_selector(EditorUI::transformObjectCall2), nullptr, 0.9);
        btn->setTag(100015);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_rotate11rBtn_001.png", menu_selector(EditorUI::transformObjectCall2), nullptr, 0.9);
        btn->setTag(100018);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_rotate11lBtn_001.png", menu_selector(EditorUI::transformObjectCall2), nullptr, 0.9);
        btn->setTag(100017);
        buttons->addObject(btn);

        btn = self->getSpriteButton2("edit_rotate5rBtn_001.png", menu_selector(EditorUI::transformObjectCall2), nullptr, 0.9);
        btn->setTag(100020);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_rotate5lBtn_001.png", menu_selector(EditorUI::transformObjectCall2), nullptr, 0.9);
        btn->setTag(100019);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_rotate1rBtn_001.png", menu_selector(EditorUI::transformObjectCall2), nullptr, 0.9);
        btn->setTag(100022);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_rotate1lBtn_001.png", menu_selector(EditorUI::transformObjectCall2), nullptr, 0.9);
        btn->setTag(100021);
        buttons->addObject(btn);
        btn = self->getSpriteButton2("edit_rotateResetBtn_001.png", menu_selector(EditorUI::transformObjectCall2), nullptr, 0.9);
        btn->setTag(100023);
        buttons->addObject(btn);

#if GAME_VERSION < GV_1_4
        btn = self->getSpriteButton("edit_editObjBtn_001.png", menu_selector(EditorUI::editObject), nullptr, 0.9);
        btn->setTag(12);
        buttons->addObject(btn);
        setEditObjectButton(self, btn);
#endif

        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        EditButtonBar* newBar = EditButtonBar::create(buttons, ccp(winSize.width * 0.5 - 5, getScreenBottom() + getUnkFloat(self) - 6.f));
        setEditButtonBar(self, newBar);
        self->addChild(newBar, 11);
    } else {
        TRAM_EditorUI_createMoveMenu(self);
    }
}

void EditorUI_om() {
    Omni::hook("_ZN8EditorUI12showMaxErrorEv",
        reinterpret_cast<void*>(EditorUI_showMaxError),
        reinterpret_cast<void**>(&TRAM_EditorUI_showMaxError));
    Omni::hook("_ZN8EditorUI26constrainGameLayerPositionEv",
        reinterpret_cast<void*>(EditorUI_constrainGameLayerPosition),
        reinterpret_cast<void**>(&TRAM_EditorUI_constrainGameLayerPosition));
    Omni::hook("_ZN8EditorUI7zoomOutEv",
        reinterpret_cast<void*>(EditorUI_zoomOut),
        reinterpret_cast<void**>(&TRAM_EditorUI_zoomOut));
#if GAME_VERSION < GV_1_5
    Omni::hook("_ZN8EditorUI4initEP16LevelEditorLayer",
        reinterpret_cast<void*>(EditorUI_init),
        reinterpret_cast<void**>(&TRAM_EditorUI_init));
#endif
#if GAME_VERSION < GV_1_6
    Omni::hook("_ZN8EditorUI15setupDeleteMenuEv",
        reinterpret_cast<void*>(EditorUI_setupDeleteMenu),
        reinterpret_cast<void**>(&TRAM_EditorUI_setupDeleteMenu));
#endif
    Omni::hook("_ZN8EditorUI15setupCreateMenuEv",
        reinterpret_cast<void*>(EditorUI_setupCreateMenu),
        reinterpret_cast<void**>(&TRAM_EditorUI_setupCreateMenu));
    Omni::hook("_ZN8EditorUI14createMoveMenuEv",
        reinterpret_cast<void*>(EditorUI_createMoveMenu),
        reinterpret_cast<void**>(&TRAM_EditorUI_createMoveMenu));
}