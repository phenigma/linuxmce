#ifndef CRITERIA_H
#define CRITERIA_H

class Criteria
{
public:
	int m_iPK_Criteria;
	class CriteriaParmNesting *m_pCriteriaParmNesting;

	Criteria(int PK_Criteria,class CriteriaParmNesting *pCriteriaParmNesting) 
	{
		m_iPK_Criteria=PK_Criteria;
		m_pCriteriaParmNesting=pCriteriaParmNesting;
	}

	virtual ~Criteria()
	{}

	bool Evaluate(class EventInfo *pEventInfo);

	virtual bool EvaluateExpression(class CriteriaParm *CriteriaParm,class EventInfo *pEventInfo);
};

#endif

