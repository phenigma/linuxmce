/*
 Media_Plugin_Grids

 */

#include "Media_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"

#include <algorithm>

#include "MediaHandlerInfo.h"
#include "MediaHandlerBase.h"
#include "Generic_NonPluto_Media.h"
#include "DCERouter.h"
#include "DeviceData_Router.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Template.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Table_Event.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_RemoteControl.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_UI.h"
#include "pluto_main/Table_EventParameter.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Device_Device_Pipe.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_Bookmark.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Disc.h"
#include "pluto_media/Table_Picture_Disc.h"
#include "pluto_media/Table_Disc_Attribute.h"
#include "pluto_media/Table_Playlist.h"
#include "pluto_media/Table_Picture.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_AttributeType.h"
#include "pluto_media/Define_MediaSource.h"
#include "Gen_Devices/AllScreens.h"

#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "pluto_main/Define_DataGrid.h"
#include "DataGrid.h"
#include "MediaListGrid.h"
#include "File_Grids_Plugin/FileListOps.h"
#include "File_Grids_Plugin/FileListGrid.h"
#include "SerializeClass/ShapesColors.h"
#include "RippingJob.h"
#include "../VFD_LCD/VFD_LCD_Base.h"
#include "UpdateMedia/PlutoMediaFile.h"

extern int UniqueColors[MAX_MEDIA_COLORS];

// Parms = PK_MediaType | , sep PK_MediaSubType | , sep PK_FileFormat | Genres to include, or empty for all (, sep PK_Attribute) |
// , sep of sources -- hardcoded strings (below) or a path if sort=0 (sort by filename) | , sep PK_Users (0=public, empty=public only) | PK_AttributeType (sort by) | PK_Users (for ratings)

class DataGridTable *Media_Plugin::MediaBrowser( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos=0;

	int PK_MediaType = atoi(StringUtils::Tokenize( Parms,"|",pos ).c_str());
	string sPK_MediaSubType = StringUtils::Tokenize( Parms,"|",pos );
	string sPK_FileFormat = StringUtils::Tokenize( Parms,"|",pos );
	string sPK_Attribute_Genres = StringUtils::Tokenize( Parms,"|",pos );
	string sPK_Sources = StringUtils::Tokenize( Parms,"|",pos );
	string sPK_Users_Private = StringUtils::Tokenize( Parms,"|",pos );
	int PK_AttributeType_Sort = atoi(StringUtils::Tokenize( Parms,"|",pos ).c_str());
	int PK_Users = atoi(StringUtils::Tokenize( Parms,"|",pos ).c_str());
	if( sPK_Sources.size()==0 )
		sPK_Sources = TOSTRING(MEDIASOURCE_File_CONST) "," TOSTRING(MEDIASOURCE_Jukebox_CONST) "," TOSTRING(MEDIASOURCE_Local_Disc_CONST);

	MediaListGrid *pMediaListGrid = new MediaListGrid(m_pDatagrid_Plugin,this);
	if( sPK_Sources.size()==0 || !PK_MediaType )
		return pMediaListGrid;
	
	AttributesBrowser( pMediaListGrid, PK_MediaType, PK_AttributeType_Sort, sPK_MediaSubType, sPK_FileFormat, sPK_Attribute_Genres, sPK_Sources, sPK_Users_Private, PK_Users, iPK_Variable, sValue_To_Assign );

	pMediaListGrid->m_listFileBrowserInfo.sort(FileBrowserInfoComparer);
	pMediaListGrid->m_pFileBrowserInfoPtr = new FileBrowserInfoPtr[ pMediaListGrid->m_listFileBrowserInfo.size() ];

	DataGridCell *pCell;
	string sDisplayGroupPrior;
	int iRow=0;
	for(list<FileBrowserInfo *>::iterator it=pMediaListGrid->m_listFileBrowserInfo.begin();it!=pMediaListGrid->m_listFileBrowserInfo.end();++it)
	{
		FileBrowserInfo *pFileBrowserInfo = *it;
		pMediaListGrid->m_pFileBrowserInfoPtr[iRow] = pFileBrowserInfo;  // Store in an array for fast retrieval by row
		if( pFileBrowserInfo->m_sDisplayGroup.size() && pFileBrowserInfo->m_sDisplayGroup!=sDisplayGroupPrior )
			pCell = new DataGridCell(pFileBrowserInfo->m_sDisplayGroup + " : " + pFileBrowserInfo->m_sDisplayName);
		else 
			pCell = new DataGridCell(pFileBrowserInfo->m_sDisplayName);
		
		pCell->SetValue(pFileBrowserInfo->m_sMRL);
		pMediaListGrid->SetData(0,iRow++,pCell);
	}

	MediaListGrid *pMediaListGrid_CoverArt = new MediaListGrid(m_pDatagrid_Plugin,this,pMediaListGrid);
	DataGridMap::iterator itdg=m_pDatagrid_Plugin->m_DataGrids.find("_" + GridID);
	if( itdg!=m_pDatagrid_Plugin->m_DataGrids.end() )
		delete itdg->second;
	m_pDatagrid_Plugin->m_DataGrids["_" + GridID]=pMediaListGrid_CoverArt;  // The same grid is used for both icons and lists.  If the name starts with an _, it is assumed to be an icon grid

	return pMediaListGrid;
}

void Media_Plugin::AttributesBrowser( MediaListGrid *pMediaListGrid,int PK_MediaType, int PK_AttributeType_Sort, string &sPK_MediaSubType, string &sPK_FileFormat, string &sPK_Attribute_Genres, string &sPK_Sources, string &sPK_Users_Private, int PK_Users, int *iPK_Variable, string *sValue_To_Assign )
{
	bool bFile=false,bJukebox=false,bLocalDisc=false,bSubDirectory=false;
	string sPath,sPath_Back;
	string::size_type pos=0;
	if( sPK_Sources[0]=='!' && sPK_Sources[1]=='D' )
	{
		string::size_type posLastPath=2,pos=2;
		while( (pos=sPath.find(',',pos))!=string::npos )
			posLastPath=++pos;

		sPath = sPK_Sources.substr(posLastPath);
		sPath_Back = posLastPath<3 ? "" : sPK_Sources.substr(2,posLastPath-3);
		bFile=true;
		bSubDirectory=true;
	}
	else
	{
		while(pos<sPK_Sources.size())
		{
			int PK_MediaSource=atoi( StringUtils::Tokenize(sPK_Sources,",",pos).c_str() );
			if( PK_MediaSource==MEDIASOURCE_File_CONST )
				bFile=true;
			else if( PK_MediaSource==MEDIASOURCE_Jukebox_CONST )
				bJukebox=true;
			else if( PK_MediaSource==MEDIASOURCE_Local_Disc_CONST )
				bLocalDisc=true;
		}
		if( bFile && PK_AttributeType_Sort==0 )
		{
			sPath = "'/home/public/data/videos','home/users_1/data/videos'"; // Todo, look at media type, and add in
		}
	}
	// First get all matching PK_File's
	string sSQL_File,sSQL_Disc,sSQL_Where;
	
	if( bFile )
		sSQL_File = "SELECT PK_File FROM File ";

	if( bJukebox || bLocalDisc )
		sSQL_Disc = "SELECT PK_Disc FROM Disc ";
	
	if( sPK_Attribute_Genres.size() )
	{
		if( bFile )
			sSQL_File += "LEFT JOIN File_Attribute ON FK_File=PK_File "
				"LEFT JOIN Attribute AS Attribute_Genre ON FK_Attribute=Attribute_Genre.PK_Attribute AND Attribute_Genre.FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Genre_CONST) " ";
		if( bJukebox || bLocalDisc )
			sSQL_Disc += "LEFT JOIN Disc_Attribute ON FK_Disc=PK_Disc "
				"LEFT JOIN Attribute AS Attribute_Genre ON FK_Attribute=Attribute_Genre.PK_Attribute AND Attribute_Genre.FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Genre_CONST) " ";
	}

	sSQL_Where = "WHERE Missing in (0,1)";
	if( PK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST )
		sSQL_Where += " AND EK_MediaType IN (" TOSTRING(MEDIATYPE_pluto_StoredAudio_CONST) "," TOSTRING(MEDIATYPE_pluto_CD_CONST) ")";
	else if( PK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST )
		sSQL_Where += " AND EK_MediaType IN (" TOSTRING(MEDIATYPE_pluto_StoredVideo_CONST) "," TOSTRING(MEDIATYPE_pluto_DVD_CONST) ")";
	else 
		sSQL_Where += " AND EK_MediaType=" + StringUtils::itos(PK_MediaType);

	if( sPK_Attribute_Genres.size() )
		sSQL_Where += " AND Attribute_Genre.PK_Attribute IN (" + sPK_Attribute_Genres + ")";

	if( sPK_FileFormat.size() )
		sSQL_Where += " AND FK_FileFormat IN (" + sPK_FileFormat + ")";

	if( sPK_MediaSubType.size() )
		sSQL_Where += " AND FK_MediaSubType IN (" + sPK_MediaSubType + ")";

	if( sPK_Users_Private.size() )
		sSQL_Where += " AND EK_Users_Private IN (" + sPK_Users_Private + ")";
	else
		sSQL_Where += " AND EK_Users_Private IS NULL";

	string sPK_File,sPK_Disc;
    PlutoSqlResult result;
    MYSQL_ROW row;
	if( bFile && ( result.r=m_pDatabase_pluto_media->mysql_query_result( sSQL_File + sSQL_Where + (sPath.size() ? " AND Path in (" + sPath + ")" : "")) ) )
        while( ( row=mysql_fetch_row( result.r ) ) )
			sPK_File += row[0] + string(",");

    if( (bJukebox || bLocalDisc) && ( result.r=m_pDatabase_pluto_media->mysql_query_result( sSQL_Disc + sSQL_Where ) ) )
        while( ( row=mysql_fetch_row( result.r ) ) )
			sPK_Disc += row[0] + string(",");

	if( sPK_File.size() )
		sPK_File[ sPK_File.size()-1 ]=' '; // Get rid of the trailing comma
	if( sPK_Disc.size() )
		sPK_Disc[ sPK_Disc.size()-1 ]=' '; // Get rid of the trailing comma
	
	// Find all the pictures for the files and discs
	map<int,int> mapFile_To_Pic,mapDisc_To_Pic;
	if( bFile )
		FetchPictures("File",sPK_File,mapFile_To_Pic);
	if( bJukebox || bLocalDisc )
		FetchPictures("Disc",sPK_Disc,mapDisc_To_Pic);

	if( bFile )
		PopulateFileBrowserInfoForFile(pMediaListGrid,PK_AttributeType_Sort,bSubDirectory,sPath_Back,sPK_File,mapFile_To_Pic);
//	void PopulateFileBrowserInfoForFile(MediaListGrid *pMediaListGrid,int PK_AttributeType_Sort, string &sPath, string &sPK_File,map<int,int> &mapFile_To_Pic);
//	if( bJukebox || bLocalDisc )
//		PopulateFileBrowserInfoForDisc(pMediaListGrid,sPK_File,mapFile_To_Pic);
}

void Media_Plugin::FetchPictures(string sWhichTable,string &sPK_File_Or_Disc,map<int,int> &mapFile_To_Pic)
{
	string sSQL	= "SELECT FK_" + sWhichTable + ",FK_Picture FROM Picture_" + sWhichTable + " WHERE FK_" + sWhichTable + " IN (" + sPK_File_Or_Disc + ")";
    PlutoSqlResult result;
    MYSQL_ROW row;
	string sPK_AlreadyGot;

	if( result.r=m_pDatabase_pluto_media->mysql_query_result( sSQL ) )
        while( ( row=mysql_fetch_row( result.r ) ) )
		{
			sPK_AlreadyGot += row[0] + string(",");
			mapFile_To_Pic[ atoi(row[0]) ] = atoi(row[1]);
		}

	sSQL = "SELECT FK_" + sWhichTable + ",FK_Picture FROM " + sWhichTable + "_Attribute JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=" + sWhichTable + "_Attribute.FK_Attribute WHERE FK_" + sWhichTable + " IN (" + sPK_File_Or_Disc + ") AND FK_" + sWhichTable + " NOT IN (" + sPK_AlreadyGot + ")";
    PlutoSqlResult result2;
    if( result2.r=m_pDatabase_pluto_media->mysql_query_result( sSQL ) )
        while( ( row=mysql_fetch_row( result2.r ) ) )
		{
			sPK_AlreadyGot += row[0] + string(",");
			mapFile_To_Pic[ atoi(row[0]) ] = atoi(row[1]);
		}
}

void Media_Plugin::PopulateFileBrowserInfoForFile(MediaListGrid *pMediaListGrid,int PK_AttributeType_Sort, bool bSubDirectory, string &sPath, string &sPK_File,map<int,int> &mapFile_To_Pic)
{
	string sSQL_Sort;
	if( PK_AttributeType_Sort==0 )
		sSQL_Sort = "SELECT PK_File,Path,Filename,IsDirectory FROM File WHERE PK_File in (" + sPK_File + ")";
	else
		sSQL_Sort = "SELECT PK_File,'',Name,0 FROM File JOIN File_Attribute ON FK_File=PK_File JOIN Attribute ON FK_Attribute=PK_Attribute AND FK_AttributeType=" + StringUtils::itos(PK_AttributeType_Sort) + " WHERE IsDirectory=0 AND PK_File in (" + sPK_File + ")";

	if( bSubDirectory )
		pMediaListGrid->m_listFileBrowserInfo.push_back( new FileBrowserInfo("back (..)",sPath,0,true,true) );

    PlutoSqlResult result;
    MYSQL_ROW row;
	map<int,int>::iterator it;
	FileBrowserInfo *pFileBrowserInfo;
	// 0 =PK_File, 1=Path, 2=Name, 3=IsDirectory
    if( result.r=m_pDatabase_pluto_media->mysql_query_result( sSQL_Sort ) )
        while( ( row=mysql_fetch_row( result.r ) ) )
		{
			if( row[3][0]=='1' )
			{
				if( sPath.length() )
					pFileBrowserInfo = new FileBrowserInfo(row[2],"!D'" + sPath + "," + row[1] + "/" + row[2] +"'",atoi(row[0]),true,false);
				else
					pFileBrowserInfo = new FileBrowserInfo(row[2],string("!D'") + row[1] + "/" + row[2] + "'",atoi(row[0]),true,false);
			}
			else
				pFileBrowserInfo = new FileBrowserInfo(row[2],string("!F") + row[0],atoi(row[0]),false,false);
			if( (it=mapFile_To_Pic.find( atoi(row[0]) ))!=mapFile_To_Pic.end() )
				pFileBrowserInfo->m_PK_Picture = it->second;
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
		}
}

