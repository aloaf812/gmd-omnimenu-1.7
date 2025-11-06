#include "hooks.cpp"
#include "JniHook.cpp"

[[gnu::constructor]]
int main() {
    initialize_hooks();
}
