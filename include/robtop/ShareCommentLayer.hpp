#pragma once

#include <cocos2d.h>
#include "FLAlertLayer.hpp"
#include "GJGameLevel.hpp"

class ShareCommentLayer : public FLAlertLayer
{
public:
	static ShareCommentLayer* create(GJGameLevel*);
};
