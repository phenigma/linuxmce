#include "MythTvWrapper.h"

#include "DCE/Logger.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include <qapplication.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlresult.h>

#include <libmyth/mythcontext.h>
#include <libmyth/mythdialogs.h>
#include <libmythtv/programinfo.h>

#include <libmythtv/remoteutil.h>

using namespace DCE;

MythContext *gContext;


MythTvWrapper::MythTvWrapper(Command_Impl *pCommandImpl)
    :m_pDCEDeviceWrapper(pCommandImpl)
{
    int argc = 1;
    char *argv[] = { "", "", "" };

    g_pPlutoLogger->Write(LV_STATUS, "Passing params: %d", argc);

    m_pQApplication = new QApplication(argc, argv, false); // we don't need to display anything on X.

    if ( ! initMythTVGlobalContext() )
    {
        delete m_pQApplication;
        throw "Could not init Myth Global Context";
    }
}

bool MythTvWrapper::initMythTVGlobalContext()
{
    gContext = new MythContext(MYTH_BINARY_VERSION, false); // we don't need X in the plugin

    QSqlDatabase *db = QSqlDatabase::addDatabase("QMYSQL3");
    if (!db)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Could not connect to mysql database");
        return false;
    }

    g_pPlutoLogger->Write(LV_STATUS, "database object was created");
    if (!gContext->OpenDatabase(db))
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Could not open mysql database");
        return false;
    }

    return true;
}

MythTvWrapper::MythTvEpgGrid::MythTvEpgGrid()
{

}

void MythTvWrapper::MythTvEpgGrid::setGridBoundaries(QDateTime startTime, QDateTime endTime)
{
    g_pPlutoLogger->Write(LV_STATUS, "Setting grid boundaries to: [%s -> %s]",
                startTime.toString().ascii(),
                endTime.toString().ascii());

    m_timeFrom = startTime;
    m_timeTo   = endTime;

    m_iQuantInSecs = 5 * 60; // 15 minutes;
    m_iColumns = m_timeFrom.secsTo(m_timeTo) / m_iQuantInSecs;

    readChannelData();

    g_pPlutoLogger->Write(LV_STATUS, "Total grid size %dx%d", GetRows(), GetCols());
}

int MythTvWrapper::MythTvEpgGrid::GetRows()
{
    return m_vectChannelIds.size();
}

int MythTvWrapper::MythTvEpgGrid::GetCols()
{
    return m_iColumns;
}

