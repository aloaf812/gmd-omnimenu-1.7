#pragma once

#include <cocos2d.h>
#include "GameObject.hpp"

typedef enum {
    Deletion = 0,
    Placement = 2
} UndoCommand;

class UndoObject : public cocos2d::CCNode {
public:
    static UndoObject* create(GameObject*, UndoCommand);
};
