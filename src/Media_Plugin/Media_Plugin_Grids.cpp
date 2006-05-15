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
#include "Gen_Devices/AllScreens.h"

#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "File_Grids_Plugin/File_Grids_Plugin.h"
#include "pluto_main/Define_DataGrid.h"
#include "DataGrid.h"
#include "File_Grids_Plugin/FileListGrid.h"
#include "SerializeClass/ShapesColors.h"
#include "RippingJob.h"
#include "../VFD_LCD/VFD_LCD_Base.h"
#include "UpdateMedia/PlutoMediaFile.h"

extern int UniqueColors[MAX_MEDIA_COLORS];

class DataGridTable *Media_Plugin::MediaBrowser( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos = Parms.find(",");
	if( pos==string::npos )
		return NULL;

	int PK_AttributeType = atoi(Parms.c_str());
	if( PK_AttributeType==0 )
	{
		DataGridGeneratorCallBack *pCB = m_pDatagrid_Plugin->GetCallBack(DATAGRID_Directory_Listing_CONST,0);
		DataGridTable *pDataGridTable=NULL;

		if( pCB )
			return CALL_MEMBER_FN( *pCB->m_pDataGridGeneratorPlugIn, pCB->m_pDCEDataGridGeneratorFn ) ( GridID, Parms.substr(pos+1), ExtraData, iPK_Variable, sValue_To_Assign, pMessage );
		else
			return NULL;
	}

	// What type of media is this
	pos = Parms.find("~MT");
	if( pos==string::npos )
		return NULL;

	int PK_MediaType = atoi(Parms.substr(pos+3).c_str());
	string sSQL = "select PK_Attribute,PK_File,Name,Path,Filename,FK_Picture FROM Attribute"
		" LEFT JOIN File_Attribute ON FK_Attribute=PK_Attribute"
		" LEFT JOIN File ON File_Attribute.FK_File=PK_File"
		" LEFT JOIN Picture_File ON Picture_File.FK_File=PK_File"
		" WHERE FK_AttributeType=" + StringUtils::itos(PK_AttributeType) + " AND EK_MediaType=" + StringUtils::itos(PK_MediaType) +
		" ORDER BY Name";

    PlutoSqlResult result;
    MYSQL_ROW row;
    DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
	int RowCount=0,ColCount=0,MaxCol=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	int PK_Attribute_Last=0;
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( sSQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
			int PK_Attribute = atoi(row[0]);
			if( PK_Attribute!=PK_Attribute_Last && PK_AttributeType!=ATTRIBUTETYPE_Title_CONST )
			{
				pCell = new DataGridCell( row[2] );
				pCell->m_Colspan = 5;
	            pDataGrid->SetData( 1, RowCount++, pCell );
				PK_Attribute_Last=PK_Attribute;
			}
			pCell = new DataGridCell( row[4] );
	        pDataGrid->SetData( ColCount++, RowCount, pCell );
			if( ColCount>=MaxCol )
			{
				ColCount=0;  RowCount++;
			}

			size_t size=0;
			if( row[5] )
				pCell->m_pGraphicData = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + string(row[5]) + ".jpg",size);
			pCell->m_GraphicLength=size;

			DCE::CMD_MH_Play_Media CMD_MH_Play_Media(pMessage->m_dwPK_Device_From,m_dwPK_Device,0,"!F" + string(row[1]),0,0,"",false,0);
			pCell->m_pMessage = CMD_MH_Play_Media.m_pMessage;
		}
	}
	
	return pDataGrid;
}

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
