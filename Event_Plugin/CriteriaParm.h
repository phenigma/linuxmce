#ifndef CRITERIAPARM_H
#define CRITERIAPARM_H

class CriteriaParm
{
public:
	string m_sComparedValue;
	bool m_bResult;
	int m_iPK_CriteriaParm;
	int m_iPK_CriteriaParmList;
	int m_iPK_ParameterType;
	int m_Operator;
	string m_sValue;

	CriteriaParm(int PK_CriteriaParm,int PK_CriteriaParmList,int PK_ParameterType,int Operator,string sValue) :
		m_iPK_CriteriaParm(PK_CriteriaParm),m_iPK_CriteriaParmList(PK_CriteriaParmList), 
		m_iPK_ParameterType(PK_ParameterType), m_Operator(Operator), m_sValue(sValue) 
	{}


};

#endif

