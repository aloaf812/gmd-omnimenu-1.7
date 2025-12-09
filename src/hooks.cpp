#include "hooks/PlayLayer.cpp"
#include "hooks/MenuLayer.cpp"
#include "hooks/GameManager.cpp"
#include "hooks/CCTextInputNode.cpp"
#include "hooks/PauseLayer.cpp"
#include "hooks/EditLevelLayer.cpp"
#include "hooks/EditorUI.cpp"
#include "hooks/CCTransitionFade.cpp"
#include "hooks/LevelInfoLayer.cpp"
#include "hooks/LevelEditorLayer.cpp"
#include "hooks/UILayer.cpp"
#include "hooks/CCString.cpp"
#include "hooks/EditorPauseLayer.cpp"
#include "hooks/CCNode.cpp"
#include "hooks/LevelSelectLayer.cpp"
#include "hooks/LevelCell.cpp"
#if GAME_VERSION > GV_1_0
#include "hooks/CommentCell.cpp"
#endif
#include "hooks/GJGameLevel.cpp"
#include "hooks/GameStatsManager.cpp"
#include "hooks/PlayerObject.cpp"
#include "hooks/CCParticleSystemQuad.cpp"
#include "hooks/GameObject.cpp"
#include "hooks/MyLevelsLayer.cpp"
#include "hooks/CCLabelBMFont.cpp"
#include "hooks/CCLabelTTF.cpp"
#include "hooks/CCScheduler.cpp"
#include "hooks/GameSoundManager.cpp"
#include "hooks/SimpleAudioEngine.cpp"
#include "hooks/CCControlColourPicker.cpp"
#include "hooks/LevelBrowserLayer.cpp"
#include "hooks/GJGarageLayer.cpp"
#include "hooks/EndPortalObject.cpp"

void initialize_hooks() {
    PlayLayer_om();
    MenuLayer_om();
    GameManager_om();
    CCTextInputNode_om();
    PauseLayer_om();
    EditLevelLayer_om();
    EditorUI_om();
    CCTransitionFade_om();
    LevelInfoLayer_om();
    LevelEditorLayer_om();
    UILayer_om();
    CCString_om();
    EditorPauseLayer_om();
    CCNode_om();
    LevelSelectLayer_om();
    LevelCell_om();
#if GAME_VERSION > GV_1_0
    CommentCell_om();
#endif
    GJGameLevel_om();
    GameStatsManager_om();
    PlayerObject_om();
    CCParticleSystemQuad_om();
    GameObject_om();
    MyLevelsLayer_om();
    CCLabelBMFont_om();
    CCLabelTTF_om();
    CCScheduler_om();
    GameSoundManager_om();
    SimpleAudioEngine_om();
    CCControlColourPicker_om();
#if GAME_VERSION >= GV_1_4
    LevelBrowserLayer_om();
#endif
#if GAME_VERSION > GV_1_2
    GJGarageLayer_om();
#endif
    EndPortalObject_om();
}