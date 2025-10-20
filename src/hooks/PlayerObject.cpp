#include "hook.hpp"
#include "PlayerObject.hpp"

void (*TRAM_PlayerObject_activateStreak)(PlayerObject* self);
void PlayerObject_activateStreak(PlayerObject* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("no_trail")) return;
    TRAM_PlayerObject_activateStreak(self);
}
void (*TRAM_PlayerObject_updateShipRotation)(PlayerObject* self, float dt);
void PlayerObject_updateShipRotation(PlayerObject* self, float dt) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("no_rotation")) return;
    TRAM_PlayerObject_updateShipRotation(self, dt);
}
void (*TRAM_PlayerObject_runRotateAction)(PlayerObject* self);
void PlayerObject_runRotateAction(PlayerObject* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("no_rotation")) return;
    TRAM_PlayerObject_runRotateAction(self);
}
void (*TRAM_PlayerObject_runBallRotation2)(PlayerObject* self);
void PlayerObject_runBallRotation2(PlayerObject* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("no_rotation")) return;
    TRAM_PlayerObject_runBallRotation2(self);
}

void PlayerObject_om() {
    Omni::hook("_ZN12PlayerObject14activateStreakEv",
        reinterpret_cast<void*>(PlayerObject_activateStreak),
        reinterpret_cast<void**>(&TRAM_PlayerObject_activateStreak));
    Omni::hook("_ZN12PlayerObject18updateShipRotationEf",
        reinterpret_cast<void*>(PlayerObject_updateShipRotation),
        reinterpret_cast<void**>(&TRAM_PlayerObject_updateShipRotation));
    Omni::hook("_ZN12PlayerObject15runRotateActionEv",
        reinterpret_cast<void*>(PlayerObject_runRotateAction),
        reinterpret_cast<void**>(&TRAM_PlayerObject_runRotateAction));
    Omni::hook("_ZN12PlayerObject16runBallRotation2Ev",
        reinterpret_cast<void*>(PlayerObject_runBallRotation2),
        reinterpret_cast<void**>(&TRAM_PlayerObject_runBallRotation2));
}