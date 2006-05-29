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
#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "OrbiterRenderer.h"
#include "CallBackData.h"
#include "ScreenHandler.h"
#include "ScreenHistory.h"
#include "RendererMNG.h"

#ifdef PRONTO
#include "CCF.h"
#endif

using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"


//=======================================================================================================
//Concrete class DesignObj_Orbiter
//-------------------------------------------------------------------------------------------------------
DesignObj_Orbiter::DesignObj_Orbiter(Orbiter *pOrbiter) 
{
	m_pOrbiter = pOrbiter;
	m_pDesignObj_Orbiter_Up=m_pDesignObj_Orbiter_Down=m_pDesignObj_Orbiter_Left=m_pDesignObj_Orbiter_Right=
		m_pDesignObj_Orbiter_TiedTo=NULL;
	m_pDataGridTable=NULL;

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
/* todo 2.0, dynamic_cast isn't cross-platform, I don't think
	if (m_pGraphic) 
	{
		//only if this is an WinGraphic object!
		if (dynamic_cast<WinGraphic *>(m_pGraphic) && 
			(dynamic_cast<WinGraphic *>(m_pGraphic))->m_pUncompressedImage
		)
		{
			m_pOrbiter->RemoveUncompressedImage((dynamic_cast<WinGraphic *>(m_pGraphic))->m_pUncompressedImage);
		}

		delete m_pGraphic;
		m_pGraphic = NULL;
	}
	
	if (m_pSelectedGraphic)
	{
		if (dynamic_cast<WinGraphic *>(m_pSelectedGraphic) && 
			(dynamic_cast<WinGraphic *>(m_pSelectedGraphic))->m_pUncompressedImage
		)
		{
			m_pOrbiter->RemoveUncompressedImage((dynamic_cast<WinGraphic *>(m_pSelectedGraphic))->m_pUncompressedImage);
		}

		delete m_pSelectedGraphic;
		m_pSelectedGraphic = NULL;
	} 
	if (m_pHighlightedGraphic)
	{
		if (dynamic_cast<WinGraphic *>(m_pHighlightedGraphic) && 
			(dynamic_cast<WinGraphic *>(m_pHighlightedGraphic))->m_pUncompressedImage
		)
		{
			m_pOrbiter->RemoveUncompressedImage((dynamic_cast<WinGraphic *>(m_pHighlightedGraphic))->m_pUncompressedImage);
		}

		delete m_pHighlightedGraphic;
		m_pHighlightedGraphic = NULL;
	} 

	int i;
*/

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
#ifdef PRONTO
	delete m_pCCF;
#endif
};
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
		m_pOrbiter->CallMaintenanceInMiliseconds( iTime, &Orbiter::PlayMNG_CallBack, this , pe_NO );
	}

	if(!pPlutoGraphic->IsEmpty())
		m_pOrbiter->Renderer()->RenderGraphic(pPlutoGraphic, rectTotal, bDisableAspectRatio, point);
#ifdef DEBUG
	else
		g_pPlutoLogger->Write(LV_STATUS, "No graphic to render for object %s", m_ObjectID.c_str());
#endif

	if(!bIsMNG && m_GraphicToDisplay == GRAPHIC_SELECTED)
		m_pOrbiter->Renderer()->SelectObject(this, point);
}


