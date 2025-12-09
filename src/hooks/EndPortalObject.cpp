#include "hook.hpp"

void (*TRAM_EndPortalObject_triggerObject)(void* self);
void EndPortalObject_triggerObject(void* self) {
    TRAM_EndPortalObject_triggerObject(self);
    HaxManager& hax = HaxManager::sharedState();
    hax.completed = true;
}

void EndPortalObject_om() {
    Omni::hook("_ZN15EndPortalObject13triggerObjectEv",
        reinterpret_cast<void*>(EndPortalObject_triggerObject),
        reinterpret_cast<void**>(&TRAM_EndPortalObject_triggerObject));
}