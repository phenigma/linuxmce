#ifdef WIN32
#include "afx.h"
#include <WinSock.h>
#else
#include <dirent.h>
#endif

#include <mysql.h>
#include <vector>
#include <sstream>
#include <time.h>
#include <string.h>

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "DataGrid.h"
#include "DataGridDevice.h"
#include "PlutoServerCore.h"
#include "PlutoEvents.h"

#include "MasterEPGGrid.h"

MasterEPGGrid::~MasterEPGGrid()
{
	map<u_int64_t, CustomEPGView *>::iterator itEV;
	for(itEV=mapEPGView.begin();itEV!=mapEPGView.end();++itEV)
	{
		CustomEPGView *p = (*itEV).second;
		delete p;
	}
}



DataGridCell *MasterEPGGrid::GetData(int Column, int Row, int *Size)
{
	if( !pCurrentView )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"GetData but no current view");
		return NULL;
	}
	else if( !pCurrentView->iNumberOfChannels )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"GetData but no rows");
		return NULL;
	}
	else if( Row>pCurrentView->iNumberOfChannels )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"GetData but row out of range");
		Row=0;
	}
	
	return DataGridTable::GetData(Column,pCurrentView->iChannelMapping[Row],Size);
}

int MasterEPGGrid::GetRows()
{
	if( !pCurrentView )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"GetRows but no current view");
		return 0;
	}
	else
		return pCurrentView->iNumberOfChannels+1;
}

void MasterEPGGrid::ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount)
{
	int EntGroup=0;
	string::size_type pos = GridID.find("_");
	if( pos==string::npos || (EntGroup = atoi(GridID.substr(pos+1).c_str()))==0 || (pCurrentView = map_EntGroupViews[EntGroup])==NULL )
	{
		Size=0;
		Data=NULL;
		g_pPlutoLogger->Write(LV_CRITICAL,"MasterEPGGrid::ToData can't find info for grid: %s",GridID.c_str());
	}
	else if( !pCurrentView )
	{
		Size=0;
		Data=NULL;
		g_pPlutoLogger->Write(LV_CRITICAL,"ToData but no current view");
		return;
	}
	else if( !pCurrentView->iNumberOfChannels )
	{
		Size=0;
		Data=NULL;
		g_pPlutoLogger->Write(LV_CRITICAL,"ToData but no rows");
		return;
	}
	else
	{
		DataGridTable::ToData(GridID,Size,Data,ColStart,RowStart,ColCount,RowCount);
	}
}
