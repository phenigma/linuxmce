#include "CriteriaList.h"
#include "CriteriaNesting.h"

bool CriteriaList::Evaluate(class EventInfo *pEventInfo)
{
	return m_ptrCriteriaNesting->Evaluate(this,pEventInfo);
}

