#ifndef R_CommitChanges_H
#define R_CommitChanges_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

class R_CommitChanges : public RA_Request
{
public:
	// Request Variables
	map<int,string> m_mapUsersPasswords;  // All the users who are checking in this session
	vector<string> m_vectTables; // The tables we will be checking in this session
	string m_sRepository;

	// Response Variables

	R_CommitChanges(string sRepository);
	R_CommitChanges() {};

	virtual unsigned long ID() { return R_COMMIT_CHANGES; }

	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_sRepository;
	}

	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif

