#ifndef FILELISTGRID_H
#define FILELISTGRID_H

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
	FileListInfo(bool bIsDirectory,string sPath) : m_bIsDirectory(bIsDirectory), m_sPath(sPath), m_PKID_MED_Attribute(0) {}
	FileListInfo(int PKID_MED_Attribute) : m_PKID_MED_Attribute(PKID_MED_Attribute) {}
	int m_PKID_MED_Attribute;
	bool m_bIsDirectory;
	string m_sPath;
};

class FileListGrid : public DataGridTable
{
public:
	FileListGrid(class Datagrid_Plugin *pDatagrid_Plugin,class Media_Plugin *pMedia_Plugin) : m_pDatagrid_Plugin(pDatagrid_Plugin), m_pMedia_Plugin(pMedia_Plugin) {}
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Media_Plugin *m_pMedia_Plugin;
	string m_sIconFile;
	int m_iType;
	void ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount);
	vector<FileListInfo *> m_vectFileInfo;
};

#endif

