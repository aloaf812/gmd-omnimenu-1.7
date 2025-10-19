#include "hook.hpp"

// Safe Mode: level completion stat
void (*TRAM_GameStatsManager_completedLevel)(void* self, int id, bool official);
void GameStatsManager_completedLevel(void* self, int id, bool official) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.isSafeMode()) return;
    TRAM_GameStatsManager_completedLevel(self, id, official);
}

void GameStatsManager_om() {
    Omni::hook("_ZN16GameStatsManager14completedLevelEib",
        reinterpret_cast<void*>(GameStatsManager_completedLevel),
        reinterpret_cast<void**>(&TRAM_GameStatsManager_completedLevel));
}