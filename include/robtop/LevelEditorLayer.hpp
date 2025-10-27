#pragma once

#include <cocos2d.h>
#include "GJGameLevel.hpp"
#include "GameObject.hpp"
#include "UndoObject.hpp"

class LevelEditorLayer : public cocos2d::CCLayer
{
public:
    static LevelEditorLayer* create(GJGameLevel* level);
    bool init(GJGameLevel* level);

    void addToSection(GameObject* object);
    void addToUndoList(UndoObject*);

    virtual int getObjectCount();
};
