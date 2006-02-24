#include "ScreenHistory.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
ScreenHistory::ScreenHistory(int nPK_Screen, class ScreenHistory *pScreenHistory_Prior)
{
	m_nPK_Screen = nPK_Screen;
	m_tTime = time(NULL);
	m_pObj = NULL;
	m_bCantGoBack = false; 
}
//-----------------------------------------------------------------------------------------------------
ScreenHistory::~ScreenHistory()
{
}
//-----------------------------------------------------------------------------------------------------
DesignObj_Orbiter *ScreenHistory::GetObj() 
{ 
	return m_pObj; 
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::SetObj(DesignObj_Orbiter *pObj) 
{ 
	m_pObj = pObj;
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "ScreenHistory - replaced in screen %d obj with %d", 
		m_nPK_Screen, pObj->m_iBaseObjectID);
#endif
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::AddToHistory()
{
	m_listObjs.push_front(m_pObj);

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS, "Added %d obj to stack (size %d) - screen %d", 
		m_pObj->m_iBaseObjectID, m_listObjs.size(), m_nPK_Screen);
#endif
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHistory::HistoryEmpty()
{
	return m_listObjs.size() == 0;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::PurgeHistory() 
{ 
	m_listObjs.clear(); 
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHistory::GoBack()
{
	if(m_listObjs.size())
	{
		m_pObj = m_listObjs.front();
		m_listObjs.pop_front();

#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS, "Removing %d from stack (size left %d) - screen %d, front left: %d", 
			m_pObj->m_iBaseObjectID, m_listObjs.size(), m_nPK_Screen, 
			m_listObjs.size() ? m_listObjs.front()->m_iBaseObjectID : 0);
#endif
		return true;
	}

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS, "Nothing to remove from queue screen %d", m_nPK_Screen);
#endif

	return false;
}
//-----------------------------------------------------------------------------------------------------
string ScreenHistory::ToString()
{
	string sOutput;
	sOutput += StringUtils::ltos(m_nPK_Screen) + "(\"" + m_sID + "\" - ";

	list<DesignObj_Orbiter *>::iterator it;
	for(it = m_listObjs.begin(); it != m_listObjs.end(); it++)
	{
		if(it != m_listObjs.begin())
			sOutput += ",";

		DesignObj_Orbiter *pObj = *it;
		sOutput += StringUtils::ltos(pObj->m_iBaseObjectID);
	}

	sOutput += ")";

	/*
	sOutput += "vars[";

	for(map<int, string>::const_iterator itm = m_mapVariable.begin(); itm != m_mapVariable.end(); ++itm)
	{
		if(itm->second != "")
			sOutput += StringUtils::ltos(itm->first) + "-" + itm->second + " ";
	}

	sOutput += "]";
	*/

	return sOutput;
}
//-----------------------------------------------------------------------------------------------------
time_t ScreenHistory::TimeCreated() const
{
	return m_tTime;
}
//-----------------------------------------------------------------------------------------------------
string ScreenHistory::ScreenID()
{
	return m_sID;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::ScreenID(string sID)
{
	m_sID = sID;
}
//-----------------------------------------------------------------------------------------------------
int ScreenHistory::PK_Screen()
{
	return m_nPK_Screen;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::PK_Screen(int nPK_Screen)
{
	m_nPK_Screen = nPK_Screen;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::SaveContext(const map<int, string>& mapVariable,
								const map<DesignObj_Orbiter *, bool>& mapVisibilityContext)
{
	g_pPlutoLogger->Write(LV_WARNING, "Saving context for screen %d, object %s (%d variables, %d visibility status) ...", 
		m_nPK_Screen, m_pObj->m_ObjectID.c_str(), mapVariable.size(), mapVisibilityContext.size());

	m_mapVariable.clear();
	for(map<int, string>::const_iterator it = mapVariable.begin(); it != mapVariable.end(); ++it)
		m_mapVariable[it->first] = it->second;

	m_mapVisibilityContext.clear();
	for(map<DesignObj_Orbiter *, bool>::const_iterator itv = mapVisibilityContext.begin(); itv != mapVisibilityContext.end(); ++itv)
		m_mapVisibilityContext[itv->first] = itv->second;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::RestoreContext(map<int, string>& mapVariable,
								   map<DesignObj_Orbiter *, bool>& mapVisibilityContext)
{
	g_pPlutoLogger->Write(LV_WARNING, "Restoring context for screen %d, object %s (%d variables, %d visibility status) ...", 
		m_nPK_Screen, m_pObj->m_ObjectID.c_str(), mapVariable.size(), mapVisibilityContext.size());

	for(map<int, string>::iterator it = m_mapVariable.begin(); it != m_mapVariable.end(); ++it)
		mapVariable[it->first] = it->second;

	mapVisibilityContext.clear();
	for(map<DesignObj_Orbiter *, bool>::iterator itv = m_mapVisibilityContext.begin(); itv != m_mapVisibilityContext.end(); ++itv)
		mapVisibilityContext[itv->first] = itv->second;
}
//-----------------------------------------------------------------------------------------------------
