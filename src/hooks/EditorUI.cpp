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
        cocos2d::CCNode* gameLayer = getEditorGameLayer(getUIEditorLayer(self));
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
}