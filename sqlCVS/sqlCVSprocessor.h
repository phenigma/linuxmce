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
	int m_iNew,m_iMod,m_iDel;

	 int m_psc_bathdr_orig,m_psc_bathdr_auth,m_psc_bathdr_unauth;

	/** @brief constructor assigning null to all the pointers */
	sqlCVSprocessor( ) : RA_Processor( 0, 1 ) { 
		m_pTable=NULL; m_pvectFields=NULL; m_i_psc_batch=1; m_pRepository=NULL; m_bSupervisor=false; m_iNew=m_iMod=m_iDel=0; 
		m_psc_bathdr_orig=m_psc_bathdr_auth=m_psc_bathdr_unauth=0;
	}
	virtual ~sqlCVSprocessor( ) { RecordChangesToTable(); }
	bool UserIsValidated(int psc_user)
	{
		return m_mapValidatedUsers.find(psc_user)!=m_mapValidatedUsers.end();
	}

	/** @brief Creates a batch for holding unauthorized transactions */
	int UnauthorizedBatch(int psc_user_needs_to_authorize);

	/** @brief As a table is updated the counters (New/Mod/Del) are incremented.  When we have finished updating a table, this will record those changes into the batch */
	void RecordChangesToTable();
};

#endif
