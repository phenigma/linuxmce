/**
 *
 * @file sqlCVSProcessor.h
 * @brief header file for the request processor class specific to the sqlCVS program
 * @author
 *
 */

#ifndef sqlCVSprocessor_h
#define sqlCVSprocessor_h

#include "RA/RA_Processor.h"

namespace sqlCVS
{
	class Table;
	class Repository;
}

/**
 * @brief request action processor class
 */
class sqlCVSprocessor : public RA_Processor
{
public:
	sqlCVS::Table *m_pTable;	/**< The table we are currently committing */
	vector<string> *m_pvectFields;	/**< The fields of the table @todo ask */
	int m_i_psc_batch;
	map<int,bool> m_mapValidatedUsers; /**< Map of user id's to bool's if the user is a supervisor */
	sqlCVS::Repository *m_pRepository; /**< The repository we're currently committing */
	bool m_bSupervisor;

	/** @brief constructor assigning null to all the pointers */
	sqlCVSprocessor( ) : RA_Processor( 0, 1 ) { m_pTable=NULL; m_pvectFields=NULL; m_i_psc_batch=1; m_pRepository=NULL; m_bSupervisor=false; }

	bool UserIsValidated(int psc_user)
	{
		return m_mapValidatedUsers.find(psc_user)!=m_mapValidatedUsers.end();
	}

	int UnauthorizedBatch(int psc_user_needs_to_authorize)
	{
		return -1;
	}
};

#endif
