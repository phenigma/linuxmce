#include "FileListGrid.h"

#define USING_ID_PLUTO_TV 1
#define USING_ID_PLUTO_RADIO 2
#define USING_ID_PLUTO_MYDISC 3
#define USING_ID_PLUTO_JUKEBOXES 4
#define USING_ID_PLUTO_COMPUTING 5
#define USING_ID_PLUTO_BURNING 6
#define USING_ID_LIBRARY_RECORDED_TV	11
#define USING_ID_LIBRARY_MUSIC			12
#define USING_ID_LIBRARY_MOVIES			13
#define USING_ID_LIBRARY_PICTURES		14
#define USING_ID_LIBRARY_HOMEVIDEOS		15
#define USING_ID_LIBRARY_PLAYLISTS		16

#ifndef WIN32
#include <dirent.h>
#endif

class DataGridTable *DCEMI_PS_VirtualDevices::PopulateWithDevicesCommands(string GridID,string Parms,void *ExtraData)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	string::size_type posParms=0;
	int Type = atoi(StringUtils::Tokenize(Parms,",",posParms).c_str()); // 1=lights, 2=a/v equipment, 3=all others, 4=everything
	int PK_Controller = atoi(StringUtils::Tokenize(Parms,",",posParms).c_str());
	int PK_Device = atoi(StringUtils::Tokenize(Parms,",",posParms).c_str()); // The device to list all the actions for
	PlutoOrbiter *ptrController = m_mapPlutoOrbiter_Find(PK_Controller);

	int iRow=0;

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
				pCell->m_pMessage = new Message(PK_Controller,DEVICEID_DATAGRID,PRIORITY_NORMAL,MESSAGETYPE_COMMAND, COMMAND_POPULATE_DATAGRID_CONST, 2, 
					COMMANDPARAMETER_DataGrid_ID_CONST, GridID.c_str(),
					COMMANDPARAMETER_Type_CONST, ("19,0," + StringUtils::itos(PK_Controller) + "," + StringUtils::itos(PK_Device)).c_str());
				pDataGrid->SetData(8,iRow,pCell);
			}

			iRow++;
		}
	}
	else
		g_pPlutoLogger->Write(LV_CRITICAL,"Query: %s failed",query.c_str());

	return pDataGrid;
}

static bool DCEFileInfoCompareFilename(DCEMI_PS_VirtualDevices::DCEFileInfo *x, DCEMI_PS_VirtualDevices::DCEFileInfo *y)
{
	return x->m_sCapsFileName < y->m_sCapsFileName ||
		(!(y->m_sCapsFileName < x->m_sCapsFileName) &&
		 difftime(x->m_tCreationDate, y->m_tCreationDate) < 0);
}

#define FILESPEC_AUDIO		5001
#define FILESPEC_VIDEO		5002
#define FILESPEC_ALLFILES	5003

class DataGridTable *DCEMI_PS_VirtualDevices::PopulateWithFileList(string GridID,string Parms,void *ExtraData)
{
	clock_t c = clock();

	string::size_type pos2 = 0;
	StringUtils::Tokenize(GridID, "_", pos2);
	int PK_Controller = atoi(StringUtils::Tokenize(GridID, "_", pos2).c_str());

	DataGridCell *pCell;

	string::size_type pos=0;
	int TypeOfRequest = atoi(StringUtils::Tokenize(Parms, ",", pos).c_str());
	string Path = StringUtils::Tokenize(Parms, ",", pos);  // this can be in the format path | parent for easy use of an 'up'
	string sPK_User = StringUtils::Tokenize(Parms, ",", pos);
	bool bSortByDate = StringUtils::Tokenize(Parms, ",", pos)=="1";
	string FileList="";

	string IconFile;  // The generic icon for this media type
	listDCEFileInfo *listFileNames = CreateDCEFileInfo(TypeOfRequest,Path,sPK_User,bSortByDate,IconFile);

	FileListGrid *pDataGrid = new FileListGrid(this);

	pDataGrid->m_sIconFile = IconFile;
	pDataGrid->m_iType=TypeOfRequest;

	pos=-1;  // this way +1 will be 0
	int SlashCount=0;
	string::size_type LastSlash=0;
#ifdef WIN32
	while( (pos=Path.find('\\',pos+1))!=string::npos && pos<Path.length()-1 )
#else
	while( (pos=Path.find('/',pos+1))!=string::npos && pos<Path.length()-1 )
#endif
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
	return pDataGrid;
};

