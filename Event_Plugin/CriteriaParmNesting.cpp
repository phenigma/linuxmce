#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "CriteriaParmNesting.h"
#include "Criteria.h"
#include "CriteriaParm.h"
#include "EventInfo.h"
#include "EventHandler.h"

bool CriteriaParmNesting::Evaluate(class Criteria *pCriteria,class EventInfo *pEventInfo)
{
	if( m_vectCriteriaParm.size()==0 && m_vectCriteriaParmNesting.size()==0 )
		return true;

	for(size_t i=0;i<m_vectCriteriaParm.size();++i)
	{
		CriteriaParm *pCriteriaParm = m_vectCriteriaParm[i];
//		pEventInfo->pEventHandler->m_vectCriteriaParm.push_back(pCriteriaParm);

		pCriteriaParm->m_bResult = pCriteria->EvaluateExpression(pCriteriaParm,pEventInfo);
		if( m_bAnd==true && pCriteriaParm->m_bResult==false )
			return !(!m_bNot ^ false) ;
		if( m_bAnd==false && pCriteriaParm->m_bResult==true )
			return !(!m_bNot ^ true);
	}

	for(size_t i=0;i<m_vectCriteriaParmNesting.size();++i)
	{
		bool bResult = m_vectCriteriaParmNesting[i]->Evaluate(pCriteria,pEventInfo);
		if( m_bAnd==true && bResult==false )
			return !(!m_bNot ^ false);
		if( m_bAnd==false && bResult==true )
			return !(!m_bNot ^ true);
	}
	// If this is an 'and' we're true because we got through everything without a false,
	// If this is an 'or' we're false because we never got a true value
	return !(!m_bNot ^ m_bAnd);  
}
