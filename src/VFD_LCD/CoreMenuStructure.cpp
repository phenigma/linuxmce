#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "CoreMenuStructure.h"

CoreMenuStructure::CoreMenuStructure(VFD_LCD_Base *pVFD_LCD_Base) 
	: MenuStructure(pVFD_LCD_Base)
{
	MenuNode *pMenuNode1,*pMenuNode2,*pMenuNode3;
	m_pMenuNode = new MenuNode(this,"Main Menu");

	m_pMenuNode->m_vectMenuNode.push_back( (pMenuNode1=new MenuNode(this,"Operating Conditions")) );
	pMenuNode1->m_vectMenuNode.push_back( (pMenuNode2=new MenuNode(this,"CPU Temperature")) );
	pMenuNode1->m_vectMenuNode.push_back( (pMenuNode2=new MenuNode(this,"CPU Load")) );
	pMenuNode1->m_vectMenuNode.push_back( (pMenuNode2=new MenuNode(this,"Most active processes")) );

	m_pMenuNode->m_vectMenuNode.push_back( (pMenuNode1=new MenuNode(this,"Maintain Storage (HDD/RAID)")) );

	m_pMenuNode->m_vectMenuNode.push_back( (pMenuNode1=new MenuNode(this,"Status Messages")) );

	m_pMenuNode->m_vectMenuNode.push_back( (pMenuNode1=new MenuNode(this,"Administrative Tasks")) );
}
