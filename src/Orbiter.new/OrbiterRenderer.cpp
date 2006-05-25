
#include "OrbiterRenderer.h"
#include "Simulator.h"
#include "ScreenHistory.h"
#include "ScreenHandler.h"
#include "MouseBehavior.h"

#include "OrbiterRendererFactory.h"

using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"

void OrbiterRenderer::RenderScreen( bool bRenderGraphicsOnly )
{
	if( Orbiter::GetInstance()->m_bQuit )
		return;

	if( !Orbiter::GetInstance()->m_pScreenHistory_Current || !Orbiter::GetInstance()->m_pScreenHistory_Current->GetObj() )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Got attempt to render null screen: %s", Orbiter::GetInstance()->m_pScreenHistory_Current );
		return;
	}

	if( !bRenderGraphicsOnly )
	{
		CallBackData *pCallBackData = Orbiter::GetInstance()->m_pScreenHandler->m_mapCallBackData_Find(cbOnRenderScreen);
		if(pCallBackData)
		{
			RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pCallBackData;
			pRenderScreenCallBackData->m_nPK_Screen = Orbiter::GetInstance()->m_pScreenHistory_Current->PK_Screen();
			pRenderScreenCallBackData->m_pObj = Orbiter::GetInstance()->m_pScreenHistory_Current->GetObj();
		}

		Orbiter::GetInstance()->ExecuteScreenHandlerCallback(cbOnRenderScreen);
	}

#ifndef WINCE
	g_pPlutoLogger->Write( LV_STATUS, "Render screen: %s", Orbiter::GetInstance()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(  ) );
#endif

#if ( defined( PROFILING ) )
	clock_t clkStart = clock();
#endif
	PLUTO_SAFETY_LOCK( cm, Orbiter::GetInstance()->m_ScreenMutex );

	if (Orbiter::GetInstance()->m_pBackgroundImage)
	{
		RenderGraphic(Orbiter::GetInstance()->m_pBackgroundImage, PlutoRectangle(0, 0, Orbiter::GetInstance()->m_iImageWidth, Orbiter::GetInstance()->m_iImageHeight), false);
	}

	if ( Orbiter::GetInstance()->m_pScreenHistory_Current  )
	{
		Orbiter::GetInstance()->m_pScreenHistory_Current->GetObj()->RenderObject( Orbiter::GetInstance()->m_pScreenHistory_Current->GetObj());
	}

	for(list<class PlutoPopup*>::iterator it=Orbiter::GetInstance()->m_listPopups.begin();it!=Orbiter::GetInstance()->m_listPopups.end();++it)
		Orbiter::GetInstance()->RenderPopup(*it, (*it)->m_Position);

	if( Orbiter::GetInstance()->m_pScreenHistory_Current  )
	{
		for(list<class PlutoPopup*>::iterator it=Orbiter::GetInstance()->m_pScreenHistory_Current->GetObj()->m_listPopups.begin();it!=Orbiter::GetInstance()->m_pScreenHistory_Current->GetObj()->m_listPopups.end();++it)
			Orbiter::GetInstance()->RenderPopup(*it, (*it)->m_Position);
	}

	cm.Release(  );
#if ( defined( PROFILING ) )
	clock_t clkFinished = clock();
	if(  Orbiter::GetInstance()->m_pScreenHistory_Current   )
	{
		g_pPlutoLogger->Write( LV_CONTROLLER, "Render screen: %s took %d ms",
			Orbiter::GetInstance()->m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str(  ), clkFinished-clkStart );
	}
#endif

	if( bRenderGraphicsOnly )
		return;

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_STATUS, "Render screen: %s finished", Orbiter::GetInstance()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(  ) );
#endif
}


/*virtual*/ void OrbiterRenderer::SelectObject( class DesignObj_Orbiter *pObj, PlutoPoint point )
{
	if(sbNoSelection != Orbiter::GetInstance()->m_nSelectionBehaviour)
		for(int i = 0; i < 4; i++)
			HollowRectangle(
				point.X + pObj->m_rBackgroundPosition.X + i, point.Y + pObj->m_rBackgroundPosition.Y + i,
				pObj->m_rBackgroundPosition.Width - 2 * i - 2, pObj->m_rBackgroundPosition.Height - 2 * i - 2,
				i < 2 ? PlutoColor::Blue() : PlutoColor::White()
			);
}

