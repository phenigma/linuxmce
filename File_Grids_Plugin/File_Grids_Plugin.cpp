/*
 File_Grids_Plugin
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

//<-dceag-d-b->
#include "File_Grids_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "FileListGrid.h"
#include "FileListOps.h"
#include "DeviceData_Router.h"
#include "DCE/DataGrid.h"
#include "DCERouter/DCERouter.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_Variable.h"

#include "FileListOps.h"

//<-dceag-const-b->
File_Grids_Plugin::File_Grids_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: File_Grids_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDatabase_pluto_main = new Database_pluto_main();
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit=true;
		return;
	}
}

//<-dceag-dest-b->
File_Grids_Plugin::~File_Grids_Plugin()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool File_Grids_Plugin::Register()
//<-dceag-reg-e->
{
	m_pDatagrid_Plugin=NULL;
	ListCommand_Impl *pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Datagrid_Plugin_CONST);

	if( !pListCommand_Impl || pListCommand_Impl->size()!=1 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"File grids cannot find datagrid handler %s",(pListCommand_Impl ? "There were more than 1" : ""));
		return false;
	}

	m_pDatagrid_Plugin=(Datagrid_Plugin *) pListCommand_Impl->front();
/*
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&File_Grids_Plugin::DevicesCommands)),
		DGTYPE_DEVICES_ACTIONS);
*/
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&File_Grids_Plugin::FileList))
		,DATAGRID_Directory_Listing_CONST);


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
void File_Grids_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void File_Grids_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

