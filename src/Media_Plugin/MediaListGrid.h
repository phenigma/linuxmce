#ifndef MediaListGrid_H
#define MediaListGrid_H

#include <mysql.h>

#include "DataGrid.h"
namespace DCE
{
	class Media_Plugin;
}

using namespace DCE;

class FileBrowserInfo
{
public:
	typedef enum {
		fbi_File,
		fbi_InternalDisc,
		fbi_Jukebox,
	} enumTypeOfMedia;

	bool m_bIsDirectory,m_bIsBack;  // Keep track of whether this is a 'go back' or not
	string m_sDisplayGroup,m_sDisplayName;
	string m_sMRL;
	int m_PK_File,m_PK_Disc,m_PK_Picture;
	enumTypeOfMedia m_enumTypeOfMedia;

	// Constructor for file info
	FileBrowserInfo(string sDisplayName, string sPath,int PK_File,bool bIsDirectory,bool bIsBack) 
	{
		m_sDisplayName=sDisplayName;
		m_enumTypeOfMedia=fbi_File;
		m_PK_Disc=0;
		m_PK_Picture=0;
		m_PK_File=PK_File;
		m_bIsDirectory=bIsDirectory;
		m_bIsBack=bIsBack;
		m_sMRL=sPath;
	}
};

typedef FileBrowserInfo * FileBrowserInfoPtr;

static bool FileBrowserInfoComparer(FileBrowserInfo *x, FileBrowserInfo *y)
{
	if( x->m_bIsBack && !y->m_bIsBack )
		return true;

	if( !x->m_bIsBack && y->m_bIsBack )
		return false;

	if( x->m_bIsDirectory && !y->m_bIsDirectory )
		return true;

	if( !x->m_bIsDirectory && y->m_bIsDirectory )
		return false;

	return x->m_sDisplayGroup+x->m_sDisplayName < y->m_sDisplayGroup+y->m_sDisplayName;
}

class DatabaseInfoOnPath
{
public:
	int m_PK_File,m_PK_Picture;
	bool m_bDirectory;

	DatabaseInfoOnPath(int PK_File,bool bDirectory,int PK_Picture)
	{
		m_PK_File=PK_File;
		m_PK_Picture=PK_Picture;
		m_bDirectory=bDirectory;
	}
};

class MediaListGrid : public DataGridTable
{
public:
	FileBrowserInfoPtr *m_pFileBrowserInfoPtr;  // This will be the same as m_listFileBrowserInfo, but in an array for faster retrieval by row number
	MediaListGrid *m_pMediaListGrid_Master;

	MediaListGrid(class Datagrid_Plugin *pDatagrid_Plugin,class Media_Plugin *pMedia_Plugin) : m_pDatagrid_Plugin(pDatagrid_Plugin), m_pMedia_Plugin(pMedia_Plugin) { m_pFileBrowserInfoPtr=NULL; m_pMediaListGrid_Master=NULL; }
	MediaListGrid(class Datagrid_Plugin *pDatagrid_Plugin,class Media_Plugin *pMedia_Plugin,MediaListGrid *pMediaListGrid) : m_pDatagrid_Plugin(pDatagrid_Plugin), m_pMedia_Plugin(pMedia_Plugin), m_pMediaListGrid_Master(pMediaListGrid) { m_pFileBrowserInfoPtr=NULL; }
	~MediaListGrid()
	{
		delete m_pFileBrowserInfoPtr;
		for(list<FileBrowserInfo *>::iterator it=m_listFileBrowserInfo.begin();it!=m_listFileBrowserInfo.end();++it)
			delete *it;
	}
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Media_Plugin *m_pMedia_Plugin;
	string m_sIconFile;
	int m_iType;
	void ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount);
	list<FileBrowserInfo *> m_listFileBrowserInfo;
};

#endif