void MythTvWrapper::MythTvEpgGrid::readChannelData()
{
    QString strQuery, strTimeFormat;

    QString startDataAsString, endDateAsString;

    strTimeFormat = "yyyy-MM-dd hh:mm:ss";

    startDataAsString = m_timeFrom.toString(strTimeFormat);
    endDateAsString   = m_timeTo.toString(strTimeFormat);

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

void MythTvWrapper::MythTvEpgGrid::readProgramInfo(int channelId, QString programName, QDateTime startTime, QDateTime endTime)
{
    int nColumnSpan = startTime.secsTo(endTime) / m_iQuantInSecs;
    int nColumnStart = m_timeFrom.secsTo(startTime) / m_iQuantInSecs;

    DCE::DataGridCell *pDataGridCell = new DCE::DataGridCell(programName.ascii());
    pDataGridCell->m_Colspan = nColumnSpan;

    SetData(nColumnStart, channelId, pDataGridCell);

    g_pPlutoLogger->Write(LV_STATUS, "Cell (%s) Span: %d Start at %d. Time: (%s <--> %s)",
                programName.ascii(),
                nColumnSpan, nColumnStart,
                startTime.toString().ascii(),
                endTime.toString().ascii());
}

void MythTvWrapper::MythTvEpgGrid::cleanChannelStorage(int channel)
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

QString MythTvWrapper::MythTvEpgGrid::getChannelList(int rowStart, int rowCount)
{
    QStringList list;

    while ( rowStart < m_vectChannelIds.size() && rowCount >= 0 )
    {
        list.append(QString::number(m_vectChannelIds[rowStart++]));
        rowCount--;
    }

    return list.join(", ");
}

void MythTvWrapper::MythTvEpgGrid::readDataGridBlock(int rowStart, int rowCount, int colStart, int colCount)
{
    QString strQuery, strTimeFormat;

    QString startDataAsString, endDateAsString;

    strTimeFormat = "yyyy-MM-dd hh:mm:ss";

    QDateTime periodStart = m_timeFrom.addSecs(colStart * m_iQuantInSecs);
    QDateTime periodEnd   = periodStart.addSecs(colCount * m_iQuantInSecs);

    startDataAsString = periodStart.toString(strTimeFormat);
    endDateAsString   = periodEnd.toString(strTimeFormat);


    strQuery.sprintf("SELECT p.chanid, p.title, p.starttime, p.endtime"
                     " FROM program p"
                     " WHERE p.chanid IN (%s) AND "
                           "("
                            "(p.starttime BETWEEN '%s' and '%s') OR "
                            "(p.endtime BETWEEN '%s' and '%s') OR "
                            "(p.starttime < '%s' AND p.endtime > '%s')"
                           ")"
                     " ORDER BY p.chanid, p.starttime",
                     getChannelList(rowStart, m_bKeepRowHeader ? rowCount - 1 : rowCount).ascii(),
                     periodStart.toString(strTimeFormat).ascii(),
                     periodEnd.addSecs(-1).toString(strTimeFormat).ascii(),
                     periodStart.addSecs(1).toString(strTimeFormat).ascii(),
                     periodEnd.toString(strTimeFormat).ascii(),
                     periodStart.toString(strTimeFormat).ascii(),
                     periodEnd.toString(strTimeFormat).ascii());

    QSqlQuery query;
    query.exec(strQuery);

//     g_pPlutoLogger->Write(LV_STATUS, "Query: %s", strQuery.ascii());
    if ( ! query.isActive() )
    {
        g_pPlutoLogger->Write(LV_WARNING,
            "The query %s for the channel count failed: %s!",
            strQuery.ascii(),
            query.lastError().text().ascii());

        return;
    }

    this->ClearData();

    if ( m_bKeepRowHeader )
        MakeTimeRow(colStart, colCount);

//     g_pPlutoLogger->Write(LV_STATUS, "Quant %d --> Period: %s <--> %s", m_iQuantInSecs, startDataAsString.ascii(), endDateAsString.ascii());
    if ( query.numRowsAffected() > 0 )
    {
        int currentChannelId = 0, nextChannelId = 0;
        int channelPos = (m_bKeepRowHeader) ? 0 : -1;
        int programColumnSpan = 0;
        int programStartColumn = 0;

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

            /*
            g_pPlutoLogger->Write(LV_STATUS, "Program %d->(%d+%d) %d: %s<-->%s (%s)",
                        channelPos,
                        programStartColumn, programColumnSpan,
                        currentChannelId,
                        dateStart.toString().ascii(),
                        dateEnd.toString().ascii(),
                        programName.ascii());
            */

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
//             g_pPlutoLogger->Write(LV_STATUS, "Setting cell to position: %d, %d", programStartColumn + colStart, channelPos);
            SetData(programStartColumn  + colStart, channelPos + rowStart, pCell);
        }
    }
    else
    {
        g_pPlutoLogger->Write(LV_WARNING, "There are no programs in this timeframe");
    }
}

void MythTvWrapper::MythTvEpgGrid::MakeChannelRow(int RowStart, int RowCount)
{
    while ( RowCount != 0 )
    {
        SetData(0, RowStart, new DataGridCell(QString::number(RowStart).ascii(), "") );
        RowStart++;
        RowCount--;
    }
}

void MythTvWrapper::MythTvEpgGrid::MakeTimeRow(int ColStart, int ColCount)
{
    QString timeCell;

    QDateTime currentTime = m_timeFrom.addSecs(m_iQuantInSecs * ColStart);

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
    }
    return;
}

void MythTvWrapper::MythTvEpgGrid::ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount)
{
    readDataGridBlock(RowStart, RowCount, ColStart, ColCount);

    // This is required because the ToData doesn't honor the Get{Cols,Rows} calls.
    m_TotalColumns = GetCols();
    m_TotalRows = GetRows();

    DataGridTable::ToData(GridID, Size, Data, ColStart, RowStart, ColCount, RowCount);
}

void MythTvWrapper::MythTvEpgGrid::setMythWrapper(MythTvWrapper *pWrapper)
{
    m_pMythWrapper = pWrapper;
}

DataGridTable *MythTvWrapper::createShowsDataGrid(string GridID, QDateTime startTime, QDateTime endTime)
{
    MythTvEpgGrid *pNewGrid = new MythTvEpgGrid();

    pNewGrid->setMythWrapper(this);
    pNewGrid->setGridBoundaries(startTime, endTime);

    return pNewGrid;
}

