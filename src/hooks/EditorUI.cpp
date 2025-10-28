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
void EditorUI::onDuplicate() {
    GameObject* selObj = getSelectedObject(this);
    CCArray* selectedObjects = getSelectedObjects(this);
    LevelEditorLayer* editLayer = getUIEditorLayer(this);
    DrawGridLayer* gridLayer = getGridLayer(editLayer);
    CCSpriteBatchNode* batchNode = getEditorBatchNode(editLayer);
    if (selObj == nullptr && selectedObjects->count() < 0) return;

    void* getSaveString = DobbySymbolResolver(MAIN_LIBRARY, "_ZN10GameObject13getSaveStringEv");
    getRedoArray(editLayer)->removeAllObjects();

    auto cyan = ccc3(0, 255, 255);

    if (selObj != nullptr) {
        std::string saveString = ((std::string(*)(GameObject*))getSaveString)(selObj);
        GameObject* newObj = GameObject::objectFromString(saveString);
        selObj->setColor(ccWHITE);
        editLayer->addToSection(newObj);
        if (getObjectType(newObj) == 7 && getShouldSpawn(newObj)) { // trigger
            gridLayer->addToEffects(newObj);
        }
        batchNode->addChild(newObj);
        this->selectObject(newObj);
        newObj->setColor(cyan);
        UndoObject* undo = UndoObject::create(newObj, UndoCommand::Placement);
        editLayer->addToUndoList(undo);
    } else {
        for (int i = 0; i < selectedObjects->count(); i++) {
            auto currObj = static_cast<GameObject*>(selectedObjects->objectAtIndex(i));
            std::string saveString = ((std::string(*)(GameObject*))getSaveString)(currObj);
            GameObject* newObj = GameObject::objectFromString(saveString);
            editLayer->addToSection(newObj);
            currObj->setColor(ccWHITE);
            if (getObjectType(newObj) == 7 && getShouldSpawn(newObj)) { // trigger effect line
                gridLayer->addToEffects(newObj);
            }
            newObj->setColor(cyan);
            batchNode->addChild(newObj);
            selectedObjects->replaceObjectAtIndex(i, newObj, false);
            UndoObject* undo = UndoObject::create(newObj, UndoCommand::Placement);
            editLayer->addToUndoList(undo);
        }
    }
}

bool (*TRAM_EditorUI_init)(EditorUI* self, LevelEditorLayer* lel);
bool EditorUI_init(EditorUI* self, LevelEditorLayer* lel) {
    if (!TRAM_EditorUI_init(self, lel)) return false;
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("copy_paste")) {
        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();

        CCMenu* btnMenu = CCMenu::create();
        self->addChild(btnMenu);

        btnMenu->setPosition(ccp(winSize.width - 30, winSize.height - 70));
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

void (*TRAM_EditorUI_setupDeleteMenu)(EditorUI* self);
void EditorUI_setupDeleteMenu(EditorUI* self) {
    TRAM_EditorUI_setupDeleteMenu(self);
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("delete_selected")) {
        CCMenu* menu = getEditorUIButtonMenu(self);

        if (!menu || menu == nullptr) { // useless failsafe but i don't like removing those
            TRAM_EditorUI_setupDeleteMenu(self);
            menu = getEditorUIButtonMenu(self);
        }

        auto delSelSpr = ButtonSprite::create("Delete Selected", 75, 0, 0.6, false, "bigFont.fnt", "GJ_button_04.png");
        // delSelSpr->setScale(0.8f);

        auto delSelBtn = CCMenuItemSpriteExtra::create(delSelSpr, delSelSpr, self, menu_selector(EditorUI::onDeleteSelected));
        menu->addChild(delSelBtn);
    }
}
#endif // GAME_VERSION < GV_1_5

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
        fuckingArray->insertObject(sep1, 28);

        CCNode* unlistedSeparator = CCNode::create();
        unlistedSeparator->setTag(0);
        fuckingArray->addObject(unlistedSeparator);
        fuckingArray->addObject(self->getCreateBtn("edit_eLevelEndBtn_001.png", 4));
        fuckingArray->addObject(self->getCreateBtn("edit_eBGEOn_001.png", 4));
        fuckingArray->addObject(self->getCreateBtn("edit_eBGEOff_001.png", 4));

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
    Omni::hook("_ZN8EditorUI15setupDeleteMenuEv",
        reinterpret_cast<void*>(EditorUI_setupDeleteMenu),
        reinterpret_cast<void**>(&TRAM_EditorUI_setupDeleteMenu));
#endif
    Omni::hook("_ZN8EditorUI15setupCreateMenuEv",
        reinterpret_cast<void*>(EditorUI_setupCreateMenu),
        reinterpret_cast<void**>(&TRAM_EditorUI_setupCreateMenu));
}