#ifndef VR_ShowMenu_H
#define VR_ShowMenu_H

#include "RA/RA_Request.h"
#include "PlutoVIPrequests.h"
#include "VIPShared/VIPMenu.h"

#define SERIALIZE_DATA_TYPE_LIST_VAR		4010
#define SERIALIZE_DATA_TYPE_LIST_BASKET		4011

class VR_ShowMenu : public RA_Request
{
public:
	// Request
	// Although the input values can be specified in the menu collection, 
	// for performance the server can just keep the menu collection as a binary
	// file and manually add some overriding input variables
	list<VIPVariable *> m_listInputVariables;
	list<BasketItem *> m_listBasket;

	// Response
	list<VIPVariable *> m_listReturnedVariables;
	u_int64_t m_iMacAddress;
	unsigned long m_iMenuCollectionID;
	char m_iMenuTermination;

	class VIPMenuCollection *m_pMenuCollection;

	// Create the Request from scratch
	VR_ShowMenu(const char *VMCFile);
	VR_ShowMenu(class VIPMenuCollection *pMenuCollection);
	VR_ShowMenu() {}

	VR_ShowMenu &operator+ ( list<VIPVariable *> &i ) { m_listItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_LIST_VAR, (void *) &i) ); return (*this); }
	VR_ShowMenu &operator+ ( list<BasketItem *> &i ) { m_listItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_LIST_BASKET, (void *) &i) ); return (*this); }
	virtual bool UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition );

	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		(*this) + m_listInputVariables + m_listBasket;
	}
	virtual void SetupSerialization_Response()
	{
		RA_Request::SetupSerialization_Response();
		StartSerializeList() + m_iMacAddress + m_iMenuCollectionID + m_iMenuTermination;
			
		(*this) + m_listReturnedVariables; // this is serialized custom
	}

	virtual unsigned long ID() { return VRP_REQUEST_SHOW_MENU; }

	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif
