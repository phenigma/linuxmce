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

	// Response Variables

	// The call will call this constructor, then ConvertRequestToBinary
	R_CommitChanges();
	R_CommitChanges(unsigned long size,const char *data) : RA_Request(size,data) {}

	virtual unsigned long ID() { return R_COMMIT_CHANGES; }

	virtual bool ProcessRequest();
};


#endif

