#include "MythTvEPGWrapper.h"
#include "DCE/Logger.h"

#include <qapplication.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlresult.h>

#include <mythtv/mythcontext.h>
#include <mythtv/mythdialogs.h>

using namespace DCE;

MythContext *gContext;

MythTvEPGWrapper::MythTvEPGWrapper()
{
    int argc = 1;
    char *argv[] = { "", "", "" };

    g_pPlutoLogger->Write(LV_STATUS, "Passing params: %d", argc);

    m_pQApplication = new QApplication(argc, argv);

    if ( ! initMythTVGlobalContext() )
    {
        delete m_pQApplication;
        throw "Could not init Myth Global Context";
    }
}

bool MythTvEPGWrapper::initMythTVGlobalContext()
{
    gContext = new MythContext(MYTH_BINARY_VERSION);
    gContext->LoadQtConfig();

    QSqlDatabase *db = QSqlDatabase::addDatabase("QMYSQL3");
    if (!db)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Could not connect to mysql database");
        return false;
    }

    if (!gContext->OpenDatabase(db))
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Could not open mysql database");
        return false;
    }

    return true;
}

void MythTvEPGWrapper::rebuildDataGrid(QDateTime startTime, QDateTime endTime)
{
    m_timeStart = startTime;
    m_timeEnd = endTime;

    m_iQuantInSecs = 15 * 60; // 15 minutes;
    getProgramData();
}

void MythTvEPGWrapper::getProgramData()
{
    QString strQuery;

    strQuery.sprintf("SELECT p.chanid FROM program p "
                        "WHERE p.starttime BETWEEN '%s' and '%s' OR "
                              "p.endtime BETWEEN '%s' and '%s'"
                        "GROUP BY p.chanid",
                    "", "", "", "");

    QSqlQuery query;
    query.exec(strQuery);

    if ( query.isActive() && query.numRowsAffected() > 0 )
    {
        g_pPlutoLogger->Write(LV_STATUS, "The query for channel count was a success : %d!", query.numRowsAffected());

        m_iChannels = query.numRowsAffected();

        int iChannelId;
        while ( query.next() )
        {
            iChannelId = query.value(0).toInt();
            getChannelData(iChannelId);
//          m_mapPrograms[iChannelId] = getChannelData(iChannelId);
        }
    }

    g_pPlutoLogger->Write(LV_WARNING, "The query for the channel count failed: %s!", query.lastError().text().ascii());
}

void MythTvEPGWrapper::getChannelData(int channel)
{
    if ( m_mapPrograms.find(channel) != m_mapPrograms.end() )
    {
        map<int, DataGridCell*>::iterator it_data = m_mapPrograms[channel].begin();

        while (it_data != m_mapPrograms[channel].end() )
        {
            delete it_data->second;
            it_data++;
        }

        m_mapPrograms[channel].clear();
    }
    else
    {
        m_mapPrograms[channel] = map<int, DataGridCell*>();
    }

    map<int, DataGridCell*> m_mapChannelData = m_mapPrograms[channel];

    g_pPlutoLogger->Write(LV_STATUS, "I should read data from the database!");
}

int MythTvEPGWrapper::GetRows()
{
    return m_iChannels;
}

int MythTvEPGWrapper::GetCols()
{
    return m_timeStart.secsTo(m_timeEnd) / m_iQuantInSecs;
}

DCE::DataGridCell *MythTvEPGWrapper::GetData(int iColumn, int iRow, int *piSize)
{
    if ( m_mapPrograms.find(iRow) == m_mapPrograms.end())
        m_mapPrograms[iRow] = map<int, DataGridCell*>();

    if ( m_mapPrograms[iRow].find(iColumn) == m_mapPrograms[iRow].end() )
        m_mapPrograms[iRow][iColumn] = new DCE::DataGridCell(
                    string("Simple cell"),
                    string("Cell value"));

    return m_mapPrograms[iRow][iColumn];
}
