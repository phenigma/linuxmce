#ifndef VA_ShowMenu_H
#define VA_ShowMenu_H

#include "RA/RA_Action.h"
#include "PlutoVIPRequests.h"

class VA_SendMenuToPhone : public RA_Action
{
public:
	PlutoDataBlock m_pdbMenu;
	u_int64_t m_iMacAddress;

	VA_SendMenuToPhone(string Filename, u_int64_t MacAddress);
	VA_SendMenuToPhone() { m_pdbMenu.m_pBlock = NULL; m_pdbMenu.m_dwSize = 0;}

	unsigned long ID() { return ACTION_SENDMENU_TOPHONE; }
	virtual void SetupSerialization(int iSC_Version)
	{
		RA_Action::SetupSerialization(int iSC_Version);
		StartSerializeList() + m_pdbMenu + m_iMacAddress;
	}

    virtual void ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor);
};


#endif