DCEMI_PS_VirtualDevices::listDCEFileInfo *DCEMI_PS_VirtualDevices::CreateDCEFileInfo(int TypeOfRequest,string Path,string sPK_User,bool bSortByDate,string &IconFile)
{
	listDCEFileInfo *listFileNames = new listDCEFileInfo();

	if( Path=="/" )
		Path=""; // Ignore a sole slash
#ifdef WIN32
	if( Path=="\\" )
		Path=""; // Ignore a sole slash

	intptr_t ptrFileList;
	_finddata_t finddata;
#endif

	string SubDirectory="";

	if( TypeOfRequest!=USING_ID_PLUTO_JUKEBOXES )
	{
		string ValidExtensions = "*.*";
		switch(TypeOfRequest)
		{
		case USING_ID_LIBRARY_MUSIC:
			ValidExtensions = "*.mp3,*.ogg,*.flac,*.wav";
			SubDirectory = "music";
			IconFile="/home/media/miscicons/music.png";
			break;
		case USING_ID_LIBRARY_MOVIES:
			ValidExtensions = "*.dvd";
			SubDirectory = "movies";
			IconFile="/home/media/miscicons/movies.png";
			break;
		case USING_ID_LIBRARY_RECORDED_TV:
			ValidExtensions = "*.vob,*.mpg,*.avi,*.mov";
			SubDirectory = "tv shows";
			IconFile="/home/media/miscicons/vcr.png";
			break;	
		case USING_ID_LIBRARY_HOMEVIDEOS:
			ValidExtensions = "*.vob,*.dv,*.mpg,*.avi,*.mov";
			SubDirectory = "home videos";
			IconFile="/home/media/miscicons/vcr.png";
			break;
		case USING_ID_LIBRARY_PICTURES:
			ValidExtensions = "*.bmp,*.jpg,*.png";
			SubDirectory = "pictures";
			IconFile="/home/media/miscicons/stillcamera.png";
			break;
		case USING_ID_LIBRARY_PLAYLISTS:
			ValidExtensions = "*.ppl";
			SubDirectory = "play lists";
			break;
		case FILESPEC_AUDIO:
			ValidExtensions = "*.mp3,*.ogg,*.flac,*.wav";
			break;
		case FILESPEC_VIDEO:
			ValidExtensions = "*.vob";
			break;
		case FILESPEC_ALLFILES:
			break;
		default:
			g_pPlutoLogger->Write(LV_CRITICAL,"PopulateWithFileList - Dont know how to handle file types: %d",TypeOfRequest);
			delete listFileNames;
			return NULL;
		}

		string BasePath;

		for(int iLoop=0;iLoop <= (Path == "" ? 1 : 0);++iLoop)
		{
			g_pPlutoLogger->Write (LV_STATUS, "file loop %d",iLoop);

			if( Path == "")
			{
#ifdef WIN32
				if( iLoop==0 )
					BasePath = "Z:\\";
				else if( iLoop==1 )
					continue;
#else
				if( iLoop==0 )
					BasePath = "/home/user_" + sPK_User + "/data/";
				else if( iLoop==1 )
					BasePath = "/home/public/data/";
#endif
				if( SubDirectory!="" )
					BasePath += SubDirectory + "/";
			}
			else
				BasePath = Path;

#ifdef WIN32
			string::size_type s;
			while( (s=BasePath.find('/'))!=string::npos )
				BasePath.replace(s,1,"\\");

			string::size_type pos=0;

			ptrFileList = _findfirst( (BasePath+"*.*").c_str(),&finddata);
			while( ptrFileList!=-1 ) 
			{
				if( (finddata.attrib==_A_SUBDIR) && finddata.name[0]!='.')
				{
					DCEFileInfo *fi = new DCEFileInfo(BasePath,finddata.name,(iLoop==0),true,time(NULL));
					listFileNames->push_back(fi);
				}
				else if (finddata.name[0] != '.')
				{
					pos=0;
					for(;;)
					{
						string s = StringUtils::Tokenize(ValidExtensions,",",pos);
						if( s.substr(0,1)=="*" )
							s = s.substr(1);
						if( s=="" )
							break;
						if ( s == ".*" || strstr(finddata.name, s.c_str())!=NULL )
						{
							DCEFileInfo *fi = new DCEFileInfo(BasePath,finddata.name,(iLoop==0),false,time(NULL));
							listFileNames->push_back(fi);
							break;
						}
					}
				}
				if( _findnext(ptrFileList,&finddata)<0 )
					break;
			}

			_findclose(ptrFileList);

#else
			DIR *dirp = opendir( BasePath.c_str() );
			struct dirent entry;
			struct dirent *direntp = &entry;

			if (dirp == NULL)
			{
				g_pPlutoLogger->Write( LV_CRITICAL, "opendir1 %s failed: %s", BasePath.c_str(), strerror(errno));
				delete listFileNames;
				return NULL;
			}
			int x;
			while ( dirp != NULL && (readdir_r( dirp, direntp, &direntp ) == 0) && direntp )
			{
				if( entry.d_type == DT_DIR && entry.d_name[0] != '.')
				{
					DCEFileInfo *fi = new DCEFileInfo(BasePath,entry.d_name,(iLoop==0),true,time(NULL));
					listFileNames->push_back(fi);
//					ostringstream ss;
//					ss << TypeOfRequest << "," << FileSpec << entry.d_name << "/";
//					ss << "," << sPK_User << (bSortByDate? "1" : "0") << std::ends;
//					listSubDirectories.push_back(ss.str());
				}
				else if (entry.d_name[0] != '.')
				{
					string::size_type pos=0;
					for(;;)
					{
						string s = StringUtils::Tokenize(ValidExtensions,",",pos);
						if( s.substr(0,1)=="*" )
							s = s.substr(1);
						if( s=="" )
							break;
						if ( s == ".*" || strstr(entry.d_name, s.c_str())!=NULL )
						{
							DCEFileInfo *fi = new DCEFileInfo(BasePath,entry.d_name,(iLoop==0),false,time(NULL));
							listFileNames->push_back(fi);
							break;
						}
					}
				}
			}
			closedir( dirp );
#endif
		}
	}
	if( Path!="" )
	{
		string::size_type pos=-1;  // this way +1 will be 0
		int SlashCount=0;
		string::size_type LastSlash=0;
#ifdef WIN32
		string::size_type s;
		while( (s=Path.find('/'))!=string::npos )
			Path.replace(s,1,"\\");
		while( (pos=Path.find('\\',pos+1))!=string::npos && pos<Path.length()-1 )
#else
		while( (pos=Path.find('/',pos+1))!=string::npos && pos<Path.length()-1 )
#endif
		{
			LastSlash=pos;
			++SlashCount;
		}

//g_pPlutoLogger->Write(LV_WARNING,"path: %s last slash: %d slashCount: %d subdirectory",
//				   Path.c_str(),LastSlash,SlashCount,SubDirectory.c_str());

		string Parent="";
#ifdef WIN32
		if( (SlashCount<=2 && SubDirectory.length()!=0) || SlashCount<=1 )
#else
		if( (SlashCount<=5 && SubDirectory.length()!=0) || SlashCount<=4 )
#endif
		{
//g_pPlutoLogger->Write(LV_WARNING,"the directory above is the parent");

		}
		else
			Parent = Path.substr(0,LastSlash);

		DCEFileInfo *fi = new DCEFileInfo(Parent,"","** Go up to the parent directory **",false,true,time(NULL));
		listFileNames->push_back(fi);
		
	}

	g_pPlutoLogger->Write (LV_STATUS, "ready to list files");

	listFileNames->sort(DCEFileInfoCompareFilename);
	list<DCEFileInfo *>::iterator iList;

	for(iList=listFileNames->begin();iList!=listFileNames->end();)
	{
		DCEFileInfo *fi = (*iList);
		// See if we've got a series of _0001.vob, _0002.vob, etc. from the pvr files
		if( fi->m_sRealFileName.length()>9 && fi->m_sRealFileName.substr( fi->m_sRealFileName.length()-4 )==".vob" && fi->m_sRealFileName.substr( fi->m_sRealFileName.length()-9,1 )=="_" )
		{
			fi->m_sDisplayName = fi->m_sRealFileName.substr(0,fi->m_sRealFileName.length()-9);

			++iList;
			while(iList!=listFileNames->end() && (*iList)->m_sRealFileName.substr(0,fi->m_sDisplayName.length())==fi->m_sDisplayName )
			{
				fi->m_sRealFileName += "|" + (*iList)->m_sPath + (*iList)->m_sRealFileName;
				delete (*iList);
				listFileNames->erase(iList++);
			}
		}
		else 
			++iList;
	}

	return listFileNames;
}


class ResetAV
{
public:
	DeviceData_Router *pDevice;
	int PK_Input;
	int PK_DSPMode;
	int PK_Command;

