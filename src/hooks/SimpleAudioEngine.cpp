#include "hook.hpp"
#include "Utils.hpp"

void (*TRAM_setBackgroundMusicTimeJNI)(float time);
void setBackgroundMusicTimeJNI(float time) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("music_bug_fix")) {
        seekBackgroundMusicTo(floorf(time * 1000));
    } else TRAM_setBackgroundMusicTimeJNI(time);
}

void SimpleAudioEngine_om() {
    Omni::hook("setBackgroundMusicTimeJNI",
        reinterpret_cast<void*>(setBackgroundMusicTimeJNI),
        reinterpret_cast<void**>(&TRAM_setBackgroundMusicTimeJNI));
}