void Media_Plugin::FileBrowser( MediaListGrid *pMediaListGrid,int PK_MediaType, string &sPK_MediaSubType, string &sPK_FileFormat, string &sPK_Attribute_Genres, string &sPK_Sources, string &sPK_Users_Private, int PK_Users, int *iPK_Variable, string *sValue_To_Assign )
{
	// Maybe we won't use the FileBrowser anymore, and just always get the data from the database
#ifdef DEBUG
g_pPlutoLogger->Write(LV_WARNING,"Starting File list");
#endif
	// A comma separated list of file extensions.  Blank means all files
	Row_MediaType *pRow_MediaType=m_pDatabase_pluto_main->MediaType_get()->GetRow(PK_MediaType);
	if( !pRow_MediaType )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find Media Type: %d",PK_MediaType);
		return;
	}
	string Extensions = pRow_MediaType->Extensions_get();

	// The first time, sSources will be empty.  For all sub directories the value will be !D + sub directory
	// For each subsequent times, it will be !D + first dir + \t + second dir and so on.
	// To go back, skip the last directory on the list
	if( sPK_Sources.length()<3 || sPK_Sources[0]!='!' || sPK_Sources[1]!='D' )
		sPK_Sources = "";

	string::size_type posLastPath=2,pos=2;
	if( sPK_Sources.length() )
		while( (pos=sPK_Sources.find("\t",pos))!=string::npos )
			posLastPath=++pos;

	string sPaths;
	if( sPK_Sources.size() )
		sPaths = sPK_Sources.substr(posLastPath);
	else
		sPaths = "/home/public/data/videos";  // Todo, fill this in for private as well

	int iRow=0;
	if( sPK_Sources.length() )
	{
		string sPrevious = posLastPath<3 ? "!D" : sPK_Sources.substr(0,posLastPath-1);
		pMediaListGrid->m_listFileBrowserInfo.push_back( new FileBrowserInfo("~S21~<-- Back (..)",sPrevious,0,true,true) );
	}

	pos=0;
    while( pos<sPaths.size() )
	{
		string sSearchPath=StringUtils::Tokenize(sPaths,"\t",pos);

		list<FileDetails *> listFileDetails;
		GetDirContents(listFileDetails,sSearchPath,Extensions);

		map<string,DatabaseInfoOnPath *> mapDatabaseInfoOnPath;
		PopulateWithDatabaseInfoOnPath(mapDatabaseInfoOnPath,sSearchPath);

		for (list<FileDetails *>::iterator i = listFileDetails.begin(); i != listFileDetails.end(); i++)
		{
			FileDetails *pFileDetails = *i;
			map<string,DatabaseInfoOnPath *>::iterator itDIOP = mapDatabaseInfoOnPath.find( pFileDetails->m_sFileName );
			DatabaseInfoOnPath *pDatabaseInfoOnPath = itDIOP==mapDatabaseInfoOnPath.end() ? NULL : itDIOP->second;

			FileBrowserInfo *pFileBrowserInfo;
			if( pFileDetails->m_bIsDir && (pDatabaseInfoOnPath==NULL || pDatabaseInfoOnPath->m_bDirectory==true) )
			{
				if( sPK_Sources.length() )
					pFileBrowserInfo = new FileBrowserInfo(pFileDetails->m_sFileName,sPK_Sources +"\t" + pFileDetails->m_sBaseName + pFileDetails->m_sFileName,0,true,false);
				else
					pFileBrowserInfo = new FileBrowserInfo(pFileDetails->m_sFileName,"!D" + pFileDetails->m_sBaseName + pFileDetails->m_sFileName,0,true,false);
			}
			else
				pFileBrowserInfo = new FileBrowserInfo(pFileDetails->m_sFileName,pFileDetails->m_sBaseName + pFileDetails->m_sFileName,0,false,false);

			if( pDatabaseInfoOnPath )
			{
				pFileBrowserInfo->m_PK_File=pDatabaseInfoOnPath->m_PK_File;
				pFileBrowserInfo->m_PK_Picture=pDatabaseInfoOnPath->m_PK_Picture;
			}
			pMediaListGrid->m_listFileBrowserInfo.push_back( pFileBrowserInfo );

			delete pFileDetails; // We won't need it anymore and it was allocated on the heap
		}
		for(map<string,DatabaseInfoOnPath *>::iterator itDIOP=mapDatabaseInfoOnPath.begin();itDIOP!=mapDatabaseInfoOnPath.end();++itDIOP)
			delete itDIOP->second;
	}
}

void Media_Plugin::PopulateWithDatabaseInfoOnPath(map<string,DatabaseInfoOnPath *> &mapDatabaseInfoOnPath,string &sSearchPath)
{
	string sSql = 
		"( "
			"SELECT Filename, PK_File, IsDirectory, Picture_Attribute.FK_Picture "
			"FROM File "
				"JOIN File_Attribute ON PK_File = FK_File "
				"JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute = File_Attribute.FK_Attribute "
				"JOIN Picture ON Picture_Attribute.FK_Picture = PK_Picture "
				"JOIN Attribute ON Picture_Attribute.FK_Attribute = PK_Attribute "
				"JOIN AttributeType ON FK_AttributeType = PK_AttributeType "
			"WHERE Path = \"" + sSearchPath + "\" "
			"GROUP BY Filename "
			"ORDER BY PicPriority DESC "
		") "
		"UNION "
		"( "
		"SELECT Filename, PK_File, IsDirectory, FK_Picture "
		"FROM File "
		"JOIN Picture_File ON PK_File = FK_File "
		"JOIN Picture ON FK_Picture = PK_Picture "
		"WHERE Path = \"" + sSearchPath + "\""
		")";

	PlutoSqlResult result;
	MYSQL_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( sSql ) ) )
        while( ( row=mysql_fetch_row( result.r ) ) )
			mapDatabaseInfoOnPath[row[0]] = new DatabaseInfoOnPath(atoi(row[1]),row[2][0]==1,row[3] ? atoi(row[3]) : 0);
}

/*  jukeboxes
	//Jukeboxes special hack
	vector<Row_Device *> vectRow_Device;
	if(sSources.find("JUKE_BOXES")!=string::npos)
	{
		//this is the movies top level folder; we'll add at the beginning an entry for each jukeboxe from current installation
		m_pDatabase_pluto_main->Device_get()->GetRows(
			"FK_DeviceTemplate = " + StringUtils::ltos(DEVICETEMPLATE_Powerfile_C200_CONST) + " AND "  + 
			"FK_Installation = " + StringUtils::ltos(m_pRouter->iPK_Installation_get()), 
			&vectRow_Device);	
		
		if(!sSubDirectory.length()) 
		{
			for(vector<Row_Device *>::iterator iPowerFile = vectRow_Device.begin(); iPowerFile != vectRow_Device.end(); iPowerFile++)
			{
				Row_Device *pRow_Device = *iPowerFile;
				string sItemName = "Jukebox: " + pRow_Device->Description_get();
				FileListInfo *pFileListInfo = new FileListInfo(true, sItemName, false);
		
				pCell = new DataGridCell("~S2~" + sItemName, sItemName);

				string newParams = sPaths + "\n" + Extensions + "\n" 
					+ "\n" + StringUtils::itos(iRow)+ "\n" + sItemName;
				DCE::CMD_NOREP_Populate_Datagrid CMDPDG(PK_Controller, m_pDatagrid_Plugin->m_dwPK_Device,
					"", GridID, DATAGRID_Directory_Listing_CONST, newParams, 0);
				pCell->m_pMessage = CMDPDG.m_pMessage;
				
				pDataGrid->SetData(0, iRow++, pCell);
				pDataGrid->m_vectFileInfo.push_back(pFileListInfo);
			}
		}
		else if( StringUtils::StartsWith(sSubDirectory,"Jukebox: ") )
		{
			//not a top level folder
			for(vector<Row_Device *>::iterator iPowerFile = vectRow_Device.begin(); iPowerFile != vectRow_Device.end(); iPowerFile++)
			{
				Row_Device *pRow_Device = *iPowerFile;
				
				if("Jukebox: " + pRow_Device->Description_get() == sSubDirectory)
				{
					//we are in a "jukebox"; let's show its movies
					string sStatus;
					CMD_Get_Jukebox_Status CMD_Get_Jukebox_Status_(m_dwPK_Device, pRow_Device->PK_Device_get(), "", &sStatus); // ""= force=no
					
					if(!SendCommand(CMD_Get_Jukebox_Status_))
					{
						FileListInfo *pFileListInfo = new FileListInfo(true, "", false);

						pCell = new DataGridCell("Unable to communicate with Powerfile '" + pRow_Device->Description_get() + "'", "");
						pDataGrid->SetData(0, iRow++, pCell);

						pDataGrid->m_vectFileInfo.push_back(pFileListInfo);					
						return pDataGrid;
					}
					
					vector<string> vectElems;
					StringUtils::Tokenize(sStatus, ",", vectElems);
					
					for(vector<string>::iterator it = vectElems.begin(); it != vectElems.end(); it++)
					{
						string sElem = *it;
						if(sElem.length() > 0 && sElem[0] == 'S')
						{
							string sSlotIndex = sElem.substr(1, sElem.find('=', 0) - 1);
							FileListInfo *pFileListInfo = new FileListInfo(true, sSlotIndex + ". " + "Movie", false);
							pCell = new DataGridCell("~S2~not identified", "not identified");
							
							pDataGrid->SetData(0, iRow++, pCell);
				
							pDataGrid->m_vectFileInfo.push_back(pFileListInfo);							
						}
					}
				}
			}
			
			return pDataGrid;
		}	
	}
*/

