#include "hook.hpp"
#include "PlayerObject.hpp"

void (*TRAM_PlayerObject_activateStreak)(PlayerObject* self);
void PlayerObject_activateStreak(PlayerObject* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("no_trail")) return;
    TRAM_PlayerObject_activateStreak(self);
}

void PlayerObject_om() {
    Omni::hook("_ZN12PlayerObject14activateStreakEv",
        reinterpret_cast<void*>(PlayerObject_activateStreak),
        reinterpret_cast<void**>(&TRAM_PlayerObject_activateStreak));
}