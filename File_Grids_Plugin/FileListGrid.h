#ifndef FILELISTGRID_H
#define FILELISTGRID_H

#include <mysql.h>

#include "DataGrid.h"

#include <vector>
using namespace std;
using namespace DCE;

class FileListInfo
{
public:
	int m_PK_MED_Attribute;
	bool m_bIsDirectory;
	string m_sPath;

	FileListInfo(bool bIsDirectory,string sPath) : m_bIsDirectory(bIsDirectory), m_sPath(sPath), m_PK_MED_Attribute(0) {}
	FileListInfo(int PK_MED_Attribute) : m_PK_MED_Attribute(PK_MED_Attribute) {}
};

class FileListGrid : public DataGridTable
{
public:
	class DCEMI_PS_VirtualDevices *m_pDCEMI_PS_VirtualDevices;
	string m_sIconFile;
	int m_iType;
	vector<FileListInfo *> m_vectFileInfo;

	FileListGrid(class DCEMI_PS_VirtualDevices *pDCEMI_PS_VirtualDevices) : m_pDCEMI_PS_VirtualDevices(pDCEMI_PS_VirtualDevices) {}
	void ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount);
};

#endif