/*virtual*/ void OrbiterRenderer::DoHighlightObject()
{
	PLUTO_SAFETY_LOCK( cm, Orbiter::GetInstance()->m_ScreenMutex );  // Protect the highlighed object
	if(sbNoSelection == Orbiter::GetInstance()->m_nSelectionBehaviour || !Orbiter::GetInstance()->m_pObj_Highlighted->m_bOnScreen )
		return;
	UnHighlightObject();

	if( !Orbiter::GetInstance()->m_pObj_Highlighted )
		return;

	Orbiter::GetInstance()->ExecuteCommandsInList( &Orbiter::GetInstance()->m_pObj_Highlighted->m_Action_HighlightList, Orbiter::GetInstance()->m_pObj_Highlighted, smHighlight, 0, 0 );

	if( Orbiter::GetInstance()->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		Orbiter::GetInstance()->GetDataGridHighlightCellCoordinates((DesignObj_DataGrid *) Orbiter::GetInstance()->m_pObj_Highlighted,Orbiter::GetInstance()->m_rectLastHighlight);
	else
		Orbiter::GetInstance()->m_rectLastHighlight = Orbiter::GetInstance()->m_pObj_Highlighted->GetHighlightRegion();

	Orbiter::GetInstance()->m_rectLastHighlight.X += Orbiter::GetInstance()->m_pObj_Highlighted->m_pPopupPoint.X;
	Orbiter::GetInstance()->m_rectLastHighlight.Y += Orbiter::GetInstance()->m_pObj_Highlighted->m_pPopupPoint.Y;

	Orbiter::GetInstance()->m_rectLastHighlight.Width++;  // GetBackground always seems to be 1 pixel to little
	Orbiter::GetInstance()->m_rectLastHighlight.Height++;

	Orbiter::GetInstance()->m_pGraphicBeforeHighlight = GetBackground(Orbiter::GetInstance()->m_rectLastHighlight);

	PlutoGraphic *pPlutoGraphic = Orbiter::GetInstance()->m_pGraphicBeforeHighlight->GetHighlightedVersion();
	if(pPlutoGraphic)
	{
		RenderGraphic(pPlutoGraphic, Orbiter::GetInstance()->m_rectLastHighlight);
		delete pPlutoGraphic;
		pPlutoGraphic = NULL;
	}

	for(int i = 0; i < 4; i++)
        HollowRectangle(
			Orbiter::GetInstance()->m_rectLastHighlight.X + i, Orbiter::GetInstance()->m_rectLastHighlight.Y + i,
			Orbiter::GetInstance()->m_rectLastHighlight.Width - 2 * i - 2, Orbiter::GetInstance()->m_rectLastHighlight.Height - 2 * i - 2,
			i < 2 ? PlutoColor::Red() : PlutoColor::White()
		);

	UpdateRect(Orbiter::GetInstance()->m_rectLastHighlight);
}

/*virtual*/ void OrbiterRenderer::UnHighlightObject( bool bDeleteOnly )
{
	PLUTO_SAFETY_LOCK( cm, Orbiter::GetInstance()->m_ScreenMutex );  // Protect the highlighed object

	if( !Orbiter::GetInstance()->m_pGraphicBeforeHighlight )
		return;

	if( !bDeleteOnly && Orbiter::GetInstance()->m_pObj_Highlighted)
	{
		RenderGraphic(Orbiter::GetInstance()->m_pGraphicBeforeHighlight, Orbiter::GetInstance()->m_rectLastHighlight);
		UpdateRect(Orbiter::GetInstance()->m_rectLastHighlight);
	}

	delete Orbiter::GetInstance()->m_pGraphicBeforeHighlight;
	Orbiter::GetInstance()->m_pGraphicBeforeHighlight=NULL;
}

int OrbiterRenderer::HandleNotOKStatus(string sStatus,string sRegenStatus,int iRegenPercent)
{
	g_pPlutoLogger->Write(LV_STATUS,"HandleNotOKStatus %s",sStatus.c_str());
	if( sStatus=="R" || sStatus=="r" )
		return MonitorRegen(Orbiter::GetInstance()->m_dwPK_Device);

	if( sStatus=="N" || ((sStatus=="D" || sStatus=="U") && !Orbiter::GetInstance()->m_dwPK_Device) )
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo};
		mapPrompts[prYes]    = "Yes - Reset it now";
		mapPrompts[prNo]     = "No - I'll do it later";
		int iResponse = PromptUser("This new Orbiter is ready to go.  But all your devices need to do a reload before you can use it.  This takes about 15 seconds.", 0, &mapPrompts);
		if( iResponse==prYes )
		{
			string sResponse;
			Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, Orbiter::GetInstance()->m_sHostName);
			event_Impl.m_pClientSocket->SendString( "RELOAD" );
			if( !event_Impl.m_pClientSocket->ReceiveString( sResponse ) || sResponse!="OK" )
			{
				Orbiter::GetInstance()->CannotReloadRouter();
				g_pPlutoLogger->Write(LV_WARNING,"Reload request denied: %s",sResponse.c_str());
			}
			Sleep(10000);
			return 2; // Retry
		}
		else
		{
			Orbiter::GetInstance()->OnQuit();
			exit(0);
		}

		return 0; // Quit
	}
	else if( sStatus=="n" )
	{
		PromptUser("Something went wrong and this orbiter's user interface wasn't created.  In Pluto Admin, go to Wizard, Devices, Orbiters and click Regen for Orbiter #" + StringUtils::itos(Orbiter::GetInstance()->m_dwPK_Device) + " and try later");
		Orbiter::GetInstance()->OnQuit();
		exit(0);
		return 0;
	}
	else if( sStatus=="D" || sStatus=="U" )
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo};
		mapPrompts[prYes]    = "Yes";
		mapPrompts[prNo]     = "No";

		int iResponse = prNo;
		if( sStatus=="D" )
			iResponse = PromptUser("Something went wrong. The device number, " + StringUtils::itos(Orbiter::GetInstance()->m_dwPK_Device) + ", doesn't seem to be an orbiter.  Reset the device number and try next time to determine it automatically?",0,&mapPrompts);
		else
			iResponse = PromptUser("Something went wrong.  The device number, " + StringUtils::itos(Orbiter::GetInstance()->m_dwPK_Device) + ", is not known to the Core.  Reset the device number and try next time to determine it automatically?",0,&mapPrompts);

		if( iResponse==prYes )
		{
			Orbiter::GetInstance()->m_dwPK_Device = 0;
			Simulator::GetInstance()->m_sDeviceID = "";
			Simulator::GetInstance()->SaveConfigurationFile();
		}
		Orbiter::GetInstance()->OnQuit();
		exit(0);
		return 0;
	}

	return 1;
}

