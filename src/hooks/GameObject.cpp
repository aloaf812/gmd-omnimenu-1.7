#include "hook.hpp"

void (*TRAM_GameObject_addGlow)(void* self);
void GameObject_addGlow(void* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("no_glow")) return;
    TRAM_GameObject_addGlow(self);
}

void GameObject_om() {
    Omni::hook("_ZN10GameObject7addGlowEv",
        reinterpret_cast<void*>(GameObject_addGlow),
        reinterpret_cast<void**>(&TRAM_GameObject_addGlow));
}