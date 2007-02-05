/**
 *
 * @file R_GetConditional_psc_id.h
 * @brief header file for the R_GetConditional_psc_id class
 * @author
 *
 */

#ifndef R_GetLineups
#define R_GetLineups

#include "RA/RA_Request.h"
#include "Tribunerequests.h"

#include <string>
#include <vector>

using namespace std;

/**
 * @brief  class modelling a GetAll_psc_id request
 * @todo complete documentation
 */

class R_GetLineups : public RA_Request
{
public:
	/** @brief Request Variables */
	string m_sTable;
	int m_zipcode;

	/** @brief Response Variables */
	vector< pair<string,string> > m_vectAll_lineups;  // psc_id + batch

	/** @brief constructor */
	R_GetLineups(string sTable, int zipcode);

	/** @brief constructor */
	R_GetLineups( ) {};

	/**
	 * @brief Returns the id of @todo ask
	 */

	virtual unsigned long ID( ) { return R_GET_LINEUPS; }

	/**
	 * @brief Sets up the serialization request
	 */

	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_sTable + m_zipcode;
	}

	/**
	 * @brief Sets up the serialization response
	 */

	virtual void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_vectAll_lineups;
	}

	/**
	 * @brief This will say whether the request was processed successfully or not
	 */

	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

