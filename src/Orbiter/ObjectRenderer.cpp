#include "ObjectRenderer.h"
#include "pluto_main/Define_Variable.h"
#include "Orbiter.h"
#include "CallBackData.h"
#include "ScreenHandler.h"
#include "RendererMNG.h"
#include "OrbiterRenderer.h"

ObjectRenderer::ObjectRenderer(DesignObj_Orbiter *pOwner) : m_pOwner(pOwner)
{
}

/*virtual*/ ObjectRenderer::~ObjectRenderer()
{
}

/*virtual*/ void ObjectRenderer::RenderGraphic(PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point)
{
	PLUTO_SAFETY_LOCK( cm, m_pOwner->m_pOrbiter->m_ScreenMutex );
	vector<PlutoGraphic*> *pVectorPlutoGraphic = m_pOwner->m_pvectCurrentGraphic;

	//we have nothing to render
	if(pVectorPlutoGraphic->size() == 0)
	return;

	//just in case
	if(int(pVectorPlutoGraphic->size()) <= m_pOwner->m_iCurrentFrame)
	m_pOwner->m_iCurrentFrame = 0;

	int iCurrentFrame = m_pOwner->m_iCurrentFrame;
	PlutoGraphic *pPlutoGraphic = (*pVectorPlutoGraphic)[iCurrentFrame];
	bool bIsMNG = pPlutoGraphic->m_GraphicFormat == GR_MNG;

	//if a button doesn't have a mng as selected state (see bDisableEffects), then the png
	//used in normal state will be rendered for selected state + a blue rectangle to show the selection
	if(!bIsMNG && m_pOwner->m_GraphicToDisplay == GRAPHIC_SELECTED)
	{
		if(m_pOwner->m_vectSelectedGraphic.size() != 0)
			pVectorPlutoGraphic = &m_pOwner->m_vectSelectedGraphic;
		else
			pVectorPlutoGraphic = &m_pOwner->m_vectGraphic; //normal state

		//we have nothing to render
		if(pVectorPlutoGraphic->size() == 0)
			return;
		if(int(pVectorPlutoGraphic->size()) <= m_pOwner->m_iCurrentFrame)
			m_pOwner->m_iCurrentFrame = 0;

		iCurrentFrame = m_pOwner->m_iCurrentFrame;
		pPlutoGraphic = (*pVectorPlutoGraphic)[iCurrentFrame];
		bIsMNG = pPlutoGraphic->m_GraphicFormat == GR_MNG;
	}

	string sFileName = "";
	if(
		pPlutoGraphic->IsEmpty() && NULL != m_pOwner->m_pOrbiter->m_pCacheImageManager && pPlutoGraphic->m_Filename.length() &&
		m_pOwner->m_pOrbiter->m_pCacheImageManager->IsImageInCache(pPlutoGraphic->m_Filename, m_pOwner->m_Priority)
	)
	{
		//if we have the file in cache
		sFileName = m_pOwner->m_pOrbiter->m_pCacheImageManager->GetCacheImageFileName(pPlutoGraphic->m_Filename);
	}
	else if(pPlutoGraphic->IsEmpty() && m_pOwner->m_pOrbiter->m_sLocalDirectory.length() > 0 && pPlutoGraphic->m_Filename.length() )
	{
		//the file is in our localdrive
		sFileName = m_pOwner->m_pOrbiter->m_sLocalDirectory + pPlutoGraphic->m_Filename;
	}

	//if we don't have the file in cache or on our localdrive
	if(pPlutoGraphic->IsEmpty() && sFileName.empty())
	{
		// Request our config info
		char *pGraphicFile=NULL;
		int iSizeGraphicFile=0;

		DCE::CMD_Request_File CMD_Request_File(
			m_pOwner->m_pOrbiter->m_dwPK_Device,m_pOwner->m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,
			"orbiter/C" + StringUtils::itos(m_pOwner->m_pOrbiter->m_dwPK_Device) + "/" + pPlutoGraphic->m_Filename,
			&pGraphicFile,&iSizeGraphicFile);
		m_pOwner->m_pOrbiter->SendCommand(CMD_Request_File);

		if (!iSizeGraphicFile)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Unable to get file from server %s", pPlutoGraphic->m_Filename.c_str());
			return;
		}

		//save the image in cache
		if(NULL != m_pOwner->m_pOrbiter->m_pCacheImageManager) //cache manager is enabled ?
		{
			m_pOwner->m_pOrbiter->m_pCacheImageManager->CacheImage(pGraphicFile, iSizeGraphicFile, pPlutoGraphic->m_Filename, m_pOwner->m_Priority);
			sFileName = m_pOwner->m_pOrbiter->m_pCacheImageManager->GetCacheImageFileName(pPlutoGraphic->m_Filename);
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
						PlutoGraphic *pGraphic = m_pOwner->m_pOrbiter->Renderer()->CreateGraphic();
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

	if(bIsMNG && m_pOwner->m_pvectCurrentPlayingGraphic == NULL)
	{
		pPlutoGraphic = (*pVectorPlutoGraphic)[0];

		int iTime = 0; //hardcoding warning! don't know from where to get the framerate yet (ask Radu, libMNG)
	#ifndef WINCE
		iTime = 15;
	#endif

		bool bLoop = false; //hardcoding warning!  (ask Radu, libMNG)

		switch(m_pOwner->m_GraphicToDisplay)
		{
		case GRAPHIC_HIGHLIGHTED:
			m_pOwner->m_iTime_Highlighted = iTime;
			m_pOwner->m_bLoop_Highlighted = bLoop;
			break;
		case GRAPHIC_SELECTED:
			m_pOwner->m_iTime_Selected = iTime;
			m_pOwner->m_bLoop_Selected = bLoop;
			break;
		case GRAPHIC_NORMAL:
			m_pOwner->m_iTime_Background = iTime;
			m_pOwner->m_bLoop_Background = bLoop;
			break;
			//todo alternate graphics ?
		}

		m_pOwner->m_iCurrentFrame = 1;

		//schedule next frame for animation
		m_pOwner->m_pvectCurrentPlayingGraphic = m_pOwner->m_pvectCurrentGraphic;
		m_pOwner->m_GraphicToPlay = m_pOwner->m_GraphicToDisplay;
		m_pOwner->m_pOrbiter->CallMaintenanceInMiliseconds( iTime, (OrbiterCallBack) &Orbiter::PlayMNG_CallBack, this , pe_NO );
	}

	if(!pPlutoGraphic->IsEmpty())
	m_pOwner->m_pOrbiter->Renderer()->RenderGraphic(pPlutoGraphic, rectTotal, bDisableAspectRatio, point);
	#ifdef DEBUG
	else
	g_pPlutoLogger->Write(LV_STATUS, "No graphic to render for object %s", m_pOwner->m_ObjectID.c_str());
	#endif

	if(!bIsMNG && m_pOwner->m_GraphicToDisplay == GRAPHIC_SELECTED)
		m_pOwner->m_pOrbiter->SelectObject(m_pOwner, point);
}

/*virtual*/ void ObjectRenderer::RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	if(!PreRenderActions(pObj_Screen, point))
		return;

	switch(m_pOwner->m_ObjectType)
	{
	case DESIGNOBJTYPE_Floorplan_CONST:
		m_pOwner->m_pOrbiter->RenderFloorplan(m_pOwner,pObj_Screen, point);
		break; // Render the child objects with their text

	case DESIGNOBJTYPE_Web_Browser_CONST:
		//      LocalHandleWebWindow( pObj,  rectTotal );
		break;

	case DESIGNOBJTYPE_App_Desktop_CONST:
	case DESIGNOBJTYPE_wxWidgets_Applet_CONST:
		if(m_pOwner->m_rPosition.Width == 0 && m_pOwner->m_rPosition.Height == 0 && m_pOwner->m_rPosition.X == 0 && m_pOwner->m_rPosition.Y == 0)
			m_pOwner->m_pOrbiter->RenderDesktop( m_pOwner, PlutoRectangle( 0, 0, -1, -1 ), point );  // Full screen
		else
			m_pOwner->m_pOrbiter->RenderDesktop( m_pOwner, m_pOwner->m_rPosition, point );
		break;

#ifdef PRONTO
	case DESIGNOBJTYPE_Pronto_File_CONST:
		if ( m_pOwner->m_pCCF == NULL )
		{
			if ( !m_pOwner->m_bInitialPaint )
			{
				string ccf = m_pOwner->GetParameterValue( C_PARAMETER_PRONTO_FILE_CONST );
				if( ccf.length(  ) == 0 && m_pOwner->m_bDeleteThis || ccf.find( '~' )==string::npos ) ccf = "3365~I49/ccf/1.ccf";
				string::size_type pos = 0;
				int IRDevice = atoi( StringUtils::Tokenize( ccf,  "~",  pos ).c_str(  ) );
				string fn = ccf.substr( pos );

				Message *pMessage=m_pOwner->m_pcRequestSocket->SendReceiveMessage( new Message( m_dwPK_Device,  DEVICEID_DCEROUTER,  PRIORITY_NORMAL,  MESSAGETYPE_FILEREQUEST,  0,  1,  1,  fn.c_str(  ) ) );
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
				m_pOwner->m_AutoInvalidateTime = clock();
			}
		}
		if ( m_pOwner->m_pCCF )
		{
			m_pOwner->m_pCCF->RenderPronto( XOffset,  YOffset, point );
		}
		break;
#endif

	case DESIGNOBJTYPE_Broadcast_Video_CONST:
		if ( m_pOwner->m_bOnScreen && !m_pOwner->m_pOrbiter->m_bAlreadyQueuedVideo )
		{
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS, "Scheduling object @%p: %s", this, m_pOwner->m_ObjectID.c_str());
#endif

			// If we've got no current graphic, such as during the first redraw since ObjectOnScreen
			// deletes it, then request a video frame right away, otherwise we're redrawing
			// and should wait the correct interval

			// Don't purge existing callbacks since there can be multiple frames on the screen
			m_pOwner->m_pOrbiter->CallMaintenanceInMiliseconds( m_pOwner->m_vectGraphic.size()==0 ? 0 : m_pOwner->m_pOrbiter->m_iVideoFrameInterval, &Orbiter::GetVideoFrame, NULL, pe_ALL );
			m_pOwner->m_pOrbiter->m_bAlreadyQueuedVideo=true;  // Only schedule once -- that will redraw all video frames
		}
		else
		{
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS, "Not Scheduling object @%p: %s because is not on screen.", this, m_pOwner->m_ObjectID.c_str());
#endif
		}

		break;
	}

	PostRenderActions(pObj_Screen, point);
}

