#ifndef MapManagement_h
#define MapManagement_h

#include <map>
#include <string>

class MapManagement{
	
public:
	MapManagement();
	static map<string,string> GetProgramRecordMap( );
	static map<int,string> GetStationMap( );
	static map<string,string> GetScheduleMap( );
	static map<string,string> GetActorMap( );
	static map<string,string> GetGenreMap( );
	static map<string,string> GetRoleMap( );
}

#endif