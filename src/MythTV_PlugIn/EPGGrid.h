#ifndef EPGGRID_H
#define EPGGRID_H

#include <mysql.h>

#include "DataGrid.h"

using namespace DCE;


class EPGGrid : public DataGridTable
{
	class MySqlHelper *m_pMySqlHelper_Myth;

public:
	EPGGrid(class MySqlHelper *pMySqlHelper_Myth);
	virtual ~EPGGrid();

	string m_sIconFile;
	int m_iType;
	void ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount);
};

#endif