/*virtual*/ bool ObjectRenderer::PreRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	if(m_pOwner->m_ObjectType == DESIGNOBJTYPE_wxWidgets_Applet_CONST)
	{
		CallBackData *pCallBackData = m_pOwner->m_pOrbiter->m_pScreenHandler->m_mapCallBackData_Find(cbOnDialogRefresh);
		if(pCallBackData)
		{
			PositionCallBackData *pPositionData = (PositionCallBackData *)pCallBackData;
			pPositionData->m_rectPosition = m_pOwner->m_rPosition;
		}

		if(m_pOwner->m_pOrbiter->ExecuteScreenHandlerCallback(cbOnDialogRefresh))
			return false;
	}

	if(m_pOwner->m_pDesignObj_Orbiter_TiedTo)
	{
		m_pOwner->m_bHidden = m_pOwner->m_pDesignObj_Orbiter_TiedTo->IsHidden();
		if(
			(m_pOwner->m_iRowTiedTo == -1 && m_pOwner->m_iColumnTiedTo == -1) || 
			m_pOwner->m_pDesignObj_Orbiter_TiedTo->m_ObjectType != DESIGNOBJTYPE_Datagrid_CONST
			)
		{
			if(m_pOwner->m_pDesignObj_Orbiter_TiedTo == m_pOwner->m_pOrbiter->m_pObj_Highlighted)
				m_pOwner->m_GraphicToDisplay = GRAPHIC_HIGHLIGHTED;
			else
				m_pOwner->m_GraphicToDisplay = m_pOwner->m_pDesignObj_Orbiter_TiedTo->m_GraphicToDisplay;
		}
		else
		{
			DesignObj_DataGrid *pObjGrid = (DesignObj_DataGrid *) m_pOwner->m_pDesignObj_Orbiter_TiedTo;

			// We must be pointing to a datagrid,  and we need to bind to the highlighted row
			if(
				(m_pOwner->m_iRowTiedTo == -1 || m_pOwner->m_iRowTiedTo == pObjGrid->m_iHighlightedRow) &&
				(m_pOwner->m_iColumnTiedTo == -1 || m_pOwner->m_iColumnTiedTo == pObjGrid->m_iHighlightedColumn)
				)
				m_pOwner->m_GraphicToDisplay = GRAPHIC_HIGHLIGHTED;
			else
				m_pOwner->m_GraphicToDisplay = GRAPHIC_NORMAL;
		}
	}

	// See if this object is only visible for a given state
	if(m_pOwner->m_sVisibleState.size())
	{
		if(  
			(
			(m_pOwner == m_pOwner->m_pOrbiter->m_pObj_Highlighted || m_pOwner->m_GraphicToDisplay==GRAPHIC_HIGHLIGHTED) && 
			m_pOwner->m_sVisibleState.find("H")==string::npos) ||
			(m_pOwner->m_GraphicToDisplay==GRAPHIC_SELECTED && m_pOwner->m_sVisibleState.find("S")==string::npos) ||
			(m_pOwner->m_GraphicToDisplay==GRAPHIC_NORMAL && m_pOwner->m_sVisibleState.find("N")==string::npos) ||
			(m_pOwner->m_GraphicToDisplay>0 && m_pOwner->m_sVisibleState.find(StringUtils::itos(m_pOwner->m_GraphicToDisplay)) == string::npos
			)
			)
			return false;
	}

	if ( m_pOwner->m_bHidden || m_pOwner->m_rPosition.Width==0 || m_pOwner->m_rPosition.Height==0 )
	{
#ifdef DEBUG
		g_pPlutoLogger->Write( LV_STATUS, "object: %s  not visible: %d", m_pOwner->m_ObjectID.c_str(), (int) m_pOwner->m_bHidden );
#endif
		if(m_pOwner->m_pOrbiter->m_bShowShortcuts && m_pOwner->m_iPK_Button)
			m_pOwner->m_pOrbiter->Renderer()->RenderShortcut(m_pOwner);

		return false;
	}

	PROFILE_START( ctObj )
		PlutoRectangle rectBackground = m_pOwner->m_rBackgroundPosition;
	PlutoRectangle rectTotal = m_pOwner->m_rPosition;

	if(
		(m_pOwner == m_pOwner->m_pOrbiter->m_pObj_Highlighted || m_pOwner->m_GraphicToDisplay == GRAPHIC_HIGHLIGHTED ||
		(m_pOwner->m_pOrbiter->m_pObj_Highlighted && m_pOwner->m_bTabStop && m_pOwner == m_pOwner->m_pOrbiter->m_pObj_Highlighted->m_pParentObject)
		) && // If I'm also a tab stop, and my child is a highlighted object, leave me highlighted too
		m_pOwner->m_vectHighlightedGraphic.size() 
		)
	{
		m_pOwner->m_pvectCurrentGraphic = &(m_pOwner->m_vectHighlightedGraphic);

		//we'll need to do the highlighting here, since we need the normal surface for un-highlighting
		//we won't do it on RealRedraw for this kind of objects
		if(NULL != m_pOwner->m_pOrbiter->m_pObj_Highlighted && m_pOwner->m_pOrbiter->m_pObj_Highlighted != m_pOwner->m_pOrbiter->m_pObj_Highlighted_Last)
		{
			m_pOwner->m_pOrbiter->m_pObj_Highlighted_Last = m_pOwner->m_pOrbiter->m_pObj_Highlighted;
			m_pOwner->m_pOrbiter->Renderer()->DoHighlightObject();
		}
	}
	else if(m_pOwner->m_GraphicToDisplay == GRAPHIC_SELECTED && m_pOwner->m_vectSelectedGraphic.size())
		m_pOwner->m_pvectCurrentGraphic = &(m_pOwner->m_vectSelectedGraphic);
	else if(m_pOwner->m_GraphicToDisplay >= 1 && m_pOwner->m_GraphicToDisplay <= int(m_pOwner->m_vectAltGraphics.size()))
		m_pOwner->m_pvectCurrentGraphic = &(m_pOwner->m_vectAltGraphics[m_pOwner->m_GraphicToDisplay - 1]);
	else // Something that was invalid was chosen,  or just the normal version
		m_pOwner->m_pvectCurrentGraphic = &(m_pOwner->m_vectGraphic);

	if(!m_pOwner->m_pvectCurrentGraphic && m_pOwner->m_GraphicToDisplay != GRAPHIC_NORMAL  )
		m_pOwner->m_pvectCurrentGraphic = &(m_pOwner->m_vectGraphic);

	if( m_pOwner->m_bCustomRender )
	{
		CallBackData *pCallBackData = m_pOwner->m_pOrbiter->m_pScreenHandler->m_mapCallBackData_Find(cbOnCustomRender);
		if(pCallBackData)
		{
			RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pCallBackData;
			pRenderScreenCallBackData->m_nPK_Screen = m_pOwner->m_pOrbiter->CurrentScreen();
			pRenderScreenCallBackData->m_pObj = m_pOwner; 
		}
		m_pOwner->m_pOrbiter->ExecuteScreenHandlerCallback(cbOnCustomRender);
		return false;
	}

	// This is somewhat of a hack, but we don't have a clean method for setting the graphics on the user & location buttons to
	if( m_pOwner->m_bIsBoundToUser )
	{
		vector<PlutoGraphic*> *pvectGraphic = m_pOwner->m_pOrbiter->m_mapUserIcons[m_pOwner->m_pOrbiter->m_dwPK_Users];

		// Put this here to debug why sometimes the user and room aren't selected when the screen first appears
		g_pPlutoLogger->Write(LV_STATUS,"Orbiter::RenderObject obj %s bound to user %d, got ptr %p",
			m_pOwner->m_ObjectID.c_str(),m_pOwner->m_pOrbiter->m_dwPK_Users,pvectGraphic);

		if( pvectGraphic )
			m_pOwner->m_pvectCurrentGraphic = pvectGraphic;
		if( m_pOwner->m_pvectCurrentGraphic )
			RenderGraphic(rectTotal, m_pOwner->m_bDisableAspectLock, point);
	}
	else if( m_pOwner->m_bIsBoundToLocation )
	{
		if( m_pOwner->m_vectDesignObjText.size()==1 )
		{
			DesignObjText *pText = *(m_pOwner->m_vectDesignObjText.begin());
			pText->m_sText = m_pOwner->m_pOrbiter->m_pLocationInfo ? m_pOwner->m_pOrbiter->m_pLocationInfo->Description : "";
		}
		else
		{
			if( m_pOwner->m_pOrbiter->m_pLocationInfo && m_pOwner->m_pOrbiter->m_pLocationInfo->m_pvectGraphic )
				m_pOwner->m_pvectCurrentGraphic = m_pOwner->m_pOrbiter->m_pLocationInfo->m_pvectGraphic;
			if( m_pOwner->m_pvectCurrentGraphic )
				RenderGraphic(rectTotal, m_pOwner->m_bDisableAspectLock, point);
		}
	}
	// Maybe we're showing a non standard state
	else if(  m_pOwner->m_pGraphicToUndoSelect && m_pOwner->m_GraphicToDisplay==GRAPHIC_NORMAL  )
	{
		vector<PlutoGraphic*> *pvectGraphic_Hold = m_pOwner->m_pvectCurrentGraphic;
		vector<PlutoGraphic*> vectGraphicToUndoSelect;
		vectGraphicToUndoSelect.push_back(m_pOwner->m_pGraphicToUndoSelect);
		m_pOwner->m_pvectCurrentGraphic = &vectGraphicToUndoSelect;
		RenderGraphic(rectTotal, m_pOwner->m_bDisableAspectLock, point);
		m_pOwner->m_pvectCurrentGraphic = pvectGraphic_Hold;

		vectGraphicToUndoSelect.clear();
		delete m_pOwner->m_pGraphicToUndoSelect;
		m_pOwner->m_pGraphicToUndoSelect=NULL;
	}
	else if(  m_pOwner->m_pvectCurrentGraphic  )
		RenderGraphic(rectTotal, m_pOwner->m_bDisableAspectLock, point );

	return true; //can continue to render
}

