#ifndef CRITERIAPARMNESTING_H
#define CRITERIAPARMNESTING_H

class CriteriaParmNesting
{
public:
	bool m_bNot,m_bAnd;
	vector<class CriteriaParm *> m_vectCriteriaParm;
	vector<class CriteriaParmNesting *> m_vectCriteriaParmNesting;

	CriteriaParmNesting(bool bNot,bool bAnd) : m_bNot(bNot), m_bAnd(bAnd) {}

	bool Evaluate(class Criteria *ptrCriteria,class EventInfo *pEventInfo);
/*
	{
		for(int i=0;i<m_vectCriteriaParm.size();++i)
		{
			bool bResult = ptrCriteriaList->EvaluateExpression(m_vectCriteriaParm[i]);
			if( bAnd==true && bResult==false )
				return !(!bNot ^ false) ;
			if( bAnd==false && bResult==true )
				return !(!bNot ^ true);
		}

		for(int i=0;i<m_vectCriteriaParmNesting.size();++i)
		{
			bool bResult = m_vectCriteriaParmNesting[i].Evaluate(ptrCriteriaList);
			if( bAnd==true && bResult==false )
				return !(!bNot ^ false);
			if( bAnd==false && bResult==true )
				return !(!bNot ^ true);
		}
		// If this is an 'and' we're true because we got through everything without a false,
		// If this is an 'or' we're false because we never got a true value
		return !(!bNot ^ bAnd);  
	}
	*/
};

#endif

