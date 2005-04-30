#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "Criteria.h"
#include "CriteriaParmNesting.h"

bool Criteria::Evaluate(class EventInfo *pEventInfo,void *pExtraInfo)
{
	return m_pCriteriaParmNesting->Evaluate(this,pEventInfo,pExtraInfo);
}