bool OrbiterRenderer::RouterNeedsReload()
{
	g_pPlutoLogger->Write(LV_STATUS,"OrbiterRenderer::RouterNeedsReload");
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, Orbiter::GetInstance()->m_sIPAddress);
	string sStatus,sRegenStatus;
	int iRegenPercent;
	DCE::CMD_Get_Orbiter_Status_DT CMD_Get_Orbiter_Status_DT( Orbiter::GetInstance()->m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
		Orbiter::GetInstance()->m_dwPK_Device,&sStatus,&sRegenStatus,&iRegenPercent);
	CMD_Get_Orbiter_Status_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse = event_Impl.SendReceiveMessage( CMD_Get_Orbiter_Status_DT.m_pMessage );
	if( !pResponse || pResponse->m_dwID != 0 )
	{
		if(pResponse)
			delete pResponse;

		PromptUser("Sorry.  There is a problem creating the new orbiter");
		return 0;
	}
	CMD_Get_Orbiter_Status_DT.ParseResponse( pResponse );
	delete pResponse;

	HandleNotOKStatus(sStatus,sRegenStatus,iRegenPercent);
	return false;  // If the user wants to do a reload, this will have already sent the request
}

int OrbiterRenderer::DeviceIdInvalid()
{
	g_pPlutoLogger->Write(LV_STATUS,"OrbiterRenderer::DeviceIdInvalid");
	if( Orbiter::GetInstance()->m_dwPK_Device )
	{
		PromptUser("Something went wrong.  The device number, " + StringUtils::itos(Orbiter::GetInstance()->m_dwPK_Device) + ", is reported as invalid.");
		Orbiter::GetInstance()->OnQuit();
		exit(0);
		return 0;
	}

	int PK_Device = 0;
	if(Orbiter::GetInstance()->IsSelfInstallable())
	{
		//Skip prompt dialogs. Create the orbiter automatically, using preseeded install values
		PK_Device = SetupNewOrbiter();
	}
	else
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo, prCancel};
		mapPrompts[prYes]    = "Yes - This is a new Orbiter";
		mapPrompts[prNo]     = "No - There is already a Device ID for this Orbiter";
		mapPrompts[prCancel] = "Cancel";
		int iResponse = PromptUser("This seems to be a new Orbiter.  Shall I set it up for you?", 0, &mapPrompts);
		if( iResponse == prCancel || PROMPT_CANCEL == iResponse )
		{
			Orbiter::GetInstance()->OnQuit();
			exit(0);
			return 0;
		}
		if( iResponse == prYes )
			PK_Device=SetupNewOrbiter();
		else
		{
			PK_Device=PickOrbiterDeviceID();
			if(PROMPT_CANCEL == PK_Device)
			{
				Orbiter::GetInstance()->OnQuit();
				exit(0);
				return 0;
			}
		}
	}

	if( !PK_Device )
	{
		Orbiter::GetInstance()->OnQuit();
		exit(0);
	}

	return PK_Device;
}

