#pragma once
#include "GameManager.hpp"
#include "UILayer.hpp"
#include "LevelInfoLayer.hpp"
#include "LevelEditorLayer.hpp"
#include "addresses.hpp"
#include <dlfcn.h>  // dlsym, RTLD_NOW
#include <dobby.h>  // DobbyHook
#include "EditorUI.hpp"
#include "LocalLevelManager.hpp"
#include "GameObject.hpp"
#include "DrawGridLayer.hpp"
#include "EditButtonBar.hpp"
#include "CCTextInputNode.hpp"
#include "ColorSelectPopup.hpp"
#include "LevelBrowserLayer.hpp"
#include "LevelSettingsObject.hpp"

#define MEMBER_BY_OFFSET(type, var, offset) \
    (*reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(var) + static_cast<uintptr_t>(offset)))

uintptr_t get_address(int offset);

PlayLayer* getPlayLayer();

PlayerObject* getPlayer(PlayLayer* playLayer);
PlayerObject* getPlayer();
GJGameLevel* getPlayLayerLevel(PlayLayer* playLayer);
GJGameLevel* getPlayLayerLevel();
UILayer* getUILayer(PlayLayer* playLayer);
UILayer* getUILayer();

GJLevelType getLevelType(GJGameLevel* level);
void setLevelType(GJGameLevel* level, GJLevelType type);

bool getLevelVerified(GJGameLevel* level);
void setLevelVerified(GJGameLevel* level, bool isVerified);

double getXVelocity(PlayerObject* player);
void setXVelocity(PlayerObject* player, double xVel);
void addXVelocity(PlayerObject* player, double adder);

double getGravity(PlayerObject* player);
void setGravity(PlayerObject* player, double gravity);
void addGravity(PlayerObject* player, double adder);

double getYStart(PlayerObject* player);
void setYStart(PlayerObject* player, double yStart);
void addYStart(PlayerObject* player, double adder);

int getLevelNormalPercent(GJGameLevel* level);
int getLevelPracticePercent(GJGameLevel* level);
void setLevelNormalPercent(GJGameLevel* level, int percent);
void setLevelPracticePercent(GJGameLevel* level, int percent);
float getCurrentPercentageF(PlayLayer* playLayer);
float getCurrentPercentageF();
int getCurrentPercentage(PlayLayer* playLayer);
int getCurrentPercentage();

int getLevelAttempts(GJGameLevel* level);
void setLevelAttempts(GJGameLevel* level, int attempts);
GJGameLevel* getInfoLayerLevel(LevelInfoLayer* infoLayer);

int getLevelID(GJGameLevel* level);
void setLevelID(GJGameLevel* level, int levelID);
std::string getLevelName(GJGameLevel* level);
std::string getLevelUsername(GJGameLevel* level);
int getLevelUserID(GJGameLevel* level);

template <typename T>
std::vector<uint8_t> toBytesLE(T value);

cocos2d::CCArray* getPlayLayerHazards(PlayLayer* playLayer);
cocos2d::CCArray* getPlayLayerHazards();
int getObjectCount(LevelEditorLayer* editorLayer);
void setObjectCount(LevelEditorLayer* editorLayer, int objectCount);
LevelEditorLayer* getLevelEditorLayer(void* pauseLayer);

bool getPlayLayerPractice(PlayLayer* playLayer);
bool getPlayLayerPractice();
void setPlayLayerPractice(PlayLayer* playLayer, bool isPractice);
void setPlayLayerPractice(bool isPractice);
cocos2d::CCArray* getPlayLayerCheckpoints(PlayLayer* playLayer);
cocos2d::CCArray* getPlayLayerCheckpoints();
cocos2d::CCPoint getCheckpointPosition(CCNode* checkpoint);
cocos2d::CCPoint getStartPos(PlayLayer* playLayer);
cocos2d::CCPoint getStartPos();
CCLayer* getSelectLayerScroll(CCLayer* selectLayer);
int getCurrentScrollScreen(CCLayer* scrollLayer);
int getScrollPageCount(CCLayer* scrollLayer);
int getCurrentScrollIndex(CCLayer* scrollLayer);
std::string getPlayerName();
void setObjectLimit(int limit);
void setZoomBypass(bool enabled);
cocos2d::CCLayer* getEditorGameLayer(LevelEditorLayer* editorLayer);
LevelEditorLayer* getUIEditorLayer(EditorUI* uiLayer);
GJGameLevel* getCellLevel(CCNode* cell);
int getCommentID(CCNode* comment);
std::string getLevelString(GJGameLevel* level);
GJGameLevel* getEditLayerLevel(CCLayer* editLayer);

