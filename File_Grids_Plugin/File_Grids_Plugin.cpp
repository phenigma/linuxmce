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
/*
	pCell = new DataGridCell("row1","");
	pDataGrid->SetData(0,0,pCell);
	pCell = new DataGridCell("row2","");
	pDataGrid->SetData(0,1,pCell);

	return pDataGrid;
*/
//	clock_t c = clock();

	string::size_type pos2 = 0;
	StringUtils::Tokenize(GridID, "_", pos2);
	int PK_Controller = atoi(StringUtils::Tokenize(GridID, "_", pos2).c_str());

	// extract individual parameters
	string::size_type pos=0;
	string TypeOfRequest = StringUtils::Tokenize(Parms, ",", pos);
	string Path = StringUtils::Tokenize(Parms, ",", pos);  // this can be in the format path | parent for easy use of an 'up'
	string sPK_User = StringUtils::Tokenize(Parms, ",", pos);
	bool bSortByDate = StringUtils::Tokenize(Parms, ",", pos)=="1";
	string Extensions=".*"; // Default get all files
	bool bIncludeParent=true;

	if( TypeOfRequest.substr(0,2)=="MT" )  // come up with a more elegant protocol
	{
		*iPK_Variable=VARIABLE_Path_CONST;
		int PK_MediaType = atoi(TypeOfRequest.substr(2).c_str());
		Row_MediaType *pRow_MediaType=m_pDatabase_pluto_main->MediaType_get()->GetRow(PK_MediaType);
		if( !pRow_MediaType )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find Media Type: %d",PK_MediaType);
			return NULL;
		}
// todo -- just show all files for now			Extensions = pRow_MediaType->Extensions_get();
		string sSubDirectory = pRow_MediaType->Subdirectory_get();
		if( sSubDirectory.length() )
			sSubDirectory += "/";
		string sUserPath = "/home/user_" + sPK_User + "/data/" + sSubDirectory;
		string sPublicPath = "/home/public/data/" + sSubDirectory;
		if( Path.length()==0 || Path==sUserPath || Path==sPublicPath )
		{
			Path = sUserPath + "," + sPublicPath;
			*sValue_To_Assign = sSubDirectory;
			bIncludeParent=false;
		}
		else
			*sValue_To_Assign = Path;
	}
	// Because the orbiter cannot execute different actions for files than directories, it will pass an "Actions" 
	// parameter that will tell the file grid generator what type of command to attach to file entries.
	// Q = "queue this file", which is a start media with the queue flag set
	// P = "play this file", which is a normal start media command
	string Actions = StringUtils::Tokenize(Parms, ",", pos);
	string FileList="";

	list<FileDetails> listFileDetails = GetDirContents(Path, sPK_User, bSortByDate,Extensions,bIncludeParent);
