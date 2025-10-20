#pragma once
#include <cocos2d.h>

// Credit to akqanile/Adelfa
float getPixelsInCocosUnit(int width, int height);
// Credit to akqanile/Adelfa
float scaleFontSize(float fontSize);
CCSprite* createInfoSprite();
ccColor3B hsvToRgb(float h, float s, float v);