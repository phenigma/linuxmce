#ifndef ClientFunctions_h
#define ClientFunctions_h

#include "RA/RA_Processor.h"
#include "CommonFunctions.h"

#include <map>
#include <string>

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

	bool GetLineups(int zipcode, map <int,pair<string,string> > &);

	bool GetChannels( int key, string extra_cond, map <string,string> & );

	string GetBlackListChannels();

	void DeleteClientLineup();
	
	void SetClientLineup(string key, string lineup);

	string GetClientLineup();

	string CalcDiffs(string lineup, string blacklist, map <string,string> &mapProgramRecord, map <int,string> &mapGetStation, map <string,string> &mapGetSchedule, map <string,string> &mapActor, map <string,string> &mapGenre, map <string,string> &mapRole);

	bool ModifyClientDatabase(string path);

	virtual ~ClientFunctions( ) 
	{ 
		delete m_pSocket;
	}

};

#endif