int OrbiterRenderer::PickOrbiterDeviceID()
{
	map<int,string> mapDevices;
	Orbiter::GetInstance()->GetDevicesByCategory(DEVICECATEGORY_Orbiter_CONST,&mapDevices);
	return PromptUser("Which Orbiter is this?  Be careful.  Don't choose an Orbiter that is running on another device or it will be disconnected when this one connects.",0, &mapDevices);
}
//-----------------------------------------------------------------------------------------------------
int OrbiterRenderer::SetupNewOrbiter()
{
	g_pPlutoLogger->Write(LV_STATUS,"start SetupNewOrbiter");
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, Orbiter::GetInstance()->m_sIPAddress);
	while(true)
	{
		string sResponse;
		if( !event_Impl.m_pClientSocket->SendString("READY") ||
			!event_Impl.m_pClientSocket->ReceiveString(sResponse) ||
			sResponse.size()==0 )
			return 0;  // Something went wrong

		if( sResponse=="YES" )
			break;
		Sleep(2000);
	}

	g_pPlutoLogger->Write(LV_STATUS,"SetupNewOrbiter prompting for inputs");

	int PK_Users = PromptFor("Users");
	if( PROMPT_CANCEL == PK_Users )
		return 0;

	int PK_Room = PromptFor("Room");
	if( PROMPT_CANCEL == PK_Room )
		return 0;

	int PK_Skin = PromptFor("Skin");
	if( PROMPT_CANCEL == PK_Skin )
		return 0;

	int PK_Language = PromptFor("Language");
	if( PROMPT_CANCEL == PK_Language )
		return 0;

	map<int,string> mapResponseYNC;
	mapResponseYNC[0]="Yes";
	mapResponseYNC[1]="No";
	mapResponseYNC[2]="Cancel";

	int Width=0,Height=0;
	string sType;
#ifdef WIN32
#ifdef WINCE
	sType="CE";
#else
	sType="Windows";
#endif
	RECT rc;
	HWND hWndDesktop = ::GetDesktopWindow();
	GetWindowRect(hWndDesktop, &rc);
	Width=rc.right;
	Height=rc.bottom;
#else
	sType="Linux";
	// HOW to do this??  TODO
#endif

	int PK_Size=0;
	PK_Size=PromptFor("Size");
	if( PROMPT_CANCEL == PK_Size)
		return 0;

	int PK_Device=0;
	DCE::CMD_New_Orbiter_DT CMD_New_Orbiter_DT(Orbiter::GetInstance()->m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse, sType,
		PK_Users,Orbiter::GetInstance()->m_dwPK_DeviceTemplate,Orbiter::GetInstance()->m_sMacAddress,PK_Room,Width,Height,PK_Skin,PK_Language,PK_Size,&PK_Device);

	CMD_New_Orbiter_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse = event_Impl.SendReceiveMessage( CMD_New_Orbiter_DT.m_pMessage );
	if( !pResponse || pResponse->m_dwID != 0 )
	{
		if(pResponse)
			delete pResponse;

		g_pPlutoLogger->Write(LV_CRITICAL,"SetupNewOrbiter unable to create orbiter");
		PromptUser("Sorry.  There is a problem creating the new orbiter.  Please check the logs.");
		return 0;
	}
	CMD_New_Orbiter_DT.ParseResponse( pResponse );
	delete pResponse;

	if( !PK_Device )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"SetupNewOrbiter unable to create orbiter #2");
		PromptUser("Sorry.  Orbiter Plugin could not create the device for some reason.  Please check the logs.");
		return 0;
	}

	Simulator::GetInstance()->m_sRouterIP = Orbiter::GetInstance()->m_sIPAddress;
	Simulator::GetInstance()->m_sDeviceID = StringUtils::itos(PK_Device);
	Simulator::GetInstance()->SaveConfigurationFile();

	g_pPlutoLogger->Write(LV_STATUS,"SetupNewOrbiter new orbiter %d",PK_Device);

	if( MonitorRegen(PK_Device)==0 )  // User hit cancel
	{
		Orbiter::GetInstance()->OnQuit();
		exit(1);
		return 0; // Don't retry to load now
	}


	return PK_Device;  // Retry loading as the specified device
}
//-----------------------------------------------------------------------------------------------------
int OrbiterRenderer::MonitorRegen(int PK_Device)
{
	g_pPlutoLogger->Write(LV_STATUS,"MonitorRegen - starting");
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, Orbiter::GetInstance()->m_sIPAddress);
	while(true)
	{
		string sStatus,sRegenStatus;
		int iRegenPercent;
		DCE::CMD_Get_Orbiter_Status_DT CMD_Get_Orbiter_Status_DT( Orbiter::GetInstance()->m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
			PK_Device,&sStatus,&sRegenStatus,&iRegenPercent);

		CMD_Get_Orbiter_Status_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
		Message *pResponse = event_Impl.SendReceiveMessage( CMD_Get_Orbiter_Status_DT.m_pMessage );
		if( !pResponse || pResponse->m_dwID != 0 )
		{
			if(pResponse)
				delete pResponse;

			g_pPlutoLogger->Write(LV_CRITICAL,"MonitorRegen - unable to check status");
			DisplayProgress("",-1);
			PromptUser("Sorry.  There is a problem creating the new orbiter");
			return 0;
		}
		CMD_Get_Orbiter_Status_DT.ParseResponse( pResponse );
		delete pResponse;

		if( sStatus!="r" && sStatus!="R" )
		{
			g_pPlutoLogger->Write(LV_STATUS,"MonitorRegen - DONE");
			break;
		}

		if( DisplayProgress(sRegenStatus,iRegenPercent) )
		{
			g_pPlutoLogger->Write(LV_STATUS,"MonitorRegen - User cancelled");
			DisplayProgress("",-1);
			return 0; // Don't try to start
		}
		Sleep(5000);
	}

	DisplayProgress("",-1);
	return 2; // Try again
}
//-----------------------------------------------------------------------------------------------------
int OrbiterRenderer::PromptFor(string sToken)
{
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, Orbiter::GetInstance()->m_sIPAddress);

	string sResults;
	DCE::CMD_Get_Orbiter_Options_DT CMD_Get_Orbiter_Options_DT(Orbiter::GetInstance()->m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
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
bool OrbiterRenderer::RegenOrbiter()
{
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, Orbiter::GetInstance()->m_sIPAddress);
	string sResponse;
	DCE::CMD_Regen_Orbiter_DT CMD_Regen_Orbiter_DT( Orbiter::GetInstance()->m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
		Orbiter::GetInstance()->m_dwPK_Device,"","");
	CMD_Regen_Orbiter_DT.m_pMessage->m_eExpectedResponse = ER_DeliveryConfirmation;
	if( !event_Impl.SendMessage(CMD_Regen_Orbiter_DT.m_pMessage,sResponse) || sResponse!="OK" )
	{
		PromptUser("Sorry.  I was unable to send this message to the Core.  Please try again or use the Pluto Admin site.");
		return true;
	}
	PromptUser("The UI is being regenerated.  This will take 15-30 minutes.  If you get this same message again after the regeneration is finished, then that means the generator on the Core is too old and you will need to reset your Core so it can update itself.  Click 'OK' to monitor the progress.");
	return false;
}

int OrbiterRenderer::PromptUser(string sPrompt,int iTimeoutSeconds,map<int,string> *p_mapPrompts)
{
#ifndef WIN32
	return PROMPT_CANCEL;  // TEMP TODO HACK -- we need to implement this for Linux SDL
#endif

#ifndef WINCE
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
