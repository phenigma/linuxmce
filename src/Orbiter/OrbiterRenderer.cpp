#include "OrbiterRenderer.h"
#include "Simulator.h"
#include "ScreenHistory.h"
#include "ScreenHandler.h"
#include "MouseBehavior.h"
#include "OrbiterRendererFactory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "../pluto_main/Define_VertAlignment.h" 
#include "../pluto_main/Define_HorizAlignment.h" 
#include <memory>
//-----------------------------------------------------------------------------------------------------
using namespace std;
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
//progress bar custom settings
#define PROGRESS_MAX 100
#define PROGRESS_OPTIMAL_SIZE 300
#define PROGRESS_UPDATE_STEP 10
#define PROGRESS_BOTTOM_DISTANCE 50
#define PROGRESS_HEIGHT 20
//-----------------------------------------------------------------------------------------------------
namespace ProgressBar
{
	PlutoColor TextColor(0xDD, 0xDD, 0xDD);
	PlutoColor TextShadowColor = PlutoColor::Black();
	PlutoColor BorderColor(0xB5, 0xB5, 0xB5);
	PlutoColor FillBackgroundColor(0x66, 0x66, 0x66);
	PlutoColor ProgressStartColor(0x10, 0x76, 0xE4);
	PlutoColor ProgressEndColor(0x3D, 0xB2, 0x14);
};
using namespace ProgressBar;
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer::OrbiterRenderer(Orbiter *pOrbiter) : m_pOrbiter(pOrbiter)
{
	ConfigureDefaultTextStyle();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterRenderer::~OrbiterRenderer()
{
}
//-----------------------------------------------------------------------------------------------------
Orbiter *OrbiterRenderer::OrbiterLogic()
{
	return m_pOrbiter;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::ConfigureDefaultTextStyle()
{
	//used to render some text
	m_spTextStyle.reset(new TextStyle());
	m_spTextStyle->m_bBold = true;
	m_spTextStyle->m_bItalic = false;
	m_spTextStyle->m_bUnderline = false;
	m_spTextStyle->m_ForeColor = PlutoColor::Black();
	m_spTextStyle->m_sFont = "Arial";
	m_spTextStyle->m_iPixelHeight = 15;
	m_spTextStyle->m_iPK_VertAlignment = VERTALIGNMENT_Middle_CONST;
	m_spTextStyle->m_iPK_HorizAlignment = HORIZALIGNMENT_Center_CONST;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::ClipRectangle(int& x, int& y, int& width, int& height)
{
	PlutoRectangle rect(x, y, width, height);
	ClipRectangle(rect);

	x = rect.X;
	y = rect.Y;
	width = rect.Width;
	height = rect.Height;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::ClipRectangle(PlutoRectangle &rect)
{
	if( rect.X >= OrbiterLogic()->m_iImageWidth )
		rect.X = OrbiterLogic()->m_iImageWidth-1;
	if( rect.X <= 0)
		rect.X = 0;
	if( rect.Y <= 0)
		rect.Y = 0;
	if( rect.Y >= OrbiterLogic()->m_iImageHeight )
		rect.Y = OrbiterLogic()->m_iImageHeight-1;

	if(rect.Width <= 0)
		rect.Width = 1;

	if(rect.Height <= 0)
		rect.Height = 1;

	if(rect.X + rect.Width >= OrbiterLogic()->m_iImageWidth && rect.Width > 0)
		rect.Width = OrbiterLogic()->m_iImageWidth - rect.X - 1;

	if(rect.Y + rect.Height >= OrbiterLogic()->m_iImageHeight && rect.Height > 0)
		rect.Height = OrbiterLogic()->m_iImageHeight - rect.Y - 1;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ int OrbiterRenderer::PromptUser(string sPrompt,int iTimeoutSeconds,map<int,string> *p_mapPrompts)
{
#ifndef WIN32
	map<int,int> mapResponse;
	cout << sPrompt << endl;
	int iCount=1;

	if( p_mapPrompts )
	{
		for(map<int,string>::iterator it=p_mapPrompts->begin();it!=p_mapPrompts->end();++it)
		{
			mapResponse[iCount]=it->first;
			cout << StringUtils::itos(iCount++) + ". " + it->second << endl;
		}

		cout << endl
			<< "Please choose: ";
	}
	else
		cout << "Press Enter to continue";

	int iResponse=0;
	while(true)
	{
		cin >> iResponse;
		if( !p_mapPrompts )
			return 0;
		if( iResponse>0 && iResponse<=int(mapResponse.size()) )
			return mapResponse[iResponse];
		cout << endl << "Invalid.  Please try again: ";
	}
#endif

	return PROMPT_CANCEL;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ int OrbiterRenderer::PromptFor(string sToken)
{
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, OrbiterLogic()->m_sIPAddress);

	string sResults;
	DCE::CMD_Get_Orbiter_Options_DT CMD_Get_Orbiter_Options_DT(OrbiterLogic()->m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
		sToken,&sResults);

	CMD_Get_Orbiter_Options_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse = event_Impl.SendReceiveMessage( CMD_Get_Orbiter_Options_DT.m_pMessage );
	if( !pResponse || pResponse->m_dwID != 0 )
	{
		if(pResponse)
			delete pResponse;

		PromptUser("Sorry.  There is a problem getting the list of " + sToken);
		return 0;
	}
	CMD_Get_Orbiter_Options_DT.ParseResponse( pResponse );
	delete pResponse;

	map<int,string> mapResponse;
	vector<string> Choices;
	StringUtils::Tokenize(sResults,"\n",Choices);
	if( sToken=="Size" )
		mapResponse[0]="Default";

	for(vector<string>::iterator it = Choices.begin(); it != Choices.end(); ++it)
	{
		string sChoise = *it;
		string::size_type pos=0;
		int Choice = atoi(StringUtils::Tokenize(sChoise,"\t",pos).c_str());
		string sDescription = StringUtils::Tokenize(sChoise,"\t",pos);

		if( Choice && sDescription.size() )
			mapResponse[Choice]=sDescription;
	}

	if(Simulator::GetInstance()->PreseededInstallValueDefined(sToken))
	{
		int nValue = Simulator::GetInstance()->GetPreseededInstallValue(sToken);

		if(mapResponse.find(nValue) != mapResponse.end())
            return nValue;
		else if(mapResponse.size())
			return mapResponse.begin()->first;
	}

	return PromptUser("Please select the " + sToken,0,&mapResponse);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterRenderer::DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress)
{
	return DisplayProgress(sMessage, nProgress);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::RenderScreen(bool bRenderGraphicsOnly)
{
	if(!OrbiterLogic()->m_pScreenHistory_Current || !OrbiterLogic()->m_pScreenHistory_Current->GetObj())
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Got attempt to render null screen");
		return;
	}

	if(!bRenderGraphicsOnly)
	{
		CallBackData *pCallBackData = OrbiterLogic()->m_pScreenHandler->m_mapCallBackData_Find(cbOnRenderScreen);
		if(pCallBackData)
		{
			RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pCallBackData;
			pRenderScreenCallBackData->m_nPK_Screen = OrbiterLogic()->m_pScreenHistory_Current->PK_Screen();
			pRenderScreenCallBackData->m_pObj = OrbiterLogic()->m_pScreenHistory_Current->GetObj();
		}

		OrbiterLogic()->ExecuteScreenHandlerCallback(cbOnRenderScreen);
	}

#ifndef WINCE
	g_pPlutoLogger->Write( LV_STATUS, "Render screen: %s", OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(  ) );
#endif

#if ( defined( PROFILING ) )
	clock_t clkStart = clock();
#endif
	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );

	if (OrbiterLogic()->m_pBackgroundImage)
	{
		RenderGraphic(OrbiterLogic()->m_pBackgroundImage, PlutoRectangle(0, 0, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight), false);
	}

	if ( OrbiterLogic()->m_pScreenHistory_Current  )
	{
		OrbiterLogic()->RenderObject( OrbiterLogic()->m_pScreenHistory_Current->GetObj(),  OrbiterLogic()->m_pScreenHistory_Current->GetObj());
	}

	for(list<class PlutoPopup*>::iterator it=OrbiterLogic()->m_listPopups.begin();it!=OrbiterLogic()->m_listPopups.end();++it)
		OrbiterLogic()->RenderPopup(*it, (*it)->m_Position);

	if( OrbiterLogic()->m_pScreenHistory_Current  )
	{
		for(list<class PlutoPopup*>::iterator it=OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_listPopups.begin();
			it!=OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_listPopups.end();++it)
			OrbiterLogic()->RenderPopup(*it, (*it)->m_Position);
	}

	cm.Release(  );
#if ( defined( PROFILING ) )
	clock_t clkFinished = clock();
	if(  OrbiterLogic()->m_pScreenHistory_Current   )
	{
		g_pPlutoLogger->Write( LV_CONTROLLER, "Render screen: %s took %d ms",
			OrbiterLogic()->m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str(  ), clkFinished-clkStart );
	}
#endif

	if( bRenderGraphicsOnly )
		return;

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_STATUS, "Render screen: %s finished", OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(  ) );
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::ShowProgress(int nPercent)
{
	static int nCount = 0;
	if(++nCount % PROGRESS_UPDATE_STEP)
		return;

	int nProgressWidth = OrbiterLogic()->m_iImageWidth < PROGRESS_OPTIMAL_SIZE ? OrbiterLogic()->m_iImageWidth - PROGRESS_MAX : PROGRESS_OPTIMAL_SIZE;
	PlutoRectangle rect(
		(OrbiterLogic()->m_iImageWidth - nProgressWidth) / 2, OrbiterLogic()->m_iImageHeight - PROGRESS_BOTTOM_DISTANCE, 
		nProgressWidth, PROGRESS_HEIGHT
		);

	SolidRectangle(rect.Left(), rect.Top(), rect.Width, rect.Height, BorderColor);
	SolidRectangle(rect.Left() + 1, rect.Top() + 1, rect.Width - 3, rect.Height - 3, FillBackgroundColor);
	SolidRectangle(rect.Left() + 1, rect.Top() + 1, rect.Width * nPercent / PROGRESS_MAX - 3, rect.Height - 3, 
		PlutoColor(
		ProgressStartColor.R() + nPercent * (ProgressEndColor.R() - ProgressStartColor.R()) / PROGRESS_MAX, 
		ProgressStartColor.G() + nPercent * (ProgressEndColor.G() - ProgressStartColor.G()) / PROGRESS_MAX, 
		ProgressStartColor.B() + nPercent * (ProgressEndColor.B() - ProgressStartColor.B()) / PROGRESS_MAX
		)
		);

	DesignObjText text;
	string sText = StringUtils::ltos(nPercent) + "%";

	BatchedTextRendering(false);

	m_spTextStyle->m_ForeColor = TextShadowColor;
	text.m_rPosition = PlutoRectangle(rect.Left() + 1, rect.Top() + 1, rect.Width, rect.Height);
	RenderText(sText, &text, m_spTextStyle.get());

	m_spTextStyle->m_ForeColor = TextColor;
	text.m_rPosition = rect;
	RenderText(sText, &text, m_spTextStyle.get());

	BatchedTextRendering(true);

	UpdateScreen();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::UpdateScreen()
{
	PlutoRectangle rect(0, 0, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight);
	UpdateRect(rect);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer::OnReload()
{
	OnQuit();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::DoHighlightObject()
{
	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );  // Protect the highlighed object
	if(sbNoSelection == OrbiterLogic()->m_nSelectionBehaviour || !OrbiterLogic()->m_pObj_Highlighted->m_bOnScreen )
		return;

	UnHighlightObject();

	if( !OrbiterLogic()->m_pObj_Highlighted )
		return;

	OrbiterLogic()->ExecuteCommandsInList( &OrbiterLogic()->m_pObj_Highlighted->m_Action_HighlightList, 
		OrbiterLogic()->m_pObj_Highlighted, smHighlight, 0, 0 );

	if( OrbiterLogic()->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		OrbiterLogic()->GetDataGridHighlightCellCoordinates((DesignObj_DataGrid *) OrbiterLogic()->m_pObj_Highlighted,OrbiterLogic()->m_rectLastHighlight);
	else
		OrbiterLogic()->m_rectLastHighlight = OrbiterLogic()->m_pObj_Highlighted->GetHighlightRegion();

	OrbiterLogic()->m_rectLastHighlight.X += OrbiterLogic()->m_pObj_Highlighted->m_pPopupPoint.X;
	OrbiterLogic()->m_rectLastHighlight.Y += OrbiterLogic()->m_pObj_Highlighted->m_pPopupPoint.Y;

	OrbiterLogic()->m_rectLastHighlight.Width++;  // GetBackground always seems to be 1 pixel to little
	OrbiterLogic()->m_rectLastHighlight.Height++;

	OrbiterLogic()->m_pGraphicBeforeHighlight = 
		OrbiterLogic()->m_pOrbiterRenderer->GetBackground(OrbiterLogic()->m_rectLastHighlight);

	PlutoGraphic *pPlutoGraphic = OrbiterLogic()->m_pGraphicBeforeHighlight->GetHighlightedVersion();
	if(pPlutoGraphic)
	{
		OrbiterLogic()->m_pOrbiterRenderer->RenderGraphic(pPlutoGraphic, OrbiterLogic()->m_rectLastHighlight);
		delete pPlutoGraphic;
		pPlutoGraphic = NULL;
	}

	for(int i = 0; i < 4; i++)
        OrbiterLogic()->m_pOrbiterRenderer->HollowRectangle(
			OrbiterLogic()->m_rectLastHighlight.X + i, OrbiterLogic()->m_rectLastHighlight.Y + i,
			OrbiterLogic()->m_rectLastHighlight.Width - 2 * i - 2, OrbiterLogic()->m_rectLastHighlight.Height - 2 * i - 2,
			i < 2 ? PlutoColor::Red() : PlutoColor::White()
		);

	UpdateRect(OrbiterLogic()->m_rectLastHighlight);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer::UnHighlightObject(bool bDeleteOnly)
{
	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );  // Protect the highlighed object

	if( !OrbiterLogic()->m_pGraphicBeforeHighlight )
		return;

	if( !bDeleteOnly && OrbiterLogic()->m_pObj_Highlighted)
	{
		RenderGraphic(OrbiterLogic()->m_pGraphicBeforeHighlight, OrbiterLogic()->m_rectLastHighlight);
		UpdateRect(OrbiterLogic()->m_rectLastHighlight);
	}

	delete OrbiterLogic()->m_pGraphicBeforeHighlight;
	OrbiterLogic()->m_pGraphicBeforeHighlight=NULL;
}
//-----------------------------------------------------------------------------------------------------
