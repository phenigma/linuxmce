#ifndef R_UpdateTable_H
#define R_UpdateTable_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

class R_UpdateTable : public RA_Request
{
public:
	// Request Variables
	string m_sTableName;
	vector<string> *m_pvectFields;
	int m_psc_batch_last_sync,m_psc_id_last_sync;

	// Response Variables

	bool m_bDeleteVectFields; // In case we create m_pvectFields ourselves

	// The call will call this constructor, then ConvertRequestToBinary
	R_UpdateTable(string sTableName,int psc_batch_last_sync,int psc_id_last_sync,vector<string> *pvectFields);

	// The server will call this constructor, then ProcessRequest
	R_UpdateTable();

	~R_UpdateTable();

	virtual unsigned long ID() { return R_UPDATE_TABLE; }

	void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_sTableName + *m_pvectFields + m_psc_batch_last_sync + m_psc_id_last_sync;
	}

	void SetupSerialization_Response()
	{
		RA_Request::SetupSerialization_Response();
		StartSerializeList() + m_psc_batch_last_sync + m_psc_id_last_sync;
	}

	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif

