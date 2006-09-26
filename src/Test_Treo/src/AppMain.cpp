/******************************************************************************
 *
 * Copyright (c) 1999-2004 PalmSource, Inc. All rights reserved.
 *
 * File: AppMain.c
 *
 *****************************************************************************/

#include <PalmOS.h>

#include "AppResources.h"
#include "PlutoInc.h"
#include "CTestz.h"


/***********************************************************************
 *
 *	Entry Points
 *
 ***********************************************************************/


/***********************************************************************
 *
 *	Internal Constants
 *
 ***********************************************************************/
#define appFileCreator			'STRT'	// register your own at http://www.palmos.com/dev/creatorid/
#define appVersionNum			0x01
#define appPrefID				0x00
#define appPrefVersionNum		0x01


/***********************************************************************
 *
 *	Internal Functions
 *
 ***********************************************************************/


/***********************************************************************
 *
 * FUNCTION:    MainFormDoCommand
 *
 * DESCRIPTION: This routine performs the menu command specified.
 *
 * PARAMETERS:  command  - menu item id
 *
 * RETURNED:    nothing
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
static Boolean MainFormDoCommand(UInt16 command)
{
	Boolean handled = false;
	FormType * pForm;

	switch (command) {
		case MainOptionsAboutStarterApp:
			pForm = FrmInitForm(AboutForm);
			FrmDoDialog(pForm);					// Display the About Box.
			FrmDeleteForm(pForm);
			handled = true;
			break;

	}
	
	return handled;
}


/***********************************************************************
 *
 * FUNCTION:    MainFormHandleEvent
 *
 * DESCRIPTION: This routine is the event handler for the 
 *              "MainForm" of this application.
 *
 * PARAMETERS:  pEvent  - a pointer to an EventType structure
 *
 * RETURNED:    true if the event has handle and should not be passed
 *              to a higher level handler.
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
 
CTestz test;

static Boolean MainFormHandleEvent(EventType* pEvent)
{
	Boolean 	handled = false;
	FormType* 	pForm;
	
	/*
	RGBColorType colorRed = {0, 255, 0, 0};
	RGBColorType colorGreen = {0, 0, 255, 0};
	RGBColorType colorBlue = {0, 0, 0, 255};
	RGBColorType colorRedGreen = {0, 255, 255, 0};
	RGBColorType colorRedBlue = {0, 255, 0, 255};
	*/
	//RGBColorType colorGreenBlue = {0, 0, 255, 255};	
	string s = "xxx", s1="abcdef";
	TextStyle ts;
	ts.m_bBold = true;
	ts.m_bUnderline = true;
	ts.m_ForeColor = PlutoColor::Blue();
	ts.m_BackColor = PlutoColor::Gray();
	

	switch (pEvent->eType) {
		case menuEvent:
			return MainFormDoCommand(pEvent->data.menu.itemID);

		case frmOpenEvent:
			pForm = FrmGetActiveForm();
			FrmDrawForm(pForm);			
			test.Configure();
			//test.erase( );
						

			test.SolidRectangle( 0, 0, 40, 40, PlutoColor::Red() );
			
			test.SolidRectangle( 0, 40, 40, 40, PlutoColor::Green() );
			test.SolidRectangle( 0, 80, 40, 40, PlutoColor::Blue() );
			test.DrawLine( 40, 0, 80, 40, PlutoColor(255,255,0) );
			test.DrawLine( 40, 40, 80, 0, PlutoColor::Gray() );
			test.HollowRectangle( 80, 80, 40, 40, PlutoColor::Black() );
			test.RenderText( s, &ts, PlutoPoint( 120, 120 ) );
			ts.m_ForeColor = PlutoColor::Red();
			ts.m_bBold = false;
			ts.m_bUnderline = false;
			test.RenderText( s1, &ts, PlutoPoint( 200, 240 ) );
			
			handled = true;
			break;
			
//		case frmUpdateEvent:
//		    break;
			
		default:
			break;
	}
	
	return handled;
}


/***********************************************************************
 *
 * FUNCTION:    AppHandleEvent
 *
 * DESCRIPTION: This routine loads form resources and set the event
 *              handler for the form loaded.
 *
 * PARAMETERS:  event  - a pointer to an EventType structure
 *
 * RETURNED:    true if the event has handle and should not be passed
 *              to a higher level handler.
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
static Boolean AppHandleEvent(EventType* pEvent)
{
	UInt16 		formId;
	FormType* 	pForm;
	Boolean		handled = false;

	if (pEvent->eType == frmLoadEvent) {
		// Load the form resource.
		formId = pEvent->data.frmLoad.formID;
		
		pForm = FrmInitForm(formId);
		FrmSetActiveForm(pForm);

		// Set the event handler for the form.  The handler of the currently
		// active form is called by FrmHandleEvent each time is receives an
		// event.
		switch (formId) {
			case MainForm:
				FrmSetEventHandler(pForm, MainFormHandleEvent);
				break;

			default:
				break;
		}
		handled = true;
	}
	
	return handled;
}


/***********************************************************************
 *
 * FUNCTION:     AppStart
 *
 * DESCRIPTION:  Get the current application's preferences.
 *
 * PARAMETERS:   nothing
 *
 * RETURNED:     Err value errNone if nothing went wrong
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
static Err AppStart(void)
{
	FrmGotoForm(MainForm);
	return errNone;
}


/***********************************************************************
 *
 * FUNCTION:    AppStop
 *
 * DESCRIPTION: Save the current state of the application.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
static void AppStop(void)
{
	// Close all the open forms.
	FrmCloseAllForms();
}


/***********************************************************************
 *
 * FUNCTION:    AppEventLoop
 *
 * DESCRIPTION: This routine is the event loop for the application.  
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
static void AppEventLoop(void)
{
	Err			error;
	EventType	event;

	do {
		EvtGetEvent(&event, evtWaitForever);

		if (SysHandleEvent(&event))
			continue;
			
		if (MenuHandleEvent(0, &event, &error))
			continue;
			
		if (AppHandleEvent(&event))
			continue;

		FrmDispatchEvent(&event);

	} while (event.eType != appStopEvent);
}



/***********************************************************************
 *
 * FUNCTION:    PilotMain
 *
 * DESCRIPTION: This is the main entry point for the application.
 *
 * PARAMETERS:  cmd - word value specifying the launch code. 
 *              cmdPB - pointer to a structure that is associated with the launch code. 
 *              launchFlags -  word value providing extra information about the launch.
 * RETURNED:    Result of launch
 *
 * REVISION HISTORY: 
 *
 *
 ***********************************************************************/
UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	Err error = errNone;
		
	

	switch (cmd) {
		case sysAppLaunchCmdNormalLaunch:
			if ((error = AppStart()) == 0) {							
				AppEventLoop();
				AppStop();
			}
			break;

		default:
			break;
	}
	
	return error;
}


