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
// Safe Mode: main level %
void (*TRAM_GameManager_reportPercentageForLevel)(GameManager* self, int level, int percentage, bool practice);
void GameManager_reportPercentageForLevel(GameManager* self, int level, int percentage, bool practice) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.isSafeMode()) return;
    TRAM_GameManager_reportPercentageForLevel(self, level, percentage, practice);
}

void GameManager_om() {
    Omni::hook("_ZN11GameManager15isColorUnlockedEib",
        reinterpret_cast<void*>(GameManager_isColorUnlocked),
        reinterpret_cast<void**>(&TRAM_GameManager_isColorUnlocked));
    Omni::hook("_ZN11GameManager14isIconUnlockedEi",
        reinterpret_cast<void*>(GameManager_isIconUnlocked),
        reinterpret_cast<void**>(&TRAM_GameManager_isIconUnlocked));
    Omni::hook("_ZN11GameManager24reportPercentageForLevelEiib",
        reinterpret_cast<void*>(GameManager_reportPercentageForLevel),
        reinterpret_cast<void**>(&TRAM_GameManager_reportPercentageForLevel));
}