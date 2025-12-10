#pragma once

#include <cocos2d.h>
#include "Module.hpp"
#include "CCMenuItemSpriteExtra.hpp"
#include "constants.hpp"
#include "../version/VersionUtils.hpp"
#include <sys/stat.h>
#include <errno.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "GJGameLevel.hpp"
#include "MyLevelsLayer.hpp"
#include "LevelBrowserLayer.hpp"

enum class CheatIndicatorColor {
    Green,
    Yellow,
    Orange,
    Red
};

class HaxManager {

public:
    static HaxManager& sharedState() {
        static HaxManager instance;
        return instance;
    }
    std::map<std::string, Module*> modules;
    cocos2d::CCLabelBMFont* cheatIndicatorLabel;
    cocos2d::CCLabelBMFont* percentageLabel;
    ModuleCategory lastCategory;
    CCMenu* pMenu;
    CCMenuItemSpriteExtra* pButton1;
    CCMenuItemSpriteExtra* pButton2;
    CCMenuItemSpriteExtra* pButton3;
    CCMenuItemSpriteExtra* pButton4;
    CCMenuItemSpriteExtra* pButton5;
    CCMenuItemSpriteExtra* pButton6;
    CCLabelBMFont* uiLabel;
    bool hasCheated;
    bool instantComped;
    bool quitPlayLayer;
    int pSpeedModified;
    int pGravityModified;
    int pYStartModified;
    // jobject activity;
    GJGameLevel* gdShareLevel;
    MyLevelsLayer* myLevelsLayer;
    LevelBrowserLayer* levelBrowserLayer;
    float bestRunStart;
    float bestRunEnd;
    int frames;
    int fps;
    float fpsCounter;
    int clicks;
    int deaths;
    int frameCount;
    int lastDeadFrame;
    CCNode* spSwitcherParent;
    int startPosIndex;
    CCLabelBMFont* switcherLabel;
    float startPercent;
    bool checkpointsInNormalMode;
    CCLayerColor* noclipTint;
    int deadFrames;
    float noclipAccuracy;
    bool completed;

#if GAME_VERSION > GV_1_4
    bool blockVerify;
#endif

    Module* getModule(const char* id) {
        return modules.at(std::string(id));
    }
    bool getModuleEnabled(const char* id) {
        return getModule(id)->enabled;
    }
    bool getLeftLILButtons() {
        return getModuleEnabled("level_copy") || getModuleEnabled("gdshare");
    }

    CheatIndicatorColor getCheatIndicatorColor() {
        if (getModuleEnabled("noclip") || 
        getModuleEnabled("instant_complete") || 
        checkpointsInNormalMode || 
        getModuleEnabled("jump_hack") || 
        pSpeedModified != 0 || pGravityModified != 0 || pYStartModified != 0) return CheatIndicatorColor::Red;
        if (hasCheated) return CheatIndicatorColor::Orange;
#ifndef FORCE_AUTO_SAFE_MODE
        if (getModuleEnabled("level_edit")) return CheatIndicatorColor::Yellow;
#endif 
        return CheatIndicatorColor::Green;
    }

    bool getShowLabel() {
        return (
            getModuleEnabled("label_attempts_session") ||
            getModuleEnabled("label_attempts_total") ||
            getModuleEnabled("label_best_run") ||
            getModuleEnabled("label_clicks") ||
            getModuleEnabled("label_clock") ||
            getModuleEnabled("label_fps") ||
            getModuleEnabled("label_frames") ||
            getModuleEnabled("label_jumps") ||
            getModuleEnabled("label_noclip_deaths") ||
            getModuleEnabled("label_pcommand") ||
            getModuleEnabled("label_player_position") ||
            getModuleEnabled("label_player_rotation") ||
            getModuleEnabled("label_time_spent")
        );
    }

