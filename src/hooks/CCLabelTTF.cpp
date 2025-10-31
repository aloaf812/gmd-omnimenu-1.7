#include "hook.hpp"

void CCLabelTTF_updateTexture(CCLabelTTF* self)
{
    CCTexture2D *tex;
	if (self->m_tDimensions.width == 0 || self->m_tDimensions.height == 0)
    {
        tex = new CCTexture2D();
        tex->initWithString(self->m_string.c_str(), self->m_pFontName->c_str(), self->m_fFontSize * CC_CONTENT_SCALE_FACTOR_ROBTOP()) ;
    }
	else
    {
        tex = new CCTexture2D();
        tex->initWithString(self->m_string.c_str(),
                            CC_SIZE_POINTS_TO_PIXELS_ROBTOP(self->m_tDimensions), 
                            self->m_hAlignment,
                            self->m_vAlignment,
                            self->m_pFontName->c_str(),
                            self->m_fFontSize * CC_CONTENT_SCALE_FACTOR_ROBTOP());
    }
		
	// iPad ?
	//if( UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ) {
    if (CCApplication::sharedApplication().isIpad())
    {            
		if (CC_CONTENT_SCALE_FACTOR_ROBTOP() == 2)
        {
            tex->setResolutionType(kCCResolutioniPadRetinaDisplay);
        }
		else
        {
            tex->setResolutionType(kCCResolutioniPad);
        }
	}
	// iPhone ?
	else
	{
		if (CC_CONTENT_SCALE_FACTOR_ROBTOP() == 2)
        {
            tex->setResolutionType(kCCResolutioniPhoneRetinaDisplay);
        }
		else
        {
            tex->setResolutionType(kCCResolutioniPhone);
        }
	}
	
    self->setTexture(tex);
    tex->release();
	
	CCRect rect = CCRectZero;

    CCSize size;
    size.width = self->m_pobTexture->getContentSizeInPixels().width / CC_CONTENT_SCALE_FACTOR_ROBTOP();
    size.height = self->m_pobTexture->getContentSizeInPixels().height / CC_CONTENT_SCALE_FACTOR_ROBTOP();

    rect.size = size;
    self->setTextureRect(rect);
}

void CCLabelTTF_om() {
#if GAME_VERSION < GV_1_7
    Omni::hook("_ZN7cocos2d10CCLabelTTF13updateTextureEv",
        reinterpret_cast<void*>(CCLabelTTF_updateTexture),
        nullptr);
#endif
}