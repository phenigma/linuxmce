#ifndef FILELISTGRID_H
#define FILELISTGRID_H

#include <mysql.h>

#include "DataGrid.h"

class FileListInfo
{
public:
	FileListInfo(bool bIsDirectory,string sPath) : m_bIsDirectory(bIsDirectory), m_sPath(sPath), m_PK_MED_Attribute(0) {}
	FileListInfo(int PK_MED_Attribute) : m_PK_MED_Attribute(PK_MED_Attribute) {}
	int m_PK_MED_Attribute;
	bool m_bIsDirectory;
	string m_sPath;
};

class FileListGrid : public DataGridTable
{
public:
	FileListGrid(class DCEMI_PS_VirtualDevices *pDCEMI_PS_VirtualDevices) : m_pDCEMI_PS_VirtualDevices(pDCEMI_PS_VirtualDevices) {}
	class DCEMI_PS_VirtualDevices *m_pDCEMI_PS_VirtualDevices;
	string m_sIconFile;
	int m_iType;
	void ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount);
	vector<FileListInfo *> m_vectFileInfo;
};

#endif