/*virtual*/ bool ObjectRenderer::PostRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	PROFILE_STOP( ctObj,  m_ObjectID.c_str(  ) )
		DesignObj_DataList::reverse_iterator iHao;

	for( iHao=m_pOwner->m_ChildObjects.rbegin(  ); iHao != m_pOwner->m_ChildObjects.rend(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  !m_pOwner->m_pGraphicToUndoSelect || m_pOwner->m_GraphicToDisplay!=GRAPHIC_NORMAL  )
			continue;
		pDesignObj_Orbiter->RenderObject(pObj_Screen, point);
	}

	for( iHao=m_pOwner->m_ChildObjects.rbegin(  ); iHao != m_pOwner->m_ChildObjects.rend(  ); ++iHao )
	{
		// Hold back the tied to's and datagrids
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  pDesignObj_Orbiter->m_pDesignObj_Orbiter_TiedTo || pDesignObj_Orbiter->m_bContainsDataGrid  )
			continue;
		pDesignObj_Orbiter->RenderObject(pObj_Screen, point);
	}
	for( iHao=m_pOwner->m_ChildObjects.rbegin(  ); iHao != m_pOwner->m_ChildObjects.rend(  ); ++iHao )
	{
		// Let the to's go through,  as long as there's no datagrid
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  !pDesignObj_Orbiter->m_pDesignObj_Orbiter_TiedTo || pDesignObj_Orbiter->m_bContainsDataGrid  )
			continue;
		pDesignObj_Orbiter->RenderObject(pObj_Screen, point);
	}
	for( iHao=m_pOwner->m_ChildObjects.rbegin(  ); iHao != m_pOwner->m_ChildObjects.rend(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  !pDesignObj_Orbiter->m_bContainsDataGrid  )
			continue;
		pDesignObj_Orbiter->RenderObject(pObj_Screen, point);
	}

	VectorDesignObjText::iterator iText;
	for( iText=m_pOwner->m_vectDesignObjText.begin(  ); iText != m_pOwner->m_vectDesignObjText.end(  ); ++iText )
	{
		DesignObjText *pText = *iText;
		if(  pText->m_bPreRender  )
			continue;
		PROFILE_START( ctText );
		TextStyle *pTextStyle = pText->m_mapTextStyle_Find( 0 );
		string TextToDisplay = m_pOwner->m_pOrbiter->SubstituteVariables(m_pOwner->m_pOrbiter->SubstituteVariables(pText->m_sText, pText->m_pObject, 0, 0), pText->m_pObject, 0, 0);
		m_pOwner->m_pOrbiter->Renderer()->RenderText(TextToDisplay, pText, pTextStyle, point);
		PROFILE_STOP( ctText,  "Text ( obj below )" );
	}
	if(m_pOwner->m_pFloorplanObject && m_pOwner->m_pOrbiter->m_mapDevice_Selected.find(m_pOwner->m_pFloorplanObject->PK_Device) != m_pOwner->m_pOrbiter->m_mapDevice_Selected.end() )
	{
		int i = 0;
		for(i = 0; i < 4; ++i)
			m_pOwner->m_pOrbiter->Renderer()->HollowRectangle(
				point.X + m_pOwner->m_rBackgroundPosition.X-i, 
				point.Y + m_pOwner->m_rBackgroundPosition.Y-i, 
				m_pOwner->m_rBackgroundPosition.Width+i+i, 
				m_pOwner->m_rBackgroundPosition.Height+i+i,
				(i==1 || i==2 ? PlutoColor::Black() : PlutoColor::White())
			);

		//force an update because the object boundaries are not respected
		PlutoRectangle rect(point.X + m_pOwner->m_rBackgroundPosition.X-i, point.Y + m_pOwner->m_rBackgroundPosition.Y-i, m_pOwner->m_rBackgroundPosition.Width+i+i, m_pOwner->m_rBackgroundPosition.Height+i+i);
		m_pOwner->m_pOrbiter->Renderer()->UpdateRect(rect, NULL != m_pOwner->m_pOrbiter->m_pActivePopup ? m_pOwner->m_pOrbiter->m_pActivePopup->m_Position : PlutoPoint(0, 0));
	}

	if(m_pOwner->m_pOrbiter->m_bShowShortcuts && m_pOwner->m_iPK_Button)
		m_pOwner->m_pOrbiter->Renderer()->RenderShortcut(m_pOwner);

	return true;
}

//-----------------------------------------------------------------------------------------------------
std::string ObjectRenderer::GetLocalDirectory()
{
	return m_pOwner->m_pOrbiter->m_sLocalDirectory;
}
//-----------------------------------------------------------------------------------------------------
CacheImageManager *ObjectRenderer::GetCacheImageManager()
{
	return m_pOwner->m_pOrbiter->m_pCacheImageManager;
}
//-----------------------------------------------------------------------------------------------------
