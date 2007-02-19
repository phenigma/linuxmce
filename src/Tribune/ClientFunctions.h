#ifndef ClientFunctions_h
#define ClientFunctions_h

#include "RA/RA_Processor.h"
#include "CommonFunctions.h"

#include <map>
#include <string>

/** Common client functions*/

using namespace Tribune;

class ClientFunctions
{
public:
	class RA_Processor *ra_Processor;
	DCE::Socket **m_pSocket;
	string Connection;
	
	ClientFunctions( DCE::Socket **pSocket )
	{ 
		ra_Processor = new RA_Processor( 1, 1, NULL, g_GlobalConfig.m_iMaxConnectAttemptCount );
		m_pSocket = pSocket;
		Connection = g_GlobalConfig.m_sTribuneHost + ":" + StringUtils::itos(g_GlobalConfig.m_iTribunePort);
	}

	ClientFunctions();

	/** get the lineups for a code provided*/
	bool GetLineups(int zipcode, map <int,pair<string,string> > &);

	/** get channels for a lineup excluding the blacklist channels */
	bool GetChannels( int key, string extra_cond, map <string,string> & );

	/** get blacklist channels  from the client database*/
	string GetBlackListChannels();

	/** delete client lineup*/
	void DeleteClientLineup();
	
	/** store the client lineup*/
	void SetClientLineup(string key, string lineup);

	string GetClientLineup();

	/** get the 'diffs' for the client*/
	string CalcDiffs(string lineup, string blacklist, map <string,string> &mapProgramRecord, map <int,string> &mapStation, map <u_int64_t,string> &mapSchedule, map <string,string> &mapActor, map <string,string> &mapGenre, map <string,string> &mapRole);

	/** execute mysql commands from the file provided by the server */
	bool ModifyClientDatabase(string path);

	virtual ~ClientFunctions( ) 
	{ 
		delete m_pSocket;
	}

};

#endif
