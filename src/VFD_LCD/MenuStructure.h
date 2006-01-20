#ifndef MenuStructure_h
#define MenuStructure_h

#include "MenuNode.h"

namespace DCE
{
	class MenuStructure
	{
	public:
		MenuNode *m_pMenuNode;  // The root menu node
		class VFD_LCD_Base *m_pVFD_LCD_Base;

		MenuStructure(VFD_LCD_Base *pVFD_LCD_Base) { m_pVFD_LCD_Base=pVFD_LCD_Base; } 
	};

}
#endif
