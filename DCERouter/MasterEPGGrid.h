#ifndef MASTEREPGGRID_H
#define MASTEREPGGRID_H

#include <mysql.h>

#include "Gen_Devices/DataGridCommand.h"
#include "DataGrid.h"

class CustomChannel
{
public:
	class CustomChannel *m_pNextChannel,*m_pPriorChannel;
	class EPGChannel *m_pEPGChannel;
	class XMLTVStation *m_pXMLChannel;
	int m_iRow,m_Channel;

	CustomChannel() : m_pNextChannel(NULL), m_pPriorChannel(NULL), m_pEPGChannel(NULL), m_iRow(0), m_Channel(0), m_pXMLChannel(NULL) {};
};

class CustomEPGView
{
public:
	list<EPGChannel *> m_listChannels;
	class CustomChannel *m_pFirstChannel;
	int *iChannelMapping;
	int iNumberOfChannels;
	map<int,CustomChannel *> m_mapProviderStation;
	EPGProvider_Station__ScheduleMap m_mapProvider_Station__Schedule;
	map<int,EPGProvider *> m_mapProviders;
	list<EPGProvider *> m_listXMLProviders;

	CustomEPGView() : iChannelMapping(NULL), m_pFirstChannel(NULL) {};
	~CustomEPGView() { 
		if( iChannelMapping )
			delete iChannelMapping;
	}
};


class MasterEPGGrid : public DataGridTable
{
public:
	// There is a CustomEPGView for each user/ent group combination
	// u_int64_t User_EntGroup = PK_User<<32;
	// User_EntGroup |= PK_EntGroup;
	map<u_int64_t, CustomEPGView *> mapEPGView;
	CustomEPGView *pCurrentView;
	class PlutoServerCore *m_pCore;
	MasterEPGGrid(class PlutoServerCore *pCore) { pCurrentView=NULL; m_pCore=pCore; }
	map<int,CustomEPGView *> map_EntGroupViews; // maps PK_EntGroup to the currentView
	void ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount);
	virtual DataGridCell *GetData(int Column, int Row, int *Size);
	virtual int GetRows();
	virtual ~MasterEPGGrid();
};

#endif

