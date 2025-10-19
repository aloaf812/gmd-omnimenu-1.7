#include "hook.hpp"
#include "GJGameLevel.hpp"

// Safe Mode: custom level %
void (*TRAM_GJGameLevel_savePercentage)(GJGameLevel* self, int percentage, bool practice);
void GJGameLevel_savePercentage(GJGameLevel* self, int percentage, bool practice) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.isSafeMode()) return;
    TRAM_GJGameLevel_savePercentage(self, percentage, practice);
}
// Safe Mode: verification
void (*TRAM_GJGameLevel_setIsVerified)(GJGameLevel* self, bool isVerified);
void GJGameLevel_setIsVerified(GJGameLevel* self, bool isVerified) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.isSafeMode() && isVerified) return;
    TRAM_GJGameLevel_setIsVerified(self, isVerified);
}

void GJGameLevel_om() {
    Omni::hook("_ZN11GJGameLevel14savePercentageEib",
        reinterpret_cast<void*>(GJGameLevel_savePercentage),
        reinterpret_cast<void**>(&TRAM_GJGameLevel_savePercentage));
    Omni::hook("_ZN11GJGameLevel13setIsVerifiedEb",
        reinterpret_cast<void*>(GJGameLevel_setIsVerified),
        reinterpret_cast<void**>(&TRAM_GJGameLevel_setIsVerified));
}