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

// Safe Mode: achievements (+ main level %)
// GameManager::reportPercentageForLevel is unhookable due to some bug in Dobby
void (*TRAM_GameManager_reportAchievementWithID)(void* self, const char* ach, int percent);
void GameManager_reportAchievementWithID(void* self, const char* ach, int percent) {
    HaxManager& hax = HaxManager::sharedState();
    std::string s = std::string(ach);
    if (hax.isSafeMode() && (
        s.rfind("geometry.ach.level", 0) == 0 ||
        s.rfind("geometry.ach.demon", 0) == 0 ||
        s.rfind("geometry.ach.stars", 0) == 0 ||
        s.rfind("geometry.ach.coins", 0) == 0 ||
        s.rfind("geometry.ach.custom", 0) == 0 ||
        !strcmp(ach, "geometry.ach.special01")
    )) {
        return;
    }
    TRAM_GameManager_reportAchievementWithID(self, ach, percent);
}
// void (*TRAM_GameManager_createAndAddParticle)(void* self, int a1, const char* file, int a2, tCCPositionType a3);
// void GameManager_createAndAddParticle(void* self, int a1, const char* file, int a2, tCCPositionType a3) { 
//     HaxManager& hax = HaxManager::sharedState();
//     if (!hax.getModuleEnabled("particle_end_wall") && !strcmp(file, "endEffectPortal.plist")) {
//         return;
//     }
//     TRAM_GameManager_createAndAddParticle(self, a1, file, a2, a3);
// }
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
    // Omni::hook("_ZN10GameObject20createAndAddParticleEiPKciN7cocos2d15tCCPositionTypeE",
    //     reinterpret_cast<void*>(GameManager_createAndAddParticle),
    //     reinterpret_cast<void**>(&TRAM_GameManager_createAndAddParticle));
}