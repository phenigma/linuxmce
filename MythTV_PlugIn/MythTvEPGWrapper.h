#ifndef MYTHTVEPGWRAPPER_H
#define MYTHTVEPGWRAPPER_H

#include "DataGrid.h"

#include <map>

#include <qdatetime.h>

class QApplication;

using namespace std;

class MythTvEPGWrapper: public DCE::DataGridTable
{
    typedef std::map<int, std::map<int, DCE::DataGridCell*> > SparseMatrix;

    QApplication *m_pQApplication;
    QDateTime m_timeStart, m_timeEnd;

    SparseMatrix m_mapPrograms;
    int m_iChannels;
    int m_iQuantInSecs;

protected:
    void getProgramData();
    void getChannelData(int channelId);
    bool initMythTVGlobalContext();

public:
    MythTvEPGWrapper();

    virtual void rebuildDataGrid(QDateTime startTime, QDateTime endTime);
    virtual int GetRows();  // The total number of rows in the whole grid
    virtual int GetCols();  // The total number of columns in the whole grid
    virtual DCE::DataGridCell *GetData(int iColumn, int iRow, int *piSize = NULL);
};

#endif