class DataGridTable *File_Grids_Plugin::DevicesCommands(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;


	// temp

	pCell = new DataGridCell("row1","");
	pDataGrid->SetData(0,0,pCell);
	pCell = new DataGridCell("row2","");
	pDataGrid->SetData(0,1,pCell);

/*
	string::size_type posParms=0;
	int Type = atoi(StringUtils::Tokenize(Parms,",",posParms).c_str()); // 1=lights, 2=a/v equipment, 3=all others, 4=everything
	int PK_Controller = atoi(StringUtils::Tokenize(Parms,",",posParms).c_str());
	int PK_Device = atoi(StringUtils::Tokenize(Parms,",",posParms).c_str()); // The device to list all the actions for
	PlutoOrbiter *ptrController = m_mapPlutoOrbiter_Find(PK_Controller);
*/
	int iRow=0;
/*

	if( PK_Device!=0 )
	{
		DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
		if( !pDevice )
			return NULL;

		// Help the controller out since we could only attach 1 message to the datagrid
		// we weren't able to set it's device variable
		if( ptrController )
			ptrController->SetVariable(VARIABLE_PK_DEVICE_CONST,StringUtils::itos(PK_Device).c_str());

		IRInformation *pIRInformation = m_mapIRInformation_Find(pDevice->m_dwPK_Device);
		for(size_t i=0;i<=pDevice->m_iNumberCommands;++i)
		{
			Command *pCommand = m_pRouter->mapCommand_Find(pDevice->m_piCommands[i]);
			if( !pCommand )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"There's a null in the pDevice->m_mapCommand");
				continue;
			}

			if( pCommand->m_iPK_Command==COMMAND_AV_INPUT_SELECT_CONST && pIRInformation  && pIRInformation ->m_listToggleInputs.size()==0 )
				continue;
			if( pCommand->m_iPK_Command==COMMAND_AV_DSP_MODE_CONST && pIRInformation && pIRInformation->m_listToggleDSPModes.size()==0 )
				continue;
			if( (pCommand->m_iPK_Command==COMMAND_GEN_OFF_CONST || pCommand->m_iPK_Command==COMMAND_GEN_ON_CONST) && pIRInformation && pIRInformation->m_bTogglePower )
				continue;

			int PK_Command = pCommand->m_iPK_Command;
			string Description = pCommand->m_sDescription;

			pCell = new DataGridCell(Description,"");
			pCell->m_Colspan = 7;
			pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, PK_Command, 0);
			pDataGrid->SetData(0,iRow,pCell);

			pCell = new DataGridCell("Learn","");
			pCell->m_Colspan = 2;
			pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, COMMAND_LEARN_IR_CONST, 1, COMMANDPARAMETER_ID_CONST, StringUtils::itos(PK_Command).c_str());
			pDataGrid->SetData(7,iRow,pCell);

			++iRow;
		}

		map<int,int>::iterator itIODSP;
		for(itIODSP=pDevice->m_mapInputs.begin();itIODSP!=pDevice->m_mapInputs.end();++itIODSP)
		{
			Command *pCommand = m_pRouter->m_mapCommand[(*itIODSP).second];
			if( !pCommand )
				g_pPlutoLogger->Write(LV_CRITICAL,"There's a null in the pDevice->m_mapInput for non-existant action: %d (%d)",(*itIODSP).second,(*itIODSP).first);
			else
			{
				if( pIRInformation && pIRInformation->m_listToggleInputs.size()!=0 ) 
				{
					bool bToggleInput=false;
					list<int>::iterator it;
					for(it=pIRInformation->m_listToggleInputs.begin();it!=pIRInformation->m_listToggleInputs.end();++it)
					{
						if( (*it)==(*itIODSP).first )
						{
							bToggleInput=true;
							break;
						}
					}
					if( bToggleInput )
						continue;
				}
				pCell = new DataGridCell("Input: " + pCommand->m_sDescription,"");
				pCell->m_Colspan = 7;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, pCommand->m_iPK_Command, 0);
				pDataGrid->SetData(0,iRow,pCell);

				pCell = new DataGridCell("Learn","");
				pCell->m_Colspan = 2;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, COMMAND_LEARN_IR_CONST, 1, COMMANDPARAMETER_ID_CONST, StringUtils::itos(pCommand->m_iPK_Command).c_str());
				pDataGrid->SetData(7,iRow,pCell);

				++iRow;
			}
		}

		for(itIODSP=pDevice->m_mapOutputs.begin();itIODSP!=pDevice->m_mapOutputs.end();++itIODSP)
		{
			Command *pCommand = m_pRouter->m_mapCommand[(*itIODSP).second];
			if( !pCommand )
				g_pPlutoLogger->Write(LV_CRITICAL,"There's a null in the pDevice->m_mapOutput for non-existant action: %d (%d)",(*itIODSP).second,(*itIODSP).first);
			else
			{
				pCell = new DataGridCell("Output: " + pCommand->m_sDescription,"");
				pCell->m_Colspan = 8;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, pCommand->m_iPK_Command, 0);
				pDataGrid->SetData(0,iRow,pCell);

				pCell = new DataGridCell("Learn","");
				pCell->m_Colspan = 2;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, COMMAND_LEARN_IR_CONST, 1, COMMANDPARAMETER_ID_CONST, StringUtils::itos(pCommand->m_iPK_Command).c_str());
				pDataGrid->SetData(7,iRow,pCell);

				++iRow;
			}
		}

		for(itIODSP=pDevice->m_mapDSPModes.begin();itIODSP!=pDevice->m_mapDSPModes.end();++itIODSP)
		{
			Command *pCommand = m_pRouter->m_mapCommand[(*itIODSP).second];
			if( !pCommand )
				g_pPlutoLogger->Write(LV_CRITICAL,"There's a null in the pDevice->m_mapDSPMode for non-existant action: %d (%d)",(*itIODSP).second,(*itIODSP).first);
			else
			{
				if( pIRInformation && pIRInformation->m_listToggleDSPModes.size()!=0 ) 
				{
					list<int>::iterator it;
					for(it=pIRInformation->m_listToggleDSPModes.begin();it!=pIRInformation->m_listToggleDSPModes.end();++it)
					{
						if( (*it)==(*itIODSP).second )
							continue;  // This is a toggle input
					}
				}

				pCell = new DataGridCell("DSPMode: " + pCommand->m_sDescription,"");
				pCell->m_Colspan = 8;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, pCommand->m_iPK_Command, 0);
				pDataGrid->SetData(0,iRow,pCell);

				pCell = new DataGridCell("Learn","");
				pCell->m_Colspan = 2;
				pCell->m_pMessage = new Message(PK_Controller,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, COMMAND_LEARN_IR_CONST, 1, COMMANDPARAMETER_ID_CONST, StringUtils::itos(pCommand->m_iPK_Command).c_str());
				pDataGrid->SetData(7,iRow,pCell);

				++iRow;
			}
		}
		return pDataGrid;
	}
	string query;

	if( Type==1 )
		query = "SELECT PK_Device, FK_Room, S_Device.Description AS Device, S_DeviceTemplate.Description AS MasterDevice, Value AS ID "
			"FROM S_Device "
			"INNER  JOIN S_DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
			"LEFT  JOIN S_Device_DeviceData ON FK_Device = PK_Device "
			"WHERE FK_DeviceCategory = 6 AND ( FK_DeviceData IS  NULL  OR FK_DeviceData = 28 )";
	else
		query = "SELECT PK_Device, FK_Room, S_Device.Description AS Device, S_DeviceTemplate.Description AS MasterDevice "
			"FROM S_Device "
			"INNER  JOIN S_DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate ";

	query += " ORDER BY FK_Room";
	PlutoSqlResult result;
	MYSQL_ROW row=NULL;
	int LastRoom = -1;

	if( (result.r=mysql_query_result(query)) )
	{
		while( (row=mysql_fetch_row(result.r)) )
		{
			int PK_Device = atoi(row[0]);
			string Description="";
			int PK_Room = atoi(row[1]);
			DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
			IRInformation *pIRInformation = m_mapIRInformation_Find(pDevice->m_dwPK_Device);

			if( Type==2 && !pIRInformation )
				continue;
			else if( Type==3 && (pIRInformation || pDevice->m_dwPK_DeviceCategory==DEVICECATEGORY_LIGHTS_CONST) )
				continue;

			if( PK_Room!=LastRoom )
			{
				LastRoom = PK_Room; 
				// I added a check to see if the room is actually there.  This bombed
				// here while testing the controller.
				int oldsize = (int) m_pRouter->m_mapDCERoom.size();
				
				map<int,DCERoom *>::iterator iRoom = m_pRouter->m_mapDCERoom.find(PK_Room);
				if (iRoom != m_pRouter->m_mapDCERoom.end())
				{
					DCERoom *pRoom = (*iRoom).second; // Split to inspect pRoom
					if(pRoom)
						Description = pRoom->m_Description + "\n";
					else
					{
						int newsize = (int) m_pRouter->m_mapDCERoom.size();
						g_pPlutoLogger->Write(LV_WARNING, "Description not set due to invalid room desc ptr, os = %d, ns = %d",oldsize,newsize);
					}
				}
			}
			Description += row[2];
			if( row[3] )
				Description += string("(") + row[3] + ")";
			pCell = new DataGridCell(Description,"");
	        pCell->m_Colspan = 6;
			pDataGrid->SetData(0,iRow,pCell);

			if( Type==1 )
			{
				pCell = new DataGridCell("ON","");
				pCell->m_pMessage = new Message(0,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_GEN_ON_CONST,0);
				pDataGrid->SetData(6,iRow,pCell);

				pCell = new DataGridCell("OFF","");
				pCell->m_pMessage = new Message(0,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_GEN_OFF_CONST,0);
				pDataGrid->SetData(7,iRow,pCell);

				pCell = new DataGridCell("50%","");
				pCell->m_pMessage = new Message(0,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_LIT_DIM_CONST,1,
					C_COMMANDPARAMETER_ABS_LEVEL_CONST,"50");
				pDataGrid->SetData(8,iRow,pCell);
			}
			else if( pDevice )
			{
				if( pDevice->SupportsCommand(COMMAND_GEN_ON_CONST) )
				{
					pCell = new DataGridCell("ON","");
					pCell->m_pMessage = new Message(0,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_GEN_ON_CONST,0);
					pDataGrid->SetData(6,iRow,pCell);
				}
				if( pDevice->SupportsCommand(COMMAND_GEN_OFF_CONST) )
				{
					pCell = new DataGridCell("OFF","");
					pCell->m_pMessage = new Message(0,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_GEN_OFF_CONST,0);
					pDataGrid->SetData(7,iRow,pCell);
				}

				pCell = new DataGridCell("...",StringUtils::itos(PK_Device));
				DERC::CMD_Populate_Datagrid msg(PK_Controller,DEVICECATEGORY_DATAGRID,"",GridID,X,"0," + StringUtils::itos(PK_Controller) + "," + StringUtils::itos(PK_Device)).c_str()));
				pCell->m_pMessage = msg.m_pMessage;
				pDataGrid->SetData(8,iRow,pCell);
			}

			iRow++;
		}
	}
	else
		g_pPlutoLogger->Write(LV_CRITICAL,"Query: %s failed",query.c_str());
*/
	return pDataGrid;
}

