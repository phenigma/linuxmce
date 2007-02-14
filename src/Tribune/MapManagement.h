#ifndef __MapManagement_h_
#define __MapManagement_h_

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

class MapManagement{
	
public:

	/** Get program records.*/
	static bool GetProgramRecordMap(map <string,string> &);
	
	static bool GetStationMap(map<int,string>&);
	
	static bool GetScheduleMap(map<string,string>&);
	
	static bool GetActorMap(map<string,string>&);
	
	static bool GetGenreMap(map<string,string>&);
	
	static bool GetRoleMap(map<string,string>&);

};

#endif
