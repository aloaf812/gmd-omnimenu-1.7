#include "hook.hpp"

CCParticleSystemQuad* (*TRAM_CCParticleSystemQuad_create)(const char* file);
CCParticleSystemQuad* CCParticleSystemQuad_create(const char* file) {
    CCParticleSystemQuad* particle = TRAM_CCParticleSystemQuad_create(file);
    HaxManager& hax = HaxManager::sharedState();
    if (!hax.getModuleEnabled("particle_death_effect") && !strcmp(file, "explodeEffect.plist")) {
        particle->setVisible(false);
        return particle;
    }
    if (!hax.getModuleEnabled("particle_fireworks") && !strcmp(file, "firework.plist")) {
        particle->setVisible(false);
        return particle;
    }
    if (!hax.getModuleEnabled("particle_level_completion") && !strcmp(file, "levelComplete01.plist")) {
        particle->setVisible(false);
        particle->stopSystem();
        return particle;
    }
    return particle;
}

void CCParticleSystemQuad_om() {
    Omni::hook("_ZN7cocos2d20CCParticleSystemQuad6createEPKc",
        reinterpret_cast<void*>(CCParticleSystemQuad_create),
        reinterpret_cast<void**>(&TRAM_CCParticleSystemQuad_create));
}