class DataGridTable *Media_Plugin::CurrentMediaSections( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

    class MediaStream *pMediaStream = DetermineStreamOnOrbiter( pMessage->m_dwPK_Device_From, true );

    if ( pMediaStream == NULL )
    {
        g_pPlutoLogger->Write(LV_WARNING, "The message was not from an orbiter or it doesn't have a media stream on it!");
        return NULL;
    }

    DataGridTable *pDataGrid = new DataGridTable();
    // DataGridCell *pCell = NULL;


	// Keep track of the ones we've already picked up from the database so we can show the user the extra ones that aren't there
	map< pair<int,int>,bool > mapSections;

    deque<MediaFile *>::iterator itFiles;
    string sCurrentFile;

    int currentPos = 0;
	if( pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_DVD_CONST && pMediaStream->m_dequeMediaFile.size()==1 )
	{
		for(size_t sTitle=0;sTitle<pMediaStream->m_dequeMediaTitle.size();++sTitle)
		{
			MediaTitle *pMediaTitle = pMediaStream->m_dequeMediaTitle[sTitle];
			string sTitleName = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeName(pMediaTitle->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Chapter_CONST));
			for(size_t sSection=0;sSection<pMediaTitle->m_dequeMediaSection.size();++sSection)
			{
				MediaSection *pMediaSection = pMediaTitle->m_dequeMediaSection[sSection];
				string sCell = StringUtils::itos(sSection+1) + " " + m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeName(pMediaSection->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Chapter_CONST));
				pDataGrid->SetData(0, currentPos++,new DataGridCell(sCell," TITLE:" + StringUtils::itos(sTitle+1) + " CHAPTER:" + StringUtils::itos(sSection+1)));
				mapSections[ make_pair<int,int> (sSection,sTitle) ] = true;
			}
		}
	}
	else
	{
		int iSection=0;
		for ( itFiles = pMediaStream->m_dequeMediaFile.begin(); itFiles != pMediaStream->m_dequeMediaFile.end(); itFiles++ )
		{
			MediaFile *pMediaFile = *itFiles;
			// int index = itFiles - pMediaStream->m_dequeMediaFile.begin();
			sCurrentFile = pMediaFile->m_sDescription.size() ? pMediaFile->m_sDescription : FileUtils::FilenameWithoutPath(pMediaFile->FullyQualifiedFile());

			pDataGrid->SetData(0, currentPos++,new DataGridCell(sCurrentFile, StringUtils::itos(itFiles - pMediaStream->m_dequeMediaFile.begin())));
			g_pPlutoLogger->Write(LV_STATUS, "Returning data: (%d) -> %s section %d", itFiles - pMediaStream->m_dequeMediaFile.begin(), ((*itFiles)->m_sFilename).c_str(),iSection);
			mapSections[ make_pair<int,int> (iSection,0) ] = true;
		}
	}

	map< pair<int,int>,bool >::iterator itSections;
	for(map< pair<int,int>,string >::iterator it = pMediaStream->m_mapSections.begin(); it!=pMediaStream->m_mapSections.end(); ++it)
	{
		itSections = mapSections.find( make_pair<int,int> ( it->first.first, it->first.second ) );
		if( itSections==mapSections.end() )
		{
			if( pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_DVD_CONST ) // There's a title
			{
				string sCell;
				if( it->first.first>=0 )
					sCell += " CHAPTER:" + StringUtils::itos(it->first.first+1);  // Internally we're zero based
				if( it->first.second>=0 )
					sCell += " TITLE:" + StringUtils::itos(it->first.second+1);
				pDataGrid->SetData(0, currentPos++,new DataGridCell(it->second,sCell));
			}
			else
				pDataGrid->SetData(0, currentPos++,new DataGridCell(it->second, StringUtils::itos(it->first.first)));
		}
	}

    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaSearchAutoCompl( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    FileListGrid *pDataGrid = new FileListGrid( m_pDatagrid_Plugin, this );
    DataGridCell *pCell;

	string::size_type pos=0;
	int PK_MediaType = atoi(StringUtils::Tokenize(Parms,"|",pos).c_str());
	string AC = StringUtils::Tokenize(Parms,"|",pos);

	if( AC.length( )==0 )
        return pDataGrid; // Nothing passed in yet

    string SQL = "select DISTINCT PK_Attribute, Name, Description, FK_Picture FROM Attribute " \
		"JOIN File_Attribute ON FK_Attribute=PK_Attribute "\
		"JOIN File ON File_Attribute.FK_File=PK_File "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN MediaType_AttributeType ON MediaType_AttributeType.FK_AttributeType=PK_AttributeType "\
		"LEFT JOIN Picture_File ON Picture_File.FK_File=File.PK_File "\
		"WHERE Name Like '" + StringUtils::SQLEscape(AC) + "%' AND Identifier>0 " +
		" AND MediaType_AttributeType.EK_MediaType=" + StringUtils::itos(PK_MediaType) +
		" AND File.EK_MediaType=" + StringUtils::itos(PK_MediaType) +
        " ORDER BY Name limit 30;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    string AttributesFirstSearch; // Because we're going to search twice and want to exclude any attributes we hit the first search

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            string label = /* string( "~`S24`" ) + */ row[1];
            label += string( "\n" ) + row[2];
            pCell = new DataGridCell( "", row[0] );
			if( row[3] && row[3][0]!='0' )
			{
				size_t st=0;
				pCell->m_pGraphicData = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + string(row[3]) + "_tn.jpg",st);
				pCell->m_GraphicLength=st;
			}
            pDataGrid->SetData( 0, RowCount, pCell );

            if( AttributesFirstSearch.length() )
                AttributesFirstSearch += ",";
            AttributesFirstSearch += row[0];

            pCell = new DataGridCell( label, row[0] );
            pCell->m_Colspan = 5;
            pDataGrid->m_vectFileInfo.push_back( new FileListInfo( atoi( row[0] ) ) );
            pDataGrid->SetData( 1, RowCount++, pCell );
        }
    }

	// If we get less than 5 rows, we will do a 2nd search
	if( result.r->row_count> 4 )
		return pDataGrid;

    SQL = "select DISTINCT PK_Attribute, Name, Description, FK_Picture FROM SearchToken "\
        "JOIN SearchToken_Attribute ON PK_SearchToken=FK_SearchToken "\
        "JOIN Attribute ON SearchToken_Attribute.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
		"JOIN File_Attribute ON File_Attribute.FK_Attribute=PK_Attribute "\
		"JOIN File ON File_Attribute.FK_File=PK_File "\
        "JOIN MediaType_AttributeType ON MediaType_AttributeType.FK_AttributeType=PK_AttributeType "\
		"LEFT JOIN Picture_File ON Picture_File.FK_File=File.PK_File "\
		"WHERE Token like '" + StringUtils::SQLEscape(AC) + "%' " +
		(AttributesFirstSearch.length() ? "AND PK_Attribute NOT IN (" + StringUtils::SQLEscape(AttributesFirstSearch) + ") " : "") +
		" AND MediaType_AttributeType.EK_MediaType=" + StringUtils::itos(PK_MediaType) +
		" AND File.EK_MediaType=" + StringUtils::itos(PK_MediaType) + " AND Identifier>0 " +
		" ORDER BY Name "\
        "limit 30;";

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            string label = /*string( "~`S24`" ) + */ row[1];
            label += string( "\n" ) + row[2];
            pCell = new DataGridCell( "", row[0] );
			if( row[3] && row[3][0]!='0' )
			{
				size_t st=0;
				pCell->m_pGraphicData = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + string(row[3]) + "_tn.jpg",st);
				pCell->m_GraphicLength=st;
			}
            pDataGrid->SetData( 0, RowCount, pCell );

            pCell = new DataGridCell( label, row[0] );
            pCell->m_Colspan = 5;
            pDataGrid->m_vectFileInfo.push_back( new FileListInfo( atoi( row[0] ) ) );
            pDataGrid->SetData( 1, RowCount++, pCell );
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaAttrFiles( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCell;

    string PK_Attribute = Parms;

    if( PK_Attribute.substr( 0, 2 )=="!A" )
        PK_Attribute = PK_Attribute.substr( 2 );
    else if( PK_Attribute.substr( 0, 2 )=="!F" )
        PK_Attribute = StringUtils::itos(m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeFromFileID( atoi( PK_Attribute.substr( 2 ).c_str( ) ) ) );

    if ( PK_Attribute == "" )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Got a null attributte from string: %s", Parms.c_str());
        return new DataGridTable();;
    }

    string SQL="select DISTINCT Dest.FK_File, Attribute.Name, AttributeType.Description "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
		"ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute=" + StringUtils::itos(atoi(PK_Attribute.c_str())) +
        " JOIN File ON Dest.FK_File=PK_File "\
        "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND MediaType_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 ORDER BY Name limit 200;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            string label = row[1];
            label += string( "\n" ) + row[2];
            pCell = new DataGridCell( label, row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaAttrCollections( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCell;

	if( Parms.length()==0 )
		return pDataGrid;

    string PK_Attribute = Parms;
    if( PK_Attribute.substr( 0, 2 )=="!A" )
        PK_Attribute = PK_Attribute.substr( 2 );
    else if( PK_Attribute.substr( 0, 2 )=="!F" )
        PK_Attribute = StringUtils::itos(
            m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeFromFileID( atoi( PK_Attribute.substr( 2 ).c_str( ) ) )
         );

    if ( PK_Attribute == "" )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Got a null attributte from string: %s", Parms.c_str());
        return new DataGridTable();
    }

    string SQL="select DISTINCT Dest.FK_Attribute, Attribute.Name, AttributeType.Description "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
        "ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute='" + StringUtils::itos(atoi(PK_Attribute.c_str())) +
        "' JOIN File ON Dest.FK_File=PK_File "\
        "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND MediaType_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=2 ORDER BY Name limit 100;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            string label = row[1];
            label += string( "\n" ) + row[2];
            pCell = new DataGridCell( label, row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaAttrXref( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCell;

	string::size_type pos=0;
	string PK_MediaType = StringUtils::Tokenize(Parms,"|",pos);
	string PK_Attribute = StringUtils::Tokenize(Parms,"|",pos);
    g_pPlutoLogger->Write(LV_STATUS, "Got this PK_Attributte: %s", PK_Attribute.c_str());

	if( PK_Attribute.length()==0 )
		return pDataGrid;

	if( PK_Attribute.substr( 0, 2 )=="!A" )
        PK_Attribute = PK_Attribute.substr( 2 );

    int PK_Picture;
    string Extension;

	if( PK_Attribute.substr( 0, 2 )=="!F" )
	{
        Extension = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromFileID(atoi(PK_Attribute.substr(2).c_str()),&PK_Picture);
        PK_Attribute = StringUtils::itos(m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeFromFileID( atoi( PK_Attribute.substr( 2 ).c_str( ) ) ) );
	}
	else
        Extension = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromAttributeID(atoi(PK_Attribute.c_str()),&PK_Picture);

    g_pPlutoLogger->Write(LV_STATUS, "Transformed PK_Attributte: %s", PK_Attribute.c_str());

    string SQL="select DISTINCT Dest.FK_Attribute, Attribute.Name, AttributeType.Description "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
        "ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute=" + PK_Attribute +
        " JOIN File ON Dest.FK_File=PK_File "\
        "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND MediaType_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier>=2 ORDER BY AttributeType.Description limit 30;";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
    {
g_pPlutoLogger->Write(LV_STATUS, "Transformed 2 PK_Attributte: %s", PK_Attribute.c_str());
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
g_pPlutoLogger->Write(LV_STATUS, "Transformed 4 PK_Attributte: %s", PK_Attribute.c_str());
            string label = row[1];
            label += string( "\n" ) + row[2];
            pCell = new DataGridCell( label, row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
        }
    }
g_pPlutoLogger->Write(LV_STATUS, "Transformed 3 PK_Attributte: %s", PK_Attribute.c_str());

    size_t length=0;
    char *buffer=NULL;
    if( PK_Picture )
        buffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + "." + Extension,length);

	pos = 0;
    StringUtils::Tokenize(GridID, "_", pos);
    string controller = StringUtils::Tokenize(GridID, "_", pos);

    DCE::CMD_Update_Object_Image CMD_Update_Object_Image(m_dwPK_Device, atoi(controller.c_str()),
        StringUtils::itos(DESIGNOBJ_objCDCover_CONST),"1",buffer,(int) length,"");
    SendCommand(CMD_Update_Object_Image);

	return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaItemAttr( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable();
    DataGridCell *pCell;

    string SQL;

    int PK_File=0;
    string PK_Attribute = Parms;
    if( PK_Attribute.length()==0 )
        return NULL;
    if( PK_Attribute.substr(0,2)=="!F" )
        PK_File = atoi(PK_Attribute.substr(2).c_str());
    else
    {
        if( PK_Attribute.substr(0,2)=="!A" )
            PK_Attribute = PK_Attribute.substr(2);
//
        PK_File = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetFileIDFromAttributeID(atoi(PK_Attribute.c_str()));

        if( !PK_File )
        {
            // They chose an attribute other than a file, like an album
            SQL = "select DISTINCT PK_Attribute,Description,Name FROM File_Attribute AS Source "\
                "JOIN File_Attribute As Dest ON Source.FK_File=Dest.FK_File "\
                "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute  "\
                "JOIN C_AttributeType ON Attribute.FK_C_AttributeType=PK_C_AttributeType  "\
                "JOIN File ON Dest.FK_File=PK_File "\
                "JOIN C_Type_C_AttributeType ON C_Type_C_AttributeType.FK_C_Type=File.FK_C_Type "\
                "AND C_Type_C_AttributeType.FK_C_AttributeType=Attribute.FK_C_AttributeType "\
                "WHERE Identifier>2 AND Source.FK_Attribute=" + StringUtils::itos(atoi(PK_Attribute.c_str())) + " ORDER BY Description";
        }
    }

    string::size_type pos = 0;
    StringUtils::Tokenize(GridID, "_", pos);
    string controller = StringUtils::Tokenize(GridID, "_", pos);

    int PK_Picture;
    string Extension;
    if( PK_File )
        Extension = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromFileID(PK_File,&PK_Picture);
    else
        Extension = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromAttributeID(atoi(PK_Attribute.c_str()),&PK_Picture);

    size_t length=0;
    char *buffer=NULL;
    if( PK_Picture )
        buffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + "." + Extension,length);

    DCE::CMD_Update_Object_Image CMD_Update_Object_Image(m_dwPK_Device, atoi(controller.c_str()),
        StringUtils::itos(DESIGNOBJ_objCDCover_CONST),"1",buffer,(int) length,"");
    SendCommand(CMD_Update_Object_Image);

    // We may have already built the sql statement
    if( SQL.length()==0 )
    {
        SQL="SELECT PK_Attribute,Description,Name FROM File "\
        "JOIN File_Attribute ON FK_File=PK_File "\
        "JOIN Attribute ON FK_Attribute=PK_Attribute "\
        "JOIN C_AttributeType ON FK_C_AttributeType=PK_C_AttributeType "\
        "WHERE PK_File=" + StringUtils::itos(PK_File) + " ORDER By Description";
    }

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( (result.r=m_pDatabase_pluto_media->mysql_query_result(SQL)) )
    {
        while( (row=mysql_fetch_row(result.r)) )
        {
            string label = row[1];
            label += ": ";
            label += row[2];
            pCell = new DataGridCell(label,row[0]);
            pDataGrid->SetData(0,RowCount++,pCell);
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::DevicesPipes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();
    //DataGridCell *pCell;

	EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(Parms.c_str()) );
	if( !pEntertainArea || !pEntertainArea->m_pMediaDevice_ActiveDest )
		return pDataGrid;

	int iRow=0;
	DevicesPipes_Loop( pMessage->m_dwPK_Device_From, pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router,
		pDataGrid,iRow);
	return pDataGrid;
}


void Media_Plugin::DevicesPipes_Loop(int PK_Orbiter,DeviceData_Router *pDevice,DataGridTable *&pDataGrid,int &iRow,int PK_Command_Input,int PK_Command_Output,vector<int> *p_vectDevice)
{
	if( !pDevice )
		return;

	bool bCreatedVect=false;
	if( !p_vectDevice )
	{
		p_vectDevice = new vector<int>;
		bCreatedVect=true;
	}

	// Be sure we skip over recursive entries
	for(size_t s=0;s<p_vectDevice->size();++s)
		if( (*p_vectDevice)[s]==pDevice->m_dwPK_Device )
			return;
	p_vectDevice->push_back(pDevice->m_dwPK_Device);

	for(map<int, class Pipe *>::iterator it=pDevice->m_mapPipe_Active.begin();
		it!=pDevice->m_mapPipe_Active.end();++it)
	{
		Pipe *pPipe = (*it).second;
		DeviceData_Router *pDevice_Pipe = m_pRouter->m_mapDeviceData_Router_Find(pPipe->m_pRow_Device_Device_Pipe->FK_Device_To_get());
		if( pDevice_Pipe && pDevice_Pipe!=pDevice )
			DevicesPipes_Loop(PK_Orbiter,(DeviceData_Router *)pDevice_Pipe,pDataGrid,iRow,pPipe->m_pRow_Device_Device_Pipe->FK_Command_Input_get(),pPipe->m_pRow_Device_Device_Pipe->FK_Command_Output_get(),p_vectDevice);
	}

	if( pDevice->m_pDevice_MD && pDevice!=pDevice->m_pDevice_MD )
		DevicesPipes_Loop(PK_Orbiter,(DeviceData_Router *) pDevice->m_pDevice_MD,pDataGrid,iRow,0,0,p_vectDevice);

	DataGridCell *pCell = new DataGridCell( pDevice->m_sDescription);
	pCell->m_Colspan = 4;
	pCell->m_pMessage = new Message(m_dwPK_Device,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_On_CONST,2,COMMANDPARAMETER_Retransmit_CONST,"1",COMMANDPARAMETER_PK_Pipe_CONST,"-1");
	pDataGrid->SetData(0,iRow,pCell);

	if( PK_Command_Input )
	{
		Command *pCommand = m_pRouter->m_mapCommand_Find( PK_Command_Input );
		if( pCommand )
		{
			pCell = new DataGridCell( pCommand->m_sDescription );
			pCell->m_pMessage = new Message(m_dwPK_Device,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Input_Select_CONST,2,COMMANDPARAMETER_Retransmit_CONST,"1",COMMANDPARAMETER_PK_Command_Input_CONST,StringUtils::itos(pCommand->m_dwPK_Command).c_str());
			pDataGrid->SetData(4,iRow,pCell);
		}
	}
	if( PK_Command_Output )
	{
		Command *pCommand = m_pRouter->m_mapCommand_Find( PK_Command_Output );
		if( pCommand )
		{
			pCell = new DataGridCell( pCommand->m_sDescription );
			pCell->m_pMessage = new Message(m_dwPK_Device,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Output_Select_CONST,2,COMMANDPARAMETER_Retransmit_CONST,"1",COMMANDPARAMETER_PK_Command_Output_CONST,StringUtils::itos(pCommand->m_dwPK_Command).c_str());
			pDataGrid->SetData(5,iRow,pCell);
		}
	}

	pCell = new DataGridCell( "Advanced" );
    pCell->m_pMessage = new Message(m_dwPK_Device,PK_Orbiter,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Goto_DesignObj_CONST,
		1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_mnuDeviceControl_CONST).c_str());
	pCell->m_pMessage->m_vectExtraMessages.push_back(
		new Message(m_dwPK_Device,PK_Orbiter,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Set_Variable_CONST,2,
		COMMANDPARAMETER_PK_Variable_CONST,StringUtils::itos(VARIABLE_Datagrid_Input_CONST).c_str(),
		COMMANDPARAMETER_Value_To_Assign_CONST,StringUtils::itos(pDevice->m_dwPK_Device).c_str())
	);

	pDataGrid->SetData(6,iRow++,pCell);

	if( bCreatedVect )
		delete p_vectDevice;
}

class DataGridTable *Media_Plugin::MediaAttrCurStream( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();
    //DataGridCell *pCell;

	EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(Parms.c_str()) );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream )
		return pDataGrid;

	int iRow=0;
	for(map<int,int>::iterator it=pEntertainArea->m_pMediaStream->m_mapPK_Attribute.begin();it!=pEntertainArea->m_pMediaStream->m_mapPK_Attribute.end();++it)
	{
		Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
		Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(it->second);
		if( pRow_AttributeType && pRow_Attribute )
		{
			string sName = pRow_AttributeType->Description_get() + ": ";
			sName += pRow_Attribute->Name_get();
			DataGridCell *pCell = new DataGridCell( sName, StringUtils::itos(it->second) );
			pDataGrid->SetData(0,iRow++,pCell);
		}
	}
	for(size_t s=0;s<pEntertainArea->m_pMediaStream->m_dequeMediaFile.size();++s)
	{
		MediaFile *pMediaFile = pEntertainArea->m_pMediaStream->m_dequeMediaFile[s];
		string sNamePrefix;
		if( pMediaFile->m_dwPK_File )
			sNamePrefix = "(" + pMediaFile->m_sFilename + ") ";  // It's a real file
		else
			sNamePrefix = "(#" + StringUtils::itos(s+1) + ") ";
		for( map<int,int>::iterator it=pMediaFile->m_mapPK_Attribute.begin();it!=pMediaFile->m_mapPK_Attribute.end();++it)
		{
			Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
			Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(it->second);
			if( pRow_AttributeType && pRow_Attribute )
			{
				string sName = sNamePrefix + pRow_AttributeType->Description_get() + "\n" + pRow_Attribute->Name_get();
				DataGridCell *pCell = new DataGridCell( sName, StringUtils::itos(it->second) );
				pDataGrid->SetData(0,iRow++,pCell);
			}
		}
		AddMediaSectionToDataGrid(pDataGrid,iRow,pMediaFile->m_dequeMediaSection,sNamePrefix);
		AddMediaTitlesToDataGrid(pDataGrid,iRow,pMediaFile->m_dequeMediaTitle,sNamePrefix);
	}

	AddMediaTitlesToDataGrid(pDataGrid,iRow,pEntertainArea->m_pMediaStream->m_dequeMediaTitle,"");

	return pDataGrid;
}

