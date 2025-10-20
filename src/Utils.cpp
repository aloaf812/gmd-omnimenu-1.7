#include "cocos2d.h"

// Credit to akqanile/Adelfa
static CCSize targetResolution = CCSizeMake(480, 320);

// Credit to akqanile/Adelfa
float getPixelsInCocosUnit(int width, int height) {
    float scaleW = static_cast<float>(width) / targetResolution.width;
    float scaleH = static_cast<float>(height) / targetResolution.height;

    return std::min(scaleW, scaleH);
}

static float standardDefinition = getPixelsInCocosUnit(1600, 900); // my waydroid resolution

// Credit to akqanile/Adelfa
float scaleFontSize(float fontSize) {
    static CCSize frameSize = CCDirector::sharedDirector()->getOpenGLView()->getFrameSize();
    return fontSize *
        (standardDefinition / getPixelsInCocosUnit(frameSize.width, frameSize.height));
}

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