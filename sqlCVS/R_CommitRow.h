#ifndef R_CommitRow_H
#define R_CommitRow_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

#define SERIALIZE_DATA_TYPE_CHANGED_ROW		1001

namespace sqlCVS
{
	class ChangedRow;
}

class R_CommitRow : public RA_Request
{
public:
	// Request Variables
	int m_psc_id, m_psc_batch, m_psc_user;
	int m_iOriginalAutoIncrID;
	int m_eTypeOfChange;
	vector<string> m_vectValues;

	// Response Variables
	int m_iNewAutoIncrID,m_psc_id_new,m_psc_batch_new;

	// The server will call this constructor, then ProcessRequest
	R_CommitRow(sqlCVS::ChangedRow *pChangedRow);
	R_CommitRow() {};

	virtual unsigned long ID() { return R_COMMIT_ROW; }

	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_psc_id + m_psc_batch + m_psc_user +
			m_iOriginalAutoIncrID + m_vectValues + m_eTypeOfChange;
	}

	virtual void SetupSerialization_Response()
	{
		RA_Request::SetupSerialization_Response();
		StartSerializeList() + m_iNewAutoIncrID + m_psc_id_new + m_psc_batch_new;
	}

	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif

