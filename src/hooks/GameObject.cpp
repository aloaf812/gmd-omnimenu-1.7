#include "hook.hpp"

void (*TRAM_GameObject_addGlow)(void* self);
void GameObject_addGlow(void* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("no_glow")) return;
    TRAM_GameObject_addGlow(self);
}
void (*TRAM_GameObject_setOpacity)(void* self, GLubyte opacity);
void GameObject_setOpacity(void* self, GLubyte opacity) {
    TRAM_GameObject_setOpacity(self, opacity);
    HaxManager& hax = HaxManager::sharedState();
    if (!hax.getModuleEnabled("particle_objects")) {
        auto p = getObjectParticles(self);
        if (p && p != nullptr) p->stopSystem();
    }
    if (hax.getModuleEnabled("no_pulse")) {
        setObjectUseAudioScale(self, false);
    }
}
void GameObject_om() {
    Omni::hook("_ZN10GameObject7addGlowEv",
        reinterpret_cast<void*>(GameObject_addGlow),
        reinterpret_cast<void**>(&TRAM_GameObject_addGlow));
    Omni::hook("_ZN10GameObject10setOpacityEh",
        reinterpret_cast<void*>(GameObject_setOpacity),
        reinterpret_cast<void**>(&TRAM_GameObject_setOpacity));
}