class DataGridTable * File_Grids_Plugin::FileList(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
g_pPlutoLogger->Write(LV_WARNING,"Starting File list");
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	string::size_type pos2 = 0;
	StringUtils::Tokenize(GridID, "_", pos2);
	int PK_Controller = atoi(StringUtils::Tokenize(GridID, "_", pos2).c_str());

	// extract individual parameters
	string::size_type pos=0;

	// Paths can be a comma separated list of Paths.  The grid will be the contents
	// of all paths merged into 1 grid
	string Paths = StringUtils::Tokenize(Parms, "|", pos);

	// A comma separated list of file extensions.  Blank means all files
	string Extensions = StringUtils::Tokenize(Parms, "|", pos);

	// Because the orbiter cannot execute different actions for files than directories, it will pass an "Actions" 
	// parameter that will tell the file grid generator what type of command to attach to file entries.
	// Q = "queue this file", which is a start media with the queue flag set
	// P = "play this file", which is a normal start media command
	// S = "show the play button", which is a normal start media command
	string Actions = StringUtils::Tokenize(Parms, "|", pos);

	bool bSortByDate = StringUtils::Tokenize(Parms, "|", pos)=="1";

	// This grid is initially called by the orbiter without the iDirNumber and sSubDirectory
	// When the grid creates cells for sub-directories, it will create an action that re-populates,
	// like a recursive function, and indicate which of the directories it recursed and what path
	// here.  When it repopulates itself again, it will this time populate only the contents of the 
	// Path[iDirNumber] + sSubDirectory.  It will also add a 'parent' button that that drops
	// 1 path off the sSubDirectory and repopulates itself
	int iDirNumber = atoi(StringUtils::Tokenize(Parms, "|", pos).c_str());
	string sSubDirectory = StringUtils::Tokenize(Parms, "|", pos);

	if( Extensions.length()>3 && Extensions.substr(0,3)=="~MT" )
	{
		int PK_MediaType = atoi(Extensions.substr(3).c_str());
		Row_MediaType *pRow_MediaType=m_pDatabase_pluto_main->MediaType_get()->GetRow(PK_MediaType);
		if( !pRow_MediaType )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find Media Type: %d",PK_MediaType);
			return NULL;
		}
		Extensions = pRow_MediaType->Extensions_get();
	}


	string PathsToScan;
	if( sSubDirectory.length() )
	{
		pos=0;
		for(int i=0;i<=iDirNumber;++i)
			PathsToScan = StringUtils::Tokenize(Paths, ",", pos);
		PathsToScan += "/" + sSubDirectory;
	}
	else 
		PathsToScan = Paths;

	list<FileDetails *> listFileDetails;
	GetDirContents(listFileDetails,PathsToScan,bSortByDate,Extensions);
