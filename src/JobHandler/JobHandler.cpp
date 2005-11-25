#include "JobHandler.h"
#include "Job.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

using namespace nsJobHandler;

JobHandler::JobHandler() : m_JobHandlerMutex("jobhandler", true)
{
}

bool JobHandler::Cancel()
{
	PLUTO_SAFETY_LOCK(jm,m_JobHandlerMutex);
	bool bSuccess=true;
	for(list<Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
	{
		if( !(*it)->Cancel() )
			bSuccess=false;
	}
	return bSuccess;
}

void JobHandler::AddJob(Job *pJob)
{
	PLUTO_SAFETY_LOCK(jm,m_JobHandlerMutex);
	m_listJob.push_back(pJob);
}

void JobHandler::RemoveJob(Job *pJob)
{
	PLUTO_SAFETY_LOCK(jm,m_JobHandlerMutex);
	m_listJob.remove(pJob);
}

void JobHandler::RemoveJob(int Job)
{
	PLUTO_SAFETY_LOCK(jm,m_JobHandlerMutex);
	for(list<class Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
	{
		if( (*it)->m_iID_get()==Job )
		{
			m_listJob.erase(it);
			return;
		}
	}
}

string JobHandler::ToString()
{
	PLUTO_SAFETY_LOCK(jm,m_JobHandlerMutex);
	string sResult = StringUtils::itos((int) m_listJob.size()) + "\t";

	for(list<class Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
		sResult += (*it)->ToString() + "\t";
	return sResult;
}

bool JobHandler::ContainsJob(string sName)
{
	PLUTO_SAFETY_LOCK(jm,m_JobHandlerMutex);
	for(list<class Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
		if( (*it)->m_sName_get()==sName )
			return true;
	return false;
}

