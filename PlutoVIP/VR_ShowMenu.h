#ifndef VR_ShowMenu_H
#define VR_ShowMenu_H

#include "RA/RA_Request.h"

class VR_ShowMenu : public RA_Request
{
public:
	// Request
	// Although the input values can be specified in the menu collection, 
	// for performance the server can just keep the menu collection as a binary
	// file and manually add some overriding input variables
	MYSTL_CREATE_LIST(m_listInputVariables,VIPVariable);
	MYSTL_CREATE_LIST(m_listBasket, BasketItem); 

	// Response
	MYSTL_CREATE_LIST(m_listReturnedVariables,VIPVariable);
	u_int64_t m_iMacAddress;
	unsigned long m_iMenuCollectionID;
	unsigned char m_iMenuTermination;

	class VIPMenuCollection *m_pMenuCollection;

	// Create the Request from scratch
	VR_ShowMenu(const char *VMCFile);
	VR_ShowMenu(class VIPMenuCollection *pMenuCollection);

	// Create the request from binary data
	VR_ShowMenu(unsigned long size,const char *data);


	virtual unsigned long ID() { return VRP_REQUEST_SHOW_MENU; }
//	

	virtual bool ProcessRequest();
	virtual bool ParseResponse(unsigned long size,const char *data);

	// These call the base class and then add their output
	virtual void ConvertRequestToBinary();
	virtual void ConvertResponseToBinary();
};


#endif
