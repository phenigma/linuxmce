/**
 *
 * @file R_CommitTable.h
 * @brief header file for the R_CommitTable class
 * @author
 *
 */
 
#ifndef R_CommitTable_H
#define R_CommitTable_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

/**
 * @brief class modelling the commit table request
 */
 
class R_CommitTable : public RA_Request
{
public:
	/** @brief Request Variables */
	string m_sTableName;
	vector<string> *m_pvectFields;

	/** @brief Response Variables */

	bool m_bDeleteVectFields; /** @brief In case we create m_pvectFields ourselves */

	/** @brief The call will call this constructor, then ConvertRequestToBinary */
	R_CommitTable( string sTableName, vector<string> *pvectFields );

	/**  @brief The server will call this constructor, then ProcessRequest */
	R_CommitTable( );

	/** @brief destructor*/
	~R_CommitTable( );

	/**
	 * @brief Returns the id of the table @todo ask
	 */
	
	virtual unsigned long ID( ) { return R_COMMIT_TABLE; }

	/**
	 * @brief Sets up the serialization request
	 */
	
	void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_sTableName + *m_pvectFields;
	}

	/**
	 * @brief This will return true if the request is processed properly
	 */
	 
	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

