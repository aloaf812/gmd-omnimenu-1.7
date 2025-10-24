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
#include <jni.h>
#include "GJGameLevel.hpp"
#include "MyLevelsLayer.hpp"

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
    bool hasCheated;
    bool instantComped;
    bool quitPlayLayer;
    int pSpeedModified;
    int pGravityModified;
    int pYStartModified;
    jobject activity;
    GJGameLevel* gdShareLevel;
    MyLevelsLayer* myLevelsLayer;

    Module* getModule(const char* id) {
        return modules.at(std::string(id));
    }
    bool getModuleEnabled(const char* id) {
        return getModule(id)->enabled;
    }

    CheatIndicatorColor getCheatIndicatorColor() {
        if (getModuleEnabled("noclip") || 
        getModuleEnabled("instant_complete") || 
        // getModuleEnabled("pcommand") || 
        getModuleEnabled("force_visibility") || 
        pSpeedModified != 0 || pGravityModified != 0 || pYStartModified != 0) return CheatIndicatorColor::Red;
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
        if (getModuleEnabled("safe_mode")) return true;
#ifndef FORCE_AUTO_SAFE_MODE
        if (!getModuleEnabled("auto_safe_mode")) return false;
#endif
        CheatIndicatorColor color = getCheatIndicatorColor();
        if (color == CheatIndicatorColor::Red || color == CheatIndicatorColor::Orange)
            return true;
        
        return false;
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
        modules.insert(std::pair<std::string, Module*>("no_glow", new Module(
                "No Glow", 
                "Disables object glow. (Note: Will only apply after re-entering the level)", 
                false, ModuleCategory::Gameplay, [](bool _){ })));
        modules.insert(std::pair<std::string, Module*>("no_mirror", new Module(
                "No Mirror", 
                "Disables mirror portals.", 
                false, ModuleCategory::Gameplay, [](bool _){
                    HaxManager& hax = HaxManager::sharedState();
                    if (_) hax.setCheating(true);
                })));
        // modules.insert(std::pair<std::string, Module*>("no_particles", new Module(
        //         "No Particles", 
        //         "Disables every particle system in the game.", 
        //         false, ModuleCategory::Gameplay, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("no_rotation", new Module(
                "No Rotation", 
                "Prevents the player from rotating.", 
                false, ModuleCategory::Gameplay, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("no_shake", new Module(
                "No Shake", 
                "Disables camera shake when completing a level.", 
                false, ModuleCategory::Gameplay, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("no_trail", new Module(
                "No Trail", 
                "Disables the player's trail at all times.", 
                false, ModuleCategory::Gameplay, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("pcommand", new Module(
                "pCommand", 
                "Re-enables the unused \"pCommand\" functionality, which allows you to alter your speed, gravity and jump height. (Note: Some creative liberties had to be taken with the controls. This module is not entirely accurate to how pCommand actually worked.)", 
                false, ModuleCategory::Gameplay, [](bool _){
                    // HaxManager& hax = HaxManager::sharedState();
                    // if (_) hax.setCheating(true);
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
                "Fixes a bug in Cocos2d where CCStrings always allocate 100 KB, instead allocating a dynamic buffer size. This fixes large levels being cut off on upload (for versions before 1.5), as well as potentially increasing performance.", 
#if GAME_VERSION < 6
                true, 
#else
                false,
#endif
                ModuleCategory::Universal, [](bool _){})));
#ifndef FORCE_AUTO_SAFE_MODE
        modules.insert(std::pair<std::string, Module*>("auto_safe_mode", new Module(
                "Auto Safe Mode",
                "Prevents any progress on any level from being saved if you have cheats enabled.", 
                false, ModuleCategory::Universal, [](bool _){})));
#endif
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
        modules.insert(std::pair<std::string, Module*>("gdshare", new Module(
                "GDShare", 
                "Adds buttons to convert a level to a .gmd file and vice-versa.", 
                false, ModuleCategory::Universal, [](bool _){})));
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



        modules.insert(std::pair<std::string, Module*>("particle_background", new Module(
                "Background",
                "Toggles the visibility of the particles that appear all over the background in the ship or UFO gamemode.", 
                true, ModuleCategory::Particles, [](bool _){})));
        modules.insert(std::pair<std::string, Module*>("particle_death_effect", new Module(
                "Death Effect",
                "Toggles the visibility of the particles that appear when the player dies.", 
                true, ModuleCategory::Particles, [](bool _){})));
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

        pSpeedModified = 0;
        pGravityModified = 0;
        pYStartModified = 0;
    }

    HaxManager(const HaxManager&) = delete;
    HaxManager& operator=(const HaxManager&) = delete;
    HaxManager(HaxManager&&) = delete;
    HaxManager& operator=(HaxManager&&) = delete;
};