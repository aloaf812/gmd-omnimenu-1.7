#include "hooks.cpp"
#include "JniHook.cpp"

[[gnu::constructor]]
int main() {
#if defined(NP4) && GAME_VERSION != GV_1_4
    #error "Building for Neopointfour while GAME_VERSION is not 1.4"
#endif
    initialize_hooks();
}