void DesignObj_Orbiter::RenderObject( DesignObj_Orbiter *pObj_Screen, PlutoPoint point, int Layer)
{
	//g_pPlutoLogger->Write(LV_CRITICAL, "Render object %s", m_ObjectID.c_str());

	if(m_ObjectType == DESIGNOBJTYPE_wxWidgets_Applet_CONST)
	{
		CallBackData *pCallBackData = m_pOrbiter->m_pScreenHandler->m_mapCallBackData_Find(cbOnDialogRefresh);
		if(pCallBackData)
		{
			PositionCallBackData *pPositionData = (PositionCallBackData *)pCallBackData;
			pPositionData->m_rectPosition = m_rPosition;
		}

		if(m_pOrbiter->ExecuteScreenHandlerCallback(cbOnDialogRefresh))
			return;
	}

	if(  m_pDesignObj_Orbiter_TiedTo  )
	{
		m_bHidden = m_pDesignObj_Orbiter_TiedTo->IsHidden(  );
		if(  ( m_iRowTiedTo==-1 && m_iColumnTiedTo==-1 ) || m_pDesignObj_Orbiter_TiedTo->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST  )
		{
			if(  m_pDesignObj_Orbiter_TiedTo==m_pOrbiter->m_pObj_Highlighted  )
				m_GraphicToDisplay = GRAPHIC_HIGHLIGHTED;
			else
				m_GraphicToDisplay = m_pDesignObj_Orbiter_TiedTo->m_GraphicToDisplay;
		}
		else
		{
			DesignObj_DataGrid *pObjGrid = ( DesignObj_DataGrid * ) m_pDesignObj_Orbiter_TiedTo;
			// We must be pointing to a datagrid,  and we need to bind to the highlighted row
			if(  ( m_iRowTiedTo==-1 || m_iRowTiedTo==pObjGrid->m_iHighlightedRow ) && ( m_iColumnTiedTo==-1 || m_iColumnTiedTo==pObjGrid->m_iHighlightedColumn )  )
				m_GraphicToDisplay = GRAPHIC_HIGHLIGHTED;
			else
				m_GraphicToDisplay = GRAPHIC_NORMAL;
		}
	}

	// See if this object is only visible for a given state
	if(  m_sVisibleState.size(  )  )
	{
		if(  (  ( this==m_pOrbiter->m_pObj_Highlighted || m_GraphicToDisplay==GRAPHIC_HIGHLIGHTED ) && m_sVisibleState.find( "H" )==string::npos ) ||
			(  m_GraphicToDisplay==GRAPHIC_SELECTED && m_sVisibleState.find( "S" )==string::npos  ) ||
			(  m_GraphicToDisplay==GRAPHIC_NORMAL && m_sVisibleState.find( "N" )==string::npos  ) ||
			(  m_GraphicToDisplay>0 && m_sVisibleState.find( StringUtils::itos( m_GraphicToDisplay ) )==string::npos  )  )
			return;
	}

	if ( m_bHidden || m_rPosition.Width==0 || m_rPosition.Height==0 )
	{
#ifdef DEBUG
		g_pPlutoLogger->Write( LV_STATUS, "object: %s  not visible: %d", m_ObjectID.c_str(), (int) m_bHidden );
#endif
		if(m_pOrbiter->m_bShowShortcuts && m_iPK_Button)
			m_pOrbiter->RenderShortcut(this);

		return;
	}

	PROFILE_START( ctObj )

	PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_pOrbiter->m_VariableMutex )
	PlutoRectangle rectBackground = m_rBackgroundPosition;
	PlutoRectangle rectTotal = m_rPosition;
	vm.Release(  );

	if( (this == m_pOrbiter->m_pObj_Highlighted || m_GraphicToDisplay == GRAPHIC_HIGHLIGHTED ||
		(m_pOrbiter->m_pObj_Highlighted && m_bTabStop && this==m_pOrbiter->m_pObj_Highlighted->m_pParentObject)  // If I'm also a tab stop, and my child is a highlighted object, leave me highlighted too
		) && m_vectHighlightedGraphic.size() )
	{
		m_pvectCurrentGraphic = &(m_vectHighlightedGraphic);

		//we'll need to do the highlighting here, since we need the normal surface for un-highlighting
		//we won't do it on RealRedraw for this kind of objects
		if(NULL != m_pOrbiter->m_pObj_Highlighted && m_pOrbiter->m_pObj_Highlighted != m_pOrbiter->m_pObj_Highlighted_Last)
		{
			m_pOrbiter->m_pObj_Highlighted_Last = m_pOrbiter->m_pObj_Highlighted;
			m_pOrbiter->Renderer()->DoHighlightObject();
		}
	}
	else if(m_GraphicToDisplay == GRAPHIC_SELECTED && m_vectSelectedGraphic.size())
		m_pvectCurrentGraphic = &(m_vectSelectedGraphic);
	else if(  m_GraphicToDisplay >= 1 && m_GraphicToDisplay <= int(m_vectAltGraphics.size()))
		m_pvectCurrentGraphic = &(m_vectAltGraphics[m_GraphicToDisplay - 1]);
	else // Something that was invalid was chosen,  or just the normal version
		m_pvectCurrentGraphic = &(m_vectGraphic);

	if(  !m_pvectCurrentGraphic && m_GraphicToDisplay != GRAPHIC_NORMAL  )
		m_pvectCurrentGraphic = &(m_vectGraphic);

	if( m_bCustomRender )
	{
		CallBackData *pCallBackData = m_pOrbiter->m_pScreenHandler->m_mapCallBackData_Find(cbOnCustomRender);
		if(pCallBackData)
		{
			RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pCallBackData;
			pRenderScreenCallBackData->m_nPK_Screen = m_pOrbiter->m_pScreenHistory_Current->PK_Screen();
			pRenderScreenCallBackData->m_pObj = this; //m_pScreenHistory_Current->GetObj();
		}
		m_pOrbiter->ExecuteScreenHandlerCallback(cbOnCustomRender);
		return;
	}

	// This is somewhat of a hack, but we don't have a clean method for setting the graphics on the user & location buttons to
	if( m_bIsBoundToUser )
	{
		vector<PlutoGraphic*> *pvectGraphic = m_pOrbiter->m_mapUserIcons[m_pOrbiter->m_dwPK_Users];

		// Put this here to debug why sometimes the user and room aren't selected when the screen first appears
		g_pPlutoLogger->Write(LV_STATUS,"Orbiter::RenderObject obj %s bound to user %d, got ptr %p",
			m_ObjectID.c_str(),m_pOrbiter->m_dwPK_Users,pvectGraphic);

		if( pvectGraphic )
			m_pvectCurrentGraphic = pvectGraphic;
		if( m_pvectCurrentGraphic )
			RenderGraphic(rectTotal, m_bDisableAspectLock, point);
	}
	else if( m_bIsBoundToLocation )
	{
		if( m_pOrbiter->m_pLocationInfo && m_pOrbiter->m_pLocationInfo->m_pvectGraphic )
			m_pvectCurrentGraphic = m_pOrbiter->m_pLocationInfo->m_pvectGraphic;
		if( m_pvectCurrentGraphic )
			RenderGraphic(rectTotal, m_bDisableAspectLock, point);
	}
	// Maybe we're showing a non standard state
	else if(  m_pGraphicToUndoSelect && m_GraphicToDisplay==GRAPHIC_NORMAL  )
	{
		vector<PlutoGraphic*> *pvectGraphic_Hold = m_pvectCurrentGraphic;
		vector<PlutoGraphic*> vectGraphicToUndoSelect;
		vectGraphicToUndoSelect.push_back(m_pGraphicToUndoSelect);
		m_pvectCurrentGraphic = &vectGraphicToUndoSelect;
		RenderGraphic( rectTotal, m_bDisableAspectLock, point );
		m_pvectCurrentGraphic = pvectGraphic_Hold;

		vectGraphicToUndoSelect.clear();
		delete m_pGraphicToUndoSelect;
		m_pGraphicToUndoSelect=NULL;
	}
	else if(  m_pvectCurrentGraphic  )
		RenderGraphic( rectTotal, m_bDisableAspectLock, point );

	switch( m_ObjectType )
	{
	case DESIGNOBJTYPE_Rectangle_CONST:
		// todo 2.0     SolidRectangle( rectTotal.X,  rectTotal.Y,  rectTotal.Width,  rectTotal.Height,  atoi( GetParameterValue( DESIGNOBJPARAMETER_Cell_Color_CONST ).c_str(  ) ),  atoi( GetParameterValue( DESIGNOBJPARAMETER_Transparency_CONST ).c_str(  ) ) );
		break;
	case DESIGNOBJTYPE_Datagrid_CONST:
		//RenderDataGrid( ( DesignObj_DataGrid * )pObj, point );
		break;
	case DESIGNOBJTYPE_Floorplan_CONST:
	//	RenderFloorplan(pObj,pObj_Screen, point);
		break; // Render the child objects with their text
	case DESIGNOBJTYPE_Web_Browser_CONST:
		//      LocalHandleWebWindow( pObj,  rectTotal );
		break;
	case DESIGNOBJTYPE_App_Desktop_CONST:
	case DESIGNOBJTYPE_wxWidgets_Applet_CONST:
		if ( m_pOrbiter->m_bYieldScreen )
			m_pOrbiter->RenderDesktop( this, PlutoRectangle( 0, 0, -1, -1 ), point );  // Full screen
		else
			m_pOrbiter->RenderDesktop( this, m_rPosition, point );
		break;
#ifdef PRONTO
	case DESIGNOBJTYPE_Pronto_File_CONST:
		if ( m_pCCF == NULL )
		{
			if ( !m_bInitialPaint )
			{
				string ccf = GetParameterValue( C_PARAMETER_PRONTO_FILE_CONST );
				if( ccf.length(  ) == 0 && m_bDeleteThis || ccf.find( '~' )==string::npos ) ccf = "3365~I49/ccf/1.ccf";
				string::size_type pos = 0;
				int IRDevice = atoi( StringUtils::Tokenize( ccf,  "~",  pos ).c_str(  ) );
				string fn = ccf.substr( pos );

				Message *pMessage=m_pcRequestSocket->SendReceiveMessage( new Message( m_dwPK_Device,  DEVICEID_DCEROUTER,  PRIORITY_NORMAL,  MESSAGETYPE_FILEREQUEST,  0,  1,  1,  fn.c_str(  ) ) );
				string sResult="";
				if ( pMessage )
				{
					if ( pMessage->m_dwID==FILERESULT_SUCCESS )
					{

						m_pCCF = new ProntoCCF( this,  pMessage->m_mapData_Parameters[1],  pMessage->m_mapData_Lengths[1],  pObj,  IRDevice );
						// Clear the data parameters so the Message's destructor doesn't delete the data.
						pMessage->m_mapData_Parameters.clear(  );
					}
					delete pMessage;
				}
			}
			else
			{
				m_AutoInvalidateTime = clock();
			}
		}
		if ( m_pCCF )
		{
			m_pCCF->RenderPronto( XOffset,  YOffset, point );
		}
		break;
#endif
	case DESIGNOBJTYPE_Broadcast_Video_CONST:
		if ( m_bOnScreen && !m_pOrbiter->m_bAlreadyQueuedVideo )
		{
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS, "Scheduling object @%p: %s", this, m_ObjectID.c_str());
#endif

			// If we've got no current graphic, such as during the first redraw since ObjectOnScreen
			// deletes it, then request a video frame right away, otherwise we're redrawing
			// and should wait the correct interval

			// Don't purge existing callbacks since there can be multiple frames on the screen
			m_pOrbiter->CallMaintenanceInMiliseconds( m_vectGraphic.size()==0 ? 0 : m_pOrbiter->m_iVideoFrameInterval, &Orbiter::GetVideoFrame, NULL, pe_ALL );
			m_pOrbiter->m_bAlreadyQueuedVideo=true;  // Only schedule once -- that will redraw all video frames
		}
		else
		{
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS, "Not Scheduling object @%p: %s because is not on screen.", this, m_ObjectID.c_str());
#endif
		}

		break;
	}

	PROFILE_STOP( ctObj,  m_ObjectID.c_str(  ) )

	DesignObj_DataList::reverse_iterator iHao;

	//#pragma warning( "todo - nasty hack -- on the hard drive screen,  you select music,  then when you go to video,  it redraws the video section,  but then redraws the deselected music with m_pGraphicToUndoSelect,  which includes a snapshot of the prior highlighters,  that are overwriting the new ones.  Redraw these first!  After render object won't draw anything" )
	for( iHao=m_ChildObjects.rbegin(  ); iHao != m_ChildObjects.rend(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  !m_pGraphicToUndoSelect || m_GraphicToDisplay!=GRAPHIC_NORMAL  )
			continue;
		pDesignObj_Orbiter->RenderObject( pObj_Screen, point, Layer );
	}


	///#pragma warning( "todo - nasty hack -- the datagrid highlighters need to be drawn after the music button.  Until I can implement layering,  scan twice and do the tied to's last" )
	for( iHao=m_ChildObjects.rbegin(  ); iHao != m_ChildObjects.rend(  ); ++iHao )
	{
		// Hold back the tied to's and datagrids
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  pDesignObj_Orbiter->m_pDesignObj_Orbiter_TiedTo || pDesignObj_Orbiter->m_bContainsDataGrid  )
			continue;
		pDesignObj_Orbiter->RenderObject( pObj_Screen, point, Layer );
	}
	for( iHao=m_ChildObjects.rbegin(  ); iHao != m_ChildObjects.rend(  ); ++iHao )
	{
		// Let the to's go through,  as long as there's no datagrid
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  !pDesignObj_Orbiter->m_pDesignObj_Orbiter_TiedTo || pDesignObj_Orbiter->m_bContainsDataGrid  )
			continue;
		pDesignObj_Orbiter->RenderObject( pObj_Screen, point, Layer );
	}
	for( iHao=m_ChildObjects.rbegin(  ); iHao != m_ChildObjects.rend(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  !pDesignObj_Orbiter->m_bContainsDataGrid  )
			continue;
		pDesignObj_Orbiter->RenderObject(pObj_Screen, point, Layer );
	}

	VectorDesignObjText::iterator iText;
	for( iText=m_vectDesignObjText.begin(  ); iText != m_vectDesignObjText.end(  ); ++iText )
	{
		DesignObjText *pText = *iText;
		if(  pText->m_bPreRender  )
			continue;
		PROFILE_START( ctText );
		TextStyle *pTextStyle = pText->m_mapTextStyle_Find( 0 );
		string TextToDisplay = m_pOrbiter->SubstituteVariables(m_pOrbiter->SubstituteVariables(pText->m_sText, pText->m_pObject, 0, 0), pText->m_pObject, 0, 0);
		m_pOrbiter->Renderer()->RenderText( TextToDisplay, pText, pTextStyle, point );
		PROFILE_STOP( ctText,  "Text ( obj below )" );
	}
	if( m_pFloorplanObject && m_pOrbiter->m_mapDevice_Selected.find(m_pFloorplanObject->PK_Device)!=m_pOrbiter->m_mapDevice_Selected.end() )
	{
		int i;
		for(i = 0; i < 4; ++i)
			m_pOrbiter->Renderer()->HollowRectangle(point.X + m_rBackgroundPosition.X-i, point.Y + m_rBackgroundPosition.Y-i, m_rBackgroundPosition.Width+i+i, m_rBackgroundPosition.Height+i+i,
			(i==1 || i==2 ? PlutoColor::Black() : PlutoColor::White()));

		//force an update because the object boundaries are not respected
		PlutoRectangle rect(point.X + m_rBackgroundPosition.X-i, point.Y + m_rBackgroundPosition.Y-i, m_rBackgroundPosition.Width+i+i, m_rBackgroundPosition.Height+i+i);
		m_pOrbiter->UpdateRect(rect, NULL != m_pOrbiter->m_pActivePopup ? m_pOrbiter->m_pActivePopup->m_Position : PlutoPoint(0, 0));
	}

	if(m_pOrbiter->m_bShowShortcuts && m_iPK_Button)
		m_pOrbiter->RenderShortcut(this);
}

bool DesignObj_Orbiter::ObjectInRect(PlutoRectangle rect)
{
	return rect.Contains(m_rPosition.X, m_rPosition.Y) || rect.Contains(m_rPosition.X + m_rPosition.Width - 1, m_rPosition.Y + m_rPosition.Height - 1);
}


//-------------------------------------------------------------------------------------------------------
