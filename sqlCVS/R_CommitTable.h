#ifndef R_CommitTable_H
#define R_CommitTable_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

class R_CommitTable : public RA_Request
{
public:
	// Request Variables
	string m_sTableName;
	vector<string> *m_pvectFields;

	// Response Variables

	bool m_bDeleteVectFields; // In case we create m_pvectFields ourselves

	// The call will call this constructor, then ConvertRequestToBinary
	R_CommitTable(string sTableName,vector<string> *pvectFields);

	// The server will call this constructor, then ProcessRequest
	R_CommitTable(unsigned long size,const char *data);

	~R_CommitTable();

	virtual unsigned long ID() { return R_COMMIT_TABLE; }

	void SetupSerialization_Request()
	{
		StartSerializeList() + m_sTableName + *m_pvectFields;
	}

	void SetupSerialization_Response()
	{
	}
	virtual bool ProcessRequest();
};


#endif

