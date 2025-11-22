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

void GameStatsManager_om() {
    Omni::hook(
#if GAME_VERSION < GV_1_3
        "_ZN16GameStatsManager14completedLevelEib",
#else
        "_ZN16GameStatsManager14completedLevelEP11GJGameLevel",
#endif
        reinterpret_cast<void*>(GameStatsManager_completedLevel),
        reinterpret_cast<void**>(&TRAM_GameStatsManager_completedLevel));
}