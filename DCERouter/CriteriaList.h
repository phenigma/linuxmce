#ifndef CRITERIALIST_H
#define CRITERIALIST_H

class CriteriaList
{
public:
	int m_iPK_CriteriaList,m_iPK_Criteria;
	class CriteriaNesting *m_ptrCriteriaNesting;

	CriteriaList(int PK_CriteriaList,int PK_Criteria,class CriteriaNesting *ptrCriteriaNesting) : 
		m_iPK_CriteriaList(PK_CriteriaList), m_iPK_Criteria(PK_Criteria), m_ptrCriteriaNesting(ptrCriteriaNesting) {}

	virtual ~CriteriaList()
	{}

	bool Evaluate(class EventInfo *pEventInfo);

	virtual bool EvaluateExpression(class CriteriaParm *CriteriaParm,class EventInfo *pEventInfo)=0; // Must be overridden
};

#endif