	ResetAV(DeviceData_Router *Device,int Input,int DSPMode,int Command) 
	{
		pDevice=Device;
		PK_Input=Input;
		PK_DSPMode=DSPMode;
		PK_Command=Command;
	}
};

/*
class DataGridTable *DCEMI_PS_VirtualDevices::PopulateWithWebFavorites(string GridID,string Parms)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
	string::size_type posParms=0;
	string PK_Users = StringUtils::Tokenize(Parms,",",posParms);

	string query = "SELECT URL,Description from WEB_Favorite WHERE FK_Users=" + PK_Users + " OR FK_Users=0 ORDER BY OrderNum";
	PlutoSqlResult result;
	MYSQL_ROW row=NULL;
	int iRow=0;

	if( (result.r=m_pCore->mysql_query_result(query)) )
	{
		while( (row=mysql_fetch_row(result.r)) )
		{
			if( row[0] )  // URL cannot be empty
			{
				if( row[1] )
					pCell = new DataGridCell(row[1],row[0]);
				else
					pCell = new DataGridCell(row[0],row[0]);

				pDataGrid->SetData(0,iRow,pCell);
			}

			iRow++;
		}
	}
	return pDataGrid;
}

class DataGridTable *DCEMI_PS_VirtualDevices::PopulateWithMediaAttrSearchAC(string GridID,string Parms)
{
	FileListGrid *pDataGrid = new FileListGrid(m_pCore);
	DataGridCell *pCell;

    string AC = Parms;

	string SQL = "select PK_MED_Attribute,Name,FirstName,Description FROM MED_Attribute " \
		"JOIN MED_C_AttributeType ON MED_Attribute.FK_MED_C_AttributeType=PK_MED_C_AttributeType "\
		"JOIN MED_C_Type_C_AttributeType ON MED_C_Type_C_AttributeType.FK_MED_C_AttributeType=PK_MED_C_AttributeType "\
		"WHERE (Name Like '" + AC + "%' OR FirstName Like '" + AC + "%') AND Identifier>0 "\
		"ORDER BY Name limit 30;";

	PlutoSqlResult result;
	MYSQL_ROW row;
	int RowCount=0;

	if( (result.r=m_pCore->mysql_query_result(SQL)) )
	{
		while( (row=mysql_fetch_row(result.r)) )
		{
			//string label = string("~`S24`") + row[1];
			string label = row[1];
			if( row[2] && *row[2] )
				label += string(",") + row[2];
			label += string("\n`S1`") + row[3];
			pCell = new DataGridCell("",row[0]);
			pDataGrid->SetData(0,RowCount,pCell);

			pCell = new DataGridCell(label,row[0]);
			pCell->m_Colspan = 5;
			pDataGrid->m_vectFileInfo.push_back(new FileListInfo(atoi(row[0])));
			pDataGrid->SetData(1,RowCount++,pCell);
		}
	}

	SQL = "select PK_MED_Attribute,Name,FirstName,Description FROM MED_SearchToken "\
		"JOIN MED_SearchToken_Attribute ON PK_MED_SearchToken=FK_MED_SearchToken "\
		"JOIN MED_Attribute ON FK_MED_Attribute=PK_MED_Attribute "\
		"JOIN MED_C_AttributeType ON FK_MED_C_AttributeType=PK_MED_C_AttributeType "\
		"WHERE Token like '" + AC + "%' "\
		"limit 30;";
	if( (result.r=m_pCore->mysql_query_result(SQL)) )
	{
		while( (row=mysql_fetch_row(result.r)) )
		{
			string label = row[1];
			if( row[2] && *row[2]  )
				label += string(",") + row[2];
			label += string("\n`S1`") + row[3];
			pCell = new DataGridCell("",row[0]);
			pDataGrid->SetData(0,RowCount,pCell);

			pCell = new DataGridCell(label,row[0]);
			pCell->m_Colspan = 5;
			pDataGrid->m_vectFileInfo.push_back(new FileListInfo(atoi(row[0])));
			pDataGrid->SetData(1,RowCount++,pCell);
		}
	}
	return pDataGrid;
}

class DataGridTable *DCEMI_PS_VirtualDevices::PopulateWithMediaAttrFiles(string GridID,string Parms)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	string PK_MED_Attribute = Parms;
	if( PK_MED_Attribute.substr(0,2)=="#A" )
		PK_MED_Attribute = PK_MED_Attribute.substr(2);
	else if( PK_MED_Attribute.substr(0,2)=="#F" )
		PK_MED_Attribute = StringUtils::itos(
			m_pMediaAttributes->GetAttributeFromFileID(atoi(PK_MED_Attribute.substr(2).c_str()))
		);


	string SQL="select DISTINCT Dest.FK_MED_File,MED_Attribute.Name,MED_Attribute.FirstName,MED_C_AttributeType.Description "\
		"FROM MED_File_Attribute As Source "\
		"JOIN MED_File_Attribute As Dest "\
		"ON Source.FK_MED_File=Dest.FK_MED_File AND Source.FK_MED_Attribute=" + PK_MED_Attribute +
		" JOIN MED_File ON Dest.FK_MED_File=PK_MED_File "\
		"JOIN MED_Attribute ON Dest.FK_MED_Attribute=PK_MED_Attribute "\
		"JOIN MED_C_AttributeType ON MED_Attribute.FK_MED_C_AttributeType=PK_MED_C_AttributeType "\
		"JOIN MED_C_Type_C_AttributeType ON MED_C_Type_C_AttributeType.FK_MED_C_Type=MED_File.FK_MED_C_Type "\
		"AND MED_C_Type_C_AttributeType.FK_MED_C_AttributeType=MED_Attribute.FK_MED_C_AttributeType "\
		"WHERE Identifier=1 ORDER BY Name limit 200;";

	PlutoSqlResult result;
	MYSQL_ROW row;
	int RowCount=0;

	if( (result.r=m_pCore->mysql_query_result(SQL)) )
	{
		while( (row=mysql_fetch_row(result.r)) )
		{
			string label = row[1];
			if( row[2] && *row[2] )
				label += string(",") + row[2];
			label += string("\n`S29`") + row[3];
			pCell = new DataGridCell(label,row[0]);
			pDataGrid->SetData(0,RowCount++,pCell);
		}
	}
	return pDataGrid;
}

class DataGridTable *DCEMI_PS_VirtualDevices::PopulateWithMediaAttrCollections(string GridID,string Parms)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	string PK_MED_Attribute = Parms;
	if( PK_MED_Attribute.substr(0,2)=="#A" )
		PK_MED_Attribute = PK_MED_Attribute.substr(2);
	else if( PK_MED_Attribute.substr(0,2)=="#F" )
		PK_MED_Attribute = StringUtils::itos(
			m_pMediaAttributes->GetAttributeFromFileID(atoi(PK_MED_Attribute.substr(2).c_str()))
		);

	string SQL="select DISTINCT Dest.FK_MED_Attribute,MED_Attribute.Name,MED_Attribute.FirstName,MED_C_AttributeType.Description "\
		"FROM MED_File_Attribute As Source "\
		"JOIN MED_File_Attribute As Dest "\
		"ON Source.FK_MED_File=Dest.FK_MED_File AND Source.FK_MED_Attribute=" + PK_MED_Attribute +
		" JOIN MED_File ON Dest.FK_MED_File=PK_MED_File "\
		"JOIN MED_Attribute ON Dest.FK_MED_Attribute=PK_MED_Attribute "\
		"JOIN MED_C_AttributeType ON MED_Attribute.FK_MED_C_AttributeType=PK_MED_C_AttributeType "\
		"JOIN MED_C_Type_C_AttributeType ON MED_C_Type_C_AttributeType.FK_MED_C_Type=MED_File.FK_MED_C_Type "\
		"AND MED_C_Type_C_AttributeType.FK_MED_C_AttributeType=MED_Attribute.FK_MED_C_AttributeType "\
		"WHERE Identifier=2 ORDER BY Name limit 100;";

	PlutoSqlResult result;
	MYSQL_ROW row;
	int RowCount=0;

	if( (result.r=m_pCore->mysql_query_result(SQL)) )
	{
		while( (row=mysql_fetch_row(result.r)) )
		{
			string label = row[1];
			if( row[2] )
				label += string(",") + row[2];
			label += string("\n`S29`") + row[3];
			pCell = new DataGridCell(label,row[0]);
			pDataGrid->SetData(0,RowCount++,pCell);
		}
	}
	return pDataGrid;
}

class DataGridTable *DCEMI_PS_VirtualDevices::PopulateWithMediaAttrXref(string GridID,string Parms)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	string PK_MED_Attribute = Parms;
	if( PK_MED_Attribute.substr(0,2)=="#A" )
		PK_MED_Attribute = PK_MED_Attribute.substr(2);
	else if( PK_MED_Attribute.substr(0,2)=="#F" )
		PK_MED_Attribute = StringUtils::itos(
			m_pMediaAttributes->GetAttributeFromFileID(atoi(PK_MED_Attribute.substr(2).c_str()))
		);

	string SQL="select DISTINCT Dest.FK_MED_Attribute,MED_Attribute.Name,MED_Attribute.FirstName,MED_C_AttributeType.Description "\
		"FROM MED_File_Attribute As Source "\
		"JOIN MED_File_Attribute As Dest "\
		"ON Source.FK_MED_File=Dest.FK_MED_File AND Source.FK_MED_Attribute=" + PK_MED_Attribute +
		" JOIN MED_File ON Dest.FK_MED_File=PK_MED_File "\
		"JOIN MED_Attribute ON Dest.FK_MED_Attribute=PK_MED_Attribute "\
		"JOIN MED_C_AttributeType ON MED_Attribute.FK_MED_C_AttributeType=PK_MED_C_AttributeType "\
		"JOIN MED_C_Type_C_AttributeType ON MED_C_Type_C_AttributeType.FK_MED_C_Type=MED_File.FK_MED_C_Type "\
		"AND MED_C_Type_C_AttributeType.FK_MED_C_AttributeType=MED_Attribute.FK_MED_C_AttributeType "\
		"WHERE Identifier=3 ORDER BY Name limit 30;";

	PlutoSqlResult result;
	MYSQL_ROW row;
	int RowCount=0;

	if( (result.r=m_pCore->mysql_query_result(SQL)) )
	{
		while( (row=mysql_fetch_row(result.r)) )
		{
			string label = row[1];
			if( row[2] && *row[2] )
				label += string(",") + row[2];
			label += string("\n`S29`") + row[3];
			pCell = new DataGridCell(label,row[0]);
			pDataGrid->SetData(0,RowCount++,pCell);
		}
	}
	return pDataGrid;
}

class DataGridTable *DCEMI_PS_VirtualDevices::PopulateWithMediaItemAttr(string GridID,string Parms)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	string SQL;

	int PK_MED_File=0;
	string PK_MED_Attribute = Parms;
	if( PK_MED_Attribute.length()==0 )
		return;
	if( PK_MED_Attribute.substr(0,2)=="#F" )
		PK_MED_File = atoi(PK_MED_Attribute.substr(2).c_str());
	else
	{
		if( PK_MED_Attribute.substr(0,2)=="#A" )
			PK_MED_Attribute = PK_MED_Attribute.substr(2);

		PK_MED_File = m_pMediaAttributes->GetFileIDFromAttributeID(atoi(PK_MED_Attribute.c_str()));

		if( !PK_MED_File )
		{
			// They chose an attribute other than a file, like an album
			SQL = "select DISTINCT PK_MED_Attribute,Description,Name,FirstName FROM MED_File_Attribute AS Source "\
				"JOIN MED_File_Attribute As Dest ON Source.FK_MED_File=Dest.FK_MED_File "\
				"JOIN MED_Attribute ON Dest.FK_MED_Attribute=PK_MED_Attribute  "\
				"JOIN MED_C_AttributeType ON MED_Attribute.FK_MED_C_AttributeType=PK_MED_C_AttributeType  "\
				"JOIN MED_File ON Dest.FK_MED_File=PK_MED_File "\
				"JOIN MED_C_Type_C_AttributeType ON MED_C_Type_C_AttributeType.FK_MED_C_Type=MED_File.FK_MED_C_Type "\
				"AND MED_C_Type_C_AttributeType.FK_MED_C_AttributeType=MED_Attribute.FK_MED_C_AttributeType "\
				"WHERE Identifier>2 AND Source.FK_MED_Attribute=" + PK_MED_Attribute + " ORDER BY Description";
		}
	}

	string::size_type pos = 0;
	StringUtils::Tokenize(GridID, "_", pos);
	string controller = StringUtils::Tokenize(GridID, "_", pos);

	int PK_MED_Picture;
	string Extension;
	if( PK_MED_File )
		Extension = m_pMediaAttributes->GetPictureFromFileID(PK_MED_File,&PK_MED_Picture);
	else
		Extension = m_pMediaAttributes->GetPictureFromAttributeID(atoi(PK_MED_Attribute.c_str()),&PK_MED_Picture);

	Message *pImageMessage = new Message(0, atoi(controller.c_str()), PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Update_Object_Image_CONST, 
		2, COMMANDPARAMETER_PK_DesignObj_CONST, StringUtils::itos(DESIGNOBJ_OBJCDCOVER_CONST).c_str(),
		COMMANDPARAMETER_Type_CONST,"1"); 
	if( PK_MED_Picture )
	{
		size_t length;
		char *buffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_MED_Picture) + "." + Extension,length);

		if( buffer )
		{
			pImageMessage->m_mapData_Parameters[COMMANDPARAMETER_Data_CONST]=buffer;
			pImageMessage->m_mapData_Lengths[COMMANDPARAMETER_Data_CONST]=(int) length;
		}
	}
	m_pCore->ReceivedMessage(NULL,pImageMessage);

	// We may have already built the sql statement
	if( SQL.length()==0 )
	{
		SQL="SELECT PK_MED_Attribute,Description,Name,FirstName FROM MED_File "\
		"JOIN MED_File_Attribute ON FK_MED_File=PK_MED_File "\
		"JOIN MED_Attribute ON FK_MED_Attribute=PK_MED_Attribute "\
		"JOIN MED_C_AttributeType ON FK_MED_C_AttributeType=PK_MED_C_AttributeType "\
		"WHERE PK_MED_File=" + StringUtils::itos(PK_MED_File) + " ORDER By Description";
	}

	PlutoSqlResult result;
	MYSQL_ROW row;
	int RowCount=0;

	if( (result.r=m_pCore->mysql_query_result(SQL)) )
	{
		while( (row=mysql_fetch_row(result.r)) )
		{
			string label = row[1];
			label += ": ";
			label += row[2];
			if( row[3] && *row[3] )
				label += string(",") + row[3];
			pCell = new DataGridCell(label,row[0]);
			pDataGrid->SetData(0,RowCount++,pCell);
		}
	}
	return pDataGrid;
}

class DataGridTable *DCEMI_PS_VirtualDevices::PopulateWithSpecialCommands(string GridID,string Parms)
{
	int PK_Controller = atoi(Parms.substr(posParms).c_str());
	// Get the ResetAV Map so we have a list of actions
	ResetAVMap mapResetAV;
//	PopulateWithResetAV(GridID,Parms,&mapResetAV,NULL);

	DataGridTable *pDataGrid = NULL;
	DataGridCell *pCell;

	if( !listCommands )
	{
		pDataGrid = new DataGridTable();
	}

	int row=0;

	ResetAVMap::iterator itMap;
	for(itMap=mapResetAV.begin();itMap!=mapResetAV.end();)
	{
		ResetAV *pResetAV=(*itMap).second;
		if( pResetAV )
		{
			if( pResetAV->pDevice )
			{
				for(int i=0;i<(int) pResetAV->pDevice->m_vectSpecialCommandGroups.size();++i)
				{
					CommandGroup *ag = pResetAV->pDevice->m_vectSpecialCommandGroups[i];
					Message *pMessage = new Message(PK_Controller,0,
						PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_EXECUTE_COMMAND_GROUP_CONST,1,
						COMMANDPARAMETER_PK_CommandGroup_CONST,StringUtils::itos(ag->m_PK_CommandGroup).c_str());
					if( listCommands )
					{
						listCommands->push_back(new ExtraCommand(pResetAV->pDevice->m_sDescription + " / " + ag->m_Description,pMessage));
					}
					else
					{
						pCell = new DataGridCell(pResetAV->pDevice->m_sDescription + "\n" + ag->m_Description);
						pCell->m_pMessage=pMessage;
						pDataGrid->SetData(0,row++,pCell);
					}
				}
				for(int i=0;i<(int) pResetAV->pDevice->m_vectSpecialCommands.size();++i)
				{
					Command *a = pResetAV->pDevice->m_vectSpecialCommands[i];
					Message *pMessage = new Message(PK_Controller,pResetAV->pDevice->m_dwPK_Device,
						PRIORITY_NORMAL,MESSAGETYPE_COMMAND,a->m_iPK_Command,0);
					if( listCommands )
					{
						listCommands->push_back(new ExtraCommand(pResetAV->pDevice->m_sDescription + " / " + a->m_sDescription,pMessage));
					}
					else
					{
						pCell = new DataGridCell(pResetAV->pDevice->m_sDescription + "\n" + a->m_sDescription);
						pCell->m_pMessage=pMessage;
						pDataGrid->SetData(0,row++,pCell);
					}
				}
			}
			delete pResetAV;
		}
		++itMap;
	}
	return pDataGrid;
}

class DataGridTable *DCEMI_PS_VirtualDevices::PopulateWithResetAV(string GridID,string Parms)
{
	bool bMustWantMap=true;
	if( !mapResetAV )
	{
		bMustWantMap=false;
		mapResetAV = new ResetAVMap();
	}

	DataGridTable *pDataGrid = NULL;
	DataGridCell *pCell;

	if( !listCommands )
	{
		pDataGrid = new DataGridTable();
		m_DataGrids[GridID] = pDataGrid;
	}

	int iRow=0;

	string::size_type posParms=0;
	int PK_Controller = atoi(StringUtils::Tokenize(Parms,",",posParms).c_str());
	int PK_Device = atoi(StringUtils::Tokenize(Parms,",",posParms).c_str());

	DeviceData_Router *pDevice=m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
	if( !pDevice )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Reset AV Cannot find device: %d",PK_Device);
		return;
	}
	MediaStream *pStream=NULL;
	bool bLiveTV=false;

	if( pDevice->m_dwPK_DeviceTemplate==DEVICETEMPLATE_LINPHONE_CONST )
	{
		pDevice = m_pCore->m_pPlutoEvents->FindSibling(pDevice,DEVICETEMPLATE_XINE_CONST);
	}
	else
	{
		PlutoOrbiter *ptrController = m_mapPlutoOrbiter_Find[PK_Controller];
		if( !ptrController || !ptrController->m_pEntGroup  || (!ptrController->m_pEntGroup->m_pWatchingStream && !ptrController->m_pEntGroup->m_pListeningStream) )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Cannot populate with reset av list");
			return;
		}

		pStream = ptrController->m_pEntGroup->m_pWatchingStream ? ptrController->m_pEntGroup->m_pWatchingStream : ptrController->m_pEntGroup->m_pListeningStream;
		if( !pStream || !pStream->Getm_pEntGroup_Primary() )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"PopulateWithResetAV: Stream is %p",pStream);
			return;
		}

		bLiveTV = pStream->m_MediaType==pluto_LiveTV && ptrController->m_pEntGroup->m_bPlutoTV;

		g_pPlutoLogger->Write(LV_STATUS,"PopulateWithResetAV: Stream is type %d, device is %d",(int) pStream->m_MediaType,pStream->m_PK_DeviceToSendTo);
		pDevice = m_pRouter->m_mapDeviceData_Router_Find(pStream->m_PK_DeviceToSendTo);
	}

	// A map of device and input #'s
	
	map<int,ResetAV *>::iterator itMap;

	if( pStream && bLiveTV )
	{
		g_pPlutoLogger->Write(LV_STATUS,"PopulateWithResetAV: Stream is live tv");
		if( !pStream->pProvider_Station__Schedule )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"PopulateWithResetAV, no pStream->pProvider_Station_Schedule");
			return;
		}
		EPGProvider *pProvider = pStream->Getm_pEntGroup_Primary()->m_mapProviders[pStream->pProvider_Station__Schedule->m_PK_Provider];
		if( pProvider )
		{
			if( pStream->Getm_pEntGroup_Primary()->m_bRealtime && pProvider->m_PK_Device_TuningLive )
			{
				pDevice = m_pRouter->m_mapDeviceData_Router_Find(pProvider->m_PK_Device_TuningLive);
				if( !pDevice->m_dwPK_Device_Audio && !pDevice->m_dwPK_Device_Video )
				{
					// If there are no audio/video devices, uses xine
					pDevice = pStream->Getm_pEntGroup_Primary()->m_ptrXine;
				}
			}
			else
				pDevice = pStream->Getm_pEntGroup_Primary()->m_ptrXine;
		}
	}

	if( !pDevice )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Trying to reset AV when there is no device");
		return;
	}


	// Don't reset any of the software modules
	if( pDevice->m_dwPK_DeviceCategory!=DEVICECATEGORY_SOFTWARE_MODULE_CONST )
		(*mapResetAV)[pDevice->m_dwPK_Device]=new ResetAV(pDevice,0,0,0);
   
	if( pStream && pStream->m_pEntGroupOption )
	{
		for(int i=0;i<pStream->m_pEntGroupOption->m_iNumberCommands;)
		{
			int PK_Device = pStream->m_pEntGroupOption->m_ptriCommands[i++];
			int PK_Command = pStream->m_pEntGroupOption->m_ptriCommands[i++];
			g_pPlutoLogger->Write(LV_STATUS,"Adding device %d action %d for ent group option",PK_Device,PK_Command);
			ResetAV *pRAV = (*mapResetAV)[PK_Device];
			if( !pRAV )
				(*mapResetAV)[PK_Device]=new ResetAV(m_pRouter->m_mapDeviceData_Router_Find(PK_Device),0,0,PK_Command);
			else
				pRAV->PK_Command=PK_Command;
		}
	}

	DeviceData_Router *pTempDevice = pDevice;
	while( pTempDevice->m_dwPK_Device_Audio )
	{
		int Input = pTempDevice->m_iPK_Input_Audio;
		pTempDevice = m_pRouter->m_mapDeviceData_Router_Find(pTempDevice->m_dwPK_Device_Audio);
		g_pPlutoLogger->Write(LV_STATUS,"Adding device %s for audio",(pTempDevice ? pTempDevice->m_sDescription.c_str() : "**NOTHING**"));
		ResetAV *pRAV = (*mapResetAV)[pTempDevice->m_dwPK_Device];
		if( !pRAV )
			(*mapResetAV)[pTempDevice->m_dwPK_Device]=new ResetAV(pTempDevice,Input,0,0);
		else
			pRAV->PK_Input=Input;
	};

	pTempDevice = pDevice;
	while( pTempDevice->m_dwPK_Device_Video )
	{
		int Input = pTempDevice->m_iPK_Input_Video;
		pTempDevice = m_pRouter->m_mapDeviceData_Router_Find(pTempDevice->m_dwPK_Device_Video);
		g_pPlutoLogger->Write(LV_STATUS,"Adding device %s for video",(pTempDevice ? pTempDevice->m_sDescription.c_str() : "**NOTHING**"));
		ResetAV *pRAV = (*mapResetAV)[pTempDevice->m_dwPK_Device];
		if( !pRAV )
			(*mapResetAV)[pTempDevice->m_dwPK_Device]=new ResetAV(pTempDevice,Input,0,0);
		else
			pRAV->PK_Input=Input;
	};

	if( pStream && pStream->pProvider_Station__Schedule && pStream->Getm_pEntGroup_Primary() )
	{
		EPGProvider *pProvider = pStream->Getm_pEntGroup_Primary()->m_mapProviders[pStream->pProvider_Station__Schedule->m_PK_Provider];
		int PK_Device=0;
	
		if( pStream->Getm_pEntGroup_Primary() && pStream->Getm_pEntGroup_Primary()->m_bRealtime && pProvider && pProvider->m_PK_Device_TuningLive!=0 )
			PK_Device = pProvider->m_PK_Device_TuningLive;
		else if( pProvider && pProvider->m_PK_Device_TuningPVR!=0 )
			PK_Device = pProvider->m_PK_Device_TuningPVR;

		if( PK_Device )
		{
			DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
			if( pDevice )
			{
				if( !pStream->Getm_pEntGroup_Primary() || !pStream->Getm_pEntGroup_Primary()->m_pCustomEPGView )
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"ResetAV, EntGroup or CustomView is NULL %p type %d parms %s",
						pStream->Getm_pEntGroup_Primary(),(int) pStream->m_MediaType,Parms.c_str());
				}
				else
				{
					CustomChannel *pChannel = pStream->Getm_pEntGroup_Primary()->m_pCustomEPGView->m_mapProviderStation[pStream->pProvider_Station__Schedule->m_PK_Provider_Station];

					if( pChannel )
					{
						Message *pMessage = new Message(DEVICEID_EVENTMANAGER, PK_Device, PRIORITY_NORMAL, 
							MESSAGETYPE_COMMAND, COMMAND_AV_CHANNEL_NUMBER_CONST, 1, 
							C_COMMANDPARAMETER_ABSOLUTE_CHANNEL_CONST, StringUtils::itos(pChannel->m_Channel).c_str());

						pCell = new DataGridCell( pDevice->m_sDescription + "\n`S29`Channel: " + StringUtils::itos(pChannel->m_Channel),"");
						pCell->m_pMessage=pMessage;
						pDataGrid->SetData(0,iRow++,pCell);
					}
				}
			}
		}
	}

	if( bMustWantMap )
		return;

	g_pPlutoLogger->Write(LV_STATUS,"Adding %d actions",(int) (*mapResetAV).size());

	// The on's
	for(itMap=(*mapResetAV).begin();itMap!=(*mapResetAV).end();++itMap)
	{
		ResetAV *pResetAV=(*itMap).second;
		g_pPlutoLogger->Write(LV_STATUS,"Device %s",(pResetAV->pDevice ? pResetAV->pDevice->m_sDescription.c_str() : "*none*"));
		if( pResetAV && pResetAV->pDevice && !pResetAV->pDevice->m_pDevice_SlaveTo &&
			(pResetAV->pDevice->m_dwPK_DeviceTemplate!=DEVICETEMPLATE_XINE_CONST &&
			 pResetAV->pDevice->m_dwPK_DeviceTemplate!=DEVICETEMPLATE_LINPHONE_CONST &&
			 pResetAV->pDevice->m_dwPK_DeviceCategory!=DEVICECATEGORY_SOFTWARE_MODULE_CONST &&
			 pResetAV->pDevice->m_dwPK_DeviceCategory!=DEVICECATEGORY_HTPC_CONST &&
			 pResetAV->pDevice->m_dwPK_DeviceCategory!=DEVICECATEGORY_SERVER_CONST
			)
		  )
		{
			Message *pMessage=new Message(DEVICEID_EVENTMANAGER,pResetAV->pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
				COMMAND_GEN_ON_CONST,2,C_COMMANDPARAMETER_RESEND_IR_CONST,"1",C_COMMANDPARAMETER_DONT_SET_INPUTS_CONST, "7");
			if( listCommands )
			{
				listCommands->push_back(new ExtraCommand(pResetAV->pDevice->m_sDescription + " / ON",pMessage));
			}
			else
			{
				pCell = new DataGridCell( (pResetAV->pDevice->m_IRInformation && pResetAV->pDevice->m_IRInformation->m_bTogglePower ? "* " : "") +
					pResetAV->pDevice->m_sDescription + "\n" + "`S29`ON","");
				pCell->m_pMessage=pMessage;
				pDataGrid->SetData(0,iRow++,pCell);
			}
		}
	}

	// The input's
	for(itMap=(*mapResetAV).begin();itMap!=(*mapResetAV).end();++itMap)
	{
		ResetAV *pResetAV=(*itMap).second;
		if( pResetAV && pResetAV->pDevice && pResetAV->PK_Input )
		{
			int PK_Command = 0;
			PK_Command = pResetAV->pDevice->m_mapInputs[pResetAV->PK_Input];

			string Description="";
			Message *pMessage=m_pCore->GetCommandForInput(pResetAV->pDevice->m_dwPK_Device,pResetAV->PK_Input);
			pMessage->m_mapParameters[C_COMMANDPARAMETER_RESEND_IR_CONST]="1";

			if( PK_Command!=0 )
			{
				Command *pCommand = m_pRouter->m_mapCommand[PK_Command];
				Description = pCommand ? pCommand->m_sDescription : "";
			}
			else
			{
				Description = "Input #" + StringUtils::itos(pResetAV->PK_Input);
			}

			if( listCommands )
			{
				listCommands->push_back(new ExtraCommand(pResetAV->pDevice->m_sDescription + " / " + Description,pMessage));
			}
			else
			{
				pCell = new DataGridCell( (pResetAV->pDevice->m_IRInformation && pResetAV->pDevice->m_IRInformation->m_listToggleInputs.size()>0 ? "* " : "") +
					pResetAV->pDevice->m_sDescription + "\n" + "`S29`" + Description,"");
				pCell->m_pMessage=pMessage;
				pDataGrid->SetData(0,iRow++,pCell);
			}
		}
	}

	// The DSP Mode's
	for(itMap=(*mapResetAV).begin();itMap!=(*mapResetAV).end();++itMap)
	{
		ResetAV *pResetAV=(*itMap).second;
		if( pResetAV && pResetAV->pDevice && pResetAV->PK_DSPMode )
		{
			int PK_Command = 0;
			PK_Command = pResetAV->pDevice->m_mapDSPModes[pResetAV->PK_DSPMode];

			string Description="";
			Message *pMessage;

			if( PK_Command!=0 )
			{
				Command *pCommand = m_pRouter->m_mapCommand[PK_Command];
				Description = pCommand ? pCommand->m_sDescription : "";

				pMessage=new Message(DEVICEID_EVENTMANAGER,pResetAV->pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
					PK_Command,1,C_COMMANDPARAMETER_RESEND_IR_CONST,"1");
			}
			else
			{
				Description = "DSP Mode#" + StringUtils::itos(pResetAV->PK_DSPMode);
				pMessage=new Message(DEVICEID_EVENTMANAGER,pResetAV->pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
					COMMAND_AV_DSP_MODE_CONST,2,COMMANDPARAMETER_ID_CONST, StringUtils::itos(pResetAV->PK_DSPMode).c_str(),C_COMMANDPARAMETER_RESEND_IR_CONST,"1");
			}
			if( listCommands )
			{
				listCommands->push_back(new ExtraCommand(pResetAV->pDevice->m_sDescription + " / " + Description,pMessage));
			}
			else
			{
				pCell = new DataGridCell( (pResetAV->pDevice->m_IRInformation && pResetAV->pDevice->m_IRInformation->m_listToggleDSPModes.size()>0 ? "* " : "") +
					pResetAV->pDevice->m_sDescription + "\n" + "`S29`" + Description,"");
				pCell->m_pMessage = pMessage;
				pDataGrid->SetData(0,iRow++,pCell);
			}
		}
	}

	// The special action's
	for(itMap=(*mapResetAV).begin();itMap!=(*mapResetAV).end();++itMap)
	{
		ResetAV *pResetAV=(*itMap).second;
		if( pResetAV && pResetAV->pDevice && pResetAV->PK_Command )
		{
			Command *pCommand = m_pRouter->m_mapCommand[pResetAV->PK_Command];
			string Description = pCommand ? pCommand->m_sDescription : "";

			Message *pMessage=new Message(DEVICEID_EVENTMANAGER,pResetAV->pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
				pResetAV->PK_Command,0);

			if( listCommands )
			{
				listCommands->push_back(new ExtraCommand(pResetAV->pDevice->m_sDescription + " / " + Description,pMessage));
			}
			else
			{
				pCell = new DataGridCell( pResetAV->pDevice->m_sDescription + "\n" + "`S29`" + Description,"");
				pCell->m_pMessage=pMessage;
				pDataGrid->SetData(0,iRow++,pCell);
			}
		}
	}

	for(itMap=(*mapResetAV).begin();itMap!=(*mapResetAV).end();++itMap)
	{
		ResetAV *pResetAV=(*itMap).second;
		if( pResetAV )
			delete pResetAV;
	}
	(*mapResetAV).clear();
	return pDataGrid;
}

// If vectStrings is not null, then don't make a grid.  Just populate the vect with all teh strings
class DataGridTable *DCEMI_PS_VirtualDevices::PopulateWithPlaylist(string GridID,string Parms)
{
	DataGridTable *pDataGrid=NULL;
	DataGridCell *pCell;

	if( !vectStrings )
	{
		pDataGrid = new DataGridTable();
	}

	int iRow=0;

	int PK_Controller = atoi(StringUtils::Tokenize(Parms,",",posParms).c_str());
	PlutoOrbiter *ptrController = m_mapPlutoOrbiter_Find[PK_Controller];

	g_pPlutoLogger->Write(LV_WARNING,"populate playlist entgroup: %p",ptrController->m_pEntGroup);
	if( ptrController->m_pEntGroup->m_pWatchingStream )
		g_pPlutoLogger->Write(LV_WARNING,"watching: %d",(int)ptrController->m_pEntGroup->m_pWatchingStream->m_MediaType);
	if( ptrController->m_pEntGroup->m_pListeningStream )
		g_pPlutoLogger->Write(LV_WARNING,"listeni: %d",(int)ptrController->m_pEntGroup->m_pListeningStream->m_MediaType);
	
	string Screen = StringUtils::Tokenize(Parms,",",posParms);  
	if( !ptrController || !ptrController->m_pEntGroup )
		return;
	int PK_Device = atoi(StringUtils::Tokenize(Parms,",",posParms).c_str()); 
	DeviceData_Router *pDevice_Controller = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
	MediaStream *pStream=NULL;
	
	if( pDevice_Controller )
		pStream=pDevice_Controller->m_pStream;
	if( !pStream )
		pStream=ptrController->m_pEntGroup->m_pWatchingStream;
	if( !pStream )
		pStream=ptrController->m_pEntGroup->m_pListeningStream;

	int iPlayListPostion=0;

	if( !pStream )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find requested playlist");
		return;
	}

	string sDataBlock;
	char *data_block=NULL;
	// Maybe it's just a list of files
	if( pStream->m_MediaType!=misc_Playlist )
	{
		if( pStream->m_sMRL.length()>0 && pStream->m_sMRL[0]=='|' )
			sDataBlock=pStream->m_sMRL.substr(1).c_str();
		else
			sDataBlock=pStream->m_sMRL.c_str();
	}
	else
	{
		FILE *file = fopen( pStream->m_sMRL.c_str(),"r");
		if( !file )
		{

			g_pPlutoLogger->Write(LV_CRITICAL,"Cannot read from playlist file: %s",pStream->m_sMRL.c_str());
			return;
		}
		fseek(file, 0L, SEEK_END);
		int total_size=ftell(file);
		if( total_size==0 )
		{
			fclose(file);
			return;
		}
		fseek(file, 0L, SEEK_SET);
		data_block=(char *)malloc(total_size+1);
		g_pPlutoLogger->Write(LV_STATUS,"Allocating data block of size: %d",(int) total_size);

		*data_block=0;  // NULL terminate it in case we don't read in anything
		size_t bytesread=fread(data_block,1,total_size,file);
		data_block[bytesread]=0;

		g_pPlutoLogger->Write(LV_STATUS,"Ready to close %p",file);
		fclose(file);
		char *p=data_block;
		while(*p)
		{
			if( *p=='\n' || *p=='\r' )
				*p='|';
			p++;
		}
		sDataBlock=data_block;
	}

//	g_pPlutoLogger->Write(LV_STATUS,"Listing populate options %p # %d %d",data_block,total_size,(int) bytesread);

	string PreviousFullPath;

	string::size_type pos=0;
	for(;;)
	{
		++iPlayListPostion; // We may skip positions if there a contiguous vob files.  This is 1 based, increment first
        string FullPath = StringUtils::Tokenize(sDataBlock, "|", pos).c_str();
		if( FullPath.length()==0 )
			break;

		// See if we've got a series of _0001.vob, _0002.vob, etc. from the pvr files
		if( FullPath.length()>9 && FullPath.substr( FullPath.length()-4 )==".vob" && FullPath.substr( FullPath.length()-9,1 )=="_" )
		{
			FullPath = FullPath.substr(0,FullPath.length()-9) + ".vob";
			if( FullPath==PreviousFullPath )
				continue;
		}

		string::size_type PreviousSlash=string::npos,LastPos=string::npos;
		while( (LastPos=FullPath.find("/",(LastPos==string::npos ? 0 : LastPos+1)))!=string::npos )
		{
			if( LastPos!=FullPath.length()-1 ) // Skip the trailing slash
				PreviousSlash=LastPos;
			else
				break;
		}

//		g_pPlutoLogger->Write(LV_STATUS,"Last position: %d",(int) LastPos);

		string label;
		string::size_type posDot=0,posExtension=string::npos;
		while( (posDot=FullPath.find('.',posDot+1))!=string::npos )
			posExtension=posDot;

		if( PreviousSlash!=string::npos )
		{
			if( vectStrings )
			{
				if( posExtension!=string::npos )
					vectStrings->push_back(FullPath.substr(PreviousSlash+1,posExtension-(PreviousSlash+1)));
				else
					vectStrings->push_back(FullPath.substr(PreviousSlash+1));
			}
			else
			{
				if( posExtension!=string::npos )
					label = "~`S24`" + FullPath.substr(PreviousSlash+1,posExtension-(PreviousSlash+1));
				else
					label = "~`S24`" + FullPath.substr(PreviousSlash+1);
				label += "\n`S1`In folder ";
				if (FullPath.substr(0,17) == "/home/public/data")
					label += FullPath.substr(17, FullPath.size()-17-(FullPath.length()-PreviousSlash));
				else if (FullPath.substr(0,17) == "/home/user_" && (posExtension=FullPath.find("data")) ) // reuse extension--don't need it anymore
					label += FullPath.substr(posExtension+5, FullPath.length()-(posExtension+5)-(FullPath.length()-PreviousSlash));
				else
					label += FullPath.substr(0, FullPath.length()-(FullPath.length()-PreviousSlash));
			}
		}
		else
		{
			if( vectStrings )
				vectStrings->push_back(FullPath);
			else
				label = "~`S24`" + FullPath;
		}
 
		if( !vectStrings )
		{
			pCell = new DataGridCell(label,StringUtils::itos(iRow).c_str());
			pCell->m_pMessage = new Message(DEVICEID_DCEROUTER,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
				COMMAND_JUMP_POS_IN_PLAYLIST_CONST,1,COMMANDPARAMETER_ID_CONST,StringUtils::itos(iPlayListPostion).c_str());  // It's 1 based
			pDataGrid->SetData(0,iRow++,pCell);
		}

		PreviousFullPath=FullPath;
	}

	if( data_block )
		delete[] data_block;
	return pDataGrid;
}

*/
