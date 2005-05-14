/*
 FileListGrid

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "File_Grids_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/CommonIncludes.h"


#ifndef WIN32
#include <dirent.h>
#endif

#include <mysql.h>
#include <vector>
#include <sstream>
#include <time.h>
#include <string.h>

#include "DataGrid.h"
#include "FileListGrid.h"
#include "Media_Plugin/MediaAttributes.h"
#include "Media_Plugin/Media_Plugin.h"

void FileListGrid::ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount)
{
	// Don't render icons in the first column if we're not displaying the first column
	if( ColStart )
	{
		DataGridTable::ToData(GridID,Size,Data,ColStart,RowStart,ColCount,RowCount);
		return;
	}

#ifndef WIN32  // We don't support pictures in Windows

#ifdef DEBUG
	clock_t cStart=clock(); // move this to within #debug
#endif
	int CurrentCellSize;
	for(int row=RowStart;row<=RowStart+RowCount && row<m_TotalRows;++row)
	{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"filelistgrid::row %d",row);
#endif
		DataGridCell *pCell = GetData(0,row,&CurrentCellSize);
		if( (!pCell || !pCell->m_pGraphicData) && (!pCell || pCell->m_Text==NULL) ) // We haven't already set a picture for this cell.  Skip it if it's a cell with text
		{
			string Extension,PictureFile;
			int PKID_MED_Picture=0;
			enum eGraphicFormat format;

			FileListInfo *flInfo = m_vectFileInfo[row];
			int n = 79,result;
			char value[80];
			memset( value, 0, sizeof( value ) );

			if ( (result=attr_get( flInfo->m_sPath.c_str( ), "PIC", value, &n, 0)) != 0 || (PKID_MED_Picture = atoi(value))==0 )
{
	g_pPlutoLogger->Write(LV_STATUS, "No PIC attribute for %s result: %d",flInfo->m_sPath.c_str( ),result);
continue;
}
g_pPlutoLogger->Write(LV_STATUS, "Pic for %s is: %d",flInfo->m_sPath.c_str( ),PKID_MED_Picture);

			char *pIconBuffer = NULL;
			size_t stIconSize;

			if( PKID_MED_Picture )
			{
				PictureFile = "/home/mediapics/" + StringUtils::itos(PKID_MED_Picture) + "_tn.jpg";
//g_pPlutoLogger->Write(LV_STATUS, "Loking for file: %s", PictureFile.c_str());
#ifdef WIN32
				string::size_type s;
				while( (s=PictureFile.find('/'))!=string::npos )
					PictureFile.replace(s,1,"\\");
#endif
				pIconBuffer = FileUtils::ReadFileIntoBuffer(PictureFile,stIconSize);
				format = GR_JPG;
			}
		}
	}
#ifdef DEBUG
	clock_t cStop = clock();
	if( cStop-cStart>CLOCKS_PER_SEC/2 )  // Nothing should take 500 ms
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"file grid pics took %d ms",(int) (cStop-cStart));
	} else if( cStop-cStart>CLOCKS_PER_SEC/10 )
	{
		g_pPlutoLogger->Write(LV_WARNING,"file grid pics took %d ms",(int) (cStop-cStart));
	}
#endif

#endif
	DataGridTable::ToData(GridID,Size,Data,ColStart,RowStart,ColCount,RowCount);
}



/*






				if( !pIconBuffer )
				{
				}


*/

/* directories
			size_t stIconSize;
			char *pIconBuffer=NULL;

			string ImageFile = Directory + "/cover.jpg";
			pIconBuffer = FileUtils::ReadFileIntoBuffer(ImageFile,stIconSize);
			if( pIconBuffer )
				pCell->SetImage(pIconBuffer,(int) stIconSize,GR_JPG);
			else
			{
				pIconBuffer = FileUtils::ReadFileIntoBuffer("/home/media/miscicons/folder.png",stIconSize);
				if( pIconBuffer )
					pCell->SetImage(pIconBuffer,(int) stIconSize,GR_PNG);
			}
*/