void Media_Plugin::AddMediaTitlesToDataGrid(DataGridTable *pDataGrid,int &iRow,deque<MediaTitle *> &dequeMediaTitle,string sPreface)
{
	for(size_t s=0;s<dequeMediaTitle.size();++s)
	{
		MediaTitle *pMediaTitle = dequeMediaTitle[s];
		string sNamePrefix = "(Title " + StringUtils::itos(s+1) + ") ";
		for( map<int,int>::iterator it=pMediaTitle->m_mapPK_Attribute.begin();it!=pMediaTitle->m_mapPK_Attribute.end();++it)
		{
			Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
			Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(it->second);
			if( pRow_AttributeType && pRow_Attribute )
			{
				string sName = sNamePrefix + pRow_AttributeType->Description_get() + "\n" + pRow_Attribute->Name_get();
				DataGridCell *pCell = new DataGridCell( sName, StringUtils::itos(it->second) );
				pDataGrid->SetData(0,iRow++,pCell);
			}
		}
		AddMediaSectionToDataGrid(pDataGrid,iRow,pMediaTitle->m_dequeMediaSection,sNamePrefix);
	}
}

void Media_Plugin::AddMediaSectionToDataGrid(DataGridTable *pDataGrid,int &iRow,deque<MediaSection *> &dequeMediaSection,string sPreface)
{
	for(size_t s=0;s<dequeMediaSection.size();++s)
	{
		MediaSection *pMediaSection = dequeMediaSection[s];
		for( map<int,int>::iterator it=pMediaSection->m_mapPK_Attribute.begin();it!=pMediaSection->m_mapPK_Attribute.end();++it)
		{
			Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
			Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(it->second);
			if( pRow_AttributeType && pRow_Attribute )
			{
				string sName = sPreface + " #" + StringUtils::itos(s+1) + " ";
				sName += pRow_AttributeType->Description_get() + "\n" + pRow_Attribute->Name_get();
				DataGridCell *pCell = new DataGridCell( sName, StringUtils::itos(it->second) );
				pDataGrid->SetData(0,iRow++,pCell);
			}
		}
	}
}





























































