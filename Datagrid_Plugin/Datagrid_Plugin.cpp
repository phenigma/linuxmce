/*
 Datagrid_Plugin
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to 
 the terms of the Pluto Public License, available at: 
 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. 
 
 See the Pluto Public License for more details.
 
 */

//<-dceag-d-b->
#include "Datagrid_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCE/DataGrid.h"

//<-dceag-const-b->! custom
Datagrid_Plugin::Datagrid_Plugin( int DeviceID, string ServerAddress, bool bConnectEventHandler, bool bLocalMode, class Router *pRouter )
 : Datagrid_Plugin_Command( DeviceID, ServerAddress, bConnectEventHandler, bLocalMode, pRouter ), 
 m_DataGridMutex( "DataGrid" )
//<-dceag-const-e->
{
 m_DataGridMutex.Init( NULL );
}

//<-dceag-dest-b->
Datagrid_Plugin::~Datagrid_Plugin()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Datagrid_Plugin::Register()
//<-dceag-reg-e->
{
 return Connect();
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Datagrid_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Datagrid_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

void Datagrid_Plugin::RegisterDatagridGenerator( class DataGridGeneratorCallBack *pCallBack, int PK_DataGrid )
{
 map<int, class DataGridGeneratorCallBack *>::iterator it = m_mapDataGridGeneratorCallBack.find( PK_DataGrid );
 if( it!=m_mapDataGridGeneratorCallBack.end() )
 {
  g_pPlutoLogger->Write( LV_CRITICAL, "Two plug-ins tried to register for DataGrid %d", PK_DataGrid );
  return;
 }
 m_mapDataGridGeneratorCallBack[PK_DataGrid]=pCallBack;
}

/*  **** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

void Datagrid_Plugin::SomeFunction()
{
 // If this is going to be loaded into the router as a plug-in, you can implement: virtual bool Register();
 // to do all your registration, such as creating message interceptors

 // If you use an IDE with auto-complete, after you type DERC:: it should give you a list of all
 // commands and requests, including the parameters. See "AllCommandsRequests.h"

 // Send Orbiters the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter. We'll use 55, 77 for X and Y.

 // Send the message to a specific orbiter, identified by OrbiterID
 QueueMessage( new DERC::CMD_Simulate_Mouse_Click( m_dwPK_Device, OrbiterID, 55, 77 );

 // Send the message to orbiters 32898 and 27283
 QueueMessage( new DERC::CMD_Simulate_Mouse_Click( m_dwPK_Device, "32898, 27283", 55, 77 );

 // Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST ( see pluto_main/Define_DeviceCategory.h )
 QueueMessage( new DERC::CMD_Simulate_Mouse_Click( m_dwPK_Device, DEVICECATEGORY_Orbiter_CONST, true, BL_SameHouse, 55, 77 );

 // Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room ( see pluto_main/Define_DeviceTemplate.h )
 QueueMessage( new DERC::CMD_Simulate_Mouse_Click( m_dwPK_Device, DeviceTemplate_Orbiter_CONST, true, BL_SameRoom, 55, 77 );

 // This time we want to wait to be sure the orbiter gets the message, and the thread will block until the orbiter receives the message
 string sResult="";
 bool bResult = SendMessageWithConfirm( new DERC::CMD_Simulate_Mouse_Click( m_dwPK_Device, OrbiterID, 55, 77 ), sResult );
 // If bResult is true, the message was received ok. Otherwise it failed, and sResult contains an explanation of the failure

 // A request is like a command, except that it has both "in" and "out" parameters, and the
 // thread blocks until the request is completed. Note that requests are sent on a separate socket, so you can continue to receive commands and send
 // events while a thread is waiting for a request to complete. However the SendMessageWithConfirm uses the same request socket, and will wait if
 // the socket is busy.

 // We want to request the contents of a file. This request takes 1 "in" parameter, the file name, and has 2 "out" parameters: the file's contents, and the date.
 // All out parameters are passed by reference.
 // Note that the "Data" parameter which contains the file is a binary parameter. Whether in or out, binary parameters always take both a pointer to the object and the size
 // This command will be sent to the "Standard Plug-in" which implements this type of request. We set the boardcast level to BL_SameHouse. Note that since this is a
 // request, the router will find the first matching device ( ie the first device "Standard Plug-in" ) and return that device's response. If this was a command, not a request, 
 // and there were multiple devices "Standard Plug-in" they would all get the message.
 PlutoDate plutoDate;
 char *FileContents;
 int FileSize;
 bool bResult = SendRequest( new DERC::REQ_File_Contents( m_dwPK_Device, DeviceTemplate_Standard_Plug_In_CONST, true, BL_SameHouse, "some_file_name", &FileContents, &FileSize, &plutoDate );

 // To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

 // Get our IP address from our data
 string sIP = DATA_Get_IP_Address();

 // Set our data "Filename" to "myfile"
 DATA_Set_Filename( "myfile" );

 // Fire the "Finished with file" event, which takes no parameters
 EVENT_Finished_with_file();


}
*/

/*

 COMMANDS TO IMPLEMENT

*/

//<-dceag-c34-b->

	/** @brief COMMAND: #34 - Request Datagrid Contents */
	/** The orbiters use this to request the contents of a datagrid, so they can display it for the user.  Before you can request a grid, the command Populate Datagrid must be called, passing in the information required to populate the grid as well a \"GridID\", */
		/** @param #10 ID */
			/** For debugging purposes if problems arise with a request not being filled, or a grid not populated when it should be.  If the Orbiter specified an ID when requesting the grid or populating it, the Datagrid plug-in will log the ID and status so the develope */
		/** @param #15 DataGrid ID */
			/** A unique identifier for the grid that was passed to populate grid. */
		/** @param #19 Data */
			/** The binary contents of the grid. */
		/** @param #32 Row */
			/** The starting row.   This is both an 'in' and 'out'.  If sSeek is not empty, the grid will ignore the value passed in here, and seek to that row instead.  It will also return the value of the row in this variable so the requestor will know the current row. */
		/** @param #33 Column */
			/** The starting column */
		/** @param #34 Row count */
			/** How many rows to fetch */
		/** @param #35 Column count */
			/** How many columns to fetch. */
		/** @param #36 Keep Row Header */
			/** True means the first row (0) is locked and will always be at the top as a header. */
		/** @param #37 Keep Column Header */
			/** True means the first column(0) is locked and will always be at the top as a header. */
		/** @param #49 Add Up-Down Arrows */
			/** If to add up/down arrows to the returned datagrid when datagrid doesn't fully fit. */
		/** @param #73 Seek */
			/** If a value is specified, the grid will seek to the first row matching this value.  The search is case insensitive.  The value of "Row" will be ignored, and the new top row will be returned as the out value of "Row".  If offset is specified, the comparisso */
		/** @param #74 Offset */
			/** See the "Seek" parameter for an explanation. */

void Datagrid_Plugin::CMD_Request_Datagrid_Contents(string sID,string sDataGrid_ID,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,char **pData,int *iData_Size,int *iRow,string &sCMD_Result,Message *pMessage)
//<-dceag-c34-e->
{
	*iData_Size=0;
	*pData=NULL;

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_DATAGRID, "Requesting grid: %s", sDataGrid_ID.c_str() );
#endif
	PLUTO_SAFETY_LOCK( s, m_DataGridMutex );
	DataGridTable *pDataGridTable=NULL;
	DataGridMap::iterator dg = m_DataGrids.find( sDataGrid_ID );
	if ( dg == m_DataGrids.end() )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Requesting data from unknown grid %s", sDataGrid_ID.c_str() );
		return;
	}
	try
	{
		pDataGridTable = ( *dg ).second;

		if( sSeek.length() )
		{
			sSeek = StringUtils::ToUpper(sSeek);
			// We need to do a seek for this value
			int dgrow;
			// Scroll through here until we find the row right after
			for(dgrow=0;dgrow<pDataGridTable->GetRows();++dgrow)
			{
				DataGridCell *pCell = pDataGridTable->GetData(iOffset,dgrow);
				if( !pCell )
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"Cannot seek because there's an empty cell");
					break;
				}
				string::size_type posStart=0,posEnd=0;
				string CellText = StringUtils::ToUpper(pCell->m_Text);
				if( CellText[0]=='~' )
					posStart++;
				if( CellText[posStart]=='`' )
				{
					while(CellText[++posStart]!='`');
				}
/* don't remember what this was needed for.  It just strips off the first character??
				posEnd = CellText.find('\n');
				if( posEnd )
					CellText = CellText.substr(posStart+1,posEnd-posStart-1);
				else
					CellText = CellText.substr(posStart+1);
*/
				if( StringUtils::ToUpper(CellText)>StringUtils::ToUpper(sSeek) )
					break;
			}
			if( dgrow>0 )
				--dgrow; // We're always the cell 1 after
			*iRow=dgrow;
		}
		pDataGridTable->m_bKeepColumnHeader = bKeep_Column_Header;
		pDataGridTable->m_bKeepRowHeader = bKeep_Row_Header;

		pDataGridTable->ToData( sDataGrid_ID, *iData_Size, *pData, iColumn, *iRow, iColumn_count, iRow_count );
		// hack, what the hell. If I delete the pointer, the system crashes. But it's a valid pointer. It was just allocated in the prior todata
		/*
		g_ptest=*pData;
		delete[] ( *pData );
		*/
