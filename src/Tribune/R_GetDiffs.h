#ifndef R_GETDIFFS__
#define R_GETDIFFS__

#include "RA/RA_Request.h"
#include "Tribunerequests.h"

#include <string>
#include <map>
#include <sstream>

using namespace std;


/**
 *in: client lineup, blacklist channels, all the client maps
 *out: a string which has all the mysql commands that must be executed by the client
 */


class R_GetDiffs : public RA_Request
{
public:
	/** @brief Request Variables */
	string m_lineup;
	string m_blacklist;
	map <string,string> m_mapProgramRecord;
        map <int,string> m_mapStation;
	map <u_int64_t,string> m_mapSchedule;
	map <string,string> m_mapActor;
	map <string,string> m_mapGenre;
	map <string,string> m_mapRole;
	std::ostringstream clientfile;

	/** @brief Response Variables */
	string m_clientfile;

	/** @brief constructor */
	
	R_GetDiffs(string lineup, string blacklist, map <string,string> mapProgramRecord, map <int,string> mapStation, map <u_int64_t,string> mapSchedule, map <string,string> mapActor, map <string,string> mapGenre, map <string,string> mapRole);


	/** @brief constructor */
	R_GetDiffs( ) {};

	/**
	 * @brief Returns the id of @todo ask
	 */

	virtual unsigned long ID( ) { return R_GET_DIFFS; }

	/**
	 * @brief Sets up the serialization request
	 */

	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		/** this must not be commented after the serialization bug is fixed*/
		StartSerializeList( ) + m_lineup + m_blacklist + m_mapProgramRecord + m_mapStation + /*m_mapSchedule +*/ m_mapActor + m_mapGenre + m_mapRole;
	}

	/**
	 * @brief Sets up the serialization response
	 */

	virtual void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_clientfile;
	}

	/**
	 * @brief This will say whether the request was processed successfully or not
	 */

	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );

	bool UpdateProgramRecord(string pk_programrecord, bool conversion);
	bool UpdateProgramRecord_Genre(string pk_programrecord);
	bool UpdateActor_ProgramRecord(string pk_programrecord);
	bool InsertSchedule(string pk_station);
	bool InsertProgramRecord_Genre(string pk_programrecord);
	bool InsertActor_ProgramRecord(string pk_programrecord);
	bool InsertProgramRecord(string pk_station);
	string ChangedTime(string time);
	string ChangedLongFormatTime(string time);
	
};


#endif