bool MythTvWrapper::decodeProgramStartTime(string sValue, int &nChanId, int &tmYear, int &tmMonth, int &tmDay, int &tmHour, int &tmMinute)
{
    vector<string> numbers;
    StringUtils::Tokenize(sValue, "|", numbers);

    if ( numbers.size() != 6 )
    {
        g_pPlutoLogger->Write(LV_STATUS, "We can't decode this: %s", sValue.c_str());
        return false;
    }

    nChanId  = atoi(numbers[0].c_str());
    tmYear   = atoi(numbers[1].c_str());
    tmMonth  = atoi(numbers[2].c_str());
    tmDay    = atoi(numbers[3].c_str());
    tmHour   = atoi(numbers[4].c_str());
    tmMinute = atoi(numbers[5].c_str());

    return true;
}
WatchTVRequestResult MythTvWrapper::ProcessWatchTvRequest(string showStartTimeEncoded)
{
    int iChanId;
    int tmYear, tmMonth, tmDay, tmHour, tmMinute;

    if ( ! decodeProgramStartTime(showStartTimeEncoded,
                                  iChanId,
                                  tmYear, tmMonth, tmDay,
                                  tmHour, tmMinute))
        return WatchTVResult_Ignored;

    WatchTVRequestResult result;
    if ( (result = ProcessWatchTvRequest(iChanId, tmYear, tmMonth, tmDay, tmHour, tmMinute)) == WatchTVResult_Tuned )
    {
        DCE::CMD_Tune_to_channel tuneToChannel(
                m_pDCEDeviceWrapper->m_dwPK_Device,
                37,
                showStartTimeEncoded);

        m_pDCEDeviceWrapper->SendCommand(tuneToChannel);
    }

    return result;
}

WatchTVRequestResult MythTvWrapper::ProcessWatchTvRequest(int iChanId, int nYear, int nMonth, int nDay, int nHour, int nMinute)
{
    QDateTime programStartTime = QDateTime(QDate(nYear, nMonth, nDay), QTime(nHour, nMinute));

    ProgramInfo *programInfo = ProgramInfo::GetProgramAtDateTime(
                                                QSqlDatabase::database(),
                                                QString::number(iChanId),
                                                programStartTime);

    if ( QDateTime::currentDateTime() > programStartTime )
        return WatchTVResult_Tuned;
    else
        return WatchTVResult_InTheFuture;
}

ScheduleRecordTvResult MythTvWrapper::ProcessAddRecordingRequest(string showStartTimeEncoded)
{
    int iChanId;
    int tmYear, tmMonth, tmDay, tmHour, tmMinute;

    if ( ! decodeProgramStartTime(showStartTimeEncoded,
                                  iChanId,
                                  tmYear, tmMonth, tmDay,
                                  tmHour, tmMinute))
        return ScheduleRecordTVResult_Failed;

    return ProcessAddRecordingRequest(iChanId, tmYear, tmMonth, tmDay, tmHour, tmMinute);
}

ScheduleRecordTvResult MythTvWrapper::ProcessAddRecordingRequest(int channelId, int year, int month, int day, int hour, int minute)
{
    QDateTime programStartTime = QDateTime(QDate(year, month, day), QTime(hour, minute));

    ProgramInfo *programInfo;

    programInfo = ProgramInfo::GetProgramAtDateTime(
                                        QSqlDatabase::database(),
                                        QString::number(channelId),
                                        programStartTime);

    programInfo->ApplyRecordStateChange(QSqlDatabase::database(), kSingleRecord);

    vector<ProgramInfo *> *conflictsWith = RemoteGetConflictList(programInfo);

    if ( conflictsWith == NULL )
    {
//         m_pDCEDeviceWrapper->EVENT_Error_Occured("The connection to master server was broken!");
        return ScheduleRecordTVResult_Failed;
    }

    if ( conflictsWith->size() != 0 )
    {
//         m_pDCEDeviceWrapper->EVENT_PVR_Rec_Sched_Conflict();

        vector<ProgramInfo*>::iterator itPrograms;

        for ( itPrograms = conflictsWith->begin(); itPrograms != conflictsWith->end(); itPrograms++ )
            delete *itPrograms;

        delete conflictsWith;
        return ScheduleRecordTVResult_WithConflicts;
    }

    delete conflictsWith;
    return ScheduleRecordTVResult_Success;
}