#ifdef DEBUG
		g_pPlutoLogger->Write( LV_DATAGRID, "Sending datagrid %s, size: %d, cols: %d, rows: %d", sDataGrid_ID.c_str(), *iData_Size, iColumn_count, iRow_count );
#endif
	}
	catch( ... )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Exception getting grid #%s pointer %p", sDataGrid_ID.c_str(), pDataGridTable );
		g_pPlutoLogger->Write( LV_CRITICAL, "col start: %d row start: %d col count: %d rowcount: %d", iColumn, iRow, iColumn_count, iRow_count );
#ifdef USE_DATAGRID2
		if( pDataGridTable )
			g_pPlutoLogger->Write( LV_CRITICAL, "cell count: %d col count: %d row count %d", pDataGridTable->m_CellCount, pDataGridTable->m_ColumnCount, pDataGridTable->m_RowCount );
#endif
	}
}

//<-dceag-c35-b->

	/** @brief COMMAND: #35 - Populate Datagrid */
	/** Before an orbiter can request the contents of a datagrid, the datagrid plug-in must first populate it.  This is how the orbiter tells the plug-in to create and populate a datagrid, referenced by the unique ID passed in with this command. */
		/** @param #4 PK_Variable */
			/** The populate grid can optionally return a variable number to assign a value into.  For example, the current path in the file grid. */
		/** @param #5 Value To Assign */
			/** The value to assign into the variable. */
		/** @param #10 ID */
			/** For debugging purposes if problems arise with a request not being filled, or a grid not populated when it should be.  If the Orbiter specified an ID when requesting the grid or populating it, the Datagrid plug-in will log the ID and status so the develope */
		/** @param #15 DataGrid ID */
			/** A unique ID for this instance of the grid that will be passed with the Request Datagrid Contents command. */
		/** @param #38 PK_DataGrid */
			/** Which grid should be populated */
		/** @param #39 Options */
			/** The options are specific the type of grid (PK_Datagrid).  These are not pre-defined.  The grid generator and orbiter must both pass the options in the correct format for the type of grid. */
		/** @param #40 IsSuccessful */
			/** Returns false if the grid could not be populated.  Perhaps there was no registered datagrid generator. */

