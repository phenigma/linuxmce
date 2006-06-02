#ifndef __ZORDER_H__
#define __ZORDER_H__

#include "utilities/linux/window_manager/WMController/WindowLayer.h"
#include "pluto_main/Define_DesignObj.h"

//temporary : to be defined in the database
#define DESIGNOBJTYPE_Orbiter_Popup_CONST

class ZOrder
{
	static WindowLayer GetLayer(int nObjectType)
	{
		if(Simulator::GetInstance()->m_nUI == 4)
		{
			switch(nObjectType)
			{
				case DESIGNOBJTYPE_App_Desktop_CONST:
					return LayerBelow;				
				case DESIGNOBJTYPE_wxWidgets_Applet_CONST:
					return LayerNormal;
				case DESIGNOBJTYPE_Orbiter_Popup_CONST
					return LayerAbove;
					
				default:
					return LayerBelow;
			}
		}
		else
		{
			switch(nObjectType)
			{
				case DESIGNOBJTYPE_App_Desktop_CONST:
					return LayerNormal;
				case DESIGNOBJTYPE_wxWidgets_Applet_CONST:
					return LayerNormal;
				case DESIGNOBJTYPE_Orbiter_Popup_CONST
					return LayerAbove;
					
				default:
					return LayerBelow;
			}			
		}
	}
	
	return LayerUnknown;
};

#endif