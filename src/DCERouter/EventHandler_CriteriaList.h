#ifndef EVENTHANDLER_CRITERIALIST_H
#define EVENTHANDLER_CRITERIALIST_H

#include "CriteriaList.h"

class EventHandler_CriteriaList : public CriteriaList
{
public:
	EventHandler_CriteriaList(int PK_CriteriaList,int PK_Criteria,class CriteriaNesting *ptrCriteriaNesting) : 
		CriteriaList(PK_CriteriaList,PK_Criteria,ptrCriteriaNesting) {}
	virtual ~EventHandler_CriteriaList()
	{}

	virtual bool EvaluateExpression(class CriteriaParm *CriteriaParm,class EventInfo *pEventInfo);
};

#endif

