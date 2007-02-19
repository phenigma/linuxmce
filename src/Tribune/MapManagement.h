#ifndef __MapManagement_h_
#define __MapManagement_h_

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

/** Class that contains methods that populates the maps from the database (key,timestamp) */

class MapManagement{
	
public:

	/** get program records from the database.*/
	static bool GetProgramRecordMap(map <string,string> &);

	/** get stations from the database.*/
	static bool GetStationMap(map<int,string>&);
	
	/** get schedule from the database.*/
	static bool GetScheduleMap(map<u_int64_t,string>&);
	
	/** get actors from the database.*/
	static bool GetActorMap(map<string,string>&);
	
	/** get genres from the database.*/
	static bool GetGenreMap(map<string,string>&);
	
	/** get maps from the database.*/
	static bool GetRoleMap(map<string,string>&);

};

#endif