g_pPlutoLogger->Write(LV_WARNING, "Build grid, actions %s GOT %d entries ", Actions.c_str(),(int) listFileDetails.size());

	int iRow=0;
	if( sSubDirectory.length() )
	{
		string sParent = FileUtils::BasePath(sSubDirectory);
		pCell = new DataGridCell("Go to parent", "");
		string newParams = Paths + "|" + Extensions + "|" + Actions + "|" + (bSortByDate ? "1" : "0") 
			+ "|" + StringUtils::itos(iDirNumber)+ "|" + sParent;
		DCE::CMD_NOREP_Populate_Datagrid_DT CMDPDG(PK_Controller, DEVICETEMPLATE_Datagrid_Plugin_CONST, BL_SameHouse,
			"", GridID, DATAGRID_Directory_Listing_CONST, newParams);
		pCell->m_pMessage = CMDPDG.m_pMessage;
		pCell->m_Colspan = 6;
		pDataGrid->SetData(1, iRow++, pCell);
	}

	for (list<FileDetails *>::iterator i = listFileDetails.begin(); i != listFileDetails.end(); i++)
	{
		FileDetails *pFileDetails = *i;
		pCell = new DataGridCell(pFileDetails->m_sFileName + " " + pFileDetails->m_sDescription, pFileDetails->m_sBaseName + pFileDetails->m_sFileName);
		pCell->m_Colspan = 6;
g_pPlutoLogger->Write(LV_WARNING, "Added '%s' to datagrid", pFileDetails->m_sFileName.c_str());
		if (pFileDetails->m_bIsDir)
		{
			string newParams = Paths + "|" + Extensions + "|" + Actions + "|" + (bSortByDate ? "1" : "0") 
				+ "|" + StringUtils::itos(iDirNumber)+ "|" + sSubDirectory + pFileDetails->m_sFileName + "/";
			DCE::CMD_NOREP_Populate_Datagrid_DT CMDPDG(PK_Controller, DEVICETEMPLATE_Datagrid_Plugin_CONST, BL_SameHouse,
				"", GridID, DATAGRID_Directory_Listing_CONST, newParams);
			pCell->m_pMessage = CMDPDG.m_pMessage;
		}
		else if( Actions.length() )
		{
			if( Actions.find('Q')!=string::npos )
			{
	g_pPlutoLogger->Write(LV_WARNING, "Added file %s with actions to datagrid", pFileDetails->m_sFileName.c_str());
				// The Orbiter wants us to attach an action to files too
				DCE::CMD_MH_Play_Media_Cat cmd(PK_Controller, DEVICECATEGORY_Media_Plugins_CONST, false, BL_SameHouse,
					0 /* any device */,"" /* default remote */,pFileDetails->m_sBaseName + pFileDetails->m_sFileName,0 /* whatever media type the file is */,0 /* any master device */,0 /* current entertain area */);
				pCell->m_pMessage = cmd.m_pMessage;
			}
			else if( Actions.find('S')!=string::npos )
			{
				// The Orbiter wants us to attach an action to files too
				DCE::CMD_Show_Object cmd(PK_Controller, PK_Controller, StringUtils::itos(DESIGNOBJ_butPreviewFileList_CONST), 0, "", "", "1");
				pCell->m_pMessage = cmd.m_pMessage;
			}
		}
		delete pFileDetails; // We won't need it anymore and it was allocated on the heap
		pDataGrid->SetData(1, iRow++, pCell);
	}

	return pDataGrid;

	//	string IconFile;  // The generic icon for this media type