CCParticleSystem* getShipLiftParticles(PlayerObject* player);
CCParticleSystem* getShipGroundParticles(PlayerObject* player);
CCParticleSystem* getGroundParticles(PlayerObject* player);
CCParticleSystem* getLandingParticles(PlayerObject* player);
CCParticleSystem* getLandingParticles2(PlayerObject* player);
CCParticleSystem* getShipFireParticles(PlayerObject* player);
CCParticleSystem* getBGParticles(PlayLayer* playLayer);
CCParticleSystem* getObjectParticles(void* object);
CCArray* getLocalLevels(LocalLevelManager* lolman);
CCArray* getLocalLevels();

bool getObjectUseAudioScale(void* object);
void setObjectUseAudioScale(void* object, bool uas);
void setFreeBuild(bool enable);

CCArray* getSelectedObjects(EditorUI* uiLayer);
void setSelectedObjects(EditorUI* uiLayer, CCArray* arr);
DrawGridLayer* getGridLayer(LevelEditorLayer* editLayer);
int getObjectType(GameObject* object);
CCArray* getRedoArray(LevelEditorLayer* editLayer);
GameObject* getSelectedObject(EditorUI* uiLayer);
CCSpriteBatchNode* getEditorBatchNode(LevelEditorLayer* editLayer);
bool getShouldSpawn(GameObject* object);

CCMenu* getEditorUIButtonMenu(EditorUI* uiLayer);

bool getOnGround(PlayerObject* player);
void setOnGround(PlayerObject* player, bool onGround);

CCArray* getCreateButtons(EditorUI* uiLayer);
void setCreateButtons(EditorUI* uiLayer, CCArray* array);
EditButtonBar* getCreateButtonBar(EditorUI* uiLayer);
void setCreateButtonBar(EditorUI* uiLayer, EditButtonBar* bar);
float getScreenBottom();
float getUnkFloat(EditorUI* uiLayer);

CCArray* getEditorSections(LevelEditorLayer* editLayer);
int getObjectKey(GameObject* obj);

int getCurrentAttempts(PlayLayer* playLayer);
int getCurrentAttempts();
int getCurrentJumps(PlayLayer* playLayer);
int getCurrentJumps();
float getClkTimer(PlayLayer* playLayer);
float getClkTimer();
std::string getPlayerUDID();

CCLabelBMFont* getAttemptLabel(PlayLayer* layer);
CCLabelBMFont* getAttemptLabel();
CCArray* getPlaySections(PlayLayer* playLayer);
CCArray* getPlaySections();
CCArray* getStartPositions(PlayLayer* playLayer);
CCArray* getStartPositions();
void setStartPos(PlayLayer* playLayer, CCPoint point);
void setStartPos(CCPoint point);
CCPoint getRealPosition(GameObject* object);

TextInputDelegate* getTextInputDelegate(CCTextInputNode* node);
void setTextInputDelegate(CCTextInputNode* node, TextInputDelegate* delegate);

cocos2d::extension::CCControlColourPicker* getColorPicker(ColorSelectPopup* popup);
cocos2d::extension::CCControlColourPicker* getColorPicker(ColorPickerPopup* popup);
void setCharLimit(CCTextInputNode* node, int limit);

void setEditButton(bool enable);
void setRestartButton(bool enable);

#if GAME_VERSION >= GV_1_4
GJSearchObject* getSearchObject(LevelBrowserLayer* browser);
int getSearchType(GJSearchObject* searcher);
void setIconHack(bool enable);
#endif

EditButtonBar* getEditButtonBar(EditorUI* uiLayer);
void setEditButtonBar(EditorUI* uiLayer, EditButtonBar* bar);
CCArray* getBarButtons(EditButtonBar* bar);
void setOriginalScale(CCMenuItemSpriteExtra* btn, float scale);

#if GAME_VERSION < GV_1_4
void setEditObjectButton(EditorUI* self, CCMenuItemSpriteExtra* btn);
#endif

int getGlobalOrderOfArrival();
LevelSettingsObject* getEditorSettingsObject(LevelEditorLayer* lel);
void setEditorSettingsObject(LevelEditorLayer* lel, LevelSettingsObject* settings);