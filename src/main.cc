#include "hooks.cpp"
#include "JniHook.cpp"

[[gnu::constructor]]
int main() {
    srand(time(0));
    initialize_hooks();
}