    void setCheating(bool val) {
        hasCheated = val;
    }
#if GAME_VERSION > GV_1_4
    void blockVerifySet(bool val) {
        setBlockVerify(val);
        blockVerify = val;
    }
#endif

    void loadSettingsFromFile() {
        FILE* fp = fopen(MENU_SETTINGS_PATH MENU_SETTINGS, "rb");
        if (!fp) {
            cocos2d::CCLog("unable to open settings file for reading");
            makeDirectory();
            return;
        }
        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        rapidjson::Document doc;
        doc.ParseStream(is);

        fclose(fp);

        std::map<std::string, Module*>::iterator it;
        for (it = modules.begin(); it != modules.end(); it++) {
            const char* id = it->first.c_str();
            Module* mod = it->second;
            if (doc.HasMember(id) && doc[id].IsBool()) {
                mod->setEnabled(doc[id].GetBool());
            }
        }
    }
    void makeDirectory() {
        int status = mkdir(MENU_SETTINGS_PATH, static_cast<mode_t>(0755));
        if (status != 0 && errno != EEXIST) {
            cocos2d::CCLog("could not make dir: %i", errno);
        }
    }
#if GAME_VERSION > GV_1_4
    void update(float dt) {
        if (isSafeMode()) {
            if (!blockVerify) blockVerifySet(true);
        } else if (blockVerify) {
            blockVerifySet(false);
        }
    }
#endif
    void saveSettingsToFile() {
        makeDirectory();
        FILE* fp = fopen(MENU_SETTINGS_PATH MENU_SETTINGS, "wb");

        if (!fp) {
            cocos2d::CCLog("unable to open settings file for writing");
            return;
        } else {
            std::map<std::string, Module*>::iterator it;
            rapidjson::Document document;
            document.SetObject();
            rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
            for (it = modules.begin(); it != modules.end(); it++) {
                rapidjson::Value jsonKey(it->first.c_str(), static_cast<rapidjson::SizeType>(it->first.length()), allocator);
                document.AddMember(jsonKey, getModuleEnabled(it->first.c_str()), allocator);
            }
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            document.Accept(writer);
            const char* output = buffer.GetString();

            fprintf(fp, "%s", output);
            fclose(fp);
        }
    }
    bool isSafeMode() {
        if (getModuleEnabled("safe_mode")) return true;
#ifndef FORCE_AUTO_SAFE_MODE
        if (!getModuleEnabled("auto_safe_mode")) return false;
#endif
        CheatIndicatorColor color = getCheatIndicatorColor();
        if (color == CheatIndicatorColor::Red || color == CheatIndicatorColor::Orange)
            return true;

        return false;
    }

    /* 
    Resets nearly all variables in the HaxManager. Called upon the creation of HaxManager, and upon the destruction of UILayer. 
    */
    void resetValues() {
        hasCheated = false;
        cheatIndicatorLabel = nullptr;
        percentageLabel = nullptr;
        pButton1 = nullptr;
        pButton2 = nullptr;
        pButton3 = nullptr;
        pButton4 = nullptr;
        pButton5 = nullptr;
        pButton6 = nullptr;
        uiLabel = nullptr;
        pMenu = nullptr;
        spSwitcherParent = nullptr;
        pSpeedModified = 0;
        pGravityModified = 0;
        pYStartModified = 0;
        bestRunStart = 0;
        bestRunEnd = 0;
        startPercent = 0;
        clicks = 0;
        deaths = 0;
        frameCount = 0;
        lastDeadFrame = -1;
        startPosIndex = -1;
        switcherLabel = nullptr;
        checkpointsInNormalMode = false;
        noclipTint = nullptr;
        deadFrames = 0;
        noclipAccuracy = 100;
        completed = false;
    }

private:
    HaxManager() {
        modules.insert(std::pair<std::string, Module*>("instant_complete", new Module(
                "Instant Complete", 
                "Completes the level as soon as it is entered.", 
                false, ModuleCategory::Player, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    if (_) hax.setCheating(true);
                })));
        modules.insert(std::pair<std::string, Module*>("jump_hack", new Module(
                "Jump Hack", 
                "Allows the player to jump in the air indefinitely.", 
                false, ModuleCategory::Player, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    if (_) hax.setCheating(true);
                })));
        modules.insert(std::pair<std::string, Module*>("music_bug_fix", new Module(
                "Music Bug Fix", 
                "Makes music seeking work properly more often.", 
                true, ModuleCategory::Player, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("noclip", new Module(
                "NoClip", 
                "Prevents the player from dying.", 
                false, ModuleCategory::Player, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    if (_) hax.setCheating(true);
                })));
        modules.insert(std::pair<std::string, Module*>("no_mirror", new Module(
                "No Mirror", 
                "Disables mirror portals.", 
                false, ModuleCategory::Player, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    if (_) hax.setCheating(true);
                })));
        modules.insert(std::pair<std::string, Module*>("pcommand", new Module(
                "pCommand", 
                "Re-enables the unused \"pCommand\" functionality, which allows you to alter your speed, gravity and jump height. (NOTE: Some creative liberties had to be taken with the controls. This module is not entirely accurate to how pCommand actually worked.)", 
                false, ModuleCategory::Player, [](bool _){})));
