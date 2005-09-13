#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "MenuNode.h"
#include "MenuStructure.h"
#include "VFD_LCD_Base.h"

using namespace DCE;

void MenuNode::Render(vector<string> *str)
{
	m_iFirstDrawnEntry=m_iSelectedEntry;
	for(size_t s=m_iSelectedEntry;s<m_vectMenuNode.size() && s<m_iFirstDrawnEntry+m_pMenuStructure->m_pVFD_LCD_Base->iNumLines_get();++s)
	{
		MenuNode *pMenuNode = m_vectMenuNode[s];
		string sText = " " + pMenuNode->GetText();
		if( s==m_iSelectedEntry )
			sText[0] = '>';

		if( sText.size() > m_pMenuStructure->m_pVFD_LCD_Base->iNumVisibleColums_get()-1 )
			sText = sText.substr(0,m_pMenuStructure->m_pVFD_LCD_Base->iNumVisibleColums_get()-1);

		if( sText.size() < m_pMenuStructure->m_pVFD_LCD_Base->iNumVisibleColums_get()-1 )
			sText += string("                                                     ").substr(0,m_pMenuStructure->m_pVFD_LCD_Base->iNumVisibleColums_get()-1-sText.size());

		if( pMenuNode->CanGoRight() )
			sText += ">";

		str->push_back( sText );
	}
}

void MenuNode::Up()
{
	if( m_iSelectedEntry>0 )
	{
		m_iSelectedEntry--;
		Rerender();
	}
}

void MenuNode::Down()
{
	if( m_iSelectedEntry<m_vectMenuNode.size() )
	{
		m_iSelectedEntry++;
		Rerender();
	}
}

void MenuNode::Left()
{
	m_pMenuNode_GoBack;
	m_pMenuStructure->m_pVFD_LCD_Base->SetMenuNode_Current(m_pMenuNode_GoBack);
	Rerender();
}

void MenuNode::Right()
{
	if( m_iSelectedEntry>=0 && m_iSelectedEntry<m_vectMenuNode.size() )
	{
		MenuNode *pMenuNode = m_vectMenuNode[m_iSelectedEntry];
		pMenuNode->m_pMenuNode_GoBack=this;
		m_pMenuStructure->m_pVFD_LCD_Base->SetMenuNode_Current(pMenuNode);
		Rerender();
	}
}

void MenuNode::Enter()
{
}

void MenuNode::Back()
{
}

void MenuNode::Home()
{
}

void MenuNode::Rerender()
{
	m_pMenuStructure->m_pVFD_LCD_Base->WakeUpThread();
}

