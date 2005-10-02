#ifndef EPGGrid_h
#define EPGGrid_h

#include "DCE/DataGrid.h"

class VDRStateInfo;

namespace VDREPG
{
	class Channel;

	class EpgGrid: public DCE::DataGridTable
	{
		class EPG *m_pEPG;
		VDRStateInfo *m_pVDRStateInfo;
		int m_dwPK_Device_Orbiter;
		int m_iGridResolution; // How many minutes per column
	public:
		EpgGrid(EPG *pEPG,class VDRStateInfo *pVDRStateInfo, int dwPK_Device_Orbiter, int iGridResolution) { m_pEPG = pEPG; m_pVDRStateInfo = pVDRStateInfo; m_dwPK_Device_Orbiter=dwPK_Device_Orbiter; m_iGridResolution=iGridResolution; };

		virtual int GetRows();
		virtual int GetCols();

		virtual void ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount);
		void PopulateRow(Channel *pChannel,int iRow,int StartTime,int StopTime);
		int GetCurrentColumn();
		Message *GetSetInfoMessage(Event *pEvent);
	};
}


#endif

