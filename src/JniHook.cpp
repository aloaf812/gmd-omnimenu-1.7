#include "HaxManager.hpp"
#include "Utils.hpp"
#include <jni.h>

extern "C" JNIEXPORT void JNICALL
Java_com_robtopx_geometryjump_GeometryJump_nativeOnFileSaveChosen(
        JNIEnv* env, jobject /*thiz*/, jstring uri) {
    const char* uriStr = env->GetStringUTFChars(uri, nullptr);
    auto& hax = HaxManager::sharedState();
    writeGMD(hax.gdShareLevel, uriStr);
    env->ReleaseStringUTFChars(uri, uriStr);
}
extern "C" JNIEXPORT void JNICALL
Java_com_robtopx_geometryjump_GeometryJump_nativeOnFileOpenChosen(
        JNIEnv* env, jobject /*thiz*/, jstring uri) {
    const char* uriStr = env->GetStringUTFChars(uri, nullptr);
    auto& hax = HaxManager::sharedState();
    hax.myLevelsLayer->loadLevel(readGMD(uriStr));
    env->ReleaseStringUTFChars(uri, uriStr);
}
extern "C" JNIEXPORT void JNICALL
Java_com_robtopx_geometryjump_GeometryJump_nativeSetActivity(
        JNIEnv* env, jobject thiz, jobject activity) {
    auto& hax = HaxManager::sharedState();
    hax.activity = env->NewGlobalRef(activity);
}
