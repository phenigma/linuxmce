#ifndef CellPhoneEmulator_H
#define CellPhoneEmulator_H


class CellPhoneEmulator
{
public:
	char *m_pcResponse;
	long m_dwResponseSize;
	class PlutoConfig *m_pPlutoConfig;
	class Logger *g_pPlutoLogger;
	void ShowMenu(class VR_ShowMenu *MenuAction);
	CellPhoneEmulator(class PlutoConfig *pPlutoConfig, class Logger *pLogger) 
	{ 
		m_pPlutoConfig=pPlutoConfig;
		g_pPlutoLogger=pLogger;
	}
	const char *ReceiveRequest(long iRequestSize,const char *pRequest,long *iResponseSize);
};

#endif
