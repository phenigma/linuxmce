#include "InputMenuItem.h"
#include "PlutoUtils/StringUtils.h"
//--------------------------------------------------------------------------------------------------------
#define EMPTY_CHAR '_'
#define MASK_CHAR  'x'
//--------------------------------------------------------------------------------------------------------
InputMenuItem::InputMenuItem(string sDescription, MenuItem *pParent, ItemType type, MenuItemAction *pMenuItemAction, string sMask) :
	MenuItem(sDescription, pParent, type, pMenuItemAction),	
	m_sMask(sMask), m_nCaretIndex(0), m_nOptionIndex(0), m_csCaretOptions("_0123456789")
{
	m_sDescription = m_sMask;
	m_sDescription = StringUtils::Replace(m_sDescription, string() + MASK_CHAR, string() + EMPTY_CHAR);
}
//--------------------------------------------------------------------------------------------------------
InputMenuItem::~InputMenuItem()
{
}
//--------------------------------------------------------------------------------------------------------
string InputMenuItem::Mask()
{
	return m_sMask;
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoRight()
{
	return true;
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoLeft()
{
	return true;
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoUp()
{
	return true;
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoDown()
{
	return true;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveUp()
{
	if(!m_sMask.empty() && m_sMask[m_nCaretIndex] == MASK_CHAR)
	{
		m_nOptionIndex = (m_csCaretOptions.length() + m_nOptionIndex - 1) % m_csCaretOptions.length();

		if(!m_sDescription.empty())
			m_sDescription[m_nCaretIndex] = m_csCaretOptions[m_nOptionIndex];
	}

	return this;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveDown()
{
	if(!m_sMask.empty() && m_sMask[m_nCaretIndex] == MASK_CHAR)
	{
		m_nOptionIndex = (m_nOptionIndex + 1) % m_csCaretOptions.length();

		if(!m_sDescription.empty())
			m_sDescription[m_nCaretIndex] = m_csCaretOptions[m_nOptionIndex];
	}

	return this;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveRight()
{
	m_nCaretIndex = (m_nCaretIndex + 1) % m_sDescription.length();
	return this;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveLeft()
{
	if(m_nCaretIndex > 0)
	{
		m_nCaretIndex = (m_sDescription.length() + m_nCaretIndex - 1) % m_sDescription.length();
		return this;
	}

	return MenuItem::MoveLeft();
}
//--------------------------------------------------------------------------------------------------------
