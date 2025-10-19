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
    // bool noClip;
    // bool iconHack;
    // bool textLengthBypass;
    // bool charFilterBypass;
    // bool swearBypass;
    // bool sliderBypass;
    // bool instantComplete;
    // bool fastMenu;
    // bool verifyHack;
    // bool noMirror;
    // bool objectLimitHack;
    // bool levelEdit;
    // bool freeScroll;
    // bool zoomBypass;
    // bool levelCopying;
    // bool pCommand;
    // bool restartOnly;
    // bool upload100KbFix;
    // bool cheatIndicator;
    // bool showPercentage;
    // bool viewAttempts;
    // bool objectCounter;
    // bool practiceMusic;
    // bool forceVisibility;
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
    bool hasCheated;
    bool instantComped;
    bool quitPlayLayer;

    Module* getModule(const char* id) {
        return modules.at(std::string(id));
    }
    bool getModuleEnabled(const char* id) {
        return getModule(id)->enabled;
    }

    CheatIndicatorColor getCheatIndicatorColor() {
        if (getModuleEnabled("noclip") || 
        getModuleEnabled("instant_complete") || 
        getModuleEnabled("no_mirror") || 
        getModuleEnabled("pcommand") || 
        getModuleEnabled("force_visibility")) return CheatIndicatorColor::Red;
        if (hasCheated) return CheatIndicatorColor::Orange;
        if (getModuleEnabled("level_edit")) return CheatIndicatorColor::Yellow;
        return CheatIndicatorColor::Green;
    }

    void setCheating(bool val) {
        hasCheated = val;
    }

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
        return getModuleEnabled("safe_mode");
    }

    // bool setAll(bool value) {
        
    // }

private:
    HaxManager() {
        modules.insert(std::pair<std::string, Module*>("cheat_indicator", new Module(
                "Cheat Indicator", 
                "Adds a dot that indicates whether you're using any unfair hacks.", 
                false, ModuleCategory::Gameplay, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("instant_complete", new Module(
                "Instant Complete", 
                "Completes the level as soon as it is entered.", 
                false, ModuleCategory::Gameplay, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    if (_) hax.setCheating(true);
                })));
        modules.insert(std::pair<std::string, Module*>("noclip", new Module(
                "NoClip", 
                "Prevents the player from dying.", 
                false, ModuleCategory::Gameplay, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    if (_) hax.setCheating(true);
                })));
        modules.insert(std::pair<std::string, Module*>("no_mirror", new Module(
                "No Mirror", 
                "Disables mirror portals.", 
                false, ModuleCategory::Gameplay, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    if (_) hax.setCheating(true);
                })));
        modules.insert(std::pair<std::string, Module*>("pcommand", new Module(
                "pCommand", 
                "Re-enables the unused \"pCommand\" functionality, which allows you to alter your speed, gravity and jump height. (Note: Some creative liberties had to be taken with the controls. This module is not entirely accurate to how pCommand actually worked.)", 
                false, ModuleCategory::Gameplay, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    if (_) hax.setCheating(true);
                })));
        modules.insert(std::pair<std::string, Module*>("practice_music", new Module(
                "Practice Music Hack", 
                "Plays the normal level music in practice mode.", 
                false, ModuleCategory::Gameplay, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("show_percentage", new Module(
                "Show Percentage", 
                "Displays the percentage the player is currently at.", 
                false, ModuleCategory::Gameplay, [](bool _){})));



        modules.insert(std::pair<std::string, Module*>("free_scroll", new Module(
                "Free Scroll", 
                "Removes the constraints for camera position in the editor.", 
                false, ModuleCategory::Editor, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("level_copy", new Module(
                "Level Copying", 
                "Adds a button to copy any level.", 
                false, ModuleCategory::Editor, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("level_edit", new Module(
                "Level Edit", 
                "Allows you to locally edit any level, as well as adding the restart button to all levels.", 
                false, ModuleCategory::Editor, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("object_counter", new Module(
                "Object Counter", 
                "Displays the object count of the level in the editor pause menu.", 
                false, ModuleCategory::Editor, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("object_hack", new Module(
                "Object Limit Bypass", 
                "Sets the object limit to 16,384. (Note: 16k Fix is not yet available. It will be added in a future update.)", 
                false, ModuleCategory::Editor, [](bool _){
                    if (_)
                        setObjectLimit(INCREASED_OBJECT_LIMIT - 1);
                    else
                        setObjectLimit(OBJECT_LIMIT);
                })));
        modules.insert(std::pair<std::string, Module*>("verify_bypass", new Module(
                "Verify Bypass", 
                "Allows you to upload any level without verifying it.", 
                false, ModuleCategory::Editor, [](bool _){})));
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
                false, ModuleCategory::Bypass, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("text_length_bypass", new Module(
                "Text Length Bypass", 
                "Allows you to write an unlimited amount of characters in any textbox.", 
                false, ModuleCategory::Bypass, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("unlock_icons", new Module(
                "Unlock Icons", 
                "Unlocks all icons and colors in the Icon Kit.", 
                false, ModuleCategory::Bypass, [](bool _){})));



        modules.insert(std::pair<std::string, Module*>("comment_ids", new Module(
                "Comment IDs", 
                "Displays comment IDs in comment cells.", 
                false, ModuleCategory::Informational, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("level_ids_in_list", new Module(
                "Level IDs in Search", 
                "Displays level IDs in level cells.", 
                false, ModuleCategory::Informational, [](bool _){})));
#if GAME_VERSION < 4
        modules.insert(std::pair<std::string, Module*>("show_difficulty", new Module(
                "Show Difficulty", 
                "Displays the level's difficulty on its page.", 
                false, ModuleCategory::Informational, [](bool _){})));
#endif
        modules.insert(std::pair<std::string, Module*>("view_level_stats", new Module(
                "View Level Stats", 
                "Adds a menu button that displays some information about the level, such as your attempt count on it.", 
                false, ModuleCategory::Informational, [](bool _){})));



        modules.insert(std::pair<std::string, Module*>("100_kb_fix", new Module(
                "100 KB Fix", 
                "Fixes a bug in Cocos2d where CCStrings always allocate 100 KB, instead allocating a dynamic buffer size. This fixes large levels being cut off on upload.", 
                true, ModuleCategory::Universal, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("fast_menu", new Module(
                "Fast Menu", 
                "Makes fade transitions instant.", 
                false, ModuleCategory::Universal, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("force_visibility", new Module(
                "Force Visibility", 
                "Makes all existing nodes visible.", 
                false, ModuleCategory::Universal, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    if (_) hax.setCheating(true);
                })));
        modules.insert(std::pair<std::string, Module*>("safe_mode", new Module(
                "Safe Mode",
                "Prevents any progress on any level from being saved.", 
                false, ModuleCategory::Universal, [](bool _){})));

        lastCategory = ModuleCategory::Gameplay;
        hasCheated = false;
        
        cheatIndicatorLabel = nullptr;
        percentageLabel = nullptr;
        pMenu = nullptr;
        pButton1 = nullptr;
        pButton2 = nullptr;
        pButton3 = nullptr;
        pButton4 = nullptr;
        pButton5 = nullptr;
        pButton6 = nullptr;
    }

    HaxManager(const HaxManager&) = delete;
    HaxManager& operator=(const HaxManager&) = delete;
    HaxManager(HaxManager&&) = delete;
    HaxManager& operator=(HaxManager&&) = delete;
};