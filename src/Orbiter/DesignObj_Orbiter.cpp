/*
 DesignObj_Orbiter
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */
 
#include "PlutoUtils/CommonIncludes.h"	
#include "DataGrid.h"
#include "RendererMNG.h"
#include "OrbiterRenderer.h"
#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "pluto_main/Define_Variable.h"

#ifdef PRONTO
#include "CCF.h"
#endif
//=======================================================================================================
//Concrete class DesignObj_Orbiter
//-------------------------------------------------------------------------------------------------------
DesignObj_Orbiter::DesignObj_Orbiter(Orbiter *pOrbiter)
{
	m_pDesignObj_Orbiter_Up=m_pDesignObj_Orbiter_Down=m_pDesignObj_Orbiter_Left=m_pDesignObj_Orbiter_Right=
		m_pDesignObj_Orbiter_TiedTo=NULL;
	m_pDataGridTable=NULL;
	m_pOrbiter = pOrbiter;

	m_MaxRow=m_MaxCol=0;
	m_vectGraphic.clear();
	m_vectSelectedGraphic.clear();
	m_vectHighlightedGraphic.clear();
	m_vectAltGraphics.clear();
	m_pvectCurrentGraphic = NULL;
	m_pGraphicToUndoSelect = NULL;
	m_pvectCurrentPlayingGraphic = NULL;
	m_pObj_Screen = NULL;
	m_bIsARemoteControl = false;

	m_iCurrentFrame = 0;

	m_pCriteria=NULL;
	m_GraphicToDisplay=GRAPHIC_NORMAL;
	m_GraphicToPlay=GRAPHIC_NORMAL;
	m_GridCurRow=-1;
	m_GridCurCol=-1;
	m_bOneTimeDontReset=false;
	m_pParentObject=NULL;
	m_pWebWindow=NULL;
	m_bHidden=false;
	m_pFloorplanObject=NULL;
#ifdef PRONTO
	m_pCCF=NULL;
#endif
	m_bDisableAspectLock=false;
	m_bOnScreen=false;
	m_bContainsDataGrid=false;
	m_bIsBoundToUser=m_bIsBoundToLocation=false;
};
//-------------------------------------------------------------------------------------------------------
DesignObj_Orbiter::~DesignObj_Orbiter()
{
	for(list<class PlutoPopup*>::iterator it=m_listPopups.begin();it!=m_listPopups.end();++it)
		delete *it;
    m_listPopups.clear();

	m_pGraphicToUndoSelect = NULL;
	m_pvectCurrentGraphic = m_pvectCurrentPlayingGraphic = NULL;

	for(VectorPlutoGraphic::iterator itGraphic = m_vectGraphic.begin(); itGraphic != m_vectGraphic.end(); ++itGraphic)
		delete *itGraphic;
	m_vectGraphic.clear();

	for(VectorPlutoGraphic::iterator itSelectedGraphic = m_vectSelectedGraphic.begin(); itSelectedGraphic != m_vectSelectedGraphic.end(); ++itSelectedGraphic)
		delete *itSelectedGraphic;
	m_vectSelectedGraphic.clear();

	for(VectorPlutoGraphic::iterator itHighlightedGraphic = m_vectHighlightedGraphic.begin(); itHighlightedGraphic != m_vectHighlightedGraphic.end(); ++itHighlightedGraphic)
		delete *itHighlightedGraphic;
	m_vectHighlightedGraphic.clear();

	for(vector<VectorPlutoGraphic>::iterator itVectAltGraphic = m_vectAltGraphics.begin(); itVectAltGraphic != m_vectAltGraphics.end(); ++itVectAltGraphic)
	{
		for(VectorPlutoGraphic::iterator itAltGraphic = itVectAltGraphic->begin(); itAltGraphic != itVectAltGraphic->end(); ++itAltGraphic)
			delete *itAltGraphic;

		itVectAltGraphic->clear();
	}
	m_vectAltGraphics.clear();


	m_bvectLoop_Alt.clear();

	DesignObjZoneList::iterator iZone;
	for(iZone = m_ZoneList.begin(); iZone !=m_ZoneList.end(); ++iZone)
		delete *iZone; 

	DesignObjCommandList::iterator iA;
	for(iA = m_Action_StartupList.begin(); iA != m_Action_StartupList.end(); ++iA)
		delete *iA;
	for(iA = m_Action_TimeoutList.begin(); iA != m_Action_TimeoutList.end(); ++iA)
		delete *iA;
	for(iA = m_Action_LoadList.begin(); iA != m_Action_LoadList.end(); ++iA)
		delete *iA; 
	for(iA = m_Action_UnloadList.begin(); iA != m_Action_UnloadList.end(); ++iA)
		delete *iA; 
	for(iA = m_Action_HighlightList.begin(); iA != m_Action_HighlightList.end(); ++iA)
		delete *iA; 
	for(iA = m_Action_UnhighlightList.begin(); iA != m_Action_UnhighlightList.end(); ++iA)
		delete *iA; 
	VectorDesignObjText::iterator iT;
	for(iT=m_vectDesignObjText.begin(); iT != m_vectDesignObjText.end(); ++iT)
	{
		//DesignObjText *pDummy = *iT;
		delete *iT; 
	}

	//IMPORTANT!!!
	//because all DesignObj_Orbiter objects are stored on Orbiter::m_map_ObjAll,
	//we don't want to delete here children of the object 
	//their children will be deleted in Orbiter destructor using map_ObjAll
	m_ChildObjects.clear();

	delete m_pDataGridTable;
// todo 2.0	delete m_pWebWindow; 
#ifdef PRONTO
	delete m_pCCF;
#endif
};
//-------------------------------------------------------------------------------------------------------
bool DesignObj_Orbiter::IsHidden() 
{ 
	if(m_bHidden) 
		return true;  
	
	if(m_pParentObject) 
		return ((DesignObj_Orbiter *) m_pParentObject)->IsHidden(); 

	return false; 
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void DesignObj_Orbiter::RenderGraphic(PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point)
{
	PLUTO_SAFETY_LOCK( cm, m_pOrbiter->m_ScreenMutex );
	vector<PlutoGraphic*> *pVectorPlutoGraphic = m_pvectCurrentGraphic;

	//we have nothing to render
	if(pVectorPlutoGraphic->size() == 0)
		return;

	//just in case
	if(int(pVectorPlutoGraphic->size()) <= m_iCurrentFrame)
		m_iCurrentFrame = 0;

	int iCurrentFrame = m_iCurrentFrame;
	PlutoGraphic *pPlutoGraphic = (*pVectorPlutoGraphic)[iCurrentFrame];
	bool bIsMNG = pPlutoGraphic->m_GraphicFormat == GR_MNG;

	//if a button doesn't have a mng as selected state (see bDisableEffects), then the png
	//used in normal state will be rendered for selected state + a blue rectangle to show the selection
	if(!bIsMNG && m_GraphicToDisplay == GRAPHIC_SELECTED)
	{
		if(m_vectSelectedGraphic.size() != 0)
			pVectorPlutoGraphic = &m_vectSelectedGraphic;
		else
			pVectorPlutoGraphic = &m_vectGraphic; //normal state

		//we have nothing to render
		if(pVectorPlutoGraphic->size() == 0)
			return;
		if(int(pVectorPlutoGraphic->size()) <= m_iCurrentFrame)
			m_iCurrentFrame = 0;

		iCurrentFrame = m_iCurrentFrame;
		pPlutoGraphic = (*pVectorPlutoGraphic)[iCurrentFrame];
		bIsMNG = pPlutoGraphic->m_GraphicFormat == GR_MNG;
	}

	string sFileName = "";
	if(pPlutoGraphic->IsEmpty() && NULL != m_pOrbiter->m_pCacheImageManager && pPlutoGraphic->m_Filename.length() &&
		m_pOrbiter->m_pCacheImageManager->IsImageInCache(pPlutoGraphic->m_Filename, m_Priority)
		)
	{
		//if we have the file in cache
		sFileName = m_pOrbiter->m_pCacheImageManager->GetCacheImageFileName(pPlutoGraphic->m_Filename);
	}
	else if(pPlutoGraphic->IsEmpty() && m_pOrbiter->m_sLocalDirectory.length() > 0 && pPlutoGraphic->m_Filename.length() )
	{
		//the file is in our localdrive
		sFileName = m_pOrbiter->m_sLocalDirectory + pPlutoGraphic->m_Filename;
	}

	//if we don't have the file in cache or on our localdrive
	if(pPlutoGraphic->IsEmpty() && sFileName.empty())
	{
		// Request our config info
		char *pGraphicFile=NULL;
		int iSizeGraphicFile=0;

		DCE::CMD_Request_File CMD_Request_File(
			m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,
			"orbiter/C" + StringUtils::itos(m_pOrbiter->m_dwPK_Device) + "/" + pPlutoGraphic->m_Filename,
			&pGraphicFile,&iSizeGraphicFile);
		m_pOrbiter->SendCommand(CMD_Request_File);

		if (!iSizeGraphicFile)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Unable to get file from server %s", pPlutoGraphic->m_Filename.c_str());
			return;
		}

		//save the image in cache
		if(NULL != m_pOrbiter->m_pCacheImageManager) //cache manager is enabled ?
		{
			m_pOrbiter->m_pCacheImageManager->CacheImage(pGraphicFile, iSizeGraphicFile, pPlutoGraphic->m_Filename, m_Priority);
			sFileName = m_pOrbiter->m_pCacheImageManager->GetCacheImageFileName(pPlutoGraphic->m_Filename);
		}

		//TODO: same logic for in-memory data
		if( (sFileName.empty() || iSizeGraphicFile) && !pPlutoGraphic->LoadGraphic(pGraphicFile, iSizeGraphicFile))
		{
			delete pGraphicFile;
			pGraphicFile = NULL;
			return;
		}

		delete pGraphicFile;
		pGraphicFile = NULL;
	}

	if(pPlutoGraphic->IsEmpty() && !sFileName.empty())
	{
		if(!FileUtils::FileExists(sFileName))
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Unable to read file %s", (sFileName).c_str());
			return;
		}

		switch(pPlutoGraphic->m_GraphicFormat)
		{
		case GR_JPG:
		case GR_GIF:
		case GR_TIF:
		case GR_PNG:
		case GR_BMP:
		case GR_OCG:
			{
				size_t size = 0;
				char *pData = FileUtils::ReadFileIntoBuffer(sFileName.c_str(), size);

				if(!size)
					return;

				if(!pPlutoGraphic->LoadGraphic(pData, size))
					return;

				delete [] pData;
			}
			break;

		case GR_MNG:
			{
				//eGraphicFormat eGF = pPlutoGraphic->m_GraphicFormat;
				eGraphicManagement eGM = pPlutoGraphic->m_GraphicManagement;
				string sMNGFileName = pPlutoGraphic->m_Filename;

				vector<PlutoGraphic *>::iterator itPlutoGraphic;
				for(itPlutoGraphic = pVectorPlutoGraphic->begin(); itPlutoGraphic != pVectorPlutoGraphic->end(); ++itPlutoGraphic)
					delete *itPlutoGraphic;
				pVectorPlutoGraphic->clear();

				InMemoryMNG *pInMemoryMNG = InMemoryMNG::CreateInMemoryMNGFromFile(sFileName, rectTotal.Size());
				size_t framesCount = pInMemoryMNG->m_vectMNGframes.size();
				for(size_t i = 0; i < framesCount; i++)
				{
					size_t iFrameSize = 0;
					char *pFrameData = NULL;

					iFrameSize = pInMemoryMNG->GetFrame(int(i), pFrameData);

					if(iFrameSize)
					{
						PlutoGraphic *pGraphic = m_pOrbiter->Renderer()->CreateGraphic();
						pGraphic->m_GraphicManagement = eGM;
						pGraphic->m_Filename = sMNGFileName;
						pGraphic->m_GraphicFormat = GR_PNG; //this is an mng with multiple png frames
						pGraphic->LoadGraphic(pFrameData, iFrameSize);
						pGraphic->m_GraphicFormat = GR_MNG;
						(*pVectorPlutoGraphic).push_back(pGraphic);
					}

					delete [] pFrameData;
				}

				delete pInMemoryMNG;
				pInMemoryMNG = NULL;
			}
			break;

		default:;

		}
	}

	if(bIsMNG && m_pvectCurrentPlayingGraphic == NULL)
	{
		pPlutoGraphic = (*pVectorPlutoGraphic)[0];

		int iTime = 0; //hardcoding warning! don't know from where to get the framerate yet (ask Radu, libMNG)
#ifndef WINCE
		iTime = 15;
#endif

		bool bLoop = false; //hardcoding warning!  (ask Radu, libMNG)

		switch(m_GraphicToDisplay)
		{
		case GRAPHIC_HIGHLIGHTED:
			m_iTime_Highlighted = iTime;
			m_bLoop_Highlighted = bLoop;
			break;
		case GRAPHIC_SELECTED:
			m_iTime_Selected = iTime;
			m_bLoop_Selected = bLoop;
			break;
		case GRAPHIC_NORMAL:
			m_iTime_Background = iTime;
			m_bLoop_Background = bLoop;
			break;
			//todo alternate graphics ?
		}

		m_iCurrentFrame = 1;

		//schedule next frame for animation
		m_pvectCurrentPlayingGraphic = m_pvectCurrentGraphic;
		m_GraphicToPlay = m_GraphicToDisplay;
		m_pOrbiter->CallMaintenanceInMiliseconds( iTime, (OrbiterCallBack) &Orbiter::PlayMNG_CallBack, this , pe_NO );
	}

	if(!pPlutoGraphic->IsEmpty())
		m_pOrbiter->Renderer()->RenderGraphic(pPlutoGraphic, rectTotal, bDisableAspectRatio, point);
