#ifndef R_GetAll_psc_id_H
#define R_GetAll_psc_id_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

class R_GetAll_psc_id : public RA_Request
{
public:
	// Request Variables
	string m_sTable;
	int m_psc_id;

	// Response Variables
	vector<int> m_vectAll_psc_id;

	R_GetAll_psc_id(string sTable,int psc_id);
	R_GetAll_psc_id() {};

	virtual unsigned long ID() { return R_GET_ALL_PSC_ID; }

	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_sTable + m_psc_id;
	}

	virtual void SetupSerialization_Response()
	{
		RA_Request::SetupSerialization_Response();
		StartSerializeList() + m_vectAll_psc_id;
	}

	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif

