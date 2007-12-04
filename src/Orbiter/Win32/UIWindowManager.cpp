#include "UIWindowManager.h"
//-----------------------------------------------------------------------------------------------------
UIWindowBase *UIWindowManager::m_pUIWindowBase = NULL;
//-----------------------------------------------------------------------------------------------------
/*static*/ UIWindowBase *UIWindowManager::CreateUIWindow(UIWindowType type) 
{
	switch(type)
	{
		case uwtMasked:
			m_pUIWindowBase = new UIWindowMasked();
			break;

		case uwtAlphaBlended:
			m_pUIWindowBase = new UIWindowAlphaBlended();
			break;

		case uwtNormal:
		default:
			m_pUIWindowBase = new UIWindowNormal();
			break;
	}

	return m_pUIWindowBase;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ UIWindowBase *UIWindowManager::UIWindow()
{
	return m_pUIWindowBase;
}
//-----------------------------------------------------------------------------------------------------