class DataGridTable *Media_Plugin::FloorplanMediaChoices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();

    EntertainArea *pEntertainArea = m_mapEntertainAreas_Find(atoi(Parms.c_str()));
    if( !pEntertainArea )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find entertainment area for flooplan");
        return new DataGridTable();
    }

    string sWhereClause = string("JOIN CommandGroup_Command ON CommandGroup_Command.FK_CommandGroup=PK_CommandGroup ") +
        "JOIN CommandGroup_EntertainArea ON CommandGroup_EntertainArea.FK_CommandGroup=PK_CommandGroup " +
        "WHERE FK_EntertainArea=" + StringUtils::itos(pEntertainArea->m_iPK_EntertainArea) + " AND FK_Command=" + StringUtils::itos(COMMAND_MH_Play_Media_CONST);
    vector<Row_CommandGroup *> vectRow_CommandGroup;
    m_pDatabase_pluto_main->CommandGroup_get()->GetRows(sWhereClause,&vectRow_CommandGroup);

    for(size_t s=0;s<vectRow_CommandGroup.size();++s)
    {
        Row_CommandGroup *pRow_CommandGroup = vectRow_CommandGroup[s];
        DataGridCell *pDataGridCell = new DataGridCell(pRow_CommandGroup->Description_get(),StringUtils::itos(pRow_CommandGroup->PK_CommandGroup_get()));
        pDataGrid->SetData(0,s,pDataGridCell);
    }

    return pDataGrid;
}