g_pPlutoLogger->Write(LV_WARNING, "Build grid, actions %s GOT %d entries ", Actions.c_str(),(int) listFileDetails.size());

	int row = 0;
	for (list<FileDetails>::iterator i = listFileDetails.begin(); i != listFileDetails.end(); i++, row++)
	{
		pCell = new DataGridCell(i->m_sFileName + " " + i->m_sDescription, i->m_sBaseName + i->m_sFileName);
		if (i->m_bIsDir)
		{
g_pPlutoLogger->Write(LV_WARNING, "Added dir '%s' to datagrid", i->m_sFileName.c_str());
			// TypeOfRequest,Path,PK_User,SortByDate(1=true)
			string newParams = TypeOfRequest + "," + i->m_sBaseName + i->m_sFileName + "," + sPK_User + ","
				+ (bSortByDate ? "1" : "0") + "," + Actions;
			DCE::CMD_NOREP_Populate_Datagrid_MD CMDPDG(PK_Controller, DEVICETEMPLATE_Datagrid_Plugin_CONST, BL_SameHouse,
				"DataGrid ID (Debug info only) goes here", GridID, DATAGRID_Directory_Listing_CONST, newParams);
			pCell->m_pMessage = CMDPDG.m_pMessage;
//			pCell->m_pMessage = new Message(DEVICEID_DATAGRID, DEVICEID_DATAGRID, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_POPULATE_DATAGRID_CONST, 2, 
//				COMMANDPARAMETER_DataGrid_ID_CONST, GridID.c_str(),
//				COMMANDPARAMETER_Type_CONST, ("1," + StringUtils::itos(TypeOfRequest) + "," + fi->m_sPath + fi->m_sRealFileName + "/," + sPK_User).c_str());
		}
		else if( Actions.length() )
		{
g_pPlutoLogger->Write(LV_WARNING, "Added file %s with actions to datagrid", i->m_sFileName.c_str());
			// The Orbiter wants us to attach an action to files too
			DCE::CMD_MH_Play_Media_Cat cmd(PK_Controller, DEVICECATEGORY_Media_Plugins_CONST, false, BL_SameHouse,
				0 /* any device */,"" /* default remote */,i->m_sBaseName + i->m_sFileName,0 /* whatever media type the file is */,0 /* any master device */,0 /* current entertain area */);
			pCell->m_pMessage = cmd.m_pMessage;
		}
		pDataGrid->SetData(0, row, pCell);
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


/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

void File_Grids_Plugin::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DERC:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Send Orbiters the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.

	// Send the message to a specific orbiter, identified by OrbiterID
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);

	// Send the message to orbiters 32898 and 27283
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,"32898,27283",55,77);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);

	// This time we want to wait to be sure the orbiter gets the message, and the thread will block until the orbiter receives the message
	string sResult="";
	bool bResult = SendMessageWithConfirm(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77),sResult);
	// If bResult is true, the message was received ok.  Otherwise it failed, and sResult contains an explanation of the failure

	// A request is like a command, except that it has both "in" and "out" parameters, and the 
	// thread blocks until the request is completed.  Note that requests are sent on a separate socket, so you can continue to receive commands and send
	// events while a thread is waiting for a request to complete.  However the SendMessageWithConfirm uses the same request socket, and will wait if
	// the socket is busy.  

	// We want to request the contents of a file.  This request takes 1 "in" parameter, the file name, and has 2 "out" parameters: the file's contents, and the date.
	// All out parameters are passed by reference.
	// Note that the "Data" parameter which contains the file is a binary parameter.  Whether in or out, binary parameters always take both a pointer to the object and the size
	// This command will be sent to the "Standard Plug-in" which implements this type of request.  We set the boardcast level to BL_SameHouse.  Note that since this is a
	// request, the router will find the first matching device (ie the first device "Standard Plug-in") and return that device's response.  If this was a command, not a request,
	// and there were multiple devices "Standard Plug-in" they would all get the message.
	PlutoDate plutoDate;
	char *FileContents;
	int FileSize;
	bool bResult = SendRequest(new DERC::REQ_File_Contents(m_dwPK_Device,DeviceTemplate_Standard_Plug_In_CONST,true,BL_SameHouse,"some_file_name",&FileContents,&FileSize,&plutoDate);

	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();


}
*/

/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

void File_Grids_Plugin::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DERC:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Send Orbiters the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.

	// Send the message to a specific orbiter, identified by OrbiterID
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);

	// Send the message to orbiters 32898 and 27283
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,"32898,27283",55,77);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);

	// This time we want to wait to be sure the orbiter gets the message, and the thread will block until the orbiter receives the message
	string sResult="";
	bool bResult = SendMessageWithConfirm(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77),sResult);
	// If bResult is true, the message was received ok.  Otherwise it failed, and sResult contains an explanation of the failure

	// A request is like a command, except that it has both "in" and "out" parameters, and the 
	// thread blocks until the request is completed.  Note that requests are sent on a separate socket, so you can continue to receive commands and send
	// events while a thread is waiting for a request to complete.  However the SendMessageWithConfirm uses the same request socket, and will wait if
	// the socket is busy.  

	// We want to request the contents of a file.  This request takes 1 "in" parameter, the file name, and has 2 "out" parameters: the file's contents, and the date.
	// All out parameters are passed by reference.
	// Note that the "Data" parameter which contains the file is a binary parameter.  Whether in or out, binary parameters always take both a pointer to the object and the size
	// This command will be sent to the "Standard Plug-in" which implements this type of request.  We set the boardcast level to BL_SameHouse.  Note that since this is a
	// request, the router will find the first matching device (ie the first device "Standard Plug-in") and return that device's response.  If this was a command, not a request,
	// and there were multiple devices "Standard Plug-in" they would all get the message.
	PlutoDate plutoDate;
	char *FileContents;
	int FileSize;
	bool bResult = SendRequest(new DERC::REQ_File_Contents(m_dwPK_Device,DeviceTemplate_Standard_Plug_In_CONST,true,BL_SameHouse,"some_file_name",&FileContents,&FileSize,&plutoDate);

	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();


}
*/
//<-dceag-sample-e->
