#ifndef EPGGrid_h
#define EPGGrid_h

#include "DCE/DataGrid.h"

namespace DCE
{
	class Message;
	class VDRPlugin;
	class VDRMediaStream;
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
		DCE::VDRMediaStream *m_pVDRMediaStream;
		int m_dwPK_Device_Orbiter;
		int m_iGridResolution; // How many minutes per column
	public:
		EpgGrid(DCE::VDRPlugin *pVDRPlugin,DCE::VDRMediaStream *pVDRMediaStream, int dwPK_Device_Orbiter, int iGridResolution) { m_pVDRPlugin = pVDRPlugin; m_pVDRMediaStream = pVDRMediaStream; m_dwPK_Device_Orbiter=dwPK_Device_Orbiter; m_iGridResolution=iGridResolution; m_pEPG=NULL; };

		virtual int GetRows();
		virtual int GetCols();

		virtual void ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount);
		void PopulateRow(Channel *pChannel,int iRow,int StartTime,int StopTime);
		int GetCurrentColumn();
	};
}


#endif