#ifdef DEBUG
	else
		g_pPlutoLogger->Write(LV_STATUS, "No graphic to render for object %s", m_ObjectID.c_str());
#endif

	if(!bIsMNG && m_GraphicToDisplay == GRAPHIC_SELECTED)
		m_pOrbiter->SelectObject(this, point);
}
//-------------------------------------------------------------------------------------------------------
string DesignObj_Orbiter::GetParameterValue(int ParameterID)
{
	map<int,string>::iterator ipParm = m_mapObjParms.find(ParameterID);
	if (ipParm==m_mapObjParms.end())
		return "";
	return m_pOrbiter->SubstituteVariables((*ipParm).second,this,0,0);
}

PlutoRectangle DesignObj_Orbiter::GetHighlightRegion()
{
	PlutoRectangle r;

	r.X = max(0,m_rPosition.X-4);
	r.Y = max(0,m_rPosition.Y-4);
	r.Right( min(m_rPosition.Right()+4,m_pOrbiter->m_Width-1) );
	r.Bottom( min(m_rPosition.Bottom()+4,m_pOrbiter->m_Height-1) );
	return r;
}

//=======================================================================================================
//Concrete class DesignObj_DataGrid
//-------------------------------------------------------------------------------------------------------
DesignObj_DataGrid::~DesignObj_DataGrid() 
{
	for(int i=0;i<CACHE_SIZE;++i)
		delete m_pDataGridTableCache[i];		
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoUp()			
{ 
	return m_GridCurRow > 0;																
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoDown()		
{ 
	return m_pDataGridTable->GetRows() > m_GridCurRow + m_MaxRow - (HasMoreUp() ? 1 : 0); 
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoLeft()		
{ 
	return m_GridCurCol > 0;																
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoRight()		
{ 
	return m_pDataGridTable->GetCols() > m_GridCurCol + m_MaxCol;							
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::VerticalOnly()		
{ 
	return !CanGoLeft() && !CanGoRight();													
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::HasMoreUp()		
{ 
	return VerticalOnly() && CanGoUp();													
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::HasMoreDown()		
{ 
	return VerticalOnly() && CanGoDown();													
}
//-------------------------------------------------------------------------------------------------------
string DesignObj_Orbiter::GetArrayValue()
{
	for(DesignObjZoneList::iterator itZ=m_ZoneList.begin();itZ!=m_ZoneList.end();++itZ)
	{
		DesignObjZone *pDesignObjZone = *itZ;

		for(DesignObjCommandList::iterator it=pDesignObjZone->m_Commands.begin();it!=pDesignObjZone->m_Commands.end();++it)
		{
			DesignObjCommand *pDesignObjCommand = *it;
			if( pDesignObjCommand->m_PK_Command==COMMAND_Set_Variable_CONST )
			{
				int PK_Variable = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST].c_str());
				if( PK_Variable == VARIABLE_Array_ID_CONST )
					return pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST];
			}
		}
	}
	return "";
}