void Datagrid_Plugin::CMD_Populate_Datagrid(string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful,string &sCMD_Result,Message *pMessage)
//<-dceag-c35-e->
{
 *iPK_Variable=0;
 *bIsSuccessful=false; // assume we're not succesful
 PLUTO_SAFETY_LOCK( s, m_DataGridMutex );
 string::size_type pos=0;

 DataGridTable *pDataGridTable=NULL;
 map<int, class DataGridGeneratorCallBack *>::iterator itGridCB = m_mapDataGridGeneratorCallBack.find( iPK_DataGrid );
 if( itGridCB != m_mapDataGridGeneratorCallBack.end() )
 {
  DataGridGeneratorCallBack *pCB = ( *itGridCB ).second;
  pDataGridTable = CALL_MEMBER_FN( *pCB->m_pDataGridGeneratorPlugIn, pCB->m_pDCEDataGridGeneratorFn ) ( sDataGrid_ID, sOptions, NULL, iPK_Variable, sValue_To_Assign, pMessage );
 }

 if( !pDataGridTable )
 {
  g_pPlutoLogger->Write( LV_CRITICAL, "Cannot populate grid: %s", sDataGrid_ID.c_str() );
  pDataGridTable = new DataGridTable();
  DataGridCell *pCell;
  pCell = new DataGridCell( "Datagrid: " + sDataGrid_ID, "" );
  pDataGridTable->SetData( 0, 0, pCell );
  pCell = new DataGridCell( "Type: " + StringUtils::itos( iPK_DataGrid ), "" );
  pDataGridTable->SetData( 0, 1, pCell );
  pCell = new DataGridCell( "is invalid", "" );
  pDataGridTable->SetData( 0, 2, pCell );
  *bIsSuccessful=false; // it didn't work
 }
 else
  *bIsSuccessful=true; // it worked

 m_DataGrids[sDataGrid_ID]=pDataGridTable;
 return;
}


//<-dceag-sample-b->!

