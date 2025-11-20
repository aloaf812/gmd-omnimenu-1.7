#pragma once

#include <cocos2d.h>
#include "CCTextInputNode.hpp"

#define RED_INPUT_TAG 1056817
#define GREEN_INPUT_TAG 2056817
#define BLUE_INPUT_TAG 3056817
#define RGBCOLORINPUTS_TAG 63584276

class RGBColorInputs : public CCLayer, public TextInputDelegate {
public:
    cocos2d::extension::CCControlColourPicker* parent;
    CCTextInputNode* redInput;
    CCTextInputNode* greenInput;
    CCTextInputNode* blueInput;
    bool ignore;
    bool dontChange;

    static RGBColorInputs* create(cocos2d::extension::CCControlColourPicker* parent);
    bool init(cocos2d::extension::CCControlColourPicker* parent);
    
    // must be called after create
    void setupDelegates();
    void createTheThings();

    virtual void textChanged(CCTextInputNode* input) override;
};