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

void MythTvEPGWrapper::setGridBoundaries(QDateTime startTime, QDateTime endTime)
{
    m_timeStart = startTime;
    m_timeEnd = endTime;

    m_iQuantInSecs = 5 * 60; // 15 minutes;
    m_iColumns = startTime.secsTo(endTime) / m_iQuantInSecs;

    readChannelData();

    g_pPlutoLogger->Write(LV_STATUS, "Total grid size %dx%d", GetRows(), GetCols());
//     getProgramData();
}

int MythTvEPGWrapper::GetRows()
{
    g_pPlutoLogger->Write(LV_STATUS, "Returning %d as the total number of rows!", m_vectChannelIds.size());
    return m_vectChannelIds.size();
}

int MythTvEPGWrapper::GetCols()
{
    g_pPlutoLogger->Write(LV_STATUS, "Returning %d as the total number of columns!", m_iColumns);
    return m_iColumns;
}

void MythTvEPGWrapper::readChannelData()
{
    QString strQuery, strTimeFormat;

    QString startDataAsString, endDateAsString;

    strTimeFormat = "yyyy-MM-dd hh:mm:ss";


    startDataAsString = m_timeStart.toString(strTimeFormat);
    endDateAsString = m_timeEnd.toString(strTimeFormat);

    strQuery.sprintf("SELECT p.chanid"
                     " FROM program p"
                     " WHERE p.starttime BETWEEN '%s' and '%s' OR "
                           "p.endtime BETWEEN '%s' and '%s' OR "
                           "(p.starttime <= '%s' AND p.endtime >= '%s')"
                     " GROUP BY p.chanid",
                     startDataAsString.ascii(),
                     endDateAsString.ascii(),
                     startDataAsString.ascii(),
                     endDateAsString.ascii(),
                     startDataAsString.ascii(),
                     endDateAsString.ascii());

    QSqlQuery query;
    query.exec(strQuery);

    if ( ! query.isActive() )
    {
        g_pPlutoLogger->Write(LV_WARNING,
            "The query %s for the channel count failed: %s!",
            strQuery.ascii(),
            query.lastError().text().ascii());

        return;
    }

    if ( query.numRowsAffected() > 0 )
    {
        g_pPlutoLogger->Write(LV_STATUS, "We have %d channels with programs in this time period!", query.numRowsAffected());

//         m_iChannels = query.numRowsAffected();

//         int currentChannelId = 0, nextChannelId;

        m_vectChannelIds.clear();
        while ( query.next() )
            m_vectChannelIds.push_back(query.value(0).toInt());
/*            nextChannelId = query.value(0).toInt();
            if ( currentChannelId != nextChannelId )
            {
                cleanChannelStorage(nextChannelId);
                currentChannelId = nextChannelId;
                g_pPlutoLogger->Write(LV_STATUS, "New channel %d", currentChannelId);
            }

            readProgramInfo(
                    currentChannelId,
                    query.value(1).toString(),
                    query.value(2).toDateTime(),
                    query.value(3).toDateTime());
        }*/
    }
    else
    {
        g_pPlutoLogger->Write(LV_STATUS, "There are no programs in this timeframe");
    }
}

void MythTvEPGWrapper::readProgramInfo(int channelId, QString programName, QDateTime startTime, QDateTime endTime)
{
//     g_pPlutoLogger->Write(LV_STATUS, "Program: %s, %s, %s",
//                     programName.ascii(),
//                     startTime.toString().ascii(),
//                     endTime.toString().ascii());

    int nColumnSpan = startTime.secsTo(endTime) / m_iQuantInSecs;
    int nColumnStart = m_timeStart.secsTo(startTime) / m_iQuantInSecs;

    DCE::DataGridCell *pDataGridCell = new DCE::DataGridCell(programName.ascii());
    pDataGridCell->m_Colspan = nColumnSpan;

    SetData(nColumnStart, channelId, pDataGridCell);

    g_pPlutoLogger->Write(LV_STATUS, "Cell (%s) Span: %d Start at %d. Time: (%s <--> %s)",
                programName.ascii(),
                nColumnSpan, nColumnStart,
                startTime.toString().ascii(),
                endTime.toString().ascii());
}

