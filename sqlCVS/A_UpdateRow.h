#ifndef A_UpdateRow_H
#define A_UpdateRow_H

#include "RA/RA_Action.h"
#include "sqlCVSrequests.h"

class A_UpdateRow : public RA_Action
{
public:
	// Request Variables
	int m_psc_id, m_psc_batch, m_psc_user;
	vector<string> m_vectValues;

	// The server will call this constructor, then ProcessRequest
	A_UpdateRow(int psc_id, int psc_batch, int psc_user) { m_psc_id=psc_id; m_psc_batch=psc_batch; m_psc_user=psc_user;};
	A_UpdateRow() {};

	virtual unsigned long ID() { return A_UPDATE_ROW; }

	virtual void SetupSerialization()
	{
		RA_Action::SetupSerialization();
		StartSerializeList() + m_psc_id + m_psc_batch + m_psc_user +
			m_vectValues;
	}

    virtual void ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor);
};


#endif