class DataGridTable *Media_Plugin::ActiveMediaStreams( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    g_pPlutoLogger->Write(LV_STATUS, "Ready to populate active media stream...");
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();

    int iRow=0;
    for(MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it)
    {
        MediaStream *pMediaStream = (*it).second;
		bool bPrivate=false;
		for( map<int,class EntertainArea *>::iterator itEntAreas=pMediaStream->m_mapEntertainArea.begin(); itEntAreas != pMediaStream->m_mapEntertainArea.end(); itEntAreas++)
		{
			if( itEntAreas->second->m_bMediaIsPrivate )
			{
				bPrivate=true;
				continue;
			}
		}
		if( bPrivate )
			continue;
        pMediaStream->m_iOrder = iRow;
        DataGridCell *pDataGridCell = new DataGridCell(pMediaStream->m_sMediaDescription,StringUtils::itos(pMediaStream->m_iStreamID_get()));
        pDataGridCell->m_AltColor = UniqueColors[iRow];
        pDataGrid->SetData(0,iRow++,pDataGridCell);
    }
    g_pPlutoLogger->Write(LV_STATUS, "Done with active media streams...");
    return pDataGrid;
}

class DataGridTable *Media_Plugin::AvailablePlaylists( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    int nWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
    int nHeight = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Height_CONST].c_str());
	
    g_pPlutoLogger->Write(LV_STATUS, "Media_Plugin::AvailablePlaylists Called to populate: %s", Parms.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

    if( Parms.length( )==0 )
        return NULL; // Nothing passed in yet

    // select PK_Playlist, Name from Playlist where EK_USER IN ( 0, 33128 ) LIMIT 30;;

    int userID = atoi(Parms.c_str());

    string SQL = "SELECT PK_Playlist, Name from Playlist where EK_USER IN ( 0, " + StringUtils::itos(userID) + " ) ORDER BY NAME LIMIT 30";

    PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    DataGridTable *pDataGrid = new DataGridTable();
    if( (result.r=m_pDatabase_pluto_media->mysql_query_result(SQL)) )
        while( (row=mysql_fetch_row(result.r)) )
		{
            // 	g_pPlutoLogger->Write(LV_CRITICAL, "Adding this entry \"%s\" to the position %d", row[1], RowCount);
			if( nHeight==1 ) // It's a horizontal list
				pDataGrid->SetData(RowCount++,0,new DataGridCell(row[1], row[0]));
			else
				pDataGrid->SetData(0,RowCount++,new DataGridCell(row[1], row[0]));
		}

    return pDataGrid;
}

class DataGridTable *Media_Plugin::DVDSubtitles( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable();

	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea );
	if( vectEntertainArea.size()==0 || vectEntertainArea[0]->m_pMediaStream==NULL )
		return pDataGrid;
	MediaStream *pMediaStream = vectEntertainArea[0]->m_pMediaStream;
	string Data = GetCurrentDeviceData( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, DEVICEDATA_Subtitles_CONST );
	string::size_type pos=0;
	int iCurrent = atoi(StringUtils::Tokenize(Data,"\n",pos).c_str());
	DataGridCell *pCell = new DataGridCell("Off", "-1");
	pDataGrid->SetData(0,0,pCell);
	for(int i=0;pos!=string::npos && pos<Data.size();++i)
	{
		DataGridCell *pCell = new DataGridCell( StringUtils::Tokenize(Data,"\n",pos), StringUtils::itos(i) );
		pDataGrid->SetData(0,i+1,pCell);
	}
	*iPK_Variable=VARIABLE_Misc_Data_1_CONST;
	*sValue_To_Assign=StringUtils::itos(iCurrent);
	return pDataGrid;
}

class DataGridTable *Media_Plugin::DVDAudioTracks( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable();

	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea );
	if( vectEntertainArea.size()==0 || vectEntertainArea[0]->m_pMediaStream==NULL )
		return pDataGrid;
	MediaStream *pMediaStream = vectEntertainArea[0]->m_pMediaStream;
	string Data = GetCurrentDeviceData( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, DEVICEDATA_Audio_Tracks_CONST );
	string::size_type pos=0;
	int iCurrent = atoi(StringUtils::Tokenize(Data,"\n",pos).c_str());
	for(int i=0;pos!=string::npos && pos<Data.size();++i)
	{
		DataGridCell *pCell = new DataGridCell( StringUtils::Tokenize(Data,"\n",pos), StringUtils::itos(i) );
		pDataGrid->SetData(0,i,pCell);
	}
	*iPK_Variable=VARIABLE_Misc_Data_2_CONST;
	*sValue_To_Assign=StringUtils::itos(iCurrent);
	return pDataGrid;
}