void MythTvEPGWrapper::cleanChannelStorage(int channel)
{
/*    if ( m_mapPrograms.find(channel) != m_mapPrograms.end() )
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
        g_pPlutoLogger->Write(LV_STATUS, "Making a new inner map for channel %d", channel);
        m_mapPrograms[channel] = map<int, DataGridCell*>();
    }*/
}

QString MythTvEPGWrapper::getChannelList(int rowStart, int rowCount)
{
    if ( rowStart + rowCount >= m_vectChannelIds.size())
        g_pPlutoLogger->Write(LV_WARNING, "You were requesting more that i can serve you.");
}

void MythTvEPGWrapper::readDataGridBlock(int rowStart, int rowCount, int colStart, int colCount)
{
    QString strQuery, strTimeFormat;

    QString startDataAsString, endDateAsString;

    strTimeFormat = "yyyy-MM-dd hh:mm:ss";

    startDataAsString = m_timeStart.toString(strTimeFormat);
    endDateAsString = m_timeEnd.toString(strTimeFormat);


    strQuery.sprintf("SELECT p.chanid, p.title, p.starttime, p.endtime"
                     " FROM program p"
//                      " WHERE p.chanid IN (%s) "
                           "p.starttime BETWEEN '%s' and '%s' OR "
                           "p.endtime BETWEEN '%s' and '%s' OR "
                           "(p.starttime <= '%s' AND p.endtime >= '%s')"
                     " ORDER BY p.chanid",
//                      getChannelList(rowStart, rowCount).ascii(),
                     startDataAsString.ascii(),
                     endDateAsString.ascii(),
                     startDataAsString.ascii(),
                     endDateAsString.ascii(),
                     startDataAsString.ascii(),
                     endDateAsString.ascii());

    QSqlQuery query;
    query.exec(strQuery);

    if ( ! query.isActive() )
    {
        g_pPlutoLogger->Write(LV_WARNING,
            "The query %s for the channel count failed: %s!",
            strQuery.ascii(),
            query.lastError().text().ascii());

        return;
    }

    if ( query.numRowsAffected() > 0 )
    {
        g_pPlutoLogger->Write(LV_STATUS, "We have %d channels with programs in this time period!", query.numRowsAffected());

//         m_iChannels = query.numRowsAffected();

//         int currentChannelId = 0, nextChannelId;

//         m_vectChannelIds.clear();
        int i = 0;
        while ( query.next() )
        {
        }
//         {
//             m_mapRowChannelId[i++] = query.value(0).toInt();
//      }
/*            nextChannelId = query.value(0).toInt();
            if ( currentChannelId != nextChannelId )
            {
                cleanChannelStorage(nextChannelId);
                currentChannelId = nextChannelId;
                g_pPlutoLogger->Write(LV_STATUS, "New channel %d", currentChannelId);
            }

            readProgramInfo(
                    currentChannelId,
                    query.value(1).toString(),
                    query.value(2).toDateTime(),
                    query.value(3).toDateTime());
        }*/
    }
    else
    {
        g_pPlutoLogger->Write(LV_STATUS, "There are no programs in this timeframe");
    }
}

void MythTvEPGWrapper::ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount)
{
    g_pPlutoLogger->Write(LV_STATUS, "Current size: %dx%d", GetRows(), GetCols());
    g_pPlutoLogger->Write(LV_STATUS, "Need to read a block of datagrid: [%d,%d] [%dx%d]", ColStart, RowStart, ColCount, RowCount);

    readDataGridBlock(RowStart, RowCount, ColStart, ColCount);
    DataGridTable::ToData(GridID, Size, Data, ColStart, RowStart, ColCount, RowCount);
}

DataGridCell *MythTvEPGWrapper::GetData(int iColumn, int iRow, int *piSize)
{
//     g_pPlutoLogger->Write(LV_STATUS, "Data was requested for cell: %d,%d", iColumn, iRow);
/*    if ( m_mapPrograms.find(iRow) == m_mapPrograms.end())
        m_mapPrograms[iRow] = map<int, DataGridCell*>();

    if ( m_mapPrograms[iRow].find(iColumn) == m_mapPrograms[iRow].end() )
        m_mapPrograms[iRow][iColumn] = new DCE::DataGridCell(
                    string("Simple cell"),
                    string("Cell value"));

    return m_mapPrograms[iRow][iColumn];*/
    return NULL;
}
