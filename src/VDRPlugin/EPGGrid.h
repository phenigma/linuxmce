#ifndef EPGGrid_h
#define EPGGrid_h

#include "DCE/DataGrid.h"

class VDRStateInfo;

namespace DCE
{
	class Message;
	class VDRPlugin;
}

namespace VDREPG
{
	class Channel;
	class Event;
	class EPG;

	class EpgGrid: public DCE::DataGridTable
	{
		EPG *m_pEPG;
		DCE::VDRPlugin *m_pVDRPlugin;
		VDRStateInfo *m_pVDRStateInfo;
		int m_dwPK_Device_Orbiter;
		int m_iGridResolution; // How many minutes per column
	public:
		EpgGrid(DCE::VDRPlugin *pVDRPlugin,class VDRStateInfo *pVDRStateInfo, int dwPK_Device_Orbiter, int iGridResolution) { m_pVDRPlugin = pVDRPlugin; m_pVDRStateInfo = pVDRStateInfo; m_dwPK_Device_Orbiter=dwPK_Device_Orbiter; m_iGridResolution=iGridResolution; m_pEPG=NULL; };

		virtual int GetRows();
		virtual int GetCols();

		virtual void ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount);
		void PopulateRow(Channel *pChannel,int iRow,int StartTime,int StopTime);
		int GetCurrentColumn();
	};
}


#endif

