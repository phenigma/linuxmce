#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/MultiThreadIncludes.h"
#include "DCEMI_PS_DataGrid.h"
#include "Router.h"
#include "Message.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_CommandParameter.h"
#include "../pluto_main/Define_DeviceCategory.h"
#include "DataGrid.h"

// temp 
#define C_COMMANDPARAMETER_DATAGRID_TABLE_CONST 9999
#define COMMAND_POPULATE_DATAGRID_CONST 9999

bool DCEMI_PS_DataGrid::Register()
{
	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_DataGrid::PopulateDataGrid))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_POPULATE_DATAGRID_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_DataGrid::RequestDataGrid))
		,0,0,0,0,MESSAGETYPE_REQUEST,REQUESTTYPE_DATAGRID);


	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_DataGrid::RequestDataGridPosition))
		,0,0,0,0,MESSAGETYPE_REQUEST,REQUESTTYPE_DATAGRID_POSITION);

	return true;
}

bool DCEMI_PS_DataGrid::PopulateDataGrid(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	PLUTO_SAFETY_LOCK(s,m_DataGridMutex);
	string::size_type pos=0;
	string Parms = pMessage->m_mapParameters[COMMANDPARAMETER_Type_CONST];
	int DataGridType = atoi( StringUtils::Tokenize(Parms,",",pos).c_str() );

	map<int,class DataGridGeneratorCallBack *>::iterator itGridCB = m_mapDataGridGeneratorCallBack.find(DataGridType);
	if( itGridCB != m_mapDataGridGeneratorCallBack.end() )
	{
		DataGridGeneratorCallBack *pCB = (*itGridCB).second;

		string GridID = pMessage->m_mapParameters[COMMANDPARAMETER_DataGrid_ID_CONST];
		DataGridTable *pDataGridTable = CALL_MEMBER_FN(*pCB->m_pDataGridGeneratorPlugIn,pCB->m_pDCEDataGridGeneratorFn) (GridID,Parms.substr(pos),NULL);

		if( pDataGridTable )
			m_DataGrids[GridID]=pDataGridTable;
	}
    
	return true;
}


bool DCEMI_PS_DataGrid::RequestDataGrid(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	if( !pSocket )
		return true;

	int NumOutputtedRows=0, NumTotalRows=-0;

	char *TableData;
	int TableDataLength;

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_DATAGRID, "Got a request for datagrid %s (# %s).", 
		pMessage->m_mapParameters[PARM_DGR_GRIDID].c_str(),pMessage->m_mapParameters[PARM_DGR_SERIALNUM].c_str());
#endif
	if (RequestGridData(pMessage->m_mapParameters[PARM_DGR_GRIDID].c_str(),
		atoi(pMessage->m_mapParameters[PARM_DGR_STARTINGCOLUMN].c_str()),
		atoi(pMessage->m_mapParameters[PARM_DGR_STARTINGROW].c_str()),
		atoi(pMessage->m_mapParameters[PARM_DGR_COLUMNCOUNT].c_str()), 
		atoi(pMessage->m_mapParameters[PARM_DGR_ROWCOUNT].c_str()),
		atoi(pMessage->m_mapParameters[PARM_DGR_KEEPCOLUMNHEADER].c_str())==1,
		atoi(pMessage->m_mapParameters[PARM_DGR_KEEPROWHEADER].c_str())==1,
		TableDataLength, TableData))
	{
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_DATAGRID, "Sending grid %s (# %s)", 
			pMessage->m_mapParameters[PARM_DGR_GRIDID].c_str(),pMessage->m_mapParameters[PARM_DGR_SERIALNUM].c_str());
#endif
	    pSocket->SendMessage(new Message(0, 0, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 
			REQUESTTYPE_DATAGRID, 1, -C_COMMANDPARAMETER_DATAGRID_TABLE_CONST, TableData, TableDataLength)); 
	}
	else
	{

		pSocket->SendString("GRID NOT FOUND");
	}

	pMessage->m_bRespondedToMessage=true;

	return true;
}

bool DCEMI_PS_DataGrid::RequestGridData(string GridID, int ColStart, int RowStart, int ColCount, int RowCount, bool bKeepColumnHeader, bool bKeepRowHeader, int &TableDataSize, char* &TableData)
{
	TableDataSize=0;
	TableData=NULL;

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_DATAGRID, "Requesting grid: %s",GridID.c_str());
#endif
	//jjhuff
	//Message *pMessage = new Message();
	PLUTO_SAFETY_LOCK(s,m_DataGridMutex);
	DataGridTable *dgs=NULL;
	DataGridMap::iterator dg = m_DataGrids.find(GridID);
	if (dg == m_DataGrids.end())
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Requesting data from unknown grid %s", GridID.c_str());
		return false;
	}
	try
	{
		dgs = (*dg).second;
		dgs->m_bKeepColumnHeader = bKeepColumnHeader;
		dgs->m_bKeepRowHeader = bKeepRowHeader;

		dgs->ToData(GridID,TableDataSize, TableData, ColStart, RowStart, ColCount, RowCount);
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_DATAGRID, "Sending datagrid %s, size: %d, cols: %d, rows: %d",GridID.c_str(),TableDataSize,ColCount,RowCount);
#endif
	}
	catch(...)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Exception getting grid #%s pointer %p", GridID.c_str(), dgs);
		g_pPlutoLogger->Write(LV_CRITICAL, "col start: %d row start: %d col count: %d rowcount: %d",ColStart,RowStart,ColCount,RowCount);
#ifdef USE_DATAGRID2
		if( dgs )
			g_pPlutoLogger->Write(LV_CRITICAL, "cell count: %d col count: %d row count %d",dgs->m_CellCount,dgs->m_ColumnCount,dgs->m_RowCount);
#endif
		return false;
	}
	return true;
}




bool DCEMI_PS_DataGrid::RequestDataGridPosition(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
{
	string DataGrid_ID = pMessage->m_mapParameters[COMMANDPARAMETER_DataGrid_ID_CONST];
	string Text = pMessage->m_mapParameters[COMMANDPARAMETER_Text_CONST];
	int Column = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Position_X_CONST].c_str());
	DataGridTable *dgTable = NULL;//m_pDataGridDevice->GetGrid(DataGrid_ID);
	int dgrow=0;
	if(dgTable)
	{
		// Scroll through here until we find the row right after
		for(dgrow=0;dgrow<dgTable->GetRows();++dgrow)
		{
			DataGridCell *pCell = dgTable->GetData(Column,dgrow);
			string::size_type posStart=0,posEnd=0;
			string CellText = StringUtils::ToUpper(pCell->m_Text);
			if( CellText[0]=='~' )
				posStart++;
			if( CellText[posStart]=='`' )
			{
				while(CellText[++posStart]!='`');
			}
			posEnd = CellText.find('\n');
			if( posEnd )
				CellText = CellText.substr(posStart+1,posEnd-posStart-1);
			else
				CellText = CellText.substr(posStart+1);

			if( CellText>Text )
				break;
		}
	}

	if( dgrow>0 )
		--dgrow; // We're always the cell 1 after
	
	// Send back the reply
	Message *pResponse = new Message(DEVICEID_DCEROUTER, pMessage->m_dwPK_Device_From, PRIORITY_NORMAL, 
		MESSAGETYPE_REPLY, dgrow,0);
	pSocket->SendMessage(pResponse);
	pMessage->m_bRespondedToMessage=true;
	return false;
}