/*
//	FileListGrid *pDataGrid = new FileListGrid(this);

//	pDataGrid->m_sIconFile = IconFile;
//	pDataGrid->m_iType=TypeOfRequest;

	pos=-1;  // this way +1 will be 0
	int SlashCount=0;
	string::size_type LastSlash=0;
//#ifdef WIN32
//	while( (pos=Path.find('\\',pos+1))!=string::npos && pos<Path.length()-1 )
//#else
	while( (pos=Path.find('/',pos+1))!=string::npos && pos<Path.length()-1 )
//#endif
	{
		LastSlash=pos;
		++SlashCount;
	}

	int row=0;
	listDCEFileInfo::iterator iList;
	for(iList=listFileNames->begin();iList!=listFileNames->end();++iList)
	{
		DCEFileInfo *fi = (*iList);
			string FullPath = fi->m_sPath + fi->m_sRealFileName;

// If this is a windows server, convert the \ back to / so the Linux DVD player will play it
#ifdef WIN32
			string::size_type s;
			while( (s=FullPath.find('\\'))!=string::npos )
				FullPath.replace(s,1,"/");
#endif

			string label = "`S24`" + fi->m_sDisplayName;
			// Don't forget the style since the scrolling arrows will need it
			if( fi->m_bIsDirectory )
				pCell = new DataGridCell ("`S24`","");  // We want the variable to be empty so it hides the play button
			else
				pCell = new DataGridCell ("`S24`",FullPath);
			pDataGrid->SetData(0, row, pCell);
			if( fi->m_bIsDirectory )
			{
				pCell->m_pMessage = new Message(DEVICEID_DATAGRID,DEVICEID_DATAGRID,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, COMMAND_POPULATE_DATAGRID_CONST, 2, 
					COMMANDPARAMETER_DataGrid_ID_CONST, GridID.c_str(),
					COMMANDPARAMETER_Type_CONST, ("1," + StringUtils::itos(TypeOfRequest) + "," + fi->m_sPath + fi->m_sRealFileName + "/," + sPK_User).c_str());
			}
			else
				FileList+=FullPath + "|";

			FileListInfo *flInfo;
			if( fi->m_bIsDirectory )
			{
				flInfo = new FileListInfo(true,fi->m_sPath + fi->m_sRealFileName);
				pCell = new DataGridCell(label,"");  // We want the variable to be empty so it hides the play button
			}
			else
			{
				flInfo = new FileListInfo(false,fi->m_sPath + fi->m_sRealFileName);
				pCell = new DataGridCell(label, FullPath);
			}
			pCell->m_Colspan=6;
			if( fi->m_bIsDirectory )
			{
				pCell->m_pMessage = new Message(DEVICEID_DATAGRID,DEVICEID_DATAGRID,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, COMMAND_POPULATE_DATAGRID_CONST, 2, 
					COMMANDPARAMETER_DataGrid_ID_CONST, GridID.c_str(),
					COMMANDPARAMETER_Type_CONST, ("1," + StringUtils::itos(TypeOfRequest) + "," + fi->m_sPath + fi->m_sRealFileName + "/," + sPK_User).c_str());
			}
			pDataGrid->m_vectFileInfo.push_back(flInfo);
			pDataGrid->SetData(1, row++, pCell);
	}
	g_pPlutoLogger->Write (LV_STATUS, "ready to list sub dirs");
//	if( Path!="" )
//		Parent += Path + "|";

	g_pPlutoLogger->Write(LV_STATUS,"File list: %s",FileList.c_str());
	if( PK_Controller )
	{
		// Set the variable for the 'play all' button
		PlutoOrbiter *ptrController = m_mapPlutoOrbiter_Find(PK_Controller);
		if( ptrController )
			ptrController->SetVariable(VARIABLE_SHORTCUT2_CONST,FileList.c_str());
		if( FileList.length()>0 && TypeOfRequest!=USING_ID_LIBRARY_MOVIES && TypeOfRequest!=USING_ID_LIBRARY_PLAYLISTS )
		{
			// Unhide
			m_pRouter->DispatchMessage(new Message(0, PK_Controller,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_NAV_HIDE_OBJECT_CONST,
				2,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_BUTPLAYALL_CONST).c_str(),COMMANDPARAMETER_OnOff_CONST,"0"));
		}
		else
		{
			m_pRouter->DispatchMessage(new Message(0, PK_Controller,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_NAV_HIDE_OBJECT_CONST,
				1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_BUTPLAYALL_CONST).c_str()));
		}
	}

	delete listFileNames;
	g_pPlutoLogger->Write(LV_STATUS,"Took %d to make the file list",c-clock());
*/
g_pPlutoLogger->Write(LV_STATUS,"End File list");
	return pDataGrid;
}



/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void File_Grids_Plugin::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->
