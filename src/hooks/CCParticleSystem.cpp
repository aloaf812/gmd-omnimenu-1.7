#include "hook.hpp"

void (*TRAM_CCParticleSystem_resumeSystem)(CCParticleSystem* self);
void CCParticleSystem_resumeSystem(CCParticleSystem* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("no_particles")) {
        self->stopSystem();
        self->setVisible(false);
        return;
    }
    TRAM_CCParticleSystem_resumeSystem(self);
}
void (*TRAM_CCParticleSystem_resetSystem)(CCParticleSystem* self);
void CCParticleSystem_resetSystem(CCParticleSystem* self) {
    TRAM_CCParticleSystem_resetSystem(self);
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("no_particles")) {
        self->stopSystem();
        self->setVisible(false);
        return;
    }
}
CCParticleSystem* (*TRAM_CCParticleSystem_create)(char const* file);
CCParticleSystem* CCParticleSystem_create(char const* file) {
    HaxManager& hax = HaxManager::sharedState();
    CCParticleSystem* particles = TRAM_CCParticleSystem_create(file);
    if (hax.getModuleEnabled("no_particles")) {
        particles->stopSystem();
        particles->setVisible(false);
    }
    return particles;
}

void CCParticleSystem_om() {
    Omni::hook("_ZN7cocos2d16CCParticleSystem12resumeSystemEv",
        reinterpret_cast<void*>(CCParticleSystem_resumeSystem),
        reinterpret_cast<void**>(&TRAM_CCParticleSystem_resumeSystem));
    Omni::hook("_ZN7cocos2d16CCParticleSystem6createEPKc",
        reinterpret_cast<void*>(CCParticleSystem_create),
        reinterpret_cast<void**>(&TRAM_CCParticleSystem_create));
    Omni::hook("_ZN7cocos2d16CCParticleSystem11resetSystemEv",
        reinterpret_cast<void*>(CCParticleSystem_resumeSystem),
        reinterpret_cast<void**>(&TRAM_CCParticleSystem_resumeSystem));
}