#ifndef MYTHTVEPGWRAPPER_H
#define MYTHTVEPGWRAPPER_H

#include "DataGrid.h"

#include <vector>

#include <qdatetime.h>
#include <qstring.h>

class QApplication;

using namespace std;
using namespace DCE;

class MythTvEPGWrapper: public DataGridTable
{
    QApplication *m_pQApplication;
    QDateTime m_timeStart, m_timeEnd;

    vector<int> m_vectChannelIds;
    int m_iQuantInSecs;
    int m_iColumns;

    map<int, map<int, pair<string, string> > > m_gridBlock;

protected:
    QString getChannelList(int rowStart, int rowCount);
    void readChannelData();
    void cleanChannelStorage(int channelId);
    void readProgramInfo(int channelId, QString programName, QDateTime startTime, QDateTime endTime);
    void readDataGridBlock(int rowStart, int rowCount, int colStart, int colCount);
    bool initMythTVGlobalContext();

public:
    MythTvEPGWrapper();

    virtual void setGridBoundaries(QDateTime startTime, QDateTime endTime);
    virtual int GetRows();  // The total number of rows in the whole grid
    virtual int GetCols();  // The total number of columns in the whole grid


    virtual void ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount);
};

#endif
