#include "cocos2d.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "DS_Dictionary.hpp"
#include "GJGameLevel.hpp"
#include "HaxManager.hpp"

CCSprite* createInfoSprite() {
#if GAME_VERSION < 7
    return CCSprite::create("GJ_infoIcon.png");
#else
    return CCSprite::createWithSpriteFrameName("GJ_infoIcon.png");
#endif
}
ccColor3B hsvToRgb(float h, float s, float v) {
    int i = int(h / 60) % 6;
    float f = h / 60 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    float r, g, b;
    switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }
    return ccc3(r * 255, g * 255, b * 255);
}

JNIEnv* getEnv() {
    JNIEnv* env;

    if (cocos2d::JniHelper::getJavaVM()->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        cocos2d::CCLog("Failed to get the environment using GetEnv()");
        return nullptr;
    }
    if (static_cast<int>(cocos2d::JniHelper::getJavaVM()->AttachCurrentThread(&env, 0)) < 0) {
        cocos2d::CCLog("Failed to get the environment using AttachCurrentThread()");
        return nullptr;
    }

    return env;
}

void writeGMD(GJGameLevel* level, const char* uriStr) {
    HaxManager& hax = HaxManager::sharedState();
    JNIEnv* env = getEnv();
    if (!env) return;
    
    jstring jUriStr = env->NewStringUTF(uriStr);

    jclass uriClass = env->FindClass("android/net/Uri");
    jmethodID parseMethod = env->GetStaticMethodID(uriClass, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
    jobject uri = env->CallStaticObjectMethod(uriClass, parseMethod, jUriStr);

    jclass activityClass = env->GetObjectClass(hax.activity);
    jmethodID getCR = env->GetMethodID(activityClass, "getContentResolver", "()Landroid/content/ContentResolver;");
    jobject resolver = env->CallObjectMethod(hax.activity, getCR);

    jclass resolverClass = env->FindClass("android/content/ContentResolver");
    jmethodID openOut = env->GetMethodID(resolverClass, "openOutputStream", "(Landroid/net/Uri;)Ljava/io/OutputStream;");
    jobject outputStream = env->CallObjectMethod(resolver, openOut, uri);

    if (!outputStream) {
        env->DeleteLocalRef(uri);
        env->DeleteLocalRef(jUriStr);
        return;
    }

    auto dict = new DS_Dictionary();
    void* encodeWithCoder = DobbySymbolResolver(MAIN_LIBRARY, "_ZN11GJGameLevel15encodeWithCoderEP13DS_Dictionary");
    ((void(*)(GJGameLevel*, DS_Dictionary*))encodeWithCoder)(level, dict);

    const char* str = dict->saveRootSubDictToString().c_str();

    std::vector<uint8_t> data(str, str + strlen(str));

    jclass outputStreamClass = env->FindClass("java/io/OutputStream");
    jmethodID writeMethod = env->GetMethodID(outputStreamClass, "write", "([B)V");
    jmethodID closeMethod = env->GetMethodID(outputStreamClass, "close", "()V");

    jbyteArray jData = env->NewByteArray(data.size());
    env->SetByteArrayRegion(jData, 0, data.size(), reinterpret_cast<const jbyte*>(data.data()));
    env->CallVoidMethod(outputStream, writeMethod, jData);
    env->CallVoidMethod(outputStream, closeMethod);
    
    env->DeleteLocalRef(jData);
    env->DeleteLocalRef(outputStream);
    env->DeleteLocalRef(uri);
    env->DeleteLocalRef(jUriStr);
}
GJGameLevel* readGMD(const char* uriStr) {
    HaxManager& hax = HaxManager::sharedState();
    JNIEnv* env = getEnv();
    if (!env) return nullptr;
    GJGameLevel* level;
    std::vector<uint8_t> result;

    jstring jUriStr = env->NewStringUTF(uriStr);
    jclass uriClass = env->FindClass("android/net/Uri");
    jmethodID parseMethod = env->GetStaticMethodID(uriClass, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
    jobject uri = env->CallStaticObjectMethod(uriClass, parseMethod, jUriStr);

    jclass activityClass = env->GetObjectClass(hax.activity);
    jmethodID getCR = env->GetMethodID(activityClass, "getContentResolver", "()Landroid/content/ContentResolver;");
    jobject resolver = env->CallObjectMethod(hax.activity, getCR);

    jclass resolverClass = env->FindClass("android/content/ContentResolver");
    jmethodID openIn = env->GetMethodID(resolverClass, "openInputStream", "(Landroid/net/Uri;)Ljava/io/InputStream;");
    jobject inputStream = env->CallObjectMethod(resolver, openIn, uri);

    if (!inputStream) {
        env->DeleteLocalRef(uri);
        env->DeleteLocalRef(jUriStr);
        return nullptr;
    }

    jclass inputStreamClass = env->FindClass("java/io/InputStream");
    jmethodID readMethod = env->GetMethodID(inputStreamClass, "read", "([B)I");
    jmethodID closeMethod = env->GetMethodID(inputStreamClass, "close", "()V");

    const int bufSize = 8192;
    jbyteArray buffer = env->NewByteArray(bufSize);

    while (true) {
        jint bytesRead = env->CallIntMethod(inputStream, readMethod, buffer);
        if (bytesRead <= 0) break;
        jbyte* bytes = env->GetByteArrayElements(buffer, nullptr);
        result.insert(result.end(), bytes, bytes + bytesRead);
        env->ReleaseByteArrayElements(buffer, bytes, JNI_ABORT);
    }

    env->CallVoidMethod(inputStream, closeMethod);
    env->DeleteLocalRef(buffer);
    env->DeleteLocalRef(inputStream);
    env->DeleteLocalRef(uri);
    env->DeleteLocalRef(jUriStr);

    const uint8_t* rawDataPtr = result.data();
    const char* str = reinterpret_cast<const char*>(rawDataPtr);

    auto dict = new DS_Dictionary();
    if (!dict->loadRootSubDictFromString(str)) return nullptr;

    level = GJGameLevel::createWithCoder(dict);

    return level;
}
jobject getGlobalContext(JNIEnv *env)
{   
    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject activityThreadObj = env->CallStaticObjectMethod(activityThread, currentActivityThread);
    
    jmethodID getApplication = env->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");
    jobject context = env->CallObjectMethod(activityThreadObj, getApplication);
    return context;
}
void copyStringToClipboard(const char* string) {
    JNIEnv* env = getEnv();
    jclass context_cls = env->FindClass("android/content/Context");
    jfieldID clipboardService_fid = env->GetStaticFieldID(context_cls, "CLIPBOARD_SERVICE", "Ljava/lang/String;");
    jmethodID getSystemService_mid = env->GetMethodID(context_cls, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject clipboardService_obj = env->CallObjectMethod(
        getGlobalContext(env), 
        getSystemService_mid, 
        (jstring)env->GetStaticObjectField(context_cls, clipboardService_fid)
    );
    
    jstring strToCopy = env->NewStringUTF(string);
    jclass clipManager_cls = env->FindClass("android/text/ClipboardManager");
    jmethodID setText_mid = env->GetMethodID(clipManager_cls, "setText", "(Ljava/lang/CharSequence;)V");
    env->CallVoidMethod(clipboardService_obj, setText_mid, strToCopy);
}