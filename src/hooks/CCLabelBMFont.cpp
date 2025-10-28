#include "hook.hpp"

#define CC_CONTENT_SCALE_FACTOR_ROBTOP() CCDirector::sharedDirector()->getContentScaleFactorHD()
#define CC_RECT_PIXELS_TO_POINTS_ROBTOP(__rect_in_pixels__)                                                                        \
    CCRectMake( (__rect_in_pixels__).origin.x / CC_CONTENT_SCALE_FACTOR_ROBTOP(), (__rect_in_pixels__).origin.y / CC_CONTENT_SCALE_FACTOR_ROBTOP(),    \
            (__rect_in_pixels__).size.width / CC_CONTENT_SCALE_FACTOR_ROBTOP(), (__rect_in_pixels__).size.height / CC_CONTENT_SCALE_FACTOR_ROBTOP() )
#define CC_POINT_PIXELS_TO_POINTS_ROBTOP(__pixels__)                                                                        \
    CCPointMake( (__pixels__).x / CC_CONTENT_SCALE_FACTOR_ROBTOP(), (__pixels__).y / CC_CONTENT_SCALE_FACTOR_ROBTOP())
#define CC_SIZE_PIXELS_TO_POINTS_ROBTOP(__size_in_pixels__)                                                                        \
    CCSizeMake( (__size_in_pixels__).width / CC_CONTENT_SCALE_FACTOR_ROBTOP(), (__size_in_pixels__).height / CC_CONTENT_SCALE_FACTOR_ROBTOP())

int cc_wcslen(const unsigned short* str)
{
    int i=0;
    while(*str++) i++;
    return i;
}


void (*TRAM_CCLabelBMFont_createFontChars)(CCLabelBMFont* self);
void CCLabelBMFont_createFontChars(CCLabelBMFont* self) {
    HaxManager& hax = HaxManager::sharedState();
    if (hax.getModuleEnabled("font_offset_fix")) {
        int nextFontPositionX = 0;
        int nextFontPositionY = 0;
        unsigned short prev = -1;
        int kerningAmount = 0;

        CCSize tmpSize = CCSizeZero;

        int longestLine = 0;
        unsigned int totalHeight = 0;

        unsigned int quantityOfLines = 1;

        unsigned int stringLen = cc_wcslen(self->m_sString);
        if (stringLen == 0)
        {
            return;
        }

        for (unsigned int i = 0; i < stringLen - 1; ++i)
        {
            unsigned short c = self->m_sString[i];
            if (c == '\n')
            {
                quantityOfLines++;
            }
        }

        totalHeight = self->m_pConfiguration->m_nCommonHeight * quantityOfLines;
        nextFontPositionY = 0-(self->m_pConfiguration->m_nCommonHeight - self->m_pConfiguration->m_nCommonHeight * quantityOfLines);

        for (unsigned int i= 0; i < stringLen; i++)
        {
            unsigned short c = self->m_sString[i];

            if (c == '\n')
            {
                nextFontPositionX = 0;
                nextFontPositionY -= self->m_pConfiguration->m_nCommonHeight;
                continue;
            }

            tFontDefHashElement *element = NULL;

            // unichar is a short, and an int is needed on HASH_FIND_INT
            unsigned int key = c;
            HASH_FIND_INT(self->m_pConfiguration->m_pFontDefDictionary, &key, element);
            CCAssert(element, "FontDefinition could not be found!");

            ccBMFontDef fontDef = element->fontDef;

            CCRect rect = fontDef.rect;
            rect = CC_RECT_PIXELS_TO_POINTS_ROBTOP(rect);

            rect.origin.x += self->m_tImageOffset.x;
            rect.origin.y += self->m_tImageOffset.y;

            CCSprite *fontChar;

            fontChar = (CCSprite*)(self->getChildByTag(i));
            if( ! fontChar )
            {
                fontChar = new CCSprite();
                fontChar->initWithTexture(self->getTextureAtlas()->getTexture(), rect);
                self->addChild(fontChar, 0, i);
                fontChar->release();
            }
            else
            {
                // reusing fonts
                fontChar->setTextureRect(rect, false, rect.size);

                // restore to default in case they were modified
                fontChar->setVisible(true);
                fontChar->setOpacity(255);
            }

            // See issue 1343. cast( signed short + unsigned integer ) == unsigned integer (sign is lost!)
            int yOffset = self->m_pConfiguration->m_nCommonHeight - fontDef.yOffset;
            CCPoint fontPos = ccp( (float)nextFontPositionX + fontDef.xOffset + fontDef.rect.size.width*0.5f + kerningAmount,
                (float)nextFontPositionY + yOffset - rect.size.height*0.5f * CC_CONTENT_SCALE_FACTOR_ROBTOP() ); // this is where the fix is. robtop forgot to change this one call from getContentScaleFactor to getContentScaleFactorHD
            fontChar->setPosition(CC_POINT_PIXELS_TO_POINTS_ROBTOP(fontPos));

            // update kerning
            nextFontPositionX += fontDef.xAdvance + kerningAmount;
            prev = c;

            // Apply label properties
            fontChar->setOpacityModifyRGB(self->m_bIsOpacityModifyRGB);
            // Color MUST be set before opacity, since opacity might change color if OpacityModifyRGB is on
            fontChar->setColor(self->getColor());

            // only apply opaccity if it is different than 255 )
            // to prevent modifying the color too (issue #610)
            if( self->getOpacity() != 255 )
            {
                fontChar->setOpacity(self->getOpacity());
            }

            if (longestLine < nextFontPositionX)
            {
                longestLine = nextFontPositionX;
            }
        }

        tmpSize.width  = (float) longestLine;
        tmpSize.height = (float) totalHeight;

        self->setContentSize(CC_SIZE_PIXELS_TO_POINTS_ROBTOP(tmpSize));
    } else TRAM_CCLabelBMFont_createFontChars(self);
}

void CCLabelBMFont_om() {
#if GAME_VERSION < GV_1_5
    Omni::hook("_ZN7cocos2d13CCLabelBMFont15createFontCharsEv",
        reinterpret_cast<void*>(CCLabelBMFont_createFontChars),
        reinterpret_cast<void**>(&TRAM_CCLabelBMFont_createFontChars));
#endif
}