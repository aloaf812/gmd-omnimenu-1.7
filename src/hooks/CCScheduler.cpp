#include "hook.hpp"

// void (*TRAM_CCDirector_drawScene)(CCDirector* self);
// void CCDirector_drawScene(CCDirector* self) {
//     TRAM_CCDirector_drawScene(self);
//     HaxManager& hax = HaxManager::sharedState();
//     self->m_uFrames++;
//     self->m_fAccumDt += self->m_fDeltaTime;
//     if (self->m_fAccumDt > 0.25) {
//         self->m_fFrameRate = self->m_uFrames / self->m_fAccumDt;
//         hax.fps = self->m_fFrameRate;
//         self->m_uFrames = 0;
//         self->m_fAccumDt = 0;
//     }
// }

// void CCDirector_om() {
//     Omni::hook("_ZN7cocos2d10CCDirector9drawSceneEv",
//         reinterpret_cast<void*>(CCDirector_drawScene),
//         reinterpret_cast<void**>(&TRAM_CCDirector_drawScene));
// }

void (*TRAM_CCScheduler_update)(CCScheduler* self, float dt);
void CCScheduler_update(CCScheduler* self, float dt) {
    HaxManager& hax = HaxManager::sharedState();
    hax.frames++;
    hax.fpsCounter += dt;
    if (hax.fpsCounter >= 1) {
        hax.fps = hax.frames;
        hax.frames = 0;
        hax.fpsCounter = 0;
    }
    TRAM_CCScheduler_update(self, dt);
}

void CCScheduler_om() {
    Omni::hook("_ZN7cocos2d11CCScheduler6updateEf",
        reinterpret_cast<void*>(CCScheduler_update),
        reinterpret_cast<void**>(&TRAM_CCScheduler_update));
}