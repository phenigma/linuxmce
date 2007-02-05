#ifndef Tribuneprocessor_h
#define Tribuneprocessor_h

#include "RA/RA_Processor.h"
#include "CommonFunctions.h"
#include "Database.h"

// namespace Tribune
// {
// 	class Table;
// 	class Repository;
// }

using namespace Tribune;
class Tribuneprocessor;
extern Tribuneprocessor *g_pTribuneprocessor;

/**
 * @brief request action processor class
 */
class Tribuneprocessor : public RA_Processor
{
public:
	time_t tTime_Creation;
	time_t tTime_LastActivity;
	class RAServerSocket *m_pRAServerSocket;
	//Tribune::Table *m_pTable;	/**< The table we are currently committing */
	//vector<string> *m_pvectFields;	/**< The fields of the table @todo ask */
	//int m_i_psc_batch;
	//Tribune::Repository *m_pRepository; /**< The repository we're currently committing */
	//bool m_bSupervisor,m_bAllAnonymous;
	//int m_iNew,m_iMod,m_iDel,m_ipsc_user_default;
	//string m_sComments;  /**< Optional comments to be included with this transaction */

	//int m_psc_bathdr_orig,m_psc_bathdr_auth,m_psc_bathdr_unauth,m_psc_user_needs_to_authorize;
	
	/**< This will create a transaction for every processor that will rollback automatically if the processor goes away with an R_CloseTransaction */
	SafetyTransaction st;

	void LogActivityTime()
	{
		tTime_LastActivity = time(NULL);
		//printf("last Activitity Time set to %i", tTime_LastActivity);
	}

	/** @brief constructor assigning null to all the pointers */
	Tribuneprocessor( RAServerSocket *pRAServerSocket ) : 
		RA_Processor( 0, 1, NULL, g_GlobalConfig.m_iMaxConnectAttemptCount ), 
		st(g_GlobalConfig.m_pDatabase) 
	{ 
		//m_pTable=NULL; m_pvectFields=NULL; m_pRepository=NULL; 
		tTime_LastActivity = tTime_Creation = time(NULL);  m_pRAServerSocket=pRAServerSocket;
	}
	
	virtual ~Tribuneprocessor( ) 
	{ 
		if( st.m_bIsOpen_get() ) 
			st.Rollback(); 
		else
			RecordChangesToTable();

		if( g_pTribuneprocessor==this )
		{
			g_pTribuneprocessor=NULL; 
			printf("Clearing g_pTribuneprocessor %p\n",this);
		}
	}

	int UnauthorizedBatch(/*int psc_user_needs_to_authorize*/);

	void RecordChangesToTable();
};

#endif
