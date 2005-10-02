#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "EPGGrid.h"

#include <iostream>
using namespace std;
using namespace DCE;
using namespace VDREPG;
#include "EPG.h"
#include "VDRStateInfo.h"

int EpgGrid::GetRows()
{
	if( !m_pEPG )
		return 0;
	return m_pEPG->m_vectChannel.size();
}

int EpgGrid::GetCols()
{
	if( !m_pEPG )
		return 0;
	return (m_pEPG->m_tTime_Last - m_pEPG->m_tTime_First) / 60 / m_iGridResolution;
}

void EpgGrid::ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount)
{
// Mutex
	ClearData();
	if( !m_pEPG )
	{
		Size=0; Data=NULL;
		return;  // Nothing we can do
	}
	if( !(*ColStart) || (m_pVDRStateInfo && !m_pVDRStateInfo->m_mapOrbiter_HasInitialPosition[m_dwPK_Device_Orbiter]) )
		*ColStart = GetCurrentColumn();

	// Start with the the current time at the top of the hour
	time_t tNow = *ColStart * 60 * m_iGridResolution;
	struct tm *tmptr = localtime(&tNow);
	tmptr->tm_min=0;
	time_t tStartOfHour = mktime(tmptr);
	tNow = time(NULL);
	tmptr = localtime(&tNow);
	struct tm tmNow = *tmptr;

	// Put a new column heading ever 30 minutes
	for(int iColHeading = tStartOfHour / 60 / m_iGridResolution; iColHeading<*ColStart + ColCount;iColHeading += 30 / m_iGridResolution )
	{
		time_t tTime_Column = iColHeading * 60 * m_iGridResolution;
		tmptr = localtime(&tTime_Column);
		string sDesc;
		if( tmNow.tm_mon != tmptr->tm_mon || tmNow.tm_mday != tmptr->tm_mday )
			sDesc = StringUtils::itos(tmptr->tm_mon+1) + "/" + StringUtils::itos(tmptr->tm_mday) + " ";
		sDesc += StringUtils::itos(tmptr->tm_hour) + ":" + (tmptr->tm_min<10 ? "0" : "") +
			StringUtils::itos(tmptr->tm_min);
		DataGridCell *pCell = new DataGridCell(sDesc);
		pCell->m_Colspan = 30 / m_iGridResolution;
		SetData(iColHeading,0,pCell);
	}

	for(int iRow=*RowStart;iRow<*RowStart + RowCount;++iRow)
	{
		if( iRow>=0 && iRow<m_pEPG->m_vectChannel.size() )
		{
			Channel *pChannel = m_pEPG->m_vectChannel[iRow];
			// Use iRow+1 since the first row we added already for the header with the times
			PopulateRow(pChannel,iRow+1,*ColStart * 60 * m_iGridResolution,(*ColStart + ColCount) * 60 * m_iGridResolution );
			DataGridCell *pCell = new DataGridCell(pChannel->m_sChannelName);
			SetData(0,iRow+1,pCell);
		}
	}
	m_TotalRows=GetRows();
	m_TotalColumns=GetCols();

	return DataGridTable::ToData(GridID, Size, Data, ColStart, RowStart, ColCount, RowCount);
}

void EpgGrid::PopulateRow(Channel *pChannel,int iRow,int StartTime,int StopTime)
{
	Event *pEvent = pChannel->m_pEvent_First,*pEvent_Prior=NULL;
	while(pEvent && pEvent->m_tStartTime<StartTime)
	{
		pEvent_Prior = pEvent;
		pEvent = pEvent->m_pEvent_Next;
	}

	// The prior show is partially in this grid
	if( pEvent_Prior && pEvent_Prior->m_tStopTime > StartTime )
	{
		DataGridCell *pCell = new DataGridCell("<" + pEvent_Prior->GetProgram(),StringUtils::itos(pEvent_Prior->m_EventID));
		pCell->m_Colspan = (pEvent_Prior->m_tStopTime - StartTime) / 60 / m_iGridResolution;
		SetData(StartTime / 60 / m_iGridResolution,iRow,pCell);
		pCell->m_pMessage = GetSetInfoMessage(pEvent_Prior);
	}

	// Add all current shows
	while(pEvent && pEvent->m_tStartTime<StopTime)
	{
		string sDesc = pEvent->GetProgram();
		if( pEvent->m_tStopTime > StopTime )
			sDesc += ">";
		DataGridCell *pCell = new DataGridCell(sDesc,StringUtils::itos(pEvent->m_EventID));
		int Column = pEvent->m_tStartTime / 60 / m_iGridResolution;
		// When shows don't start/stop at even times, rounding times cause gaps.  So if this show
		// stops at the same time as the next starts, calculate the column width that way we there are no gaps
		if( pEvent->m_pEvent_Next && pEvent->m_pEvent_Next->m_tStartTime == pEvent->m_tStopTime )
			pCell->m_Colspan = pEvent->m_pEvent_Next->m_tStartTime / 60 / m_iGridResolution - Column;
		else
			pCell->m_Colspan = (pEvent->m_tStopTime - pEvent->m_tStartTime) / 60 / m_iGridResolution;
		SetData(Column,iRow,pCell);
		pEvent = pEvent->m_pEvent_Next;
		pCell->m_pMessage = GetSetInfoMessage(pEvent_Prior);
	}
}

int EpgGrid::GetCurrentColumn()
{
	return time(NULL) / 60 / m_iGridResolution;
}

Message *EpgGrid::GetSetInfoMessage(Event *pEvent)
{
	string sDesc;
	if( pEvent->m_pChannel )
		sDesc = pEvent->m_pChannel->m_sDescription + " / ";
	sDesc += pEvent->m_sTitle;
	if( pEvent->m_sDescription_Long.size() )
		sDesc += "\n" + pEvent->m_sDescription_Long;
	else if( pEvent->m_sDescription_Short.size() )
		sDesc += "\n" + pEvent->m_sDescription_Long;
	DCE::CMD_Set_Text CMD_Set_Text(0,DEVICETEMPLATE_This_Orbiter_CONST,"",
		sDesc,TEXT_EPG_SHOW_PREVIEW_CONST);
	return CMD_Set_Text.m_pMessage;
}

