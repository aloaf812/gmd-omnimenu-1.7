// Credit to akqanile/Adelfa

#include "cocos2d.h"

static CCSize targetResolution = CCSizeMake(480, 320);

float getPixelsInCocosUnit(int width, int height) {
    float scaleW = static_cast<float>(width) / targetResolution.width;
    float scaleH = static_cast<float>(height) / targetResolution.height;

    return std::min(scaleW, scaleH);
}

static float standardDefinition = getPixelsInCocosUnit(1600, 900); // my waydroid resolution

float scaleFontSize(float fontSize) {
    static CCSize frameSize = CCDirector::sharedDirector()->getOpenGLView()->getFrameSize();
    return fontSize *
        (standardDefinition / getPixelsInCocosUnit(frameSize.width, frameSize.height));
}