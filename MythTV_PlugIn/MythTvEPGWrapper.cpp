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
    g_pPlutoLogger->Write(LV_STATUS,
            "Setting grid from %s to %s",
                startTime.toString().ascii(),
                endTime.toString().ascii());

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
    return m_vectChannelIds.size();
}

int MythTvEPGWrapper::GetCols()
{
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

        m_vectChannelIds.clear();
        while ( query.next() )
            m_vectChannelIds.push_back(query.value(0).toInt());
    }
    else
    {
        g_pPlutoLogger->Write(LV_STATUS, "There are no programs in this timeframe");
    }
}

void MythTvEPGWrapper::readProgramInfo(int channelId, QString programName, QDateTime startTime, QDateTime endTime)
{
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
    QStringList list;

    while ( rowStart < m_vectChannelIds.size() && rowCount >= 0 )
    {
        list.append(QString::number(m_vectChannelIds[rowStart++]));
        rowCount--;
    }

    return list.join(", ");
}

void MythTvEPGWrapper::readDataGridBlock(int rowStart, int rowCount, int colStart, int colCount)
{
    QString strQuery, strTimeFormat;

    QString startDataAsString, endDateAsString;

    strTimeFormat = "yyyy-MM-dd hh:mm:ss";

    QDateTime periodStart = m_timeStart.addSecs(colStart * m_iQuantInSecs);
    QDateTime periodEnd = periodStart.addSecs(colCount * m_iQuantInSecs);

    startDataAsString = periodStart.toString(strTimeFormat);
    endDateAsString = periodEnd.toString(strTimeFormat);

    strQuery.sprintf("SELECT p.chanid, p.title, p.starttime, p.endtime"
                     " FROM program p"
                     " WHERE p.chanid IN (%s) AND "
                           "p.starttime BETWEEN '%s' and '%s' OR "
                           "p.endtime BETWEEN '%s' and '%s' OR "
                           "(p.starttime <= '%s' AND p.endtime >= '%s')"
                     " ORDER BY p.chanid, p.starttime",
                     getChannelList(m_bKeepRowHeader ? rowStart + 1 : rowStart, m_bKeepRowHeader ? rowCount - 1 : rowCount).ascii(),
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

//     g_pPlutoLogger->Write(LV_STATUS, "Query %s", strQuery.ascii());

    ClearData();

    if ( m_bKeepRowHeader )
        MakeTimeRow(colStart, colCount);

    if ( query.numRowsAffected() > 0 )
    {
        int currentChannelId = 0, nextChannelId = 0;
        int channelPos = (m_bKeepRowHeader) ? 0 : -1;
        int programColumnSpan = 0;
        int programStartColumn = 0;

        m_gridBlock.clear();

        QString programName;
        QDateTime dateStart, dateEnd;
        while ( query.next() )
        {
            nextChannelId = query.value(0).toInt();
            programName = query.value(1).toString();
            dateStart = query.value(2).toDateTime();
            dateEnd = query.value(3).toDateTime();

            if ( currentChannelId != nextChannelId )
            {
                channelPos++;
                currentChannelId = nextChannelId;
            }

            programColumnSpan = dateStart.secsTo(dateEnd) / m_iQuantInSecs;
            programStartColumn = periodStart.secsTo(dateStart) / m_iQuantInSecs;

            if ( programStartColumn < 0 )
            {
                programColumnSpan += programStartColumn;
                programStartColumn = 0;
            }

            DataGridCell *pCell = new DataGridCell(
                    programName.ascii(),
                    QString::number(currentChannelId)
                        .append("|")
                        .append(QString::number(dateStart.date().year()))
                        .append("|")
                        .append(QString::number(dateStart.date().month()))
                        .append("|")
                        .append(QString::number(dateStart.date().day()))
                        .append("|")
                        .append(QString::number(dateStart.time().hour()))
                        .append("|")
                        .append(QString::number(dateStart.time().minute())).ascii());

            pCell->m_Colspan = programColumnSpan;
            SetData(programStartColumn  + colStart, channelPos, pCell);
        }
    }
    else
    {
        g_pPlutoLogger->Write(LV_WARNING, "There are no programs in this timeframe");
    }
}
void MythTvEPGWrapper::MakeTimeRow(int ColStart, int ColCount)
{
    QString timeCell;

    QDateTime currentTime = m_timeStart.addSecs(m_iQuantInSecs * ColStart);

    while ( ColCount != 0 && ColStart + ColCount < m_iColumns)
    {
        DataGridCell *pCell = new DataGridCell(currentTime.toString("MM/dd hh:mm"), "");
        pCell->m_Colspan = 4;
        SetData(ColStart, 0, pCell);
        currentTime = currentTime.addSecs(m_iQuantInSecs * 4);
        ColStart+=4;
        ColCount-=4;

        if ( ColCount < 0 )
            ColCount = 0;

        g_pPlutoLogger->Write(LV_STATUS, "added time cell: %s", pCell->m_Text);
    }
    return;
}

void MythTvEPGWrapper::ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount)
{
    readDataGridBlock(RowStart, RowCount, ColStart, ColCount);

    // This is required because the ToData doesn't honor the Get{Cols,Rows} calls.
    m_TotalColumns = GetCols();
    m_TotalRows = GetRows();

    DataGridTable::ToData(GridID, Size, Data, ColStart, RowStart, ColCount, RowCount);
}
