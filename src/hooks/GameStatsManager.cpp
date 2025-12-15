#include "hook.hpp"
#include "GJGameLevel.hpp"

#if GAME_VERSION < GV_1_3
// Safe Mode: level completion stat
void (*TRAM_GameStatsManager_completedLevel)(void* self, int id, bool official);
void GameStatsManager_completedLevel(void* self, int id, bool official) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.isSafeMode()) return;
    TRAM_GameStatsManager_completedLevel(self, id, official);
}
#else
// Safe Mode: level completion stat & star/demon awards
void (*TRAM_GameStatsManager_completedLevel)(void* self, GJGameLevel* level);
void GameStatsManager_completedLevel(void* self, GJGameLevel* level) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.isSafeMode()) return;
    TRAM_GameStatsManager_completedLevel(self, level);
}
#endif

#if GAME_VERSION >= GV_1_6
// Safe Mode: map pack completion
void (*TRAM_GameStatsManager_completedMapPack)(void* self, void* mapPack);
void GameStatsManager_completedMapPack(void* self, void* mapPack) {
    HaxManager& hax = HaxManager::sharedState();
    /*
    in FASM builds, you won't be able to complete the levels with cheats to get the mappack to be completeable anyways.
    and it shouldn't prevent from being collected if you have cheats and auto safe mode enabled on non-FASM builds, 
    since this function gets fired in the menu.

    which is why i check for safe_mode only.
    */
    if (hax.getModuleEnabled(ModuleID::SAFE_MODE)) return;
    TRAM_GameStatsManager_completedMapPack(self, mapPack);
}
#endif

void GameStatsManager_om() {
    Omni::hook(
#if GAME_VERSION < GV_1_3
        "_ZN16GameStatsManager14completedLevelEib",
#else
        "_ZN16GameStatsManager14completedLevelEP11GJGameLevel",
#endif
        reinterpret_cast<void*>(GameStatsManager_completedLevel),
        reinterpret_cast<void**>(&TRAM_GameStatsManager_completedLevel));
#if GAME_VERSION >= GV_1_6
    Omni::hook("_ZN16GameStatsManager16completedMapPackEP9GJMapPack",
        reinterpret_cast<void*>(GameStatsManager_completedMapPack),
        reinterpret_cast<void**>(&TRAM_GameStatsManager_completedMapPack));
#endif
}