/**
 *
 * @file R_GetAll_psc_id.h
 * @brief header file for the R_GetAll_psc_id class
 * @author
 *
 */
 
#ifndef R_GetAll_psc_id_H
#define R_GetAll_psc_id_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

/**
 * @brief  class modelling a GetAll_psc_id request
 * @todo complete documentation
 */
 
class R_GetAll_psc_id : public RA_Request
{
public:
	/** @brief Request Variables */
	string m_sTable;
	int m_psc_id,m_psc_batch;
	vector<int> m_vectRestrictions; /** The restrictions we're interested in */

	/** @brief Response Variables */
	vector< pair<int,int> > m_vectAll_psc_id;  // psc_id + batch

	/** @brief constructor */
	R_GetAll_psc_id(string sTable, vector<int> *p_vectRestrictions);
	
	/** @brief constructor */
	R_GetAll_psc_id( ) {};

	/**
	 * @brief Returns the id of @todo ask
	 */
	 
	virtual unsigned long ID( ) { return R_GET_ALL_PSC_ID; }

	/**
	 * @brief Sets up the serialization request
	 */
	 	
	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_sTable + m_vectRestrictions;
	}

	/**
	 * @brief Sets up the serialization response
	 */
	 	
	virtual void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_vectAll_psc_id;
	}

	/**
	 * @brief This will say whether the request was processed successfully or not
	 */
	 	
	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