class DataGridTable *Media_Plugin::DVDAngles( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable();

	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea );
	if( vectEntertainArea.size()==0 || vectEntertainArea[0]->m_pMediaStream==NULL )
		return pDataGrid;
	MediaStream *pMediaStream = vectEntertainArea[0]->m_pMediaStream;
	string Data = GetCurrentDeviceData( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, DEVICEDATA_Angles_CONST );
	string::size_type pos=0;
	int iCurrent = atoi(StringUtils::Tokenize(Data,"\n",pos).c_str());
	for(int i=0;pos!=string::npos && pos<Data.size();++i)
	{
		DataGridCell *pCell = new DataGridCell( StringUtils::Tokenize(Data,"\n",pos), StringUtils::itos(i) );
		pDataGrid->SetData(0,i,pCell);
	}
	*iPK_Variable=VARIABLE_Misc_Data_3_CONST;
	*sValue_To_Assign=StringUtils::itos(iCurrent);
	return pDataGrid;
}

class DataGridTable *Media_Plugin::Bookmarks( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    int nWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());

    DataGridTable *pDataGrid = new DataGridTable();
	g_pPlutoLogger->Write(LV_STATUS, "Media_Plugin::Bookmarks Called to populate: %s", Parms.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// If this is called to browse the bookmarks within the currently playing file, this will be called with
	// MediaType=0 and we will assign the EntArea.  Otherwise the MediaType and current user will be passed in
	EntertainArea *pEntertainArea=NULL;
	string::size_type pos=0;
	int PK_MediaType = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int PK_Users = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	
	string sWhere;
	if( PK_MediaType )
		sWhere = "EK_MediaType=" + StringUtils::itos(PK_MediaType) + " AND FK_File IS NOT NULL AND (EK_Users IS NULL OR EK_Users="+StringUtils::itos(PK_Users)+")";
	else
	{
		vector<EntertainArea *> vectEntertainArea;
		// Only an Orbiter will tell us to play media
		DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea );
		if( vectEntertainArea.size() )
		{
			pEntertainArea = vectEntertainArea[0];
			if( pEntertainArea->m_pMediaStream )
			{
				if( pEntertainArea->m_pMediaStream->m_dwPK_Disc )
					sWhere = "FK_Disc=" + StringUtils::itos(pEntertainArea->m_pMediaStream->m_dwPK_Disc) + " AND (EK_Users IS NULL OR EK_Users="+StringUtils::itos(PK_Users)+")";
				else if( pEntertainArea->m_pMediaStream->m_dequeMediaFile.size() &&
					pEntertainArea->m_pMediaStream->m_iDequeMediaFile_Pos>=0 &&
					pEntertainArea->m_pMediaStream->m_iDequeMediaFile_Pos<pEntertainArea->m_pMediaStream->m_dequeMediaFile.size() )
				{
					MediaFile *pMediaFile = pEntertainArea->m_pMediaStream->GetCurrentMediaFile();
					sWhere = "FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File) + " AND (EK_Users IS NULL OR EK_Users="+StringUtils::itos(PK_Users)+")";
				}
			}
		}
	}

	if( sWhere.size()==0 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Media_Plugin::Bookmarks no where clause %s",Parms.c_str());
		return pDataGrid;
	}

	vector<Row_Bookmark *> vectRow_Bookmark;
	m_pDatabase_pluto_media->Bookmark_get()->GetRows(sWhere,&vectRow_Bookmark);

	for(size_t s=0;s<vectRow_Bookmark.size();++s)
	{
		Row_Bookmark *pRow_Bookmark = vectRow_Bookmark[s];

        if(nWidth > 1)
        {
            DataGridCell *pDataGridCell,*pDataGridCell_Cover,*pDataGridCell_Preview;
            pDataGridCell = new DataGridCell(pRow_Bookmark->Description_get(), StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
            pDataGridCell_Cover = new DataGridCell("", StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
            pDataGridCell_Preview = new DataGridCell("", StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));

            pDataGrid->SetData(0,s,pDataGridCell_Cover);
            pDataGrid->SetData(1,s,pDataGridCell_Preview);
            pDataGrid->SetData(2,s,pDataGridCell);

            Row_Picture *pRow_Picture=NULL;
            if( (pRow_Picture=pRow_Bookmark->FK_Picture_getrow())!=NULL )
            {
                size_t iSize;
                char *pBuffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + "_tn." + pRow_Picture->Extension_get(),iSize);
                if( pBuffer )
                {
                    pDataGridCell_Preview->m_pGraphicData = pBuffer;
                    pDataGridCell_Preview->m_GraphicLength = iSize;
                }
            }

			int PK_Picture = 0;
            Row_File *pRow_File = pRow_Bookmark->FK_File_getrow();

			if(pRow_File)
			{
				string sFilePath = pRow_File->Path_get();
				string sFileName = pRow_File->Filename_get();
				PlutoMediaFile PlutoMediaFile_(m_pDatabase_pluto_media, m_pRouter->iPK_Installation_get(), sFilePath, sFileName);
				PK_Picture = PlutoMediaFile_.GetPicAttribute(pRow_File->PK_File_get());

				if(PK_Picture > 0)
				{
					size_t iSize;
					char *pBuffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + "_tn.jpg",iSize);
					if( pBuffer )
					{
						pDataGridCell_Cover->m_pGraphicData = pBuffer;
						pDataGridCell_Cover->m_GraphicLength = iSize;
					}
					g_pPlutoLogger->Write(LV_WARNING,"pic file 2 %p",pBuffer);
				}
				else
					g_pPlutoLogger->Write(LV_WARNING,"File %s pic %d",(pRow_File->Path_get() + "/" + pRow_File->Filename_get()).c_str( ),PK_Picture);
			}
        }
        else
        {
            Row_File *pRow_File = pRow_Bookmark->FK_File_getrow();
            string sItem;
			if( pRow_File )
				sItem = pRow_File->Filename_get() +  
	                "\n" + pRow_Bookmark->Description_get();
			else
				sItem = pRow_Bookmark->Description_get();

			DataGridCell *pDataGridCell = pDataGridCell = new DataGridCell(sItem, 
                StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
            pDataGrid->SetData(0,s,pDataGridCell);
        }
	}

    return pDataGrid;
}

class DataGridTable *Media_Plugin::BookmarksByMediaType( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable();

	g_pPlutoLogger->Write(LV_STATUS, "Media_Plugin::BookmarksByMediaType Called to populate: %s", Parms.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	string::size_type pos=0;
	int PK_MediaType = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int PK_Users = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int iMaxColumns = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());

	string sWhere = "EK_MediaType=" + StringUtils::itos(PK_MediaType) + " AND (EK_Users IS NULL OR EK_Users="+StringUtils::itos(PK_Users)+")";
	vector<Row_Bookmark *> vectRow_Bookmark;
	m_pDatabase_pluto_media->Bookmark_get()->GetRows(sWhere,&vectRow_Bookmark);
	int iRow=0,iColumn=0;

	for(size_t s=0;s<vectRow_Bookmark.size();++s)
	{
		Row_Bookmark *pRow_Bookmark = vectRow_Bookmark[s];
		DataGridCell *pDataGridCell = new DataGridCell("", StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
		Row_Picture *pRow_Picture=NULL;
		if( (pRow_Picture=pRow_Bookmark->FK_Picture_getrow())!=NULL )
		{
			size_t iSize;
			char *pBuffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + "_tn." + pRow_Picture->Extension_get(),iSize);
			if( pBuffer )
			{
				pDataGridCell->m_pGraphicData = pBuffer;
				pDataGridCell->m_GraphicLength = iSize;
			}
		}
		pDataGridCell->m_Rowspan=2;
		pDataGrid->SetData(iColumn,iRow,pDataGridCell);
		pDataGridCell = new DataGridCell(pRow_Bookmark->Description_get(), StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
		pDataGrid->SetData(iColumn,iRow+2,pDataGridCell);

		if( iColumn++ >= iMaxColumns-1 )
		{
			iColumn=0;
			iRow+=3;
		}
	}

	return pDataGrid;
}
