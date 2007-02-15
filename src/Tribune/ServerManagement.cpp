#include "ServerManagement.h"

#include "CommonFunctions.h"
//#include "Database.h"
//#include "MapManagement.h"
#include <iostream>
#include <sstream>
#include <map>
#include <string>

using namespace std;
using namespace Tribune;

ServerManagement* ServerManagement::instance = NULL;

ServerManagement::ServerManagement()
	: TribuneMutex("TribuneMapMutex")
{
	pthread_mutexattr_init( &m_MutexAttr );
	pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	TribuneMutex.Init( &m_MutexAttr );
}

ServerManagement::~ServerManagement()
{
}


ServerManagement* ServerManagement::getInstance(){
	
	if (instance==NULL){
		instance = new ServerManagement();
	}
	return instance;
}

void* ServerManagement::ServerManagement_Thread(void *p){

	ServerManagement *pServerManagement = (ServerManagement *) p;
	if( pServerManagement != NULL )
	{
		pServerManagement->Run();
	}
	
	return NULL;
}

void ServerManagement::Run(){
	
	while (true){
		
		if(g_GlobalConfig.m_bImportTable){

			PLUTO_SAFETY_LOCK(tb,TribuneMutex);

			if (! MapManagement::GetProgramRecordMap(g_GlobalConfig.mapPrimaryKey_Timestam_ProgramRecord) ){
		
				cerr << "Cannot fill the program record map " << endl;
			}
			if (! MapManagement::GetStationMap(g_GlobalConfig.mapPrimaryKey_Timestam_Station) ){

				cerr << "Cannot fill the station map " << endl;
			}

			if (! MapManagement::GetScheduleMap(g_GlobalConfig.mapPrimaryKey_Timestam_Schedule) ){
			
				cerr << "Cannot fill the schedule map: " << endl;
			}

			g_GlobalConfig.m_bImportTable=false;
		}
		
		Sleep(10);
	}
}
