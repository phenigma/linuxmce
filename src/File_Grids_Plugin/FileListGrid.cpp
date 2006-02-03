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
#include <attr/attributes.h>
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
#include "UpdateMedia/PlutoMediaFile.h"

void FileListGrid::ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount)
{
	// Don't render icons in the first column if we're not displaying the first column
	if( *ColStart )
	{
		DataGridTable::ToData(GridID,Size,Data,ColStart,RowStart,ColCount,RowCount);
		return;
	}

#ifdef DEBUG
	clock_t cStart=clock(); // move this to within #debug
#endif
	for(int row=*RowStart;row<=*RowStart+RowCount && row<m_TotalRows;++row)
	{
		DataGridCell *pCell = GetData(0,row);
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"filelistgrid::row %d graphic data: %p rowstart: %d rowCount: %d totalrows: %d",
	row,(pCell ? pCell->m_pGraphicData : NULL),*RowStart,RowCount,m_TotalRows);
#endif
		if( (!pCell || !pCell->m_pGraphicData) && (!pCell || pCell->m_Text==NULL || pCell->m_Text[0]==0) ) // We haven't already set a picture for this cell.  Skip it if it's a cell with text
		{
			string Extension,PictureFile;
			int PKID_MED_Picture=0;
			enum eGraphicFormat format;

			FileListInfo *flInfo = m_vectFileInfo[row];
			if( flInfo->m_bIsBack )
				continue;

			if(0 == (PKID_MED_Picture = PlutoMediaFile::GetPictureIdFromExtendentAttributes(flInfo->m_sPath)))
			{
				string sFilePath = FileUtils::BasePath(flInfo->m_sPath);
				string sFileName = FileUtils::FilenameWithoutPath(flInfo->m_sPath, false);
				PlutoMediaFile PlutoMediaFile_(m_pMedia_Plugin->m_pDatabase_pluto_media, m_pMedia_Plugin->m_pDatabase_pluto_main, sFilePath, sFileName);
				int PK_File = PlutoMediaFile_.GetFileAttribute(false);
				if(!PK_File ||  !(PKID_MED_Picture = PlutoMediaFile_.GetPicAttribute(PK_File)))
				{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS, "No PIC extended attribute or id3 tag for picture id for %s", flInfo->m_sPath.c_str());
#endif

					continue;
				}
			}

#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS, "Pic for %s is: %d",flInfo->m_sPath.c_str( ),PKID_MED_Picture);
#endif
			char *pIconBuffer = NULL;
			size_t stIconSize;

			if( PKID_MED_Picture )
			{
				PictureFile = "/home/mediapics/" + StringUtils::itos(PKID_MED_Picture) + "_tn.jpg";
				pIconBuffer = FileUtils::ReadFileIntoBuffer(PictureFile,stIconSize);
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS, "Pic file: %s has size: %d", PictureFile.c_str(),stIconSize);
#endif
				format = GR_JPG;
			}

			if( pIconBuffer )
			{
				if( !pCell )
					pCell = new DataGridCell("","");
				pCell->SetImage(pIconBuffer,(int) stIconSize,format);
				SetData(0,row,pCell);
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
g_pPlutoLogger->Write(LV_WARNING,"file grid pics took %d ms",(int) (cStop-cStart));
#endif

	DataGridTable::ToData(GridID,Size,Data,ColStart,RowStart,ColCount,RowCount);
}

