#ifndef CRITERIAPARM_H
#define CRITERIAPARM_H

class CriteriaParm
{
public:
	string m_sComparedValue;
	bool m_bResult;
	int m_iPK_CriteriaParm;
	int m_iPK_CriteriaParmList;
	int m_iPK_CannedEvents_CriteriaParmList;
	int m_Operator;
	string m_sValue;

	CriteriaParm(int PK_CriteriaParm,int PK_CriteriaParmList,int PK_CannedEvents_CriteriaParmList,int Operator,string sValue) :
		m_iPK_CriteriaParm(PK_CriteriaParm),m_iPK_CriteriaParmList(PK_CriteriaParmList), 
		m_iPK_CannedEvents_CriteriaParmList(PK_CannedEvents_CriteriaParmList), m_Operator(Operator), m_sValue(sValue) 
	{}


};

#endif

