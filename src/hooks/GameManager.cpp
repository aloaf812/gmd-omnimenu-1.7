#include "hook.hpp"
#include "GameManager.hpp"

bool (*TRAM_GameManager_isColorUnlocked)(GameManager* self, int idx, bool secondary);
bool GameManager_isColorUnlocked(GameManager* self, int idx, bool secondary) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("unlock_icons")) {
        return true;
    } else return TRAM_GameManager_isColorUnlocked(self, idx, secondary);
}
bool (*TRAM_GameManager_isIconUnlocked)(GameManager* self, int idx);
bool GameManager_isIconUnlocked(GameManager* self, int idx) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("unlock_icons")) {
        return true;
    } else return TRAM_GameManager_isIconUnlocked(self, idx);
}
// void (*TRAM_GameManager_reportPercentageForLevel)(GameManager* self, int level, int percentage, bool practice);
// void GameManager_reportPercentageForLevel(GameManager* self, int level, int percentage, bool practice) {
//     HaxManager& hax = HaxManager::sharedState();
//     if (hax.isSafeMode()) return;
//     cocos2d::CCLog("i have to put something here at least");
//     TRAM_GameManager_reportPercentageForLevel(self, level, percentage, practice);
// }

// Safe Mode: main level %
// GameManager::reportPercentageForLevel is unhookable due to some bug in Dobby
void (*TRAM_GameManager_reportAchievementWithID)(void* self, const char* ach, int percent);
void GameManager_reportAchievementWithID(void* self, const char* ach, int percent) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.isSafeMode() && (
        !strcmp(ach, "geometry.ach.level01a") || 
        !strcmp(ach, "geometry.ach.level02a") || 
        !strcmp(ach, "geometry.ach.level03a") || 
        !strcmp(ach, "geometry.ach.level04a") || 
        !strcmp(ach, "geometry.ach.level05a") || 
        !strcmp(ach, "geometry.ach.level06a") || 
        !strcmp(ach, "geometry.ach.level07a") || 
        !strcmp(ach, "geometry.ach.level08a") || 
        !strcmp(ach, "geometry.ach.level09a") || 
        !strcmp(ach, "geometry.ach.level10a") || 
        !strcmp(ach, "geometry.ach.level11a") || 
        !strcmp(ach, "geometry.ach.level12a") || 
        !strcmp(ach, "geometry.ach.level13a") || 
        !strcmp(ach, "geometry.ach.level14a") || 
        !strcmp(ach, "geometry.ach.level15a") || 
        !strcmp(ach, "geometry.ach.level16a") || 
        !strcmp(ach, "geometry.ach.level01b") || 
        !strcmp(ach, "geometry.ach.level02b") || 
        !strcmp(ach, "geometry.ach.level03b") || 
        !strcmp(ach, "geometry.ach.level04b") || 
        !strcmp(ach, "geometry.ach.level05b") || 
        !strcmp(ach, "geometry.ach.level06b") || 
        !strcmp(ach, "geometry.ach.level07b") || 
        !strcmp(ach, "geometry.ach.level08b") || 
        !strcmp(ach, "geometry.ach.level09b") || 
        !strcmp(ach, "geometry.ach.level10b") || 
        !strcmp(ach, "geometry.ach.level11b") || 
        !strcmp(ach, "geometry.ach.level12b") || 
        !strcmp(ach, "geometry.ach.level13b") || 
        !strcmp(ach, "geometry.ach.level14b") || 
        !strcmp(ach, "geometry.ach.level15b") || 
        !strcmp(ach, "geometry.ach.level16b")
    )) {
        return;
    }
    TRAM_GameManager_reportAchievementWithID(self, ach, percent);
}

void GameManager_om() {
    Omni::hook("_ZN11GameManager15isColorUnlockedEib",
        reinterpret_cast<void*>(GameManager_isColorUnlocked),
        reinterpret_cast<void**>(&TRAM_GameManager_isColorUnlocked));
    Omni::hook("_ZN11GameManager14isIconUnlockedEi",
        reinterpret_cast<void*>(GameManager_isIconUnlocked),
        reinterpret_cast<void**>(&TRAM_GameManager_isIconUnlocked));
    Omni::hook("_ZN11GameManager23reportAchievementWithIDEPKci",
        reinterpret_cast<void*>(GameManager_reportAchievementWithID),
        reinterpret_cast<void**>(&TRAM_GameManager_reportAchievementWithID));
}