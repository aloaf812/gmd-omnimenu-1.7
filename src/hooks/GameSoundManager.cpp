#include "hook.hpp"
#include "GameSoundManager.hpp"

void (*TRAM_GameSoundManager_preload)(GameSoundManager* self);
void GameSoundManager_preload(GameSoundManager* self) {
    TRAM_GameSoundManager_preload(self);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("pih.mp3");
}

void GameSoundManager_om() {
#ifdef PING_SPOOFING
    Omni::hook("_ZN16GameSoundManager7preloadEv",
        reinterpret_cast<void*>(GameSoundManager_preload),
        reinterpret_cast<void**>(&TRAM_GameSoundManager_preload));
#endif
}