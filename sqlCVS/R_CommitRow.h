/**
 *
 * @file R_CommitRow.h
 * @brief header file for the R_CommitRow class
 * @author
 *
 */
 
#ifndef R_CommitRow_H
#define R_CommitRow_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

#define SERIALIZE_DATA_TYPE_CHANGED_ROW		1001

namespace sqlCVS
{
	class ChangedRow;
}

/**
 * @brief class modelling  commit row requests
 * @todo complete documentation
 */
 
class R_CommitRow : public RA_Request
{
public:
	/** @brief Request Variables */
	int m_psc_id;
	int m_psc_batch;
	int m_psc_user;
	int m_iOriginalAutoIncrID;
	int m_eTypeOfChange;
	vector<string> m_vectValues;

	/** @brief Response Variables */
	int m_iNewAutoIncrID;
	int m_psc_id_new;
	int m_psc_batch_new;

	/** @brief The server will call this constructor, then ProcessRequest */
	R_CommitRow( sqlCVS::ChangedRow *pChangedRow );
	
	/** @brief constructor */
	R_CommitRow( ) {};

	/**
	 * @brief  Returns the id of the request
	 */
	virtual unsigned long ID( ) { return R_COMMIT_ROW; }

	/**
	 * @brief Sets up the serialization request
	 */	
	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_psc_id + m_psc_batch + m_psc_user 
			+ m_iOriginalAutoIncrID + m_vectValues + m_eTypeOfChange;
	}

	/**
	 * @brief Sets up the serialization response
	 */	
	virtual void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_iNewAutoIncrID + m_psc_id_new + m_psc_batch_new;
	}

	/**
	 * @brief This will return true if the request is processed properly
	 */	
	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

