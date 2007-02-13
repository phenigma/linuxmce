#include "ServerManagement.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/StringUtils.h"
#include "CommonFunctions.h"
#include "Database.h"
#include "MapManagement.h"

#include <iostream>
#include <sstream>
#include <map>
#include <string>

ServerManagement::instance = NULL;
MapManagement::mapManag = NULL;

pthread_mutexattr_init( &m_MutexAttr );
pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
TribuneMutex.Init( &m_MutexAttr );

ServerManagement* ServerManagement::getInstance(){
	
	if (instance==NULL){
		instance = new ServerManagement();
		mapManag = new MapManagement();
	}
	return instance;
}

void ServerManagement::ServerManagement_Thread(void *p){

	ServerManagement *pServerManagement = (ServerManagement *) p;
	if( pServerManagement != NULL )
	{
		pServerManagement->Run();
	}
}

void ServerManagement::Run(){
	
	while (true){
		
		if(g_GlobalConfig.m_bImportTable){
			PLUTO_SAFETY_LOCK(tb,TribuneMutex);
			g_GlobalConfig.mapPrimaryKey_Timestam_ProgramRecord=mapManag->GetProgramRecordMap();
			g_GlobalConfig.m_bImportTable=false;
		}
		
		Sleep(10);
	}
}
