/**
 *
 * @file R_CommitChanges.h
 * @brief header file for the R_CommitChanges class
 * @author
 *
 */
 
#ifndef R_CommitChanges_H
#define R_CommitChanges_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

/**
 * @brief class modelling commit changes requests
 */
 
class R_CommitChanges : public RA_Request
{
public:
	/** @brief Request Variables */
	map<int, string> m_mapUsersPasswords; /**< All the users who are checking in this session */
	vector<string> m_vectTables; /**< The tables we will be checking in this session     */
	string m_sRepository;

	/** @brief Response Variables */

	/** @brief constructors */
	R_CommitChanges( string sRepository );
	R_CommitChanges( ) {};

	/**
	 * @brief Returns the id of the request @todo ask
	 */
	virtual unsigned long ID( ) { return R_COMMIT_CHANGES; }

	/**
	 * @brief Sets up the serialization request
	 */
	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_sRepository;
	}

	/**
	 * @brief  This will say whether the request was processed successfully or not
	 */
	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

