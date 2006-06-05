#ifndef MediaListGrid_H
#define MediaListGrid_H

#include <mysql.h>

#include "DataGrid.h"
namespace DCE
{
	class Media_Plugin;
}

using namespace DCE;

class FileListInfo
{
public:
	FileListInfo(bool bIsDirectory,string sPath,bool bIsBack) : m_PKID_MED_Attribute(0), m_bIsDirectory(bIsDirectory), m_bIsBack(bIsBack), m_sPath(sPath) {}
	FileListInfo(int PKID_MED_Attribute) : m_PKID_MED_Attribute(PKID_MED_Attribute) {}
	int m_PKID_MED_Attribute;
	bool m_bIsDirectory,m_bIsBack;  // Keep track of whether this is a 'go back' or not
	string m_sPath;
};

class MediaListGrid : public DataGridTable
{
	MediaListGrid *m_pMediaListGrid_Master;
public:
	MediaListGrid(class Datagrid_Plugin *pDatagrid_Plugin,class Media_Plugin *pMedia_Plugin) : m_pDatagrid_Plugin(pDatagrid_Plugin), m_pMedia_Plugin(pMedia_Plugin), m_pMediaListGrid_Master(NULL) {}
	MediaListGrid(class Datagrid_Plugin *pDatagrid_Plugin,class Media_Plugin *pMedia_Plugin,MediaListGrid *pMediaListGrid) : m_pDatagrid_Plugin(pDatagrid_Plugin), m_pMedia_Plugin(pMedia_Plugin), m_pMediaListGrid_Master(pMediaListGrid) {}
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Media_Plugin *m_pMedia_Plugin;
	string m_sIconFile;
	int m_iType;
	void ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount);
	vector<FileListInfo *> m_vectFileInfo;
};

#endif