#if GAME_VERSION < GV_1_2
        modules.insert(std::pair<std::string, Module*>("rotation_bug_fix", new Module(
                "Rotation Bug Fix", 
                "Makes it so hitboxes rotate properly along with the object. (module by akqanile/Adelfa)\nNOTE: toggling this mid-attempt counts as cheating.", 
                false, ModuleCategory::Player, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    hax.setCheating(true);
                })));
#endif
        modules.insert(std::pair<std::string, Module*>("practice_music", new Module(
                "Practice Music Hack", 
                "Plays the normal level music in practice mode.", 
                false, ModuleCategory::Player, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("show_restart_button", new Module(
                "Show Restart Button", 
                "Adds the restart button to the pause menu of all levels", 
                false, ModuleCategory::Player, [](bool _){
                    setRestartButton(_);
                })));
        modules.insert(std::pair<std::string, Module*>("start_pos_switcher", new Module(
                "Start Pos Switcher", 
                "Adds the ability to switch between start positions on the fly.", 
                false, ModuleCategory::Player, [](bool _){})));



        modules.insert(std::pair<std::string, Module*>("hide_attempts", new Module(
                "Hide Attempts", 
                "Hides the attempts label while playing.", 
                false, ModuleCategory::Visual, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("noclip_tint_on_death", new Module(
                "NoClip Tint On Death", 
                "Tints the screen red when the player dies with NoClip enabled.", 
                false, ModuleCategory::Visual, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("no_glow", new Module(
                "No Glow", 
                "Disables object glow. (Note: Will only apply after re-entering the level)", 
                false, ModuleCategory::Visual, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("no_pulse", new Module(
                "No Pulse", 
                "Disables object pulses.", 
                false, ModuleCategory::Visual, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("no_rotation", new Module(
                "No Rotation", 
                "Prevents the player from rotating.", 
                false, ModuleCategory::Visual, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("no_shake", new Module(
                "No Shake", 
                "Disables camera shake when completing a level.", 
                false, ModuleCategory::Visual, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("no_trail", new Module(
                "No Trail", 
                "Disables the player's trail at all times.", 
                false, ModuleCategory::Visual, [](bool _){})));
#if GAME_VERSION == GV_1_4
        modules.insert(std::pair<std::string, Module*>("obj_color_fix", new Module(
                "Obj Color Fix", 
                "Fixes the Object color not resetting to white after death.", 
                false, ModuleCategory::Visual, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("show_percentage", new Module(
                "Show Percentage", 
                "Displays the percentage the player is currently at.", 
                false, ModuleCategory::Visual, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("show_percentage_decimal", new Module(
                "Decimal Percentage", 
                "Puts 3 decimal places after the percentage if you have Show Percentage enabled and in the Best Run label.", 
                false, ModuleCategory::Visual, [](bool _){})));



        // 16k fix code: https://github.com/cierra-kb/legacy-starry-sky/blob/main/src/modules/editor.cpp
        modules.insert(std::pair<std::string, Module*>("16k_fix", new Module(
                "16K Fix (Read Desc)", 
                "Fixes a bug where only 16,384 objects can render in the editor by culling the objects. (module by akqanile/Adelfa)\nNOTE: this can be potentially incompatible with vanilla layering, and makes the editor way laggier on dense levels.", 
                false, ModuleCategory::Editor, [](bool _){})));
// #if GAME_VERSION < GV_1_6
//         modules.insert(std::pair<std::string, Module*>("back_button_pause", new Module(
//                 "Back Button Pause", 
//                 "Opens the editor pause menu when pressing the back button on the keypad.", 
//                 false, ModuleCategory::Editor, [](bool _){})));
// #endif
#if GAME_VERSION < GV_1_5
        modules.insert(std::pair<std::string, Module*>("copy_paste", new Module(
                "Copy & Paste", 
                "Adds a button that duplicates the selected objects.", 
                false, ModuleCategory::Editor, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("delete_selected", new Module(
#if GAME_VERSION < GV_1_5
                "Delete Selected", 
                "Adds a button that removes the selected objects.", 
#else
                "Delete Selected (2.0)",
                "Moves Delete Selected next to the Undo/Redo buttons.",
#endif
                false, ModuleCategory::Editor, [](bool _){})));
#if GAME_VERSION < GV_1_6
        modules.insert(std::pair<std::string, Module*>("delete_start_pos", new Module(
                "Delete Start Pos", 
                "Adds a button that removes all start positions in the level.", 
                false, ModuleCategory::Editor, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("extra_edit_buttons", new Module(
                "Extra Edit Buttons", 
                "Adds extra buttons to the edit tab that allow you to control objects more precisely.", 
                false, ModuleCategory::Editor, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("free_build", new Module(
                "Free Build", 
                "Removes the constraints for placing and moving objects in the editor.", 
                false, ModuleCategory::Editor, [](bool _){
                    setFreeBuild(_);
                })));
        modules.insert(std::pair<std::string, Module*>("free_scroll", new Module(
                "Free Scroll", 
                "Removes the constraints for camera position in the editor.", 
                false, ModuleCategory::Editor, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("level_copy", new Module(
                "Level Copying", 
                "Adds a button to copy any level.", 
                false, ModuleCategory::Editor, [](bool _){})));
#ifndef FORCE_AUTO_SAFE_MODE
        modules.insert(std::pair<std::string, Module*>("level_edit", new Module(
                "Level Edit", 
                "Allows you to locally edit any level.", 
                false, ModuleCategory::Editor, [](bool _){
                    setEditButton(_);
                })));
#endif
#if GAME_VERSION < GV_1_6
        modules.insert(std::pair<std::string, Module*>("object_counter", new Module(
                "Object Counter", 
                "Displays the object count of the level in the editor pause menu.", 
                false, ModuleCategory::Editor, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("object_hack", new Module(
                "Object Limit Bypass", 
#if GAME_VERSION < GV_1_5
                "Sets the object limit to the 32-bit integer limit, or to 16,384 if 16k Fix is not enabled.",
#else
                "Removes the object limit.",
#endif
                false, ModuleCategory::Editor, [](bool _){
                    if (_) {
                        HaxManager& hax = HaxManager::sharedState();
                        if (hax.getModuleEnabled("16k_fix"))
                            setObjectLimit(2147483646);
                        else
                            setObjectLimit(INCREASED_OBJECT_LIMIT - 1);
                    } else
                        setObjectLimit(OBJECT_LIMIT);
                })));
        modules.insert(std::pair<std::string, Module*>("rgb_color_inputs", new Module(
                "RGB Color Inputs", 
                "Allows you to directly input the RGB values in color selection menus.", 
                false, ModuleCategory::Editor, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("unlisted_objects", new Module(
                "Unlisted Objects", 
                "Adds unlisted objects to the editor, allowing them to be used in levels.", 
                false, ModuleCategory::Editor, [](bool _){})));
#ifndef FORCE_AUTO_SAFE_MODE
        modules.insert(std::pair<std::string, Module*>("verify_bypass", new Module(
                "Verify Bypass", 
                "Allows you to upload any level without verifying it.", 
                false, ModuleCategory::Editor, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("zoom_bypass", new Module(
                "Zoom Bypass", 
                "Removes editor zoom restrictions.", 
                false, ModuleCategory::Editor, [](bool _){
                    setZoomBypass(_);
                })));



        modules.insert(std::pair<std::string, Module*>("char_filter_bypass", new Module(
                "Character Filter Bypass", 
                "Allows you to use any character in any textbox.", 
                false, ModuleCategory::Bypass, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("swear_filter_bypass", new Module(
                "Swear Filter Bypass", 
                "Allows you to use profanities in textboxes.", 
                false, ModuleCategory::Bypass, [](bool _){
#if GAME_VERSION > GV_1_4
                    setProfanityBypass(_);
#endif                       
                })));
        modules.insert(std::pair<std::string, Module*>("text_length_bypass", new Module(
                "Text Length Bypass", 
                "Allows you to write an unlimited amount of characters in any textbox.", 
                false, ModuleCategory::Bypass, [](bool _){
#if GAME_VERSION > GV_1_4
                   setCharLimitBypass(_);
#endif
                })));
#if GAME_VERSION >= GV_1_6
        modules.insert(std::pair<std::string, Module*>("unlock_clubstep", new Module(
                "Unlock Clubstep", 
                "Allows you to play Clubstep without getting 20 secret coins.", 
                false, ModuleCategory::Bypass, [](bool _){
                    setClubstepBypass(_);
                })));
#endif
        modules.insert(std::pair<std::string, Module*>("unlock_icons", new Module(
                "Unlock Icons", 
                "Unlocks all icons and colors in the Icon Kit.", 
                false, ModuleCategory::Bypass, [](bool _){
#if GAME_VERSION >= GV_1_4
                    setIconHack(_);
#endif
                })));



#if GAME_VERSION > GV_1_0
        modules.insert(std::pair<std::string, Module*>("comment_ids", new Module(
                "Comment IDs", 
                "Displays comment IDs in comment cells.", 
                false, ModuleCategory::Informational, [](bool _){})));
#endif
#if GAME_VERSION > GV_1_2
        modules.insert(std::pair<std::string, Module*>("demons_in_garage", new Module(
                "Demons in Garage", 
                "Displays your demon count in the icon kit (garage). (module by Hris69)", 
                false, ModuleCategory::Informational, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("level_ids_in_list", new Module(
                "Level IDs in Search", 
                "Displays level IDs in level cells.", 
                false, ModuleCategory::Informational, [](bool _){})));
#if GAME_VERSION < GV_1_3
        modules.insert(std::pair<std::string, Module*>("show_difficulty", new Module(
                "Show Difficulty", 
                "Displays the level's difficulty on its page.", 
                false, ModuleCategory::Informational, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("view_level_stats", new Module(
#if GAME_VERSION < GV_1_6
                "View Level Stats", 
                "Adds a menu button that displays some information about the level, such as your attempt count on it.",
#else
                "View More Stats", 
                "Displays more stats about the level, and adds the ability to view stats for local levels.",
#endif
                false, ModuleCategory::Informational, [](bool _){})));


        modules.insert(std::pair<std::string, Module*>("100_kb_fix", new Module(
                "100 KB Fix", 
                "Fixes a bug in Cocos2d where CCStrings always allocate 100 KB, instead allocating a dynamic buffer size. This fixes large levels being cut off on upload (for versions before 1.5), as well as potentially increasing performance. (module by akqanile/Adelfa)", 
                true, ModuleCategory::Universal, [](bool _){})));
#ifndef FORCE_AUTO_SAFE_MODE
        modules.insert(std::pair<std::string, Module*>("auto_safe_mode", new Module(
                "Auto Safe Mode",
                "Prevents any progress on any level from being saved if you have cheats enabled.", 
                false, ModuleCategory::Universal, [](bool _){})));
#endif
        // modules.insert(std::pair<std::string, Module*>("eeffoc", new Module(
        //         "Eeffoc",
        //         "!ynnuf si hcihW", 
        //         false, ModuleCategory::Universal, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("fast_menu", new Module(
                "Fast Menu", 
                "Makes fade transitions instant.", 
                false, ModuleCategory::Universal, [](bool _){})));
#if GAME_VERSION < GV_1_5
        modules.insert(std::pair<std::string, Module*>("font_offset_fix", new Module(
                "Font Offset Fix", 
                "Fixes a pre-1.5 bug where text labels get offset further downwards the higher your resolution is.", 
                true, ModuleCategory::Universal, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("force_visibility", new Module(
                "Force Visibility", 
                "Makes all existing nodes visible.", 
                false, ModuleCategory::Universal, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    if (_) hax.setCheating(true);
                })));
        modules.insert(std::pair<std::string, Module*>("gdshare", new Module(
                "GDShare", 
                "Adds buttons to convert a level to a .gmd file and vice-versa. May not work on older Android versions.", 
                false, ModuleCategory::Universal, [](bool _){})));
#if GAME_VERSION < GV_1_2
        modules.insert(std::pair<std::string, Module*>("input_bug_fix", new Module(
                "Input Bug Fix", 
                "Fixes a pre-1.2 bug where typing more than 1 character doesn't work on certain keyboards.", 
                true, ModuleCategory::Universal, [](bool _){})));
#endif
#ifdef PING_SPOOFING
        modules.insert(std::pair<std::string, Module*>("ping_spoofing", new Module(
                "Pig Spoofing", 
                "Self-explanatory.", 
                false, ModuleCategory::Universal, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("safe_mode", new Module(
                "Safe Mode",
#ifndef FORCE_AUTO_SAFE_MODE
                "Prevents any progress on any level from being saved.", 
#else
                "Prevents any progress on any level from being saved. (Note: this build has Auto Safe Mode enabled forcibly, which means the effects of this module are automatically applied if you are using any cheats)",
#endif 

                false, ModuleCategory::Universal, [](bool _){})));



        modules.insert(std::pair<std::string, Module*>("cheat_indicator", new Module(
                "Cheat Indicator", 
                "Adds a dot that indicates whether any unfair hacks are currently enabled.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_attempts_session", new Module(
                "Attempt Count (Session)", 
                "Displays the current attempt count.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_attempts_total", new Module(
                "Attempt Count (Total)", 
                "Displays the total attempt count.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_best_run", new Module(
                "Best Run (Session)", 
                "Displays the best percentage achieved this session.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_clicks", new Module(
                "Clicks", 
                "Displays the amount of clicks this attempt.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_clock", new Module(
                "Clock", 
                "Displays the current time.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_fps", new Module(
                "FPS", 
                "Displays the current frames per second.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_frames", new Module(
                "Frames", 
                "Displays the amount of frames that have passed since the start of the attempt.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_jumps", new Module(
                "Jumps", 
                "Displays the current jump count.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_noclip_accuracy", new Module(
                "NoClip Accuracy", 
                "If NoClip is enabled, displays how accurately the level has been played thus far.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_noclip_deaths", new Module(
                "NoClip Deaths", 
                "If NoClip is enabled, displays the amount of times you would have died.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_pcommand", new Module(
                "pCommand", 
                "Displays the pCommand values.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_player_position", new Module(
                "Player Position", 
                "Displays the player's X and Y coordinates.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_player_rotation", new Module(
                "Player Rotation", 
                "Displays the player's rotation.", 
                false, ModuleCategory::Label, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("label_time_spent", new Module(
                "Time Spent", 
                "Displays the amount of time spent this session.", 
                false, ModuleCategory::Label, [](bool _){})));



        modules.insert(std::pair<std::string, Module*>("particle_background", new Module(
                "Background",
                "Toggles the visibility of the particles that appear all over the background in the ship or UFO gamemode.", 
                true, ModuleCategory::Particles, [](bool _){})));
#if GAME_VERSION >= GV_1_5
        modules.insert(std::pair<std::string, Module*>("particle_burst", new Module(
                "Burst",
                "Toggles the visibility of the particles that appear when the UFO jumps.", 
                true, ModuleCategory::Particles, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("particle_death_effect", new Module(
                "Death Effect",
                "Toggles the visibility of the particles that appear when the player dies.", 
                true, ModuleCategory::Particles, [](bool _){})));
#if GAME_VERSION >= GV_1_6
        modules.insert(std::pair<std::string, Module*>("particle_destructible_blocks", new Module(
                "Destructible Blocks",
                "Toggles the visibility of the particles that appear when colliding with destructible blocks.", 
                true, ModuleCategory::Particles, [](bool _){})));
#endif
        // modules.insert(std::pair<std::string, Module*>("particle_end_wall", new Module(
        //         "End Wall",
        //         "Toggles the visibility of the particles that are emitted by the end wall.", 
        //         true, ModuleCategory::Particles, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("particle_fireworks", new Module(
                "Fireworks",
                "Toggles the visibility of the particles that are emitted by the fireworks during the level completion animation.", 
                true, ModuleCategory::Particles, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("particle_level_completion", new Module(
                "Level Completion",
                "Toggles the visibility of the particles that are emitted during the level completion animation.", 
                true, ModuleCategory::Particles, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("particle_ground", new Module(
                "Ground",
                "Toggles the visibility of the particles that appear when the cube or ball is dragging across the ground.", 
                true, ModuleCategory::Particles, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("particle_landing", new Module(
                "Landing",
                "Toggles the visibility of the particles that appear when the cube or ball lands on the ground.", 
                true, ModuleCategory::Particles, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("particle_objects", new Module(
                "Objects",
                "Toggles the visibility of the particles that are emitted by objects, such as portals, pads and orbs.", 
                true, ModuleCategory::Particles, [](bool _){})));
#if GAME_VERSION >= GV_1_6
        modules.insert(std::pair<std::string, Module*>("particle_secret_coins", new Module(
                "Secret Coins",
                "Toggles the visibility of the particles that appear when colliding with secret coinbs.", 
                true, ModuleCategory::Particles, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("particle_ship_fire", new Module(
                "Ship Fire",
                "Toggles the visibility of the particles that appear behind the ship.", 
                true, ModuleCategory::Particles, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("particle_ship_ground", new Module(
                "Ship Ground",
                "Toggles the visibility of the particles that appear when the ship is dragging across the ground.", 
                true, ModuleCategory::Particles, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("particle_ship_lift", new Module(
                "Ship Lift",
                "Toggles the visibility of the particles that appear when the ship is flying.", 
                true, ModuleCategory::Particles, [](bool _){})));

        lastCategory = ModuleCategory::Player;
#if GAME_VERSION > GV_1_4
        blockVerify = false;
#endif

        resetValues();
    }

    HaxManager(const HaxManager&) = delete;
    HaxManager& operator=(const HaxManager&) = delete;
    HaxManager(HaxManager&&) = delete;
    HaxManager& operator=(HaxManager&&) = delete;
};