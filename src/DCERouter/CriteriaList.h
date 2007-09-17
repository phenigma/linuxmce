/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com


 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

/** @file CriteriaList.h
For ???
 */
#ifndef CRITERIALIST_H
#define CRITERIALIST_H

/** @class CriteriaList
For ???
*/
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

