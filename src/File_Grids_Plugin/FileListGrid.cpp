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
		if( !pCell || !pCell->m_pGraphicData ) // We haven't already set a picture for this cell
		{
			string Extension,PictureFile;
			int PKID_MED_Picture=0;
			enum eGraphicFormat format;

			FileListInfo *flInfo = m_vectFileInfo[row];
			g_pPlutoLogger->Write(LV_STATUS, "File info pointer: %p", flInfo);

			g_pPlutoLogger->Write(LV_STATUS, "Attr type: %d", flInfo->m_PKID_MED_Attribute);
			if( flInfo->m_PKID_MED_Attribute )
			{
#pragma warning("I commmented out the 3 following pictures because there seems to be a mysql related bug that, at random, intermittently, mysql starts taking 9 seconds to execute each query");
				Extension = m_pMedia_Plugin->m_pMediaAttributes->GetPictureFromAttributeID(flInfo->m_PKID_MED_Attribute,&PKID_MED_Picture);
			}
			else if( !flInfo->m_bIsDirectory )
			{
				g_pPlutoLogger->Write(LV_STATUS, "Not a folder: %s : %d", flInfo->m_sPath.c_str(), PKID_MED_Picture);
				Extension = m_pMedia_Plugin->m_pMediaAttributes->GetPictureFromFilePath(flInfo->m_sPath,&PKID_MED_Picture);
				g_pPlutoLogger->Write(LV_STATUS, "We are here after the folder: %s : %d... ext: %s", flInfo->m_sPath.c_str(), PKID_MED_Picture, Extension.c_str());
			}
			else if( flInfo->m_sPath.length()>0 )
			{
				g_pPlutoLogger->Write(LV_STATUS, "We are here: %s : %d", flInfo->m_sPath.c_str(), PKID_MED_Picture);
				Extension = m_pMedia_Plugin->m_pMediaAttributes->GetAnyPictureUnderDirectory(flInfo->m_sPath,&PKID_MED_Picture,5);
				g_pPlutoLogger->Write(LV_STATUS, "We are here: %s : %d... ext: %s", flInfo->m_sPath.c_str(), PKID_MED_Picture, Extension.c_str());
			}

			char *pIconBuffer = NULL;
			size_t stIconSize;

//g_pPlutoLogger->Write(LV_STATUS, "Checking picture attri");
			if( PKID_MED_Picture )
			{
				PictureFile = "/home/mediapics/" + StringUtils::itos(PKID_MED_Picture) + "_tn.png";
//g_pPlutoLogger->Write(LV_STATUS, "Loking for file: %s", PictureFile.c_str());
#ifdef WIN32
				string::size_type s;
				while( (s=PictureFile.find('/'))!=string::npos )
					PictureFile.replace(s,1,"\\");
#endif
				pIconBuffer = FileUtils::ReadFileIntoBuffer(PictureFile,stIconSize);
				format = GR_JPG;
			}

			if( !pIconBuffer && !flInfo->m_bIsDirectory)
			{
				g_pPlutoLogger->Write(LV_STATUS, "Looking for file on disc");
				PictureFile = flInfo->m_sPath;
				string::size_type tmpPos=-1;
				string::size_type posExtension=string::npos;

				while( (tmpPos=PictureFile.find('.',tmpPos+1))!=string::npos && tmpPos<PictureFile.length()-1 )
				{
					posExtension=tmpPos;
				}
//g_pPlutoLogger->Write(LV_STATUS, "Got extension position for file name \"%s\" to be \"%d\"", PictureFile.c_str(), posExtension);

				if( posExtension!=string::npos )
				{
					PictureFile = PictureFile.substr(0,posExtension) + "_tn.png";
//g_pPlutoLogger->Write(LV_STATUS, "Loking for file: %s", PictureFile.c_str());
					pIconBuffer = FileUtils::ReadFileIntoBuffer(PictureFile,stIconSize);
					if( !pIconBuffer )
					{
						PictureFile = PictureFile.substr(0,posExtension) + ".png";
//g_pPlutoLogger->Write(LV_STATUS, "Loking for file: %s", PictureFile.c_str());
						pIconBuffer = FileUtils::ReadFileIntoBuffer(PictureFile,stIconSize);
					}
					format = GR_JPG;
				}
			}

			if( !pIconBuffer ) // Still no luck.  Use a default picture
			{

				if( pCell && pCell->m_pMessage )
				{
					pIconBuffer = FileUtils::ReadFileIntoBuffer("/home/media/miscicons/folder.png",stIconSize);
g_pPlutoLogger->Write(LV_STATUS, "Loking for default file: %s", PictureFile.c_str());
					format = GR_PNG;
				}
				else if( m_sIconFile.length()>0 )
				{
					pIconBuffer = FileUtils::ReadFileIntoBuffer(m_sIconFile,stIconSize);
g_pPlutoLogger->Write(LV_STATUS, "Looking for default file: %s", PictureFile.c_str());
					format = GR_PNG;
				}
			}

			if( pIconBuffer )
			{
				if( !pCell )
					pCell = new DataGridCell("","");
				pCell->SetImage(pIconBuffer,(int) stIconSize,format);
				SetData(0,row,pCell);
			}
else
{
	g_pPlutoLogger->Write(LV_STATUS, "Could not find image data for this entry");
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
