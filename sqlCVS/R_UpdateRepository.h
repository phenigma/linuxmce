/**
 *
 * @file R_UpdateRepository.h
 * @brief header file for the R_UpdateRepository class
 * @author
 *
 */
 
#ifndef R_UpdateRepository_H
#define R_UpdateRepository_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

/**
 * @brief class modelling the update repository request
 */
 
class R_UpdateRepository : public RA_Request
{
public:
	/** @brief Request Variables */
	map<string, string> m_mapUsersPasswords; /**< All the users who are checking in this session */
	vector<string> m_vectTables; /**< The tables we will be checking in this session  */
	string m_sRepository;

	/** Response Variables */

	/** @brief constructor */
	
	R_UpdateRepository( string sRepository );
	
	/** @brief constructor */
	
	R_UpdateRepository( ) {};

	/**
	 * @brief  Returns the id of the updated repository
	 */
	 
	virtual unsigned long ID( ) { return R_UPDATE_REPOSITORY; }

	/**
	 * @brief Sets up the serialization request
	 */
	 	
	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_sRepository;
	}

	/**
	 * @brief This will say whether the request was processed successfully or not
	 */
	 	
	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

