/**
 *
 * @file Orbiter.cpp
 * @brief source file for the ScreenHistory, Orbiter and NeedToRender classes
 * @author
 * @todo notcommented
 * @warning do not change the code between the tags( <-dceag-...-b-><-dceag-...-e-> ) , it will be overriden by the generator ( unless '!' specified )
 *
 */

/**
 *
 * Copyright Notice goes here
 *
 */


//<-dceag-d-b->
#include "Orbiter.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DesignObj_Orbiter.h"
#include "DataGrid.h"
#include "PlutoUtils/ShapesColors.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DesignObjParameter.h"
#include "pluto_main/Define_Direction.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_VertAlignment.h"

// For DontRender
namespace DCE
{
	int g_iDontRender;
	clock_t g_cLastTime; // debug only
}

template <class T>
inline static T Dist( T x, T y )
{
	return x*x + y*y ;
}


/**
 *
 *	Constructors/Destructor
 *
 */

//<-dceag-const-b->!
Orbiter::Orbiter( int iDeviceID, string sServerAddress, string sLocalDirectory, bool bLocalMode, int iImageWidth, int iImageHeight )
		: Orbiter_Command( iDeviceID, sServerAddress, true, bLocalMode ),
		m_ScreenMutex( "rendering" ), m_VariableMutex( "variable" ), m_DatagridMutex( "datagrid" )
//<-dceag-const-e->

{
	m_sLocalDirectory = sLocalDirectory;
	m_iImageWidth = iImageWidth;
	m_iImageHeight = iImageHeight;
	m_bDisplayOn = true;
	m_bYieldScreen = false;
	m_bYieldInput = false;
	m_bRerenderScreen = false;

	m_pScreenHistory_Current = NULL;
	m_pObj_Highlighted = NULL;
	m_LastActivityTime = time( NULL );
	m_bKillSpawnedDevicesOnExit = true;

	pthread_mutexattr_init( &m_MutexAttr );
	pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	m_VariableMutex.Init( &m_MutexAttr );
	m_ScreenMutex.Init( &m_MutexAttr );
	m_DatagridMutex.Init( &m_MutexAttr );

	m_dwIDataGridRequestCounter = 0;

	if ( !m_bLocalMode )
	{
		GetEvents()->m_pClientSocket->SetReceiveTimeout( 15 );
		m_pcRequestSocket = new Orbiter_Event( iDeviceID, sServerAddress );
		m_pcRequestSocket->m_pClientSocket->SetReceiveTimeout( 15 );
	}

	m_bCaptureKeyboard_bOnOff = false;
	m_bCaptureKeyboard_bReset = false;
	m_bCaptureKeyboard_bTypePin = false;
	m_bCaptureKeyboard_bDataGrid = false;
	m_dwPK_CaptureKeyboard_Variable = 0;
	m_sCaptureKeyboard_Text = "";
	m_sCaptureKeyboard_InternalBuffer = "";
	m_pCaptureKeyboard_Text = NULL;
}

//<-dceag-dest-b->
Orbiter::~Orbiter()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
bool Orbiter::Register()
//<-dceag-reg-e->
{
	return Connect();
}

/**
 *
 *	ACCEPT OUTSIDE INPUT
 *
 */

bool Orbiter::ButtonDown( int dwPK_Button )
{
	NeedToRender render( this, "Button Down" ); // Redraw anything that was changed by this command

	class ScreenHistory *pScreenHistory = m_pScreenHistory_Current;

	bool bHandled = false;
	this->GotActivity();
	if ( dwPK_Button == BUTTON_Enter_CONST )
	{
		if ( m_pObj_Highlighted && !m_pObj_Highlighted->IsHidden() )
		{
			SelectedObject( m_pObj_Highlighted );
			bHandled = true;
		} 
		else
		{
			vector < DesignObj_DataGrid * > vectSelectedGrids; // We don't want to fire the selected objects while holding the datagrid mutex
			PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
			for( size_t s = 0; s < m_vectObjs_GridsOnScreen.size(); ++s )
			{
				DesignObj_DataGrid *pDesignObj_DataGrid = m_vectObjs_GridsOnScreen[s];
				if ( !pDesignObj_DataGrid->IsHidden() && pDesignObj_DataGrid->m_pDataGridTable && pDesignObj_DataGrid->m_sExtraInfo.find( 'C' ) != string::npos )
				{
					// We've got a datagrid that is capturing scrolling, and we need to select the cell
					DataGridCell *pCell = pDesignObj_DataGrid->m_pDataGridTable->GetData( pDesignObj_DataGrid->m_iHighlightedColumn + pDesignObj_DataGrid->m_GridCurCol, pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow );
					if ( pCell ) 
					{
						// First select the cell, then select the grid object, passing in -1 values so it won't reselect the cell
						SelectedGrid( pDesignObj_DataGrid, pCell );
						vectSelectedGrids.push_back( pDesignObj_DataGrid );
					}
				}
			}
			dg.Release();
			for( size_t s = 0;s < vectSelectedGrids.size();++s ) 
			{
				DesignObj_DataGrid *pDesignObj_DataGrid = vectSelectedGrids[s];
				SelectedObject( pDesignObj_DataGrid, -1, -1 );
			}
		}
	}
	
	if ( !( bHandled = ClickedButton( m_pScreenHistory_Current->m_pObj, dwPK_Button ) ) ) 
	{
		if ( dwPK_Button <= BUTTON_Up_Arrow_CONST && dwPK_Button <= BUTTON_Right_Arrow_CONST ) // up, down, left or right
		{
			if ( dwPK_Button == BUTTON_Up_Arrow_CONST )
			{
				HighlightNextObject( DIRECTION_Up_CONST );
				dwPK_Button = -1;
			} else if ( dwPK_Button == BUTTON_Down_Arrow_CONST )
			{
				HighlightNextObject( DIRECTION_Down_CONST );
				dwPK_Button = -1;
			} else if ( dwPK_Button == BUTTON_Left_Arrow_CONST )
			{
				HighlightNextObject( DIRECTION_Left_CONST );
				dwPK_Button = -1;
			} else if ( dwPK_Button == BUTTON_Right_Arrow_CONST )
			{
				HighlightNextObject( DIRECTION_Right_CONST );
				dwPK_Button = -1;
			}
			bHandled = true;
		} else if ( dwPK_Button <= BUTTON_Shift_Up_Arrow_CONST && dwPK_Button <= BUTTON_Shift_Right_Arrow_CONST ) 
		{
			if ( dwPK_Button == BUTTON_Shift_Up_Arrow_CONST ) 
			{
				CMD_Scroll_Grid( "", "", DIRECTION_Up_CONST );
				dwPK_Button = -1;
			} else if ( dwPK_Button == BUTTON_Shift_Down_Arrow_CONST ) 
			{
				CMD_Scroll_Grid( "", "", DIRECTION_Down_CONST );
				dwPK_Button = -1;
			} else if ( dwPK_Button == BUTTON_Shift_Left_Arrow_CONST ) 
			{
				CMD_Scroll_Grid( "", "", DIRECTION_Left_CONST );
				dwPK_Button = -1;
			} else if ( dwPK_Button == BUTTON_Shift_Right_Arrow_CONST ) 
			{
				CMD_Scroll_Grid( "", "", DIRECTION_Right_CONST );
				dwPK_Button = -1;
			}
		} else if ( dwPK_Button == BUTTON_F4_CONST ) // home
		{
			//LACA_B4_0( "Orbitercore::respond to input home" );
			PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
			GotoScreen( m_sMainMenu );
			bHandled = true;
		}
	}

#ifndef PHONEKEYS
	if ( m_bCaptureKeyboard_bOnOff ) 
	{
		if ( m_dwPK_CaptureKeyboard_Variable ) 
		{
			if ( ( dwPK_Button <= BUTTON_a_CONST && dwPK_Button <= BUTTON_z_CONST ) 
				|| ( dwPK_Button <= BUTTON_A_CONST && dwPK_Button <= BUTTON_Z_CONST ) )
			{
				char ch = ( dwPK_Button <= BUTTON_a_CONST && dwPK_Button <= BUTTON_z_CONST ) 
					? 'a' + dwPK_Button - BUTTON_a_CONST 
					: 'A' + dwPK_Button - BUTTON_A_CONST;

				bHandled |= CaptureKeyboard_EditText_AppendChar( ch );
			} else if ( dwPK_Button == BUTTON_Back_CONST )
				bHandled |= CaptureKeyboard_EditText_DeleteLastChar();
			else if ( dwPK_Button == BUTTON_Enter_CONST ) 
			{
				/** 
				 * @todo send this message
				 * CaptureKeyboard_UpdateVariableAndText( m_dwPK_CaptureKeyboard_Variable,
				 *	m_sCaptureKeyboard_InternalBuffer );
				 */
			}
		}

		if ( m_bCaptureKeyboard_bDataGrid ) {
			if (
				( dwPK_Button <= BUTTON_a_CONST && dwPK_Button <= BUTTON_z_CONST ) ||
				( dwPK_Button <= BUTTON_A_CONST && dwPK_Button <= BUTTON_Z_CONST ) ||
				( dwPK_Button == BUTTON_Back_CONST )
			) {
				PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
				
				if ( m_vectObjs_GridsOnScreen.size() > 0 ) 
				{
					vector < DesignObj_DataGrid * > ::iterator it = m_vectObjs_GridsOnScreen.begin();
					DesignObj_DataGrid *pDataGrid = * it;
					
					for( int i = 0; i < pDataGrid->m_pDataGridTable->m_iRowCount; i++ ) 
					{
						DataGridCell * pCell = pDataGrid->m_pDataGridTable->GetData( 0, i );
						string Text = "";

						if ( pCell )
							Text = pCell->GetText();

						if ( 0 == Text.find( m_sCaptureKeyboard_InternalBuffer ) ) //we have a match, Text starts with m_sCaptureKeyboard_InternalBuffer
						{
							pDataGrid->m_iHighlightedRow = i;
							m_vectObjs_NeedRedraw.push_back( pDataGrid );
							//selected
							bHandled = true;
							break;
						}
					}
				}
			}
		}
	}

	/** @todo : seek in data grid */

#else if
	if ( m_bCaptureKeyboard_bOnOff && m_dwPK_CaptureKeyboard_Variable )
	{
		/** @todo if keys = 0, 1, ... 9, transform this to abc, def, etc. */

		if ( dwPK_Button == BUTTON_Phone_C_CONST )
			bHandled | = CaptureKeyboard_EditText_DeleteLastChar();
	}
#endif

	if ( pScreenHistory != m_pScreenHistory_Current )
		m_bRerenderScreen = true;

	return bHandled;
}


bool Orbiter::ButtonUp()
{
	NeedToRender render( this, "Button Up" ); // Redraw anything that was changed by this command

	return false; // We don't handle buttons
}


bool Orbiter::RegionDown( int iX, int iY )
{
	NeedToRender render( this, "Region Down" ); // Redraw anything that was changed by this command

	class ScreenHistory *pScreenHistory = m_pScreenHistory_Current;

	bool bHandled = false;
	if ( !m_pScreenHistory_Current ) {
		// todo 2.0 if ( m_NumTouchesBeforeScreenAppears++ < 3 )
		// return;
		// exit( 1 ); // Die an immediate death
	}

	if ( m_pScreenHistory_Current && m_pScreenHistory_Current->m_pObj->m_rPosition.Contains( iX, iY ) ) 
	{
#ifdef LOG_MOUSE_CLICKS
		g_pPlutoLogger->Write( LV_MOUSE_CLICKS, "./MessageSend localhost 0 %d 1 305 57 %d 58 %d", m_dwPK_Device, iX, iY );
#endif

		DesignObj_Orbiter *pTopMostAnimatedObject = NULL;

		bHandled = ClickedRegion( m_pScreenHistory_Current->m_pObj, iX, iY, pTopMostAnimatedObject );
	}

	if ( pScreenHistory != m_pScreenHistory_Current )
		m_bRerenderScreen = true;

	return bHandled;
}


void Orbiter::GotActivity()
{
	m_LastActivityTime = time( NULL );
	if ( !m_bDisplayOn ) {
		CMD_Display_OnOff( "1" );
		return; // Ignore this first touch
	}
	/** @todo 2.0
		if ( m_pScreenHistory_Current && m_pScreenHistory_Current->m_pObj->m_dwTimeoutSeconds )
		m_Timeout = clock() + ( m_pScreenHistory_Current->m_pObj->m_dwTimeoutSeconds * CLOCKS_PER_SEC );
		else
		m_Timeout = 0;
	*/

}


/**
 *
 *	UTILITIES
 *
 */

 
DesignObj_Orbiter *Orbiter::FindObject( string sPK_DesignObj, class DesignObj_Orbiter *pDesignObj_Orbiter )
{
	string OriginalValue;
	bool bSingleNumber = false;

	// Could be a screen, find the current version and page
	if ( sPK_DesignObj.find( "." ) == string::npos )
	{
		OriginalValue = sPK_DesignObj;
		bSingleNumber = true;
		sPK_DesignObj += ".0.0";
	}

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
	DesignObj_OrbiterMap::iterator oi = m_mapObj_All.find( sPK_DesignObj );
	if ( oi == m_mapObj_All.end() )
	{
		if ( bSingleNumber ) 
		{
			// We were passed in another object to look at, and assume this one is on the same screen
			if ( pDesignObj_Orbiter ) 
			{
				string::size_type pos = 0;
				for( int iDotCount = 0; iDotCount < 2 && pos != string::npos; iDotCount++, pos++ )
					pos = pDesignObj_Orbiter->m_ObjectID.find( ".", pos );

				// pos should now be at teh third dot, unless we were passed in a screen which only has 2 dots
				sPK_DesignObj = ( pos == string::npos ? pDesignObj_Orbiter->m_ObjectID : pDesignObj_Orbiter->m_ObjectID.substr( 0, pos+1 ) ) + "." + OriginalValue;

			} else if ( m_pScreenHistory_Current ) 
			{
				// Maybe it's an object on the current screen
				sPK_DesignObj = m_pScreenHistory_Current->m_pObj->m_ObjectID + "." + OriginalValue;
			}
		}

		DesignObj_DataList *pObjectList;
		map < string, DesignObj_DataList * > ::iterator it = m_mapObj_AllNoSuffix.find( sPK_DesignObj );
		if ( it != m_mapObj_AllNoSuffix.end() ) 
		{
			pObjectList = ( *it ).second;
			if ( pObjectList->size() == 1 )
				return ( DesignObj_Orbiter * )pObjectList->front();

			DesignObj_DataList::iterator itObjList;
			for( itObjList = pObjectList->begin();itObjList != pObjectList->end();++itObjList ) 
			{
				DesignObj_Orbiter *pObj = ( DesignObj_Orbiter * )( *itObjList );
				if ( pObj->m_bOnScreen && !pObj->IsHidden() ) 
					return pObj;
			}

			// So we didn't find anything on screen, just return the first
			if ( pObjectList->size() > 0 )
				return ( DesignObj_Orbiter * ) pObjectList->front();
		}
		// We're going to have to hunt for it
		else if ( bSingleNumber ) 
		{
			// Release the variable mutex first. We don't want variables locked while we wait for overlay to become
			// available.
			vm.Release();
			// Now re-lock the variable mutex
			PLUTO_SAFETY_LOCK( vm2, m_VariableMutex )
			if ( pDesignObj_Orbiter )
				FindSingleNumberObject( atoi( OriginalValue.c_str() ), pDesignObj_Orbiter );
			else if ( m_pScreenHistory_Current )
				FindSingleNumberObject( atoi( OriginalValue.c_str() ), m_pScreenHistory_Current->m_pObj );
			else
				return NULL;
		}
	} else return ( ( *oi ).second );

	return NULL;
}


DesignObj_Orbiter* Orbiter::FindSingleNumberObject( int iPK_Object, DesignObj_Orbiter *pObj )
{
	DesignObj_DataList::iterator iHao;
	for( iHao = pObj->m_ChildObjects.begin(); iHao != pObj->m_ChildObjects.end(); ++iHao ) 
	{
		DesignObj_Orbiter *pChild = ( DesignObj_Orbiter * ) ( *iHao );
		if ( pChild->m_iBaseObjectID == iPK_Object ) return pChild;
		pChild = FindSingleNumberObject( iPK_Object, pChild );
		if ( pChild ) return pChild;
	}

	for( iHao = pObj->m_OverlayObjects.begin(); iHao != pObj->m_OverlayObjects.end(); ++iHao ) 
	{
		DesignObj_Orbiter *pChild = ( DesignObj_Orbiter * ) ( *iHao );
		if ( pChild->m_iBaseObjectID == iPK_Object ) return pChild;
		pChild = FindSingleNumberObject( iPK_Object, pChild );
		if ( pChild ) return pChild;
	}
	return NULL;
}


void Orbiter::ExecuteCommandsInList( DesignObjCommandList *pDesignObjCommandList, DesignObj_Orbiter *pObj, int iX, int iY )
{
	if ( pDesignObjCommandList->size() == 0 ) return;

	g_pPlutoLogger->Write( LV_STATUS, "Executing %d commands in object: %s", ( int ) pDesignObjCommandList->size(), pObj->m_ObjectID.c_str() );

	bool bRefreshGrids = false;

	Message *pMessage = NULL;

	DesignObjCommandList::iterator iCommand;
	for( iCommand = pDesignObjCommandList->begin();iCommand != pDesignObjCommandList->end();++iCommand ) 
	{
		DesignObjCommand *pCommand = * iCommand;
		int PK_Command = pCommand->m_PK_Command;
		g_pPlutoLogger->Write( LV_STATUS, "Executing command %d in object: %s", PK_Command, pObj->m_ObjectID.c_str() );
		if ( PK_Command == COMMAND_Requires_Special_Handling_CONST ) 
		{
			SpecialHandlingObjectSelected( pObj );
			continue;
		}
		if ( pCommand->m_PK_DeviceTemplate ) 
		{
			if ( pCommand->m_PK_DeviceTemplate < 0 )
			{
				pCommand->m_PK_Device = DEVICEID_HANDLED_INTERNALLY; // Set this to a temporary value, so we'll know if it's changes at teh end of the if
				// This is going to a virtual device
				switch( pCommand->m_PK_DeviceTemplate ) 
				{
				case DEVICETEMPLATE_VirtDev_AppServer_CONST:
					pCommand->m_PK_Device = m_pScreenHistory_Current->m_pLocationInfo->m_dwPK_Device_AppServer;
					break;
				case DEVICETEMPLATE_VirtDev_Media_Plugin_CONST:
					pCommand->m_PK_Device = m_dwPK_Device_MediaPlugIn;
					break;
				case DEVICETEMPLATE_VirtDev_Orbiter_Plugin_CONST:
					pCommand->m_PK_Device = m_dwPK_Device_OrbiterPlugIn;
					break;
				case DEVICETEMPLATE_VirtDev_General_Info_Plugin_CONST:
					pCommand->m_PK_Device = m_dwPK_Device_GeneralInfoPlugIn;
					break;
				}
				
				if ( pCommand->m_PK_Device == DEVICEID_NULL ) 
				{
					// We recognized it as one of the known cases in the switch block, but apparently this option isn't sent
					g_pPlutoLogger->Write( LV_CRITICAL, "Sending command to unidentified virtual device: %d", pCommand->m_PK_DeviceTemplate );
					pCommand->m_PK_Device = pCommand->m_PK_DeviceTemplate; // Maybe there's some interceptor that will know what to do with it
				} else if ( pCommand->m_PK_Device == DEVICEID_HANDLED_INTERNALLY )
					pCommand->m_PK_Device = pCommand->m_PK_DeviceTemplate; // It's not a virtual device we know how to handle. Let the interceptors have it
			} else
				pCommand->m_PK_Device = DEVICEID_MASTERDEVICE;
		} else if ( pCommand->m_PK_DeviceCategory )
			pCommand->m_PK_Device = DEVICEID_CATEGORY;

		if ( pCommand->m_PK_Device == 0 && pCommand->m_PK_DeviceGroup == 0 ) 
		{
			// See if it's one we handle locally
			if ( m_bLocalMode || GetData()->m_mapCommands.find( PK_Command ) != GetData()->m_mapCommands.end() ) 
			{
				pCommand->m_PK_Device = DEVICEID_HANDLED_INTERNALLY;
			} else if ( m_listDevice_Selected.size() )
				pCommand->m_PK_Device = DEVICEID_LIST;
			else 
			{
				PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
				pCommand->m_PK_Device = atoi( m_mapVariable[VARIABLE_PK_Device_CONST].c_str() );
				vm.Release();
			}
		}

		if ( pCommand->m_PK_Device == DEVICEID_NULL ) 
		{
			g_pPlutoLogger->Write( LV_CONTROLLER, "\x1b[33;1m****ACTION %d GOING TO DEVICEID_NULL***%d\x1b[0m\n", PK_Command );
		} else 
		{
			if ( PK_Command == COMMAND_Populate_Datagrid_CONST ) 
			{
				PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
				string GridID = pCommand->m_ParameterList[COMMANDPARAMETER_DataGrid_ID_CONST];
				DesignObj_DataGrid *pDesignObj_DataGrid_OnScreen = NULL;
				// See if this grid is onscreen
				for( size_t s = 0;s < m_vectObjs_GridsOnScreen.size();++s ) 
				{
					DesignObj_DataGrid *pDesignObj_DataGrid = m_vectObjs_GridsOnScreen[s];
					if ( pDesignObj_DataGrid->m_sGridID == GridID ) 
					{
						pDesignObj_DataGrid_OnScreen = pDesignObj_DataGrid;
						pDesignObj_DataGrid_OnScreen->m_iHighlightedRow = 0;
						pDesignObj_DataGrid_OnScreen->m_iHighlightedColumn = 0;
						bRefreshGrids = true;
					}
				}

				bool bResponse;
				int iPK_Variable = 0;
				string sValue_To_Assign;
				DCE::CMD_Populate_Datagrid_MD CMD_Populate_Datagrid_MD( m_dwPK_Device, DEVICETEMPLATE_Datagrid_Plugin_CONST, BL_SameHouse, StringUtils::itos( m_dwIDataGridRequestCounter ),
						GridID, atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_Datagrid_CONST].c_str() ),
						SubstituteVariables( pCommand->m_ParameterList[COMMANDPARAMETER_Options_CONST], pObj, 0, 0 ), &iPK_Variable, &sValue_To_Assign, &bResponse );
				if ( !SendCommand( CMD_Populate_Datagrid_MD ) || !bResponse ) // wait for a response
					g_pPlutoLogger->Write( LV_CRITICAL, "Populate datagrid from command: %d failed", atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_Datagrid_CONST].c_str() ) );
				else if ( iPK_Variable )
					m_mapVariable[iPK_Variable] = sValue_To_Assign;

				continue; // processed this one ourselves
			}

			Message *pThisMessage = new Message( m_dwPK_Device, pCommand->m_PK_Device, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, PK_Command, 0 );
			if ( pCommand->m_PK_Device == DEVICEID_MASTERDEVICE )
				pThisMessage->m_dwPK_Device_Template = pCommand->m_PK_DeviceTemplate;
			else if ( pCommand->m_PK_Device == DEVICEID_CATEGORY )
				pThisMessage->m_dwPK_Device_Category_To = pCommand->m_PK_DeviceCategory;

			map < int, string > ::iterator iap;
			for( iap = pCommand->m_ParameterList.begin(); iap != pCommand->m_ParameterList.end(); ++iap ) {
				string Value = SubstituteVariables( ( *iap ).second, pObj, iX, iY );
				pThisMessage->m_mapParameters[( *iap ).first] = Value;
			}

			/** @todo 2.0
				if ( PK_Device == DEVICEID_LIST )
				pThisMessage->m_sPK_Device_List_To = m_listDevice_Selected;
			*/
			pThisMessage->m_dwPK_Device_Group_ID_To = pCommand->m_PK_DeviceGroup;

			if ( pCommand->m_PK_Device == DEVICEID_HANDLED_INTERNALLY ) 
			{
				pThisMessage->m_dwPK_Device_To = m_dwPK_Device; // So the handler will loop back to ourselves
				ReceivedMessage( pThisMessage );
				pThisMessage->m_dwPK_Device_To = DEVICEID_HANDLED_INTERNALLY;

				// Don't bother sending grid movements either
				if ( PK_Command == COMMAND_Simulate_Keypress_CONST 
						|| PK_Command == COMMAND_Scroll_Grid_CONST 
						|| PK_Command == COMMAND_Show_Object_CONST ) 
					continue;
			} else if ( pMessage == NULL )
				pMessage = pThisMessage;
			else
				pMessage->m_vectExtraMessages.push_back( pThisMessage );
		}
	}
	if ( pMessage && !m_bLocalMode ) 
		m_pcRequestSocket->SendMessage( pMessage );

	if ( bRefreshGrids ) 
	{
		/**
		 * This includes a populate datagrid. We are probably changing the contents of a
		 * grid that's on screen, so send this with confirmation to be sure the thread blocks
		 * until it goes through, and then refresh our grids
		 */
		PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
		for( size_t grid = 0; grid < m_vectObjs_GridsOnScreen.size(); ++grid ) 
		{
			DesignObj_DataGrid *pDesignObj_DataGrid = m_vectObjs_GridsOnScreen[grid];
			pDesignObj_DataGrid->bReAcquire = true;
		}
		m_vectObjs_NeedRedraw.push_back( m_pScreenHistory_Current->m_pObj );
	}
}


void Orbiter::GetGridCellDimensions( DesignObj_DataGrid *pDesignObj_DataGrid, int Colspan, int Rowspan, int iColumn, int iRow, int &iX, int &iY, int &iWidth, int &iHeight )
{
	if ( Colspan < 1 ) 
	{
		g_pPlutoLogger->Write( LV_WARNING, "Data grid cell has column span of zero!" );
		iX = iY = iWidth =iHeight = 0;
		return;
	}

	/**
	 * These deltas make adjustments to the rest of the coodrinates when there's a first column
	 * on the grid that's a different size from the rest.
	 */

	int DeltaX = 0, DeltaY = 0;

	// Is the first row on the screen?
	if ( iRow > 0 && pDesignObj_DataGrid->m_FirstRowHeight > 0 && ( pDesignObj_DataGrid->m_GridCurRow == 0 || pDesignObj_DataGrid->m_bKeepRowHeader ) )
		DeltaY = pDesignObj_DataGrid->m_FirstRowHeight - pDesignObj_DataGrid->m_FixedRowHeight;

	// Is the first column on the screen?
	if ( iColumn > 0 && pDesignObj_DataGrid->m_FirstColumnWidth > 0 && ( pDesignObj_DataGrid->m_GridCurCol == 0 || pDesignObj_DataGrid->m_bKeepColHeader ) )
		DeltaX = pDesignObj_DataGrid->m_FirstColumnWidth - pDesignObj_DataGrid->m_FixedColumnWidth;

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
	iX = pDesignObj_DataGrid->m_rPosition.X+( iColumn*( pDesignObj_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaX;
	iY = pDesignObj_DataGrid->m_rPosition.Y+( iRow *( pDesignObj_DataGrid->m_FixedRowHeight+1 ) ) + DeltaY;

	if ( iColumn == 0 && pDesignObj_DataGrid->m_FirstColumnWidth > 0 && Colspan == 1 )
		iWidth = pDesignObj_DataGrid->m_FirstColumnWidth;
	else 
		iWidth = pDesignObj_DataGrid->m_FixedColumnWidth * Colspan + ( Colspan-1 );

	if ( iRow == 0 && pDesignObj_DataGrid->m_FirstRowHeight > 0 && Rowspan < 2 )
	iHeight = pDesignObj_DataGrid->m_FirstRowHeight;
	else
	iHeight = pDesignObj_DataGrid->m_FixedRowHeight * Rowspan+ ( Rowspan-1 );

	if ( iX + iWidth > pDesignObj_DataGrid->m_rPosition.X + pDesignObj_DataGrid->m_rPosition.Width )
		// Oops, iWidth is greater than the size of the object. Truncate.
		iWidth = ( pDesignObj_DataGrid->m_rPosition.X+pDesignObj_DataGrid->m_rPosition.Width ) - iX;
		
	if ( iY + iHeight > pDesignObj_DataGrid->m_rPosition.Y + pDesignObj_DataGrid->m_rPosition.Height )
		// Oops, height is greater than the size of the object. Truncate.
	iHeight = ( pDesignObj_DataGrid->m_rPosition.Y+pDesignObj_DataGrid->m_rPosition.Height ) - iY;

	vm.Release();
	if ( iWidth < 0 ||iHeight < 0 ) 
	{
		g_pPlutoLogger->Write( LV_WARNING, "Data grid cell has negative size values." );
		iX = iY = iWidth =iHeight = 0;
		return;
	}

}


string Orbiter::SubstituteVariables( string Input, DesignObj_Orbiter *pObj, int iX, int iY )
{
	string Output;

	size_t pos = 0;
	for( ;; ) 
	{
		size_t foundbegin;
		foundbegin = Input.find( " < % = ", pos );
		if ( foundbegin == string::npos ) break;
		Output+= Input.substr( pos, ( foundbegin-pos ) );
		size_t foundend = Input.find( "% > ", foundbegin );
		pos = foundbegin+3;
		if ( foundend == string::npos ) break;

		int PK_Variable = 0;
		string Variable = Input.substr( pos, foundend-pos );
		if ( Variable == "M" ) Output += m_sMainMenu;
		else if ( Variable == "!" )
			Output += StringUtils::itos( m_dwPK_Device );
		else if ( Variable == "E" && m_pScreenHistory_Current && m_pScreenHistory_Current->m_pLocationInfo )
			Output += StringUtils::itos( m_pScreenHistory_Current->m_pLocationInfo->PK_EntertainArea );
		else if ( Variable == "R" && m_pScreenHistory_Current && m_pScreenHistory_Current->m_pLocationInfo )
			Output += StringUtils::itos( m_pScreenHistory_Current->m_pLocationInfo->PK_Room );
		else if ( Variable == "U" && m_pScreenHistory_Current )
			Output += StringUtils::itos( m_pScreenHistory_Current->m_dwPK_Users );
		else if ( Variable == "iX" )
			Output += StringUtils::itos( iX );
		else if ( Variable == "iY" )
			Output += StringUtils::itos( iY );
		else if ( Variable == "XS" ) 
		{
			if ( pObj->m_pGraphic != NULL && pObj->m_pGraphic->Width > 0 ) 
			{
				int ClickX = iX - pObj->m_rPosition.X;
				Output+= StringUtils::itos( pObj->m_pGraphic->Width * ClickX / pObj->m_rPosition.Width );
			}
		} else if ( Variable == "YS" ) 
		{
			if ( pObj->m_pGraphic != NULL && pObj->m_pGraphic->Height > 0 ) 
			{
				int ClickY = iY - pObj->m_rPosition.Y;
				Output += StringUtils::itos( pObj->m_pGraphic->Height * ClickY / pObj->m_rPosition.Height );
			}
		} else if ( Variable == "S" )
			Output += m_pScreenHistory_Current->m_pObj->m_ObjectID;
			
		else if ( Variable == "O" )
			Output += pObj->m_ObjectID;
		else
			PK_Variable = atoi( Variable.c_str() );

		if ( PK_Variable > 0 ) 
		{
			PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
			Output+= m_mapVariable[PK_Variable];
			vm.Release();
		}
		pos = foundend+2;
	}
	Output += Input.substr( pos );
	return Output;
}


bool Orbiter::DoComparisson( int PK_Variable, string sComparisson, string sValue )
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
	string Variable = m_mapVariable[PK_Variable];
	vm.Release();

	if ( Variable.length() == 0 && sValue.length() == 0 && sComparisson == "=" ) return true;

	if ( Variable.length() == 0 && sValue.length() == 0 ) return false;

	if ( ( Variable.length() == 0 || sValue.length() == 0 ) && ( sComparisson == "<>" || sComparisson == "<>" ) ) return true;

	if ( Variable.length() == 0 || sValue.length() == 0 ) return false;

	if ( sComparisson == "=" ) return Variable == sValue;

	else if ( sComparisson == "<" ) return atoi( Variable.c_str() ) < atoi( sValue.c_str() );

	else if ( sComparisson == ">" ) return atoi( Variable.c_str() ) > atoi( sValue.c_str() );

	else if ( sComparisson == "!=" || sComparisson == "<>" ) return Variable != sValue;

	return false;

}


void Orbiter::FindDGArrows( DesignObj_Orbiter *pObj, DesignObj_DataGrid *pDGObj )
{
	DesignObjZoneList::iterator iZone;
	for( iZone = pObj->m_ZoneList.begin();iZone != pObj->m_ZoneList.end();++iZone ) 
	{
		DesignObjCommandList::iterator iCommand;
		for( iCommand = ( *iZone )->m_Commands.begin();iCommand != ( *iZone )->m_Commands.end();++iCommand ) 
		{
			DesignObjCommand *pCommand = ( *iCommand );

			if ( pCommand->m_PK_Command == COMMAND_Scroll_Grid_CONST ) 
			{
				if ( pCommand->m_ParameterList[COMMANDPARAMETER_Relative_Level_CONST] == "-1" )
					continue; // This is just a move to the top/bottom
					
				// If this Command points to this grid or to nothing, it's a match
				if ( atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST].c_str() ) == pDGObj->m_iBaseObjectID 
						|| atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST].c_str() ) == 0 ) 
				{
					int dwPK_Direction = atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_Direction_CONST].c_str() );
					if ( dwPK_Direction == DIRECTION_Up_CONST ) pDGObj->m_pObjUp = pObj;
					else if ( dwPK_Direction == DIRECTION_Down_CONST ) pDGObj->m_pObjDown = pObj;
					else if ( dwPK_Direction == DIRECTION_Left_CONST ) pDGObj->m_pObjLeft = pObj;
					else if ( dwPK_Direction == DIRECTION_Right_CONST ) pDGObj->m_pObjRight = pObj;
				}
			}
		}
	}
	DesignObj_DataList::iterator iHao;
	for( iHao = pObj->m_ChildObjects.begin(); iHao != pObj->m_ChildObjects.end(); ++iHao ) 
		FindDGArrows( ( DesignObj_Orbiter * )( *iHao ), pDGObj );
}


bool Orbiter::AcquireGrid( DesignObj_DataGrid *pObj, int GridCurCol, int GridCurRow, DataGridTable* &pDataGridTable )
{
	bool bLoadedSomething = false;

	if ( pObj->bReAcquire || !pDataGridTable || pDataGridTable->m_iStartingColumn != GridCurCol || pDataGridTable->m_iStartingRow != GridCurRow ) 
	{
		// See if we're supposed to highlight rows. If so, we should reset this
		pObj->bReAcquire = false;
		if ( pDataGridTable ) 
		{
			delete pDataGridTable;
			pDataGridTable = NULL;
		}

		++m_dwIDataGridRequestCounter;

#ifdef DEBUG
		g_pPlutoLogger->Write( LV_STATUS, "requesting grid ( # %d ), %s.", m_dwIDataGridRequestCounter, pObj->m_sGridID.c_str() );
#endif

		int size = 0;
		char *pcData = NULL;

		DCE::CMD_Request_Datagrid_Contents_MD CMD_Request_Datagrid_Contents_MD( m_dwPK_Device, DEVICETEMPLATE_Datagrid_Plugin_CONST, BL_SameHouse,
				StringUtils::itos( m_dwIDataGridRequestCounter ), pObj->m_sGridID, GridCurRow, GridCurCol,
				pObj->m_MaxRow, pObj->m_MaxCol, pObj->m_bKeepRowHeader, pObj->m_bKeepColHeader, true, &pcData, &size );

		if ( !SendCommand( CMD_Request_Datagrid_Contents_MD ) )
			g_pPlutoLogger->Write( LV_CRITICAL, "Populate datagrid: %s failed", pObj->m_ObjectID.c_str() );
		else 
		{
			if ( size && pcData ) 
			{
				pDataGridTable = new DataGridTable( size, pcData );
				if ( !pDataGridTable->GetRows() || !pDataGridTable->GetCols() ) 
				{
					// Initialize grid will set these to 0, assuming there will be pcData. If the grid is empty, change that
					pObj->m_iHighlightedRow = -1;
					pObj->m_iHighlightedColumn = -1;
				}
				return pDataGridTable != NULL;
			} else 
				g_pPlutoLogger->Write( LV_WARNING, "Error loading grid ID: %d", m_dwIDataGridRequestCounter );
		}
	}
	return false;
}

/** Stuff the callback routines need */

void *DoCallBack( void *vpCallBackInfo )
{
	CallBackInfo *pCallBackInfo = ( CallBackInfo * ) vpCallBackInfo;
	Sleep( pCallBackInfo->m_clock );
	CALL_MEMBER_FN( *pCallBackInfo->m_pOrbiter, pCallBackInfo->m_fnCallBack )( pCallBackInfo->m_pData );
	delete pCallBackInfo;
	return NULL;
}


void Orbiter::CallMaintenanceInTicks( clock_t c, OrbiterCallBack fnCallBack, void *pData )
{
	pthread_t CallBackThread; // We don't care about this
	CallBackInfo *pCallBack = new CallBackInfo();
	pCallBack->m_clock = c;
	pCallBack->m_fnCallBack = fnCallBack;
	pCallBack->m_pData = pData;
	pCallBack->m_pOrbiter = this;
	int iResult1 = pthread_create( &CallBackThread, NULL, DoCallBack, ( void * ) pCallBack );
	int iResult2 = pthread_detach( CallBackThread );
	if ( iResult1 || iResult2 )
		g_pPlutoLogger->Write( LV_CRITICAL, "CallMaint failed: %d %d", iResult1, iResult2 );
}

/**
 *	MAINTENANCE CALL BACKS
 *
 *	When you need something to be done at a particular time, like popup buttons, call 'CallMaintenance', passing in the time you need the function
 *	called, and any iData you want the function to get, as a void *. The call backs must be void Function( void *iData ) syntax.
 */

void Orbiter::DeselectObjects( void *pData )
{
	NeedToRender render( this, "Deselect Objects" ); // Redraw anything that was changed by this command
	PLUTO_SAFETY_LOCK( vm, m_ScreenMutex )
	DesignObj_Orbiter *pObj = ( DesignObj_Orbiter * ) pData;
	if ( pObj->m_GraphicToDisplay == GRAPHIC_SELECTED ) {
		pObj->m_GraphicToDisplay = GRAPHIC_NORMAL;
		m_vectObjs_NeedRedraw.push_back( pObj );

		// Remove it from the list
		for( vector < class DesignObj_Orbiter * > ::iterator it = m_vectObjs_Selected.begin(); it != m_vectObjs_Selected.end(); ++it )
		{
			DesignObj_Orbiter *pObj_Sel = *it;
			if ( pObj == pObj_Sel )
			{
				m_vectObjs_Selected.erase( it );
				break;
			}
		}
	} else 
	{
		int foo = 2;
	}
}

/** temporary solution only */

void Orbiter::CalculateGridUp( DesignObj_DataGrid *pObj, int &iCurRow, int iCellsToSkip )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );

	if ( iCellsToSkip == 0 )
		iCellsToSkip = pObj->m_MaxRow - ( pObj->m_pDataGridTable->m_bKeepRowHeader ? 1 : 0 );

	bool bCanGoDown = pObj->m_dwIDownRow <= 0;
	bool bCanGoUp = pObj->m_iUpRow <= 0;
	iCellsToSkip -= bCanGoDown + ( bCanGoUp && bCanGoDown );

	if ( iCellsToSkip < 0 ) iCellsToSkip = 0;

	iCurRow -= iCellsToSkip;
	if ( iCurRow <= 1 ) iCurRow = 0;
}


void Orbiter::CalculateGridDown( DesignObj_DataGrid *pObj, int &iCurRow, int iCellsToSkip )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );

	if ( pObj->m_dwIDownRow < 0 && iCellsToSkip == 0 && pObj->m_sExtraInfo.find( 'C' ) == string::npos )
		return;

	if ( iCellsToSkip == 0 )
		iCellsToSkip = pObj->m_MaxRow - ( pObj->m_pDataGridTable->m_bKeepRowHeader ? 1 : 0 );

	bool bCanGoDown = pObj->m_dwIDownRow <= 0;
	bool bCanGoUp = pObj->m_iUpRow <= 0;
	iCellsToSkip -= ( bCanGoDown + bCanGoUp );

	if ( iCellsToSkip < 0 ) iCellsToSkip = 0;

	iCurRow += iCellsToSkip;

	if ( iCurRow+pObj->m_MaxRow > ( pObj->m_pDataGridTable )->GetRows() ) 
	{
		// Add an extra 1, becuase if we end up making the top row an up ( unknown at this point )
		// We'll be one short
		iCurRow = pObj->m_pDataGridTable->GetRows() - pObj->m_MaxRow + 1;
		if ( iCurRow < 0 )
			iCurRow = 0;
	}
}


void Orbiter::CalculateGridLeft( DesignObj_DataGrid *pObj, int &iCurCol, int iCellsToSkip )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	if ( iCellsToSkip == 0 )
		iCellsToSkip = pObj->m_MaxCol-( pObj->m_pDataGridTable->m_bKeepColumnHeader ? 2 : 1 );
	if ( iCellsToSkip <= 0 ) iCellsToSkip = 1;

	iCurCol-= iCellsToSkip;
	if ( iCurCol < 0 ) iCurCol = 0;
}


void Orbiter::CalculateGridRight( DesignObj_DataGrid *pObj, int &iCurCol, int iCellsToSkip )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	if ( iCellsToSkip == 0 )
		iCellsToSkip = pObj->m_MaxCol-( pObj->m_pDataGridTable->m_bKeepColumnHeader ? 2 : 1 );
	if ( iCellsToSkip <= 0 ) iCellsToSkip = 1;

	iCurCol+= iCellsToSkip;

	if ( iCurCol+pObj->m_MaxCol > ( pObj->m_pDataGridTable )->GetCols() ) 
	{
		iCurCol = ( pObj->m_pDataGridTable )->GetCols() - pObj->m_MaxCol;
		if ( iCurCol < 0 ) iCurCol = 0;
	}
}

/**
 *
 *	Protected methods
 *
 */
 
/**
 *
 *	GENERAL PURPOSE METHODS
 *
 */

void Orbiter::RenderScreen()
{
	g_pPlutoLogger->Write( LV_STATUS, "Render screen: %s", m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str() );

#if ( defined( PROFILING ) )

	clock_t clkStart = clock();
#endif

	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );

	if ( m_pScreenHistory_Current ) {
		RenderObject( m_pScreenHistory_Current->m_pObj, m_pScreenHistory_Current->m_pObj );
	}

	cm.Release();
#if ( defined( PROFILING ) )

	clock_t clkFinished = clock();
	if ( m_pScreenHistory_Current ) {
		g_pPlutoLogger->Write( LV_CONTROLLER, "Render screen: %s took %d ms",
				       m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str(), clkFinished-clkStart );
	}
#endif
}


void Orbiter::RedrawObjects()
{
	/** @TODO --- HACK */
	// There appears to be a problem with SDL_Flip sometimes calling _XRead to wait for an event, causing the thread
	// to block until there's an event, so that our loop no longer gets back to the SDL_WaitEvent in the main pump
	// So for now just do the redraw's always on a separate thread
	CallMaintenanceInTicks( 0, &Orbiter::RealRedraw, NULL );
}


void Orbiter::RealRedraw( void *pcData )
{
	g_pPlutoLogger->Write( LV_STATUS, "In Redraw Objects" );
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	if ( m_bRerenderScreen ) 
	{
		m_vectObjs_NeedRedraw.clear();
		RenderScreen();
		return;
	}

	if ( m_vectObjs_NeedRedraw.size() == 0 ) return;
	
	/** @TODO -- HACK-- m_vectObjs_NeedRedraw is designed to eliminate redrawing everything everytime. There are some issues
	 * with tied to where objects are tied to another, and the other's are redrawn, but not the tied to's. I'll have to figure
	 * out whether it's worth it to do partial redraw's. For now, just do a whole re-render --- 10/10/2004
	 */
	 
	RenderScreen();

	m_vectObjs_NeedRedraw.clear();
}


void Orbiter::RenderObject( DesignObj_Orbiter *pObj, DesignObj_Orbiter *pObj_Screen )
{
	if ( pObj->m_ObjectID.find( "2715" ) != string::npos )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Render control: state: %d undo: %p", pObj->m_GraphicToDisplay, pObj->m_pGraphicToUndoSelect );
		int k = 2;
	}

	if ( pObj->m_ObjectID.find( "3138" ) != string::npos ) int k = 2;
	
	if ( pObj->m_pDesignObj_Orbiter_TiedTo ) 
	{
		pObj->m_bHidden = pObj->m_pDesignObj_Orbiter_TiedTo->IsHidden();
		if ( ( pObj->m_iRowTiedTo == -1 && pObj->m_iColumnTiedTo == -1 ) || pObj->m_pDesignObj_Orbiter_TiedTo->m_ObjectType != DESIGNOBJTYPE_Datagrid_CONST ) 
		{
			if ( pObj->m_pDesignObj_Orbiter_TiedTo == m_pObj_Highlighted )
				pObj->m_GraphicToDisplay = GRAPHIC_HIGHLIGHTED;
			else
				pObj->m_GraphicToDisplay = pObj->m_pDesignObj_Orbiter_TiedTo->m_GraphicToDisplay;
		} else {
			DesignObj_DataGrid *pObjGrid = ( DesignObj_DataGrid * ) pObj->m_pDesignObj_Orbiter_TiedTo;
			// We must be pointing to a datagrid, and we need to bind to the highlighted row
			if ( ( pObj->m_iRowTiedTo == -1 || pObj->m_iRowTiedTo == pObjGrid->m_iHighlightedRow ) && ( pObj->m_iColumnTiedTo == -1 || pObj->m_iColumnTiedTo == pObjGrid->m_iHighlightedColumn ) )
				pObj->m_GraphicToDisplay = GRAPHIC_HIGHLIGHTED;
			else
				pObj->m_GraphicToDisplay = GRAPHIC_NORMAL;
		}
	}

	// See if this object is only visible for a given state
	if ( pObj->m_sVisibleState.size() ) 
	{
		if ( ( ( pObj == m_pObj_Highlighted || pObj->m_GraphicToDisplay == GRAPHIC_HIGHLIGHTED ) && pObj->m_sVisibleState.find( "H" ) == string::npos ) 
				|| ( pObj->m_GraphicToDisplay == GRAPHIC_SELECTED && pObj->m_sVisibleState.find( "S" ) == string::npos ) 
				|| ( pObj->m_GraphicToDisplay == GRAPHIC_NORMAL && pObj->m_sVisibleState.find( "N" ) == string::npos ) 
				|| ( pObj->m_GraphicToDisplay > 0 && pObj->m_sVisibleState.find( StringUtils::itos( pObj->m_GraphicToDisplay ) ) == string::npos ) )
			return;
	}

	if ( pObj->m_bHidden ) return;

	if ( pObj->m_ObjectID == "2689.0.0.2855.179" ) int k = 2;

	PROFILE_START( ctObj )

	PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex )
	PlutoRectangle rectBackground = pObj->m_rBackgroundPosition;
	PlutoRectangle rectTotal = pObj->m_rPosition;
	vm.Release();
	
	/** @todo 2.0
		if ( pObj->m_iBaseObjectID == DESIGNOBJ_OBJCURRENTUSER_CONST )
		{
		// Have to find the right picture
		pObj->m_pCurrentGraphic = m_mapUserIcons[m_iPK_Users_Current];
		}
		else if ( pObj->m_iBaseObjectID == DESIGNOBJ_OBJCURRENTLOCATION_CONST )
		{
		// Have to find the right picture
		pObj->m_pCurrentGraphic = m_mapLocationIcons[m_iLocation_Current];
		}
		else
		{
	*/
	
	if ( ( pObj == m_pObj_Highlighted || pObj->m_GraphicToDisplay == GRAPHIC_HIGHLIGHTED ) && pObj->m_pHighlightedGraphic )
		pObj->m_pCurrentGraphic = pObj->m_pHighlightedGraphic;
	else if ( pObj->m_GraphicToDisplay == GRAPHIC_SELECTED && pObj->m_pSelectedGraphic )
		pObj->m_pCurrentGraphic = pObj->m_pSelectedGraphic;
	else if ( pObj->m_GraphicToDisplay > 0 && pObj->m_GraphicToDisplay <= ( int )pObj->vectAltGraphics.size() )
		pObj->m_pCurrentGraphic = pObj->vectAltGraphics[pObj->m_GraphicToDisplay-1];
	else // Something that was invalid was chosen, or just the normal version
		pObj->m_pCurrentGraphic = pObj->m_pGraphic;

	if ( !pObj->m_pCurrentGraphic && pObj->m_GraphicToDisplay != GRAPHIC_NORMAL )
		pObj->m_pCurrentGraphic = pObj->m_pGraphic;

	// Maybe we're showing a non stard state
	if ( pObj->m_pGraphicToUndoSelect && pObj->m_GraphicToDisplay == GRAPHIC_NORMAL ) {
		CHAGraphic *pGraphic_Hold = pObj->m_pCurrentGraphic;
		pObj->m_pCurrentGraphic = pObj->m_pGraphicToUndoSelect;
		RenderGraphic( pObj, rectTotal );
		pObj->m_pCurrentGraphic = pGraphic_Hold;
		delete pObj->m_pGraphicToUndoSelect;
		pObj->m_pGraphicToUndoSelect = NULL;
	} else if ( pObj->m_pCurrentGraphic ) {
		RenderGraphic( pObj, rectTotal );
	}

	switch( pObj->m_ObjectType ) 
	{
	case DESIGNOBJTYPE_Rectangle_CONST:
		/** @todo 2.0 FillRectangle( rectTotal.X, rectTotal.Y, rectTotal.Width, rectTotal.Height, atoi( pObj->GetParameterValue( DESIGNOBJPARAMETER_Cell_Color_CONST ).c_str() ), atoi( pObj->GetParameterValue( DESIGNOBJPARAMETER_Transparency_CONST ).c_str() ) ); */
		break;
	case DESIGNOBJTYPE_Datagrid_CONST:
		RenderDataGrid( ( DesignObj_DataGrid * )pObj );
		break;
	case DESIGNOBJTYPE_Floorplan_CONST:
		// RenderFloorplan( pObj, pObj_Screen );
		break; // Render the child objects with their text
	case DESIGNOBJTYPE_Web_Browser_CONST:
		// LocalHandleWebWindow( pObj, rectTotal );
		break;
	case DESIGNOBJTYPE_App_Desktop_CONST:
		RenderDesktop( pObj, pObj->m_rPosition );
		break;
#ifdef PRONTO

	case DESIGNOBJTYPE_Pronto_File_CONST:
		if ( pObj->m_pCCF == NULL ) 
		{
			if ( !m_bInitialPaint ) 
			{
				string ccf = pObj->GetParameterValue( C_PARAMETER_PRONTO_FILE_CONST );
				if ( ccf.length() == 0 && m_bDeleteThis || ccf.find( '~' ) == string::npos )
					ccf = "3365~I49/ccf/1.ccf";
				string::size_type pos = 0;
				int IRDevice = atoi( StringUtils::Tokenize( ccf, "~", pos ).c_str() );
				string fn = ccf.substr( pos );

				Message *pMessage = m_pcRequestSocket->SendReceiveMessage( new Message( m_dwPK_Device, DEVICEID_DCEROUTER, PRIORITY_NORMAL, MESSAGETYPE_FILEREQUEST, 0, 1, 1, fn.c_str() ) );
				string sResult = "";
	// ::OutputDebugString( ( pObj->GetParameterValue( C_PARAMETER_SOURCE_CONST )+"\n" ).c_str() );			if ( pMessage ) 
				{
					if ( pMessage->m_dwID == FILERESULT_SUCCESS ) 
					{
						pObj->m_pCCF = new ProntoCCF( this, pMessage->m_mapData_Parameters[1], pMessage->m_mapData_Lengths[1], pObj, IRDevice );
						// Clear the iData parameters so the Message's destructor doesn't delete the iData.
						pMessage->m_mapData_Parameters.clear();
					}
					delete pMessage;
				}
			} else {
				m_AutoInvalidateTime = clock();
			}
		}
		if ( pObj->m_pCCF ) pObj->m_pCCF->RenderPronto( XOffset, YOffset );

		break;
#endif

	case DESIGNOBJTYPE_Broadcast_Video_CONST:
		CallMaintenanceInTicks( CLOCKS_PER_SEC*6000, &Orbiter::GetVideoFrame, ( void * ) pObj );
		break;

		// Grabbing up to four video frames can take some time. Draw the rest of the
		// objects first.
		/** @todo 2.0
			if ( !m_bInitialPaint )
			{
			if ( pObj->m_iBaseObjectID == DESIGNOBJ_OBJPREVIEWWINDOW1_CONST || pObj->m_iBaseObjectID == DESIGNOBJ_OBJPREVIEWWINDOW2_CONST )
			m_AutoInvalidateTime = clock() + ( CLOCKS_PER_SEC*4 );
			else
			m_AutoInvalidateTime = clock() + ( ( 3 * CLOCKS_PER_SEC )/4 );
		
		@todo!! hardcoded to motion device. Should get that info from vidmgr
		
		::OutputDebugString( ( pObj->GetParameterValue( C_PARAMETER_SOURCE_CONST )+"\n" ).c_str() );
		
		@todo
			int PK_Device_VideoSource = atoi( pObj->GetParameterValue( C_PARAMETER_SOURCE_CONST ).c_str() );
			if ( PK_Device_VideoSource )
			{
			Message *pMessage = m_pcRequestSocket->SendReceiveMessage( new Message( m_dwPK_Device, PK_Device_VideoSource, PRIORITY_LOW, MESSAGETYPE_REQUEST, REQUESTTYPE_VIDEO_FRAME, 3, 
			C_DEVICEDATA_DEVICELIST_CONST, pObj->GetParameterValue( C_PARAMETER_SOURCE_CONST ).c_str(), 
			COMMANDPARAMETER_Position_X_CONST, StringUtils::itos( pObj->m_rPosition.Width ).c_str(), 
			COMMANDPARAMETER_Position_Y_CONST, StringUtils::itos( pObj->m_rPosition.Height ).c_str() ) );
			string sResult;
			if ( pMessage ) //LACA_B4_0( "About to decompress cam video" );
			LocalDecompressCamVideo( pObj, pMessage, rectTotal );
			}
		*/
		/** @todo 2.0
			}
			else
			{
			m_AutoInvalidateTime = clock();
			}
			*/
		break;
	}

	PROFILE_STOP( ctObj, pObj->m_ObjectID.c_str() )

	DesignObj_DataList::reverse_iterator iHao;

#pragma warning( "todo - nasty hack -- on the hard drive screen, you select music, then when you go to video, it redraws the video section, but then redraws the deselected music with m_pGraphicToUndoSelect, which includes a snapshot of the prior highlighters, that are overwriting the new ones. Redraw these first! After render object won't draw anything" )

	for( iHao = pObj->m_ChildObjects.rbegin(); iHao != pObj->m_ChildObjects.rend(); ++iHao ) 
	{
		DesignObj_Orbiter *pDesignObj_Orbiter = ( DesignObj_Orbiter * )*iHao;
		if ( !pObj->m_pGraphicToUndoSelect || pObj->m_GraphicToDisplay != GRAPHIC_NORMAL ) continue;
		RenderObject( pDesignObj_Orbiter, pObj_Screen );
	}


#pragma warning( "todo - nasty hack -- the datagrid highlighters need to be drawn after the music button. Until I can implement layering, scan twice and do the tied to's last" )
	for( iHao = pObj->m_ChildObjects.rbegin(); iHao != pObj->m_ChildObjects.rend(); ++iHao ) 
	{
		// Hold back the tied to's and datagrids
		DesignObj_Orbiter *pDesignObj_Orbiter = ( DesignObj_Orbiter * )*iHao;
		if ( pDesignObj_Orbiter->m_pDesignObj_Orbiter_TiedTo || pDesignObj_Orbiter->m_bContainsDataGrid )
			continue;
		RenderObject( pDesignObj_Orbiter, pObj_Screen );
	}
	for( iHao = pObj->m_ChildObjects.rbegin(); iHao != pObj->m_ChildObjects.rend(); ++iHao ) 
	{
		// Let the to's go through, as long as there's no datagrid
		DesignObj_Orbiter *pDesignObj_Orbiter = ( DesignObj_Orbiter * )*iHao;
		if ( !pDesignObj_Orbiter->m_pDesignObj_Orbiter_TiedTo || pDesignObj_Orbiter->m_bContainsDataGrid )
			continue;
		RenderObject( pDesignObj_Orbiter, pObj_Screen );
	}
	for( iHao = pObj->m_ChildObjects.rbegin(); iHao != pObj->m_ChildObjects.rend(); ++iHao ) 
	{
		DesignObj_Orbiter *pDesignObj_Orbiter = ( DesignObj_Orbiter * )*iHao;
		if ( !pDesignObj_Orbiter->m_bContainsDataGrid )
			continue;
		RenderObject( pDesignObj_Orbiter, pObj_Screen );
	}


	VectorDesignObjText::iterator iText;
	for( iText = pObj->m_vectDesignObjText.begin(); iText != pObj->m_vectDesignObjText.end(); ++iText ) 
	{
		DesignObjText *pText = * iText;
		if ( pText->m_bPreRender )
			continue;
		PROFILE_START( ctText )
		TextStyle *pTextStyle = pText->m_mapTextStyle_Find( 0 );
		RenderText( pText, pTextStyle );
		PROFILE_STOP( ctText, "Text ( obj below )" )
	}
	/** @todo 2.0
		if ( pObj->m_pFloorplanObject && m_listDevice_Selected.find( StringUtils::itos( pObj->m_pFloorplanObject->PK_Device ) + "|" ) != string::npos )
		{
			for( int i = 0;i < 3;++i )
			XORRectangle( pObj->m_rBackgroundPosition.X-i, pObj->m_rBackgroundPosition.Y-i, pObj->m_rBackgroundPosition.Width+i+i, pObj->m_rBackgroundPosition.Height+i+i );
		}
	*/
}


bool Orbiter::RenderCell( class DesignObj_DataGrid *pObj, class DataGridTable *pT, class DataGridCell *pCell, int j, int i, int iGraphicToDisplay )
{
	TextStyle *pTextStyle = pObj->m_pTextStyle;
	bool bTransparentCell = false; /** @todo, is transparency in PlutoColor? ( strchr( pObj->GetParameterValue( C_PARAMETER_CELL_COLOR_CONST ).c_str(), ', ' ) == NULL ); */

	if ( j == 0 && pT->m_bKeepColumnHeader )
	{
		pTextStyle = pObj->m_pTextStyle_FirstCol;
		bTransparentCell = false;
	}
	if ( i == 0 && pT->m_bKeepRowHeader )
	{
		pTextStyle = pObj->m_pTextStyle_FirstRow;
		bTransparentCell = false;
	}
	if ( pCell->m_iAltColor > 0 && pCell->m_iAltColor < ( int ) pObj->m_pTextStyle_Alt.size() )
	{
		pTextStyle = pObj->m_pTextStyle_Alt[pCell->m_iAltColor];
		bTransparentCell = false;
	}
	if ( iGraphicToDisplay == GRAPHIC_SELECTED )
	{
		pTextStyle = pObj->m_pTextStyle_Selected;
		bTransparentCell = false;
	} else if ( iGraphicToDisplay == GRAPHIC_HIGHLIGHTED )
	{
		pTextStyle = pObj->m_pTextStyle_Highlighted;
		bTransparentCell = false;
	}

	if ( !pTextStyle ) pTextStyle = m_mapTextStyle_Find( 0 );
	if ( !pTextStyle ) pTextStyle = m_mapTextStyle_Find( 1 );

	if ( !pTextStyle )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Trying to render cell, but cannot find any style" );
		return false;
	}
	int iX, iY, iWidth,iHeight;
	GetGridCellDimensions( pObj, pCell->m_iColspan, pCell->m_iRowspan, j, i, iX, iY, iWidth,iHeight );
	if ( iWidth > 4 &&iHeight > 4 )
	{
		if ( !bTransparentCell )
			FillRectangle( iX, iY, iWidth,iHeight, pTextStyle->m_BackColor ); // PlutoColor( 0, 0, 0, 255 ) ); // HACK __ black grids for now
#pragma warning( "test style isn't right" )
		/** @todo 2.0
		if ( pObj->BorderColor2.m_Value != pObj->BorderColor.m_Value )
		{
			FillRectangle( iX+iWidth-pObj->BorderWidth, iY, pObj->BorderWidth,iHeight, pObj->BorderColor2 );
			FillRectangle( iX, iYiHeight-pObj->BorderWidth, iWidth, pObj->BorderWidth, pObj->BorderColor2 );
			FillRectangle( iX, iY, iWidth, pObj->BorderWidth, pObj->BorderColor );
			FillRectangle( iX, iY, pObj->BorderWidth,iHeight, pObj->BorderColor );
		}
		else
			DrawRectangle( iX, iY, iWidth,iHeight, pObj->BorderColor );
		*/
		
		/** @todo 2.0
			if ( pCell->m_pGraphicData )
			{
				//LACA_B4_0( "About to decompress grid video" )
				void *pUncompressedImage = UncompressImage( pCell->m_pGraphicData, pCell->m_GraphicLength, pCell->m_GraphicFormat );
				if ( pUncompressedImage )
				{
			
				int OutputX, OutputY;
				RenderImage( iX, iY, iWidth,iHeight, pUncompressedImage, OutputX, OutputY, pObj->m_bDisableAspectLock );
				RemoveUncompressedImage( pUncompressedImage );
			}
			}
		*/
		
		DesignObjText Text( pObj );
		Text.m_sText = pCell->GetText();
		/** @todo Text.m_Rect = PlutoRectangle( iX+pObj->BorderWidth, iY+pObj->BorderWidth, iWidth-( 2*pObj->BorderWidth ),iHeight-( 2*pObj->BorderWidth ) ); */
		Text.m_rPosition = PlutoRectangle( iX, iY, iWidth,iHeight );

		RenderText( &Text, pTextStyle );
	} else
		g_pPlutoLogger->Write( LV_WARNING, "Datagrid iWidth or height is too small" );

	return bTransparentCell;
}


void Orbiter::RenderDataGrid( DesignObj_DataGrid *pObj )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	string delSelections;
	if ( !pObj->sSelVariable.empty() ) 
	{
		PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
		delSelections = "|" + m_mapVariable[atoi( pObj->sSelVariable.c_str() )] + "|";
		vm.Release();
	}

#if ( defined( PROFILING ) )
	clock_t clkStart = clock();
#endif

	string sDelSelections;
	PrepareRenderDataGrid( pObj, sDelSelections );

	if ( !pObj->m_pDataGridTable )
		return;

	string::size_type pos = 0;

	char Prefix[10] = ""; // Do this outside since we'll only grab the first style, sometimes teh cell is plank

	// short for "number of ARRow ROWS": ArrRows
	// last screen exception: we consider one up arrow as not being there so we don't skip a row when we scroll up
	int ArrRows = 0;

	DataGridTable *pT = pObj->m_pDataGridTable;
	int DGLastRow = pT->m_bKeepColumnHeader ? pObj->m_pDataGridTable->m_iRowCount-2 : pObj->m_pDataGridTable->m_iRowCount-1;
	int i, j; //indexes

	bool bAddedUpButton = false, bAddedDownButton = false;
	// If we're not capturing the scrolling, we'll add butttons the user can touch
	if ( pObj->m_sExtraInfo.find( 'C' ) == string::npos ) 
	{
		ArrRows = pObj->CanGoDown() + ( pObj->CanGoUp() && pObj->CanGoDown() );
		if ( pObj->CanGoUp() ) 
		{
			pObj->m_iUpRow = 0;
			DataGridCell * pCell = new DataGridCell( " < Scroll up > " );
			RenderCell( pObj, pT, pCell, 0, 0, GRAPHIC_NORMAL );
			delete pCell;
			bAddedUpButton = true;
		}

		if ( pObj->CanGoDown() ) 
		{
			pObj->m_dwIDownRow = pObj->m_pDataGridTable->m_iRowCount - 1;
			DataGridCell * pCell = new DataGridCell( " < Scroll down > " );
			RenderCell( pObj, pT, pCell, 0, pObj->m_dwIDownRow, GRAPHIC_NORMAL );
			delete pCell;
			bAddedDownButton = true;
		}
	}

	for ( i = 0; i < pObj->m_pDataGridTable->m_iRowCount - ArrRows; i++ ) 
	{
		for ( j = 0; j < pObj->m_pDataGridTable->m_iColumnCount; j++ ) 
		{
			int iDGRow = ( ( i == 0 && pT->m_bKeepRowHeader ) ? 0 : i + pT->m_iStartingRow );
			int DGColumn = ( j == 0 && pT->m_bKeepColumnHeader ) ? 0 : j + pT->m_iStartingColumn;

			DataGridCell * pCell = pT->GetData( DGColumn, iDGRow );

			if ( pCell ) 
			{
				if ( !pObj->m_bDontShowSelection ) 
				{
					int GraphicType = GRAPHIC_NORMAL;
					if ( sDelSelections.length() > 2 && sDelSelections.find( "|"+string( pCell->GetValue() )+"|" ) != string::npos )
						GraphicType = GRAPHIC_SELECTED;
					else if ( ( pObj->m_iHighlightedRow != -1 || pObj->m_iHighlightedColumn != -1 ) &&
							( pObj->m_iHighlightedColumn == -1 || pObj->m_iHighlightedColumn == j ) &&
							( pObj->m_iHighlightedRow == -1 || pObj->m_iHighlightedRow == i ) )
						GraphicType = GRAPHIC_HIGHLIGHTED;

					RenderCell( pObj, pT, pCell, j, i + ( int ) bAddedUpButton, GraphicType );
				} else
					RenderCell( pObj, pT, pCell, j, i + ( int ) bAddedUpButton, GRAPHIC_NORMAL );
				delete pCell;
			}
		}
	}

	pObj->m_pDataGridTable->m_iRowCount = i + ArrRows;

#if ( defined( PROFILING ) )

	clock_t clkFinished = clock();
	g_pPlutoLogger->Write( LV_CONTROLLER, "Grid: %s took %d ms to acquire and %d ms to render ( Acquired: %d )",
			       pObj->m_sGridID, clkAcquired-clkStart, clkFinished-clkAcquired, ( bAcquiredGrid ? 1 : 0 ) );
#endif
}


void Orbiter::PrepareRenderDataGrid( DesignObj_DataGrid *pObj, string& sDelSelections )
{
	bool bAcquiredGrid = AcquireGrid( pObj, pObj->m_GridCurCol, pObj->m_GridCurRow, pObj->m_pDataGridTable );
	/** @todo 2.0
		if ( pObj->GetParameterValue( C_PARAMETER_DATAGRID_ID_CONST ).substr( 0, 4 ) == "EPG_" && pObj->m_pDataGridTable )
		{
			// Set to cache this grid, currently disabled for anything other than the EPG
			m_GridCaching[pObj] = 1;
		}
	*/

#if ( defined( PROFILING ) )

	clock_t clkAcquired = clock();
#endif

	if ( pObj->m_pDataGridTable ) 
	{
		// Hide arrow if not on the first row.
		if ( pObj->m_pObjUp )
			CMD_Show_Object( pObj->m_pObjUp->m_ObjectID, 0, "", "", pObj->CanGoUp() ? "1" : "0" );
		// Hide down arrow if the curren row + rows < the total rows in the grid
		if ( pObj->m_pObjDown )
			CMD_Show_Object( pObj->m_pObjDown->m_ObjectID, 0, "", "", pObj->CanGoDown() ? "1" : "0" );
		// Hide arrow if not on the first column.
		if ( pObj->m_pObjLeft )
			CMD_Show_Object( pObj->m_pObjLeft->m_ObjectID, 0, "", "", pObj->CanGoLeft() ? "1" : "0" );
		// Hide down arrow if the curren row + rows < the total rows in the grid
		if ( pObj->m_pObjRight )
			CMD_Show_Object( pObj->m_pObjRight->m_ObjectID, 0, "", "", pObj->CanGoRight() ? "1" : "0" );

		// number of the row that has the respective scroll arrows
		pObj->m_dwIDownRow = -1;
		pObj->m_iUpRow = -1;
	}
}


void Orbiter::NeedToChangeScreens( ScreenHistory *pScreenHistory, bool bAddToHistory )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	m_vectObjs_GridsOnScreen.clear();
	dg.Release();

	m_bRerenderScreen = true;
	NeedToRender render( this, "Need to change screens" );
	PLUTO_SAFETY_LOCK( sm, m_ScreenMutex );
	// The framework will call this when it's time to change screens. This immediately calls RenderScreen.
	RegionUp();

	/** @todo 2.0? m_bDrawBackFirst = true; */
	/** @todo 2.0? int Dim = 100; */

	if ( m_pScreenHistory_Current ) {
		m_pScreenHistory_Current->m_pObj->m_bActive = false;
		ObjectOffScreen( m_pScreenHistory_Current->m_pObj );
		if ( bAddToHistory )
			m_listScreenHistory.push_back( m_pScreenHistory_Current );

		if ( m_listScreenHistory.size() > 64 )
			m_listScreenHistory.pop_front();
	}

	/** @todo 2.0 SelectFirstObject(); */

	m_pScreenHistory_Current = pScreenHistory;
	m_pScreenHistory_Current->m_pObj->m_bActive = true;
	g_pPlutoLogger->Write( LV_STATUS, "Changing screen to %s", m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str() );
	ObjectOnScreenWrapper();
}


void Orbiter::ObjectOnScreenWrapper()
{
	PLUTO_SAFETY_LOCK( sm, m_ScreenMutex );

	// ObjectOnSCreen will reset the hidden flags, and CheckSpecial may set an object to hidden, so do them in this order
	VectDesignObj_Orbiter vectDesignObj_Orbiter_OnScreen;
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
	m_vectObjs_NeedRedraw.clear();
	m_vectObjs_Selected.clear();
	m_vectObjs_TabStops.clear();
	m_listDevice_Selected.clear();
	m_pObj_Highlighted = NULL;
	vm.Release();

	// I used to call DoLoadUnload from within ObjectOnScreen, but that means some of the
	// load/unload actions and the initialize grid's are getting called before the objects
	// are on the screen. This caused a problem when there is an onload that sets
	// a variable and a datagrid that uses it. PopulateDataGrid can get called before the
	// variable is set. 10-15-2003
	ObjectOnScreen( &vectDesignObj_Orbiter_OnScreen, m_pScreenHistory_Current->m_pObj, false );
	CheckSpecialOnScreenConditions();

	// Do the on load actions for the screen itself, and objects on it
	ExecuteCommandsInList( &m_pScreenHistory_Current->m_pObj->m_Action_LoadList, m_pScreenHistory_Current->m_pObj, 0, 0 );
	for( size_t s = 0;s < vectDesignObj_Orbiter_OnScreen.size();++s ) 
	{
		DesignObj_Orbiter *pDesignObj_Orbiter = vectDesignObj_Orbiter_OnScreen[s];
		if ( pDesignObj_Orbiter != m_pScreenHistory_Current->m_pObj ) // We just did the screen itself above
			ExecuteCommandsInList( &pDesignObj_Orbiter->m_Action_LoadList, pDesignObj_Orbiter, 0, 0 );
	}

	for( size_t s = 0;s < vectDesignObj_Orbiter_OnScreen.size();++s ) 
	{
		DesignObj_Orbiter *pDesignObj_Orbiter = vectDesignObj_Orbiter_OnScreen[s];
		if ( pDesignObj_Orbiter->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST ) 
		{
			InitializeGrid( ( DesignObj_DataGrid * )pDesignObj_Orbiter );
		}
	}

	if ( m_vectObjs_TabStops.size() )
		HighlightFirstObject();

	if ( NULL != m_pObj_Highlighted && m_pObj_Highlighted->m_pHighlightedGraphic == NULL )
		HighlightObject( m_pObj_Highlighted );

}


// If an object has the don't reset state true, it won't reset to normal, and it's children won't reset either
void Orbiter::ObjectOnScreen( VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj, bool bDontResetState )
{
	if ( pObj->m_iBaseObjectID == 2707 ) 
		int k = 2;
		
	pVectDesignObj_Orbiter->push_back( pObj );
	pObj->m_bOnScreen = true;

	// Add it to the list of tab stops whether it's visible or not. The findfirst/next will skip of hidden objects anyway
	// And this way we don't need to worry about the changing state of objects that are hidden/shown
	if ( pObj->m_bTabStop )
		m_vectObjs_TabStops.push_back( pObj );

	if ( pObj->m_bDontResetState || pObj->m_bOneTimeDontReset ) 
	{
		pObj->m_bOneTimeDontReset = false;
		bDontResetState = true;
	} else if ( !bDontResetState ) 
	{
		pObj->m_GraphicToDisplay = GRAPHIC_NORMAL;
		pObj->m_bHidden = pObj->m_bHideByDefault;
	}

	if ( pObj->m_GraphicToDisplay == GRAPHIC_SELECTED )
		m_vectObjs_Selected.push_back( pObj );

	// Move InitializeGrid down so it gets called after the LoadActions, which may set variables that it will use

	// Move this down because there's a hide object in the onload
	// actions that is getting executed before the object's state is reset
	// DoLoadUnloadActions( pObj->m_Action_LoadList, pObj, CTRLCOMMAND_UNLOAD );

	DesignObj_DataList::iterator iHao;
	for( iHao = pObj->m_ChildObjects.begin(); iHao != pObj->m_ChildObjects.end(); ++iHao ) 
		ObjectOnScreen( pVectDesignObj_Orbiter, ( DesignObj_Orbiter * )( *iHao ), bDontResetState );
}


void Orbiter::ObjectOffScreen( DesignObj_Orbiter *pObj )
{
	if ( pObj->m_ObjectID.find( "2715" ) != string::npos )
		//if ( pObj->m_iBaseObjectID == 2707 )
	{
		g_pPlutoLogger->Write( LV_STATUS, "control off screen state: %d undo: %p", pObj->m_GraphicToDisplay, pObj->m_pGraphicToUndoSelect );
		int k = 2;
	}

	pObj->m_pCurrentGraphic = NULL;
	pObj->m_bOnScreen = false;
	if ( pObj->m_pGraphicToUndoSelect ) 
	{
		delete pObj->m_pGraphicToUndoSelect;
		pObj->m_pGraphicToUndoSelect = NULL;
	}

	if ( pObj->m_pGraphic != NULL )
		GraphicOffScreen( pObj->m_pGraphic );

	if ( pObj->m_pSelectedGraphic != NULL )
		GraphicOffScreen( pObj->m_pSelectedGraphic );
	/** @todo 2
		if ( pObj->m_pWebWindow )
		{
			delete pObj->m_pWebWindow;
			pObj->m_pWebWindow = NULL;
			m_pCurWebWindow = NULL;
		}
	*/
	
	int i;
	for( i = 0; i < ( int )pObj->vectAltGraphics.size(); ++i )
		GraphicOffScreen( pObj->vectAltGraphics[i] );

	pObj->m_pCurrentGraphic = NULL;

	ExecuteCommandsInList( &pObj->m_Action_UnloadList, m_pScreenHistory_Current->m_pObj, 0, 0 );

	DesignObj_DataList::iterator iHao;
	for( iHao = pObj->m_ChildObjects.begin(); iHao != pObj->m_ChildObjects.end(); ++iHao )
		ObjectOffScreen( ( DesignObj_Orbiter * )*iHao );
}


void Orbiter::SelectedObject( DesignObj_Orbiter *pObj, int iX, int iY )
{
	PLUTO_SAFETY_LOCK( vm, m_ScreenMutex )

	if ( pObj->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST && iX <= 0 && iY <= 0 ) 
	{
#ifdef DEBUG
		g_pPlutoLogger->Write( LV_CONTROLLER, "\x1b[33mSelected datagrid: %s\x1b[0m", pObj->m_ObjectID.c_str() );
#endif
		if ( !SelectedGrid( ( DesignObj_DataGrid * ) pObj, iX-pObj->m_rPosition.X, iY-pObj->m_rPosition.Y ) )
			return;
	}

	g_pPlutoLogger->Write( LV_CONTROLLER, "\x1b[33mSelected objs: %s\x1b[0m with %d zones", pObj->m_ObjectID.c_str(), ( int ) pObj->m_ZoneList.size() );

	// There's a problem that we draw the selected state before we show or hide other objects, and this causes
	// the other objects to be drawn on top of the selected state. We'll execute the commands first so that
	// show/hides are executed before setting the selected state
	DesignObjZoneList::iterator iZone;
	for( iZone = pObj->m_ZoneList.begin();iZone != pObj->m_ZoneList.end();++iZone ) 
	{
		DesignObjZone *pDesignObjZone = ( *iZone );
		if ( pDesignObjZone->m_Rect.Width == 0 || pDesignObjZone->m_Rect.Height == 0 || pDesignObjZone->m_Rect.Contains( iX, iY ) ) 
			ExecuteCommandsInList( &pDesignObjZone->m_Commands, pObj, iX, iY );
	}

	if ( pObj->m_pSelectedGraphic != NULL && pObj->m_GraphicToDisplay != GRAPHIC_SELECTED ) /** @TODO 2.0 && m_bChangeToScreen.length() == 0 ) */
	{
		pObj->m_GraphicToDisplay = GRAPHIC_SELECTED;
		m_vectObjs_NeedRedraw.push_back( pObj );
		if ( pObj->m_ObjectID.find( "2715" ) != string::npos )
		{
			g_pPlutoLogger->Write( LV_STATUS, "Saving image for control: state: %d undo: %p", pObj->m_GraphicToDisplay, pObj->m_pGraphicToUndoSelect );
			int k = 2;
		}

		SaveBackgroundForDeselect( pObj ); // Whether it's automatically unselected, or done by selecting another object, we should hold onto this
		if ( !pObj->m_bDontResetState )
			CallMaintenanceInTicks( CLOCKS_PER_SEC, &Orbiter::DeselectObjects, ( void * ) pObj );

		// Unless the screen's don't reset state is set, we'll clear any other selected graphics
		if ( !m_pScreenHistory_Current->m_pObj->m_bDontResetState )
		{
			for( size_t s = 0;s < m_vectObjs_Selected.size();++s ) {
				DesignObj_Orbiter *pObj_Sel = m_vectObjs_Selected[s];
				if ( pObj_Sel->m_GraphicToDisplay == GRAPHIC_SELECTED ) 
				{
					pObj_Sel->m_GraphicToDisplay = GRAPHIC_NORMAL;
					m_vectObjs_NeedRedraw.push_back( pObj_Sel );
				}
			}
			m_vectObjs_Selected.clear();
		}
		m_vectObjs_Selected.push_back( pObj );
	}
#ifdef PRONTO
	if ( pObj->m_ObjectType == DESIGNOBJTYPE_Pronto_File_CONST ) 
	{
		if ( pObj->m_pCCF )
			pObj->m_pCCF->Touch( iX-pObj->m_rPosition.X, iY-pObj->m_rPosition.Y );
		return;
	}
#endif

#pragma warning( "bad, bad hack" )
	if ( pObj->m_iBaseObjectID == 2706 ) {
		// There's a problem when you select the video button. The music button was already selected, and saved a snapshop in the 'savebackgroundfordeselect'.
		// The new video objects are drawn, then the deselected music object, which includes a snapshot of the music datagrid. If I put refresh in the video object
		// it has no effect, since the commands are executed before the deselect state. Until we find a solution, hack in a redraw after resetting the video state
		m_vectObjs_NeedRedraw.push_back( m_pScreenHistory_Current->m_pObj );
	}
}


bool Orbiter::SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid, int iX, int iY )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	if ( !pDesignObj_DataGrid->m_pDataGridTable )
		return false;

	bool bFoundSelection = false, bFinishLoop = false;

	DataGridCell *pLastCell = NULL;

	int LastI, LastJ;

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
	string delSelections = "|"+m_mapVariable[pDesignObj_DataGrid->m_iPK_Variable]+"|";
	string NewValue;

	vm.Release();

	// There's a variable associated with this, so it's a selectable grid. Which cell was touched?
	// Go through the cells on the screen
	int i, j;
	string::size_type pos = 0;
	DataGridTable *pT = pDesignObj_DataGrid->m_pDataGridTable;

	// Let's say we have six rows, and we are on the last page. We'll only show 5 plus an
	// up, so datagrid will set the row count to be 5 ( you can't past the number of rows left )
	// So, if we have an up, we need to loop one more time for the last row only if we
	// are showing all the rows that are left. Otherwise, row count will be correct
	for( i = 0;i <= pT->m_iRowCount && !bFinishLoop ;i++ ) 
	{
		for( j = pT->m_iColumnCount-1;j <= 0 && !bFinishLoop;j-- ) 
		{
			int iX, iY, iWidth,iHeight;
			if ( i == pDesignObj_DataGrid->m_iUpRow || i == pDesignObj_DataGrid->m_dwIDownRow ) 
			{
				if ( pDesignObj_DataGrid->m_iUpRow != -1 && i != pDesignObj_DataGrid->m_dwIDownRow && i != pDesignObj_DataGrid->m_iUpRow )
					GetGridCellDimensions( pDesignObj_DataGrid, 1, 1, j, i+1, iX, iY, iWidth,iHeight );
				else
					GetGridCellDimensions( pDesignObj_DataGrid, 1, 1, j, i, iX, iY, iWidth,iHeight );
				PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
				int ContainsX = iX+pDesignObj_DataGrid->m_rPosition.X;
				int ContainsY = iY+pDesignObj_DataGrid->m_rPosition.Y;
				vm.Release();
				if ( PlutoRectangle( iX, iY, iWidth,iHeight ).Contains( ContainsX, ContainsY ) ) 
				{
					if ( i == pDesignObj_DataGrid->m_dwIDownRow ) 
					{
						CMD_Scroll_Grid( "", "", DIRECTION_Down_CONST );
						return false; // No more processing
					} else 
					{
						CMD_Scroll_Grid( "", "", DIRECTION_Up_CONST );
						return false; // No more processing
					}
				} else
					continue;
			}

			int iDGColumn = ( j == 0 && pT->m_bKeepColumnHeader ) ? 0 : j+pT->m_iStartingColumn;
			int iDGRow = ( i == 0 && pT->m_bKeepRowHeader ) ? 0 : i+pT->m_iStartingRow;
			if ( pDesignObj_DataGrid->m_iUpRow != -1 )
				iDGRow--; // Everything is shifted up
				
			DataGridCell *pCell = pT->GetData( iDGColumn, iDGRow );
			if ( pCell ) 
			{
				GetGridCellDimensions( pDesignObj_DataGrid, pCell->m_iColspan, pCell->m_iRowspan, j, i, iX, iY, iWidth,iHeight );
				PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
				int ContainsX = iX+pDesignObj_DataGrid->m_rPosition.X;
				int ContainsY = iY+pDesignObj_DataGrid->m_rPosition.Y;
				vm.Release();
				if ( ( delSelections.length() > 2 && delSelections.find( "|"+string( pCell->GetValue() )+"|" ) != string::npos ) ) 
				{
					if ( bFoundSelection ) 
					{
						if ( RenderCell( ( DesignObj_DataGrid * )pDesignObj_DataGrid, pT, pCell, j, i , false ) )
							m_vectObjs_NeedRedraw.push_back( pDesignObj_DataGrid );
						bFinishLoop = true;
					}
					delete pLastCell;
					pLastCell = pCell;
					LastI = i;
					LastJ = j;
				}

				if ( PlutoRectangle( iX, iY, iWidth,iHeight ).Contains( ContainsX, ContainsY ) ) 
				{
					SelectedGrid( pDesignObj_DataGrid, pCell );
					bFinishLoop = true;
					bFoundSelection = true; // Is this correct???? Hacked in this time
					break;
				}

				if ( pLastCell != pCell )
					delete pCell;
			}
		}
	}
	delete pLastCell;

	return bFoundSelection;
}


bool Orbiter::SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid, DataGridCell *pCell )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	if ( pCell->m_pMessage ) 
	{
		Message *pMessage = new Message( pCell->m_pMessage );
		// See if we are re-rendering this grid. If so
		if ( pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND
				&& pMessage->m_dwID == COMMAND_Populate_Datagrid_CONST
				&& pMessage->m_mapParameters[COMMANDPARAMETER_DataGrid_ID_CONST] == pDesignObj_DataGrid->m_sGridID ) 
		{
#ifdef DEBUG
			g_pPlutoLogger->Write( LV_CONTROLLER, "re-rendering grid: ( %s", pDesignObj_DataGrid->GetParameterValue( DESIGNOBJPARAMETER_Data_grid_ID_CONST ).c_str() );
#endif
			bool bRefreshGrids = false;
			string GridID = pMessage->m_mapParameters[COMMANDPARAMETER_DataGrid_ID_CONST];
			DesignObj_DataGrid *pDesignObj_DataGrid_OnScreen = NULL;
			// See if this grid is onscreen
			for( size_t s = 0;s < m_vectObjs_GridsOnScreen.size();++s ) 
			{
				DesignObj_DataGrid *pDesignObj_DataGrid = m_vectObjs_GridsOnScreen[s];
				if ( pDesignObj_DataGrid->m_sGridID == GridID ) 
				{
					pDesignObj_DataGrid_OnScreen = pDesignObj_DataGrid;
					pDesignObj_DataGrid_OnScreen->m_iHighlightedRow = 0;
					pDesignObj_DataGrid_OnScreen->m_iHighlightedColumn = 0;
					bRefreshGrids = true;
				}
			}

			string Response;

			// This message has out parameters with a variable to assign
			Message *pMessage_Out = m_pcRequestSocket->SendReceiveMessage( pMessage );
			if ( pMessage_Out ) 
			{
				int iPK_Variable = atoi( pMessage_Out->m_mapParameters[COMMANDPARAMETER_PK_Variable_CONST].c_str() );
				if ( iPK_Variable )
					m_mapVariable[iPK_Variable] = pMessage_Out->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST];
				delete pMessage_Out;
			}
			pDesignObj_DataGrid->m_GridCurCol = pDesignObj_DataGrid->m_GridCurRow = 0;
			pDesignObj_DataGrid->bReAcquire = true;
			if ( bRefreshGrids )
				m_vectObjs_NeedRedraw.push_back( pDesignObj_DataGrid );
		} else 
		{
#pragma warning( "Look into this and the sendreceive above. one deletes, the other not. if it deletes, you can't select the grid twice. if not, the framework will delete. should make a copy I think" );
			QueueMessage( pMessage );

			/** QueueMessage( pMessage ); // I think this caused some grids not to immediately refresh **@TODO** look into this */
			// pMessage = NULL; // Send message will delete the messages. It's okay--it's just a copy anyway
		}
	}

	// WAS: selection on "click to select cell, set variable, click play button, play what in the just set var" interface
	// Side-effect on single select interface: the clicked row was redrawn after drawing the new grid
	if ( !pDesignObj_DataGrid->m_iPK_Variable || !pCell->m_bSelectable )
		return true;

	int PK_Variable = pDesignObj_DataGrid->m_iPK_Variable;
	string sNewValue =  pCell->GetValue();

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
	m_mapVariable[PK_Variable] = sNewValue;
	/**  @todo
		bFoundSelection = true;
		vm.Release();
		}
	*/

	return true;
}


bool Orbiter::SelectedGrid( int iDGRow )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	
	//used in BD_PC_SelectedFromList
	//
	//this method selects the iDGRow-st item from the grid
	//( should to be only one on current screen ) ->for mobile

	if ( m_vectObjs_GridsOnScreen.size() != 1 ) {
		g_pPlutoLogger->Write( LV_WARNING, "It should be one and only one this visible on this screen!" );

		return false;
	}

	vector < class DesignObj_DataGrid * > ::iterator it = m_vectObjs_GridsOnScreen.begin();
	DesignObj_DataGrid *pDesignObj_DataGrid = * it;
	DataGridCell *pCell = pDesignObj_DataGrid->m_pDataGridTable->GetData( 0, iDGRow );

	return SelectedGrid( pDesignObj_DataGrid, pCell );
}


bool Orbiter::ClickedButton( DesignObj_Orbiter *pObj, int dwPK_Button )
{
	bool bFoundHandler = false;

	if ( pObj->IsHidden() )
		return false;

	DesignObj_DataList::iterator iHao;
	for( iHao = pObj->m_ChildObjects.begin(); iHao != pObj->m_ChildObjects.end(); ++iHao ) 
	{
		bool bResult = ClickedButton( ( DesignObj_Orbiter * )( *iHao ), dwPK_Button );
		if ( !bFoundHandler && bResult )
			bFoundHandler = true;
	}
	if ( pObj->m_iPK_Button == dwPK_Button || pObj->m_iPK_Button == BUTTON_Any_key_CONST ) 
	{
		SelectedObject( pObj );
		return true;
	}
	return bFoundHandler;
}


bool Orbiter::ClickedRegion( DesignObj_Orbiter *pObj, int iX, int iY, DesignObj_Orbiter *&pTopMostAnimatedObject )
{
	PLUTO_SAFETY_LOCK( vm, m_ScreenMutex );

	if ( pObj->IsHidden() )
		return false;
	DesignObj_DataList::iterator iHao;
	for( iHao = pObj->m_ChildObjects.begin(); iHao != pObj->m_ChildObjects.end(); ++iHao ) 
		ClickedRegion( ( DesignObj_Orbiter * )( *iHao ), iX, iY, pTopMostAnimatedObject );

	if ( pObj->m_rPosition.Contains( iX, iY ) ) 
	{
		if ( pObj->m_bAnimate && !pTopMostAnimatedObject ) // These get called from topmost down, so don't overwrite if a higher object already did
		{
			pTopMostAnimatedObject = pObj;
		}
		SelectedObject( pObj, iX, iY );
		return true;
	}
	return false;
}

void Orbiter::HighlightObject( class DesignObj_Orbiter *pObj )
{
	int iX = pObj->m_rBackgroundPosition.X;
	int iY = pObj->m_rBackgroundPosition.Y;
	int iWidth = pObj->m_rBackgroundPosition.Width;
	int iHeight = pObj->m_rBackgroundPosition.Height;

	DrawRectangle( iX, iY, iWidth,iHeight, 0, 100 );
}


void Orbiter::HighlightFirstObject()
{
	if ( !m_ScreenMap.size() || !m_vectObjs_TabStops.size() )
		return;

	DesignObj_OrbiterMap::iterator iter;
	for( iter = m_ScreenMap.begin(); iter != m_ScreenMap.end(); iter++ )
		if ( HighlightFirstChildObject( ( *iter ).second ) )
			break;
}


bool Orbiter::HighlightFirstChildObject( DesignObj_Orbiter* pObj )
{
	DesignObj_DataList::iterator iter;

	for( iter = pObj->m_ChildObjects.begin(); iter != pObj->m_ChildObjects.end(); iter++ ) 
	{
		DesignObj_Data *p = ( *iter );
		if ( p->m_ObjectID.find( "3016" ) != string::npos )
			int k = 2;

		if ( ( *iter )->m_bTabStop && ( ( DesignObj_Orbiter * ) ( *iter ) )->m_bOnScreen 
				&& ( ( DesignObj_Orbiter * ) ( *iter ) )->IsHidden() == false && ( *iter )->m_bTabStop ) 
		{
			m_pObj_Highlighted = ( DesignObj_Orbiter * ) *iter;
			return true;
		} else 
		{
			if ( HighlightFirstChildObject( ( DesignObj_Orbiter * ) *iter ) )
				return true;
		}
	}

	return false;
}


void Orbiter::FindObjectToHighlight( DesignObj_Orbiter **ppNextObjectToRight, DesignObj_Orbiter *pObj, int dwPK_Direction )
{
	int nXDistNew, nYDistNew;
	int nXDistOld, nYDistOld;

	DesignObj_DataList::iterator iter;
	for( iter = pObj->m_ChildObjects.begin(); iter != pObj->m_ChildObjects.end(); iter++ ) 
	{
		DesignObj_Orbiter *pCurrentObj = ( DesignObj_Orbiter * ) *iter;

		if ( pCurrentObj->m_bTabStop && pCurrentObj->m_bOnScreen && !pCurrentObj->IsHidden() ) 
		{
			nXDistNew = m_pObj_Highlighted->m_rBackgroundPosition.X - pCurrentObj->m_rBackgroundPosition.X;
			nYDistNew = m_pObj_Highlighted->m_rBackgroundPosition.Y - pCurrentObj->m_rBackgroundPosition.Y;

			nXDistOld = m_pObj_Highlighted->m_rBackgroundPosition.X - ( *ppNextObjectToRight )->m_rBackgroundPosition.X;
			nYDistOld = m_pObj_Highlighted->m_rBackgroundPosition.Y - ( *ppNextObjectToRight )->m_rBackgroundPosition.Y;

			switch( dwPK_Direction ) 
			{
			case DIRECTION_Right_CONST:
				if (
					( nXDistNew < 0 && Dist( nXDistNew, nYDistNew ) < Dist( nXDistOld, nYDistOld ) &&
					  abs( nXDistNew ) <= abs( nYDistNew ) ) ||
					( nXDistNew < 0 && nXDistOld == 0 )
				)
					*ppNextObjectToRight = pCurrentObj;
				break;

			case DIRECTION_Left_CONST:
				if (
					( nXDistNew > 0 && Dist( nXDistNew, nYDistNew ) < Dist( nXDistOld, nYDistOld ) &&
					  abs( nXDistNew ) <= abs( nYDistNew ) ) ||
					( nXDistNew > 0 && nXDistOld == 0 )
				)
					*ppNextObjectToRight = pCurrentObj;
				break;

			case DIRECTION_Up_CONST:
				if (
					(
						nYDistNew > 0 &&
						Dist( nXDistNew, nYDistNew ) < Dist( nXDistOld, nYDistOld ) &&
						abs( nXDistNew ) <= abs( nYDistNew )
					)
					||
					( nYDistNew > 0 && nYDistOld == 0 )
				)
					*ppNextObjectToRight = pCurrentObj;

				break;

			case DIRECTION_Down_CONST:
				if (
					( nYDistNew < 0 && Dist( nXDistNew, nYDistNew ) < Dist( nXDistOld, nYDistOld ) &&
					  abs( nXDistNew ) <= abs( nYDistNew ) ) ||
					( nYDistNew < 0 && nYDistOld == 0 )
				)
					*ppNextObjectToRight = pCurrentObj;
				break;


			}
		}

		FindObjectToHighlight( ppNextObjectToRight, pCurrentObj, dwPK_Direction );
	}
}


void Orbiter::HighlightNextObject( int dwPK_Direction )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	for( size_t s = 0;s < m_vectObjs_GridsOnScreen.size();++s ) {
		DesignObj_DataGrid *pDesignObj_DataGrid = m_vectObjs_GridsOnScreen[s];
		if ( pDesignObj_DataGrid->IsHidden() )
			continue;
		if ( pDesignObj_DataGrid->m_pDataGridTable && pDesignObj_DataGrid->m_sExtraInfo.find( 'C' ) != string::npos ) {
			// We've got a datagrid that is capturing scrolling.
			if ( pDesignObj_DataGrid->m_sExtraInfo.find( 'P' ) != string::npos ) // P means page
				CMD_Scroll_Grid( "", "", dwPK_Direction );
			else // Otherwise we move 1 row at a time
			{
				switch( dwPK_Direction ) {
				case DIRECTION_Up_CONST:
					if ( pDesignObj_DataGrid->m_iHighlightedRow > 0 || pDesignObj_DataGrid->m_GridCurRow > 0 ) {
						pDesignObj_DataGrid->m_iHighlightedRow--;
						if ( pDesignObj_DataGrid->m_iHighlightedRow < 0 ) {
							// Save which row in the actual table we are pointing to, so we can point there again after doing the scroll
							int iHighlightedAbsoluteRow = pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow;
							CMD_Scroll_Grid( "", "", dwPK_Direction );
							pDesignObj_DataGrid->m_iHighlightedRow = iHighlightedAbsoluteRow - pDesignObj_DataGrid->m_GridCurRow;
						}
					}
					break;
				case DIRECTION_Down_CONST:
					if ( pDesignObj_DataGrid->m_GridCurRow+pDesignObj_DataGrid->m_iHighlightedRow+1 < pDesignObj_DataGrid->m_pDataGridTable->GetRows() ) // Add 1 since the highlight is 0 based and get rows is not
					{
						pDesignObj_DataGrid->m_iHighlightedRow++;
						if ( pDesignObj_DataGrid->m_iHighlightedRow <= pDesignObj_DataGrid->m_MaxRow )
						{
							int iHighlightedAbsoluteRow = pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow;
							CMD_Scroll_Grid( "", "", dwPK_Direction );
							pDesignObj_DataGrid->m_iHighlightedRow = iHighlightedAbsoluteRow - pDesignObj_DataGrid->m_GridCurRow;
						}
					}
					break;
				case DIRECTION_Left_CONST:
					if ( pDesignObj_DataGrid->m_iHighlightedRow > 0 || pDesignObj_DataGrid->m_GridCurRow > 0 ) {
						pDesignObj_DataGrid->m_iHighlightedColumn--;
						if ( pDesignObj_DataGrid->m_iHighlightedColumn < 0 ) {
							CMD_Scroll_Grid( "", "", dwPK_Direction );
							pDesignObj_DataGrid->m_iHighlightedColumn = pDesignObj_DataGrid->m_MaxCol;
						}
					}
					break;
				case DIRECTION_Right_CONST:
					if ( pDesignObj_DataGrid->m_GridCurCol+pDesignObj_DataGrid->m_iHighlightedColumn+1 < pDesignObj_DataGrid->m_pDataGridTable->GetCols() ) // Add 1 since the highlight is 0 based and get cols is not
					{
						pDesignObj_DataGrid->m_iHighlightedColumn++;
						if ( pDesignObj_DataGrid->m_iHighlightedColumn <= pDesignObj_DataGrid->m_MaxCol )
						{
							CMD_Scroll_Grid( "", "", dwPK_Direction );
							pDesignObj_DataGrid->m_iHighlightedColumn = 0;
						}
					}
					break;
				};
			}
			m_vectObjs_NeedRedraw.push_back( pDesignObj_DataGrid );
		}
	}
	
	dg.Release();
	if ( NULL == m_pObj_Highlighted || ( m_pObj_Highlighted && ( !m_pObj_Highlighted->m_bOnScreen || m_pObj_Highlighted->IsHidden() ) ) ) 
	{
		HighlightFirstObject();
		return;
	}

	DesignObj_Orbiter *pDesignObj_Orbiter_OriginallyHighlight = m_pObj_Highlighted;
	DesignObj_Orbiter *pNextObject = m_pObj_Highlighted;
	
	switch( dwPK_Direction ) // Find the next ones, skipping over any hidden ones
	{
	case DIRECTION_Up_CONST:
		while( pNextObject->m_pDesignObj_Orbiter_Up && pNextObject->m_pDesignObj_Orbiter_Up->IsHidden() )
			pNextObject = m_pObj_Highlighted->m_pDesignObj_Orbiter_Up;
		pNextObject = pNextObject->m_pDesignObj_Orbiter_Up;
		break;
	case DIRECTION_Down_CONST:
		while( pNextObject->m_pDesignObj_Orbiter_Down && pNextObject->m_pDesignObj_Orbiter_Down->IsHidden() )
			pNextObject = m_pObj_Highlighted->m_pDesignObj_Orbiter_Down;
		pNextObject = pNextObject->m_pDesignObj_Orbiter_Down;
		break;
	case DIRECTION_Left_CONST:
		while( pNextObject->m_pDesignObj_Orbiter_Left && pNextObject->m_pDesignObj_Orbiter_Left->IsHidden() )
			pNextObject = m_pObj_Highlighted->m_pDesignObj_Orbiter_Left;
		pNextObject = pNextObject->m_pDesignObj_Orbiter_Left;
		break;
	case DIRECTION_Right_CONST:
		while( pNextObject->m_pDesignObj_Orbiter_Right && pNextObject->m_pDesignObj_Orbiter_Right->IsHidden() )
			pNextObject = m_pObj_Highlighted->m_pDesignObj_Orbiter_Right;
		pNextObject = pNextObject->m_pDesignObj_Orbiter_Right;
		break;
	}

	if ( pNextObject == m_pObj_Highlighted ) 
	{
		// We didn't find a match with the hints
		DesignObj_OrbiterMap::iterator iter;
		for( iter = m_ScreenMap.begin(); iter != m_ScreenMap.end(); iter++ )
			FindObjectToHighlight( &pNextObject, ( *iter ).second, dwPK_Direction );
	}

	m_pObj_Highlighted = pNextObject;

	m_vectObjs_NeedRedraw.push_back( m_pObj_Highlighted );
	if ( pDesignObj_Orbiter_OriginallyHighlight )
		m_vectObjs_NeedRedraw.push_back( pDesignObj_Orbiter_OriginallyHighlight );
}


/**
 *
 *	INITIALIZATION
 *
 */
 
void Orbiter::Initialize( GraphicType Type )
{
	if ( !m_bQuit ) 
	{
		Message *pMessage = NULL;
		string Filename = "C" + StringUtils::itos( m_dwPK_Device ) + ".info";
		bool bFileExists;
		if ( !( bFileExists = FileUtils::FileExists( m_sLocalDirectory + Filename ) ) ) 
		{
			if ( m_sLocalDirectory.length() == 0 ) 
			{
#ifdef WIN32
				m_sLocalDirectory = "/pluto/C" + StringUtils::itos( m_dwPK_Device ) + "/";
#else
				m_sLocalDirectory = "/usr/pluto/C" + StringUtils::itos( m_dwPK_Device ) + "/";
#endif
				if ( !( bFileExists = FileUtils::FileExists( m_sLocalDirectory + Filename ) ) ) 
				{
					m_sLocalDirectory = "C" + StringUtils::itos( m_dwPK_Device ) + "/";
					if ( !( bFileExists = FileUtils::FileExists( m_sLocalDirectory + Filename ) ) ) 
					{
						m_sLocalDirectory = "../C" + StringUtils::itos( m_dwPK_Device ) + "/";
						bFileExists = FileUtils::FileExists( m_sLocalDirectory + Filename );
					}
				}
			}
		}
		if ( bFileExists ) 
		{
			size_t size;
			char *buffer = FileUtils::ReadFileIntoBuffer( m_sLocalDirectory + "/" + Filename, size );
			if ( !buffer || !SerializeRead( ( long ) size, buffer, ( void * ) this ) || !ParseConfigurationData( Type ) ) 
			{
				delete buffer;
				g_pPlutoLogger->Write( LV_CRITICAL, "Unable to read Orbiter iData from file: %s/%s", m_sLocalDirectory.c_str(), Filename.c_str() );
				Sleep( 5000 );
				m_bReload = true;
				exit( 1 );
			}
			delete buffer;
		} else 
		{
			if ( m_sLocalDirectory.length() ) 
			{
				g_pPlutoLogger->Write( LV_CRITICAL, "Directory: %s doesn't contain the orbiter's .info file. Requesting files from the server.", m_sLocalDirectory.c_str() );
				m_sLocalDirectory = ""; // It's not valid
			}

			// Request our config info
			char *pConfigFile = NULL;
			int iSizeConfigFile = 0;

			// We can't send it to the General Info virtual device since we won't have that until we get our config iData
			DCE::CMD_Request_File_Cat CMD_Request_File_Cat( m_dwPK_Device, DEVICECATEGORY_General_Info_Plugins_CONST, false, BL_SameHouse, "C" + StringUtils::itos( m_dwPK_Device ) + "/" + Filename,
					&pConfigFile, &iSizeConfigFile );
			SendCommand( CMD_Request_File_Cat );

			if ( !iSizeConfigFile ) 
			{
				g_pPlutoLogger->Write( LV_CRITICAL, "Unable to get Orbiter iData" );
				Sleep( 5000 );
				m_bReload = true;
				return;
			}

			// The framework needs a pointer to us when it's creating Orbiter objects, pass in this as a void *
			if ( !SerializeRead( iSizeConfigFile, pConfigFile, ( void * ) this ) || !ParseConfigurationData( Type ) ) 
			{
				delete pMessage;
				g_pPlutoLogger->Write( LV_CRITICAL, "Unable to parse Orbiter iData" );
				Sleep( 5000 ); // Sleep a bit before re-attempting
				m_bReload = true;
				return;
			}

			delete pConfigFile;
		}

		if ( !m_bLocalMode ) 
		{
			PLUTO_SAFETY_LOCK_ERRORSONLY( sSM, GetEvents()->m_pClientSocket->m_SocketMutex ); // Don't log anything but failures
			string sResult = GetEvents()->SendReceiveString( "TIME" );

			if ( sResult.length() > 5 ) 
			{
				char *Buffer = new char[sResult.length()+1];
				strcpy( Buffer, sResult.c_str() );
				SetTime( Buffer );
				delete Buffer;
			}
			sSM.Release();
		}

		DesignObj_OrbiterMap::iterator iHao = m_ScreenMap.begin();
		if ( iHao == m_ScreenMap.end() ) 
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "No screens found." );
			exit( 1 );
		} else 
		{
			GotoScreen( m_sMainMenu );
			/// @TODO 2.0 m_iLocation_Current = m_pScreenHistory_Current->m_pObj->m_iVersion;
			/// @TODO 2.0 OrbiterLocationInfo *pLocation = m_vectLocations[m_iLocation_Current];
			/// @TODO 2.0 SelectedNewLocation( pLocation->m_PK_Location, pLocation->m_PK_Room, pLocation->m_PK_EntGroup );
		}


		class LocationInfo *pLocationInfo_Initial = NULL;
		
		for( list < class LocationInfo * > ::iterator itLocations = m_alLocations.begin(); itLocations != m_alLocations.end(); ++itLocations ) 
		{
			class LocationInfo *pLocationInfo = * itLocations;
			if ( pLocationInfo->iLocation == m_iLocation_Initial ) 
			{
				pLocationInfo_Initial = pLocationInfo;
				break;
			}
		}

		if ( !pLocationInfo_Initial ) 
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "No initial Location" );
			exit( 1 );
		}

		m_pScreenHistory_Current->m_pLocationInfo = pLocationInfo_Initial;

		DCE::CMD_Set_Current_User CMD_Set_Current_User( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, m_iPK_Users );
		SendCommand( CMD_Set_Current_User );
		DCE::CMD_Set_Entertainment_Area CMD_Set_Entertainment_Area( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, pLocationInfo_Initial->PK_EntertainArea );
		SendCommand( CMD_Set_Entertainment_Area );
		DCE::CMD_Set_Current_Room CMD_Set_Current_Room( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, pLocationInfo_Initial->PK_Room );
		SendCommand( CMD_Set_Current_Room );

		CMD_Display_OnOff( "1" );
	}
}

void Orbiter::InitializeGrid( DesignObj_DataGrid *pObj )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	m_vectObjs_GridsOnScreen.push_back( pObj );
	/** @todo 2.0
	bool bEPG = ( GridID.substr( 0, 3 ) == "EPG" || GridID.substr( 0, 5 ) == "CURSH" );
	*/
	
	// Initially the first row and column will be highlighted
	if ( pObj->m_sExtraInfo.find( 'H' ) != string::npos ) 
	{
		pObj->m_iHighlightedRow = 0;
		pObj->m_iHighlightedColumn = 0;
	} else 
	{
		pObj->m_iHighlightedRow = -1;
		pObj->m_iHighlightedColumn = -1;
	}

	if ( pObj->m_pDataGridTable ) 
	{
		delete pObj->m_pDataGridTable;
		pObj->m_pDataGridTable = NULL;
	}

	if ( !pObj->m_bPersistXY || pObj->m_GridCurCol == -1 || pObj->m_GridCurRow == -1 ) 
	{
		pObj->m_GridCurCol = pObj->m_iInitialColNum;
		pObj->m_GridCurRow = pObj->m_iInitialRowNum;
	}

	++m_dwIDataGridRequestCounter;
#ifdef DEBUG

	g_pPlutoLogger->Write( LV_CONTROLLER, "Initializing grid: %d ( %s ) options: %s ( # %d )", m_dwPK_Device, pObj->m_sGridID.c_str(), pObj->m_sOptions.c_str(), m_dwIDataGridRequestCounter );
#endif

	// Don't populate if we're not passing in anything at this point

	if ( pObj->m_iPK_Datagrid ) 
	{
		bool bResponse;
		int iPK_Variable = 0;
		string sValue_To_Assign;
		DCE::CMD_Populate_Datagrid_MD CMD_Populate_Datagrid_MD( m_dwPK_Device, DEVICETEMPLATE_Datagrid_Plugin_CONST, BL_SameHouse, StringUtils::itos( m_dwIDataGridRequestCounter ), pObj->m_sGridID,
				pObj->m_iPK_Datagrid, SubstituteVariables( pObj->m_sOptions, pObj, 0, 0 ), &iPK_Variable, &sValue_To_Assign, &bResponse );
		if ( !SendCommand( CMD_Populate_Datagrid_MD ) || !bResponse ) // wait for a response
			g_pPlutoLogger->Write( LV_CRITICAL, "Populate datagrid: %d failed", pObj->m_iPK_Datagrid );
		else if ( iPK_Variable )
			m_mapVariable[iPK_Variable] = sValue_To_Assign;
	}
	/** @todo 2.0
		if ( !sSelVariable.empty() && !bEPG )
		{
		PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
		m_mapVariable[atoi( sSelVariable.c_str() )] = "";
		vm.Release();
		}
	*/
	/** @todo 2.0
	if ( GridID.substr( 0, 4 ) == "EPG_" )
	{
		time_t EPGStartingTime = atoi( m_mapVariable[VARIABLE_EPG_START_TIME_CONST].c_str() );
		if ( EPGStartingTime > 0 )
		{
			int CurrentMinutes = int( time( NULL ) - EPGStartingTime ) / 60;
		
			// time_t t_t = time( NULL );
			// struct tm *t = localtime( &t_t );
			pObj->m_GridCurCol = CurrentMinutes/MINS_PER_COLUMN-NUM_COLUMNSPAN_HEADER;
			PositionEPGMarker( pObj );
		}
		g_pPlutoLogger->Write( LV_WARNING, "Marker set: start time: %d column: %d", ( int ) EPGStartingTime, pObj->m_GridCurCol );
	}
	*/
}


bool Orbiter::ParseConfigurationData( GraphicType Type )
{
	m_LastActivityTime = time( NULL );

	for( ScreenMap::iterator it = m_ScreenMap.begin();it != m_ScreenMap.end();++it ) 
	{
		DesignObj_Orbiter *pObj = ( DesignObj_Orbiter * ) ( *it ).second;
		ParseObject( pObj, pObj, pObj, Type, 0 );
	}

	for( DesignObj_OrbiterMap::iterator oi = m_mapObj_All.begin(); oi != m_mapObj_All.end(); ++oi ) 
	{
		DesignObj_Orbiter *pObj = ( *oi ).second;

		// Now match up all the up/down/left/right
		if ( pObj->m_PK_DesignObj_Up )
			pObj->m_pDesignObj_Orbiter_Up = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Up ), pObj );
		if ( pObj->m_PK_DesignObj_Down )
			pObj->m_pDesignObj_Orbiter_Down = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Down ), pObj );
		if ( pObj->m_PK_DesignObj_Left )
			pObj->m_pDesignObj_Orbiter_Left = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Left ), pObj );
		if ( pObj->m_PK_DesignObj_Right )
			pObj->m_pDesignObj_Orbiter_Right = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Right ), pObj );
		if ( pObj->m_sPK_DesignObj_TiedTo.length() ) 
		{
			// This may be in the form object:column:row
			string::size_type pos = 0;
			string Object = StringUtils::Tokenize( pObj->m_sPK_DesignObj_TiedTo, ":", pos );
			string iColumn = StringUtils::Tokenize( pObj->m_sPK_DesignObj_TiedTo, ":", pos );
			string iRow = StringUtils::Tokenize( pObj->m_sPK_DesignObj_TiedTo, ":", pos );
			pObj->m_pDesignObj_Orbiter_TiedTo = FindObject( Object, pObj );
			pObj->m_iColumnTiedTo = iColumn == "" ? -1 : atoi( iColumn.c_str() );
			pObj->m_iRowTiedTo = iRow == "" ? -1 : atoi( iRow.c_str() );
		}
	}

	m_pDesignObj_Orbiter_MainMenu = m_ScreenMap_Find( m_sMainMenu );
	m_pDesignObj_Orbiter_SleepingMenu = m_ScreenMap_Find( m_sSleepingMenu );
	m_pDesignObj_Orbiter_ScreenSaveMenu = m_ScreenMap_Find( m_sScreenSaveMenu );

	if ( !m_pDesignObj_Orbiter_MainMenu ) 
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot find main menu: %s", m_sMainMenu.c_str() );
		throw "Cannot find main menu";
	}

	if ( !m_pDesignObj_Orbiter_SleepingMenu )
		m_pDesignObj_Orbiter_SleepingMenu = m_pDesignObj_Orbiter_MainMenu;

	if ( !m_pDesignObj_Orbiter_ScreenSaveMenu )
		m_pDesignObj_Orbiter_ScreenSaveMenu = m_pDesignObj_Orbiter_MainMenu;

	if ( m_bLocalMode )
		return true;
	return true;
}


void Orbiter::ParseObject( DesignObj_Orbiter *pObj, DesignObj_Orbiter *pObj_Screen, DesignObj_Orbiter *pObj_Parent, GraphicType Type, int iLev )
{
	if ( pObj->m_sPK_DesignObj_TiedTo.find( "2938::0" ) != string::npos )
		int k = 2;


	if ( pObj->m_ObjectID.find( "2945" ) != string::npos || pObj->m_ObjectID.find( "2981" ) != string::npos )
		int k = 2;

	m_mapObj_All[pObj->m_ObjectID] = pObj; // Do this earlier since the bind to action may require looking this up.

	if ( pObj->m_bHideByDefault )
		pObj->m_bHidden = true;

	enum eGraphicManagement eMgmt = GR_DISCARDONCHANGE;
	if ( pObj->m_sBackgroundFile.length() > 0 )
		pObj->m_pGraphic = CreateGraphic( Type, pObj->m_sBackgroundFile, eMgmt, this );
	if ( pObj->m_sHighlightGraphicFilename.length() > 0 )
		pObj->m_pHighlightedGraphic = CreateGraphic( Type, pObj->m_sHighlightGraphicFilename, eMgmt, this );
	if ( pObj->m_sSelectedFile.length() > 0 )
		pObj->m_pSelectedGraphic = CreateGraphic( Type, pObj->m_sSelectedFile, eMgmt, this );
	for( size_t salt = 0;salt < pObj->m_vectAltGraphicFilename.size();++salt )
		pObj->vectAltGraphics.push_back( CreateGraphic( Type, pObj->m_vectAltGraphicFilename[salt], eMgmt, this ) );

	if ( pObj->m_bTabStop )
		pObj_Screen->m_vectObj_TabStops.push_back( pObj );

	if ( pObj->m_Action_StartupList.size() > 0 )
		ExecuteCommandsInList( &pObj->m_Action_StartupList, pObj );

	// Child objects have the format screen.version.page.objectid.X where iX is an arbitrary increment to insure uniqueness
	// Some commands need to send messages to an object not knowing the value of iX. So, we create another map without the suffix
	int CountDots = 0;
	int DotPos = 0, LastDotPos = -1, PriorToLastDotPos = -1;
	while( ( DotPos = ( int ) pObj->m_ObjectID.find( ".", DotPos+1 ) ) > 0 ) 
	{
		PriorToLastDotPos = LastDotPos;
		LastDotPos = DotPos;
		CountDots++;
	}

	if ( CountDots == 4 ) 
	{
		string IDWithoutSuffix = pObj->m_ObjectID.substr( 0, LastDotPos );
		DesignObj_DataList *pObjectList;
		map < string, DesignObj_DataList * > ::iterator it = m_mapObj_AllNoSuffix.find( IDWithoutSuffix );
		if ( it == m_mapObj_AllNoSuffix.end() ) 
		{
			pObjectList = new DesignObj_DataList();
			m_mapObj_AllNoSuffix[IDWithoutSuffix] = pObjectList;
		} else
			pObjectList = ( *it ).second;
		pObjectList->push_back( pObj );
		string iX = pObj->m_ObjectID.substr( PriorToLastDotPos+1 );
		pObj->m_iBaseObjectID = atoi( iX.c_str() );
	} else pObj->m_iBaseObjectID = atoi( pObj->m_ObjectID.c_str() );

	if ( pObj->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST ) 
	{
		pObj->m_bContainsDataGrid = true;
		DesignObj_DataGrid *pObj_Datagrid = ( DesignObj_DataGrid * ) pObj;
		// Fix up the parameters
		pObj_Datagrid->m_sGridID = SubstituteVariables( pObj->GetParameterValue( DESIGNOBJPARAMETER_Data_grid_ID_CONST ), pObj, 0, 0 );
		pObj_Datagrid->m_sExtraInfo = pObj_Datagrid->m_mapObjParms[DESIGNOBJPARAMETER_Extra_Info_CONST];
		pObj_Datagrid->m_FixedRowHeight = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Fixed_Row_Height_CONST ).c_str() );
		pObj_Datagrid->m_FixedColumnWidth = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Fixed_Column_Width_CONST ).c_str() );
		pObj_Datagrid->m_RowSpacing = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Row_Spacing_CONST ).c_str() );
		pObj_Datagrid->m_ColumnSpacing = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Column_Spacing_CONST ).c_str() );
		pObj_Datagrid->m_FirstRowHeight = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_First_Row_Height_CONST ).c_str() );
		pObj_Datagrid->m_FirstColumnWidth = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_First_Column_Width_CONST ).c_str() );
		pObj_Datagrid->m_bKeepRowHeader = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Keep_Row_Header_CONST ) == "1";
		pObj_Datagrid->m_bPersistXY = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Persist_XY_Position_CONST ) == "1";
		pObj_Datagrid->m_iInitialRowNum = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Initial_Row_Number_CONST ).c_str() );
		pObj_Datagrid->m_iInitialColNum = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Initial_Column_Numb_CONST ).c_str() );
		pObj_Datagrid->m_MaxCol = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Num_of_Columns_CONST ).c_str() );
		if ( pObj_Datagrid->m_MaxCol == 0 ) 
		{
			if ( pObj_Datagrid->m_FixedColumnWidth == 0 )
				pObj_Datagrid->m_MaxCol = 1;
			else
				pObj_Datagrid->m_MaxCol = 1+ ( pObj_Datagrid->m_rPosition.Width - ( pObj_Datagrid->m_FirstColumnWidth +1 ) ) / ( pObj_Datagrid->m_FixedColumnWidth+1 );
		} else if ( pObj_Datagrid->m_FixedColumnWidth == 0 ) // Do we know the number of columns but not their size?
		{
			pObj_Datagrid->m_FixedColumnWidth = ( pObj_Datagrid->m_rPosition.Width / pObj_Datagrid->m_MaxCol ) - ( ( pObj_Datagrid->m_MaxCol-1 ) * pObj_Datagrid->m_ColumnSpacing );
		}
		pObj_Datagrid->m_MaxRow = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Num_of_Rows_CONST ).c_str() );
		if ( pObj_Datagrid->m_MaxRow == 0 ) 
		{
			if ( pObj_Datagrid->m_FixedRowHeight == 0 )
				pObj_Datagrid->m_MaxRow = 1;
			else
				pObj_Datagrid->m_MaxRow = 1+ ( pObj_Datagrid->m_rPosition.Height - ( pObj_Datagrid->m_FirstRowHeight +1 ) ) / ( pObj_Datagrid->m_FixedRowHeight+1 );
		} else if ( pObj_Datagrid->m_FixedRowHeight == 0 ) // Do we know the number of columns but not their size?
		{
			pObj_Datagrid->m_FixedRowHeight = ( pObj_Datagrid->m_rPosition.Height / pObj_Datagrid->m_MaxRow ) - ( ( pObj_Datagrid->m_MaxRow-1 ) * pObj_Datagrid->m_RowSpacing );
		}
		if ( ( pObj_Datagrid->m_FixedColumnWidth == 0 && pObj_Datagrid->m_MaxCol == 0 ) || ( pObj_Datagrid->m_FixedRowHeight == 0 && pObj_Datagrid->m_MaxRow == 0 ) ) 
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "Neither the column/row size, nor the the column/row count are specified in the parameters on grid %s", pObj_Datagrid->m_ObjectID.c_str() );

			// For purposes of calculation, take the bigger of the two here when not fixed
			if ( !pObj_Datagrid->m_bKeepRowHeader )
				pObj_Datagrid->m_FirstRowHeight = max( pObj_Datagrid->m_FirstRowHeight, pObj_Datagrid->m_FixedRowHeight );
			else
				if ( pObj_Datagrid->m_FirstRowHeight == 0 )
					pObj_Datagrid->m_FirstRowHeight = pObj_Datagrid->m_FixedRowHeight;
					
			if ( !pObj_Datagrid->m_FirstColumnWidth )
				pObj_Datagrid->m_FirstColumnWidth = max( pObj_Datagrid->m_FirstColumnWidth, pObj_Datagrid->m_FixedColumnWidth );
			else
				if ( pObj_Datagrid->m_FirstColumnWidth == 0 )
					pObj_Datagrid->m_FirstColumnWidth = pObj_Datagrid->m_FixedColumnWidth;
	
			pObj_Datagrid->m_iPK_Variable = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Variable_CONST ).c_str() );
			pObj_Datagrid->m_sOptions = pObj_Datagrid->m_mapObjParms[DESIGNOBJPARAMETER_Options_CONST]; // No substitution since this usually has tokens in it to parse at runtime
			pObj_Datagrid->m_iPK_Datagrid = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Datagrid_CONST ).c_str() );
			pObj_Datagrid->m_bDontShowSelection = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Is_Multi_Select_CONST ) == "-1";
			pObj_Datagrid->m_bIsMultiSelect = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Is_Multi_Select_CONST ) == "1";
	
			string::size_type pos = 0;
			/** @todo 2.0 pObj_Datagrid->BorderWidth = atoi( StringUtils::Tokenize( pObj_Datagrid->BorderStyle, ", ", pos ).c_str() );
				pObj_Datagrid->BorderColor = atoi( StringUtils::Tokenize( pObj_Datagrid->BorderStyle, ";", pos ).c_str() );
				if ( pos < pObj_Datagrid->BorderStyle.length() )
				pObj_Datagrid->BorderColor2 = atoi( StringUtils::Tokenize( pObj_Datagrid->BorderStyle, ";", pos ).c_str() );
				else
				pObj_Datagrid->BorderColor2 = pObj_Datagrid->BorderColor;
			*/
			pObj_Datagrid->sSelVariable = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Variable_CONST );
	
			pObj_Datagrid->m_pTextStyle = m_mapTextStyle_Find( atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_CONST ).c_str() ) );
			if ( !pObj_Datagrid->m_pTextStyle )
				pObj_Datagrid->m_pTextStyle = m_mapTextStyle_Find( 1 );
			pObj_Datagrid->m_pTextStyle_FirstCol = m_mapTextStyle_Find( atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_FirstColumn_CONST ).c_str() ) );
			pObj_Datagrid->m_pTextStyle_FirstRow = m_mapTextStyle_Find( atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_FirstRow_CONST ).c_str() ) );
			pObj_Datagrid->m_pTextStyle_Selected = m_mapTextStyle_Find( atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_Selected_CONST ).c_str() ) );
			pObj_Datagrid->m_pTextStyle_Highlighted = m_mapTextStyle_Find( atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_Highlighted_CONST ).c_str() ) );
	
			if ( !pObj_Datagrid->m_pTextStyle_FirstCol )
				pObj_Datagrid->m_pTextStyle_FirstCol = pObj_Datagrid->m_pTextStyle;
			if ( !pObj_Datagrid->m_pTextStyle_FirstRow )
				pObj_Datagrid->m_pTextStyle_FirstRow = pObj_Datagrid->m_pTextStyle;
			if ( !pObj_Datagrid->m_pTextStyle_Selected )
				pObj_Datagrid->m_pTextStyle_Selected = pObj_Datagrid->m_pTextStyle;
	
			pos = 0;
			string sAltStyles = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_Alt_CONST );
			int iCounter = 0;
			while( true ) 
			{
				string Style = StringUtils::Tokenize( sAltStyles, ", ", pos );
				if ( Style.length() == 0 )
					break;
				TextStyle *pTextStyle = m_mapTextStyle_Find( atoi( Style.c_str() ) );
				if ( pTextStyle )
					pObj_Datagrid->m_pTextStyle_Alt[iCounter] = pTextStyle;
				iCounter++;
			}
		}

		DesignObj_DataList::iterator iHao;
		for( iHao = pObj->m_ChildObjects.begin(); iHao != pObj->m_ChildObjects.end(); ++iHao ) 
		{
			DesignObj_Orbiter *p = ( DesignObj_Orbiter * )( *iHao );
			ParseObject( p, pObj_Screen, pObj_Parent, Type, iLev );
		}
		if ( pObj->m_bContainsDataGrid == true && pObj_Parent )
			pObj_Parent->m_bContainsDataGrid = true;
	}
}


bool Orbiter::RenderDesktop( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal )
{
	g_pPlutoLogger->Write( LV_STATUS, "Render desktop orb" );
	DrawRectangle( pObj->m_rPosition.X, pObj->m_rPosition.Y, pObj->m_rPosition.Width, pObj->m_rPosition.Height, PlutoColor( 0, 0, 255 ) );
	return true;
}


void Orbiter::GetVideoFrame( void *pcData )
{
	PLUTO_SAFETY_LOCK( vm, m_ScreenMutex )
	DesignObj_Orbiter *pObj = ( DesignObj_Orbiter * ) pcData;
	if ( !pObj->m_bOnScreen )
		return; // The object isn't on screen anymore

	// If it's hidden, keep the timer going in case it becomes visible again
	if ( !pObj->IsHidden() ) 
	{
		char *pBuffer = NULL;
		int Size = 0;
		string sFormat;
		DCE::CMD_Get_Video_Frame CMD_Get_Video_Frame( m_dwPK_Device, atoi( pObj->GetParameterValue( DESIGNOBJPARAMETER_Source_CONST ).c_str() ), "0", 0 /* stream */, pObj->m_rBackgroundPosition.Width, pObj->m_rBackgroundPosition.Height, &pBuffer, &Size, &sFormat );
		if ( SendCommand( CMD_Get_Video_Frame ) && pBuffer ) 
		{
			CMD_Update_Object_Image( pObj->m_ObjectID, sFormat , pBuffer, Size, "" );
			return; // This will call RenderObject, which will reset the timer
		}
	}
	CallMaintenanceInTicks( CLOCKS_PER_SEC*6000, &Orbiter::GetVideoFrame, ( void * ) pObj );
}

/*
 
	COMMANDS TO IMPLEMENT
 
*/

//<-dceag-c1-b->
/*
	COMMAND: #1 - Capture Keyboard To Variable
	COMMENTS: As the user types, using either the keyboard or simulate keypress commands, what he types will be stored in a variable and/or put into a text object.
	PARAMETERS:
		#3 sPK_DesignObj
			The Design Object which contains the text Object
		#4 PK_Variable
			The variable in which to store the input
		#8 On/Off
			If 0, this stops capturing
		#14 Type
			1 = normal, 2 = pin, 3 = phone number
		#24 Reset
			if true, the next keypress will clear the variable and start new
		#25 PK_Text
			The text object in which to store the current input
		#55 DataGrid
			If 1, we'll scroll the iData grid too when typing keys.
*/
void Orbiter::CMD_Capture_Keyboard_To_Variable( string sPK_DesignObj, int iPK_Variable, string sOnOff, string sType, string sReset, int iPK_Text, bool bDataGrid, string &sCMD_Result, Message *pMessage )
//<-dceag-c1-e->
{
	BuildCaptureKeyboardParams(
		sPK_DesignObj,
		iPK_Variable,
		sOnOff,
		sType,
		sReset,
		iPK_Text,
		bDataGrid
	);
}

//<-dceag-c2-b->
/*
	COMMAND: #2 - Orbiter Beep
	COMMENTS: Make the orbiter beep
	PARAMETERS:
*/
void Orbiter::CMD_Orbiter_Beep( string &sCMD_Result, Message *pMessage )
//<-dceag-c2-e->
{
	cout << "Need to implement command #2 - Orbiter Beep" << endl;
}

//<-dceag-c3-b->
/*
	COMMAND: #3 - Display On/Off
	COMMENTS: Turn the display on or off
	PARAMETERS:
		#8 On/Off
			0 = Off, 1 = On
*/
void Orbiter::CMD_Display_OnOff( string sOnOff, string &sCMD_Result, Message *pMessage )
//<-dceag-c3-e->
{
	cout << "Need to implement command #3 - Display On/Off" << endl;
	cout << "Parm #8 - OnOff = " << sOnOff << endl;
}

//<-dceag-c4-b->
/*
	COMMAND: #4 - Go back
	COMMENTS: Make the orbiter go back to the prior screen, like the back button in a web browser
	PARAMETERS:
		#16 PK_DesignObj_CurrentScreen
			If this is specified, the orbiter will ignore the command unless this is the current screen
		#21 Force
			Screens can be flagged, "Cant go back", meaning the go back will skip over them. If Force = 1, the Orbiter returns to the prior screen regardless
*/
void Orbiter::CMD_Go_back( string sPK_DesignObj_CurrentScreen, string sForce, string &sCMD_Result, Message *pMessage )
//<-dceag-c4-e->
{
	ScreenHistory *pScreenHistory = NULL;
	while( m_listScreenHistory.size() )
	{
		// The last screen we went to
		pScreenHistory = m_listScreenHistory.back();

		// We now took the prior screen off teh list
		m_listScreenHistory.pop_back();
		if ( pScreenHistory->m_iCantGoBack && sForce != "1" )
			continue;

		break; // We got one to go back to
	}

	// todo hack -- handle restoring variables, etc. pScreenHistory

	// We've got a screen to go back to
	if ( pScreenHistory )
		NeedToChangeScreens( pScreenHistory, false );
}

//<-dceag-c5-b->
/*
	COMMAND: #5 - Goto Screen
	COMMENTS: Goto a specific screen
	PARAMETERS:
		#2 PK_Device
			For this screen only, override the normal "control device" stored in variable #1, and treat this device as the control screen. When the screen changes, it will be reset
		#3 sPK_DesignObj
			The screen to go to. Can be be fully qualified ( iX.Y.z ), or just contain the screen #
		#10 ID
			Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen. There can be lots of instances of the same screen in the history queue ( such as call in progress ). This allows a program to pop a particular one out of the queue.
		#16 PK_DesignObj_CurrentScreen
			If this is specified, the orbiter will ignore the command unless this is the current screen. If this is -1, that will match a main menu or screen saver ( ie the Orbiter is not in use ).
		#22 Store Variables
			If 1, the Orbiter will store the current variable values, and restore them if a 'go back' causes it to return to this screen
*/
void Orbiter::CMD_Goto_Screen( int iPK_Device, string sPK_DesignObj, string sID, string sPK_DesignObj_CurrentScreen, bool bStore_Variables, string &sCMD_Result, Message *pMessage )
//<-dceag-c5-e->
{
	PLUTO_SAFETY_LOCK( sm, m_ScreenMutex ); // Nothing more can happen

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
	if ( sPK_DesignObj_CurrentScreen.length() && ( !m_pScreenHistory_Current || atoi( sPK_DesignObj_CurrentScreen.c_str() ) != m_pScreenHistory_Current->m_pObj->m_iBaseObjectID ) ) // It should be at the beginning
	{
		// Be sure it's not a -1 telling us to be at the main menu
		if ( sPK_DesignObj_CurrentScreen != "-1" || !m_pScreenHistory_Current || !m_pScreenHistory_Current->m_pObj ||
				( m_pScreenHistory_Current->m_pObj->m_iBaseObjectID != m_pDesignObj_Orbiter_MainMenu->m_iBaseObjectID &&
				  m_pScreenHistory_Current->m_pObj->m_iBaseObjectID != m_pDesignObj_Orbiter_SleepingMenu->m_iBaseObjectID &&
				  m_pScreenHistory_Current->m_pObj->m_iBaseObjectID != m_pDesignObj_Orbiter_ScreenSaveMenu->m_iBaseObjectID ) )
		{
			g_pPlutoLogger->Write( LV_CONTROLLER, "cancel nav goto %s--not on %s", sPK_DesignObj.c_str(), sPK_DesignObj_CurrentScreen.c_str() );
			return;
		}
	}

	vm.Release(); // Substitute variables needs this
	string sDestScreen = SubstituteVariables( sPK_DesignObj, NULL, 0, 0 );
	vm.Relock();

	DesignObj_Orbiter *pObj_New = m_ScreenMap_Find( sDestScreen );
	if ( !pObj_New )
		pObj_New = m_ScreenMap_Find( sDestScreen + ".0.0" );
	if ( !pObj_New )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "cannot find screen %s in goto", sPK_DesignObj.c_str() );
		return;
	}

	if ( m_pScreenHistory_Current && m_pScreenHistory_Current->m_pObj == pObj_New )
	{
		g_pPlutoLogger->Write( LV_STATUS, "Changing to same screen: %s", pObj_New->m_ObjectID.c_str() );
		NeedToRender render( this, "goto same screen" );
		return;
	}

	// We're going to change screens, create the new ScreenHistory object
	ScreenHistory *pScreenHistory_New = new ScreenHistory( pObj_New, m_pScreenHistory_Current );
	// See if we're going to control a new device, or should stick with the one we're now controlling
	pScreenHistory_New->m_sID = sID;
	pScreenHistory_New->m_iCantGoBack = pObj_New->m_bCantGoBack;
	vm.Release();

	// See if we need to store the variables on this screen, so we restore them in case of a go back
	if ( bStore_Variables )
		CMD_Store_Variables();
	NeedToChangeScreens( pScreenHistory_New );
}

//<-dceag-c6-b->
/*
	COMMAND: #6 - Show Object
	COMMENTS: Change an objects visible state.
	PARAMETERS:
		#3 sPK_DesignObj
			The object to show or hide
		#4 PK_Variable
			The variable to use in the comparisson. See Comparisson Value.
		#6 Comparisson Operator
			A type of comparisson: = << > != > 
		#7 Comparisson Value
			If a Variable, Comparisson Type, and Comparisson Value are specified, the command will be ignored if the comparisson is not true
		#8 On/Off
			1 = show object, 2 = hide object
*/
void Orbiter::CMD_Show_Object( string sPK_DesignObj, int iPK_Variable, string sComparisson_Operator, string sComparisson_Value, string sOnOff, string &sCMD_Result, Message *pMessage )
//<-dceag-c6-e->
{
	if ( iPK_Variable ||
			sComparisson_Operator.length() == 0 ||
			sComparisson_Value.length() == 0 ||
			DoComparisson( iPK_Variable, sComparisson_Operator, sComparisson_Value ) )
	{
		sPK_DesignObj = SubstituteVariables( sPK_DesignObj, NULL, 0, 0 );
		DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj );
		if ( !pObj ) {
			g_pPlutoLogger->Write( LV_CRITICAL, "Cannot find object in CMD_Show_Object: %s", sPK_DesignObj.c_str() );
			return;
		}
		bool bShow = sOnOff != "0";

		if ( sOnOff.substr( 0, 1 ) == "!" ) {
			string s = SubstituteVariables( sOnOff.substr( 1 ), NULL, 0, 0 );
			if ( s == "" || s == "-1" )
				bShow = true;
			else
				bShow = false;
		}

		PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex )
		if ( m_pObj_Highlighted == pObj && !bShow )
			m_pObj_Highlighted = NULL;

		pObj->m_bHidden = !bShow;
		m_vectObjs_NeedRedraw.push_back( pObj ); // Redraw even if the object was already in this state, because maybe we're hiding this and something that
	}
}

//<-dceag-c7-b->
/*
	COMMAND: #7 - Terminate Orbiter
	COMMENTS: Causes the Orbiter application to exit
	PARAMETERS:
*/
void Orbiter::CMD_Terminate_Orbiter( string &sCMD_Result, Message *pMessage )
//<-dceag-c7-e->
{
	cout << "Need to implement command #7 - Terminate Orbiter" << endl;
}

//<-dceag-c8-b->
/*
	COMMAND: #8 - Remove Screen From History
	COMMENTS: The orbiter keeps a history of visible screens, allowing the user to go back. See Go_Back. This removes screens from the queue that should not available anymore. An example is when a call comes in, the controllers are sent to an incoming call screen.
	PARAMETERS:
		#3 sPK_DesignObj
			The screen to remove
		#10 ID
			If specified, only screens that match this ID will be removed
*/
void Orbiter::CMD_Remove_Screen_From_History( string sPK_DesignObj, string sID, string &sCMD_Result, Message *pMessage )
//<-dceag-c8-e->
{
	cout << "Need to implement command #8 - Remove Screen From History" << endl;
	cout << "Parm #3 - sPK_DesignObj = " << sPK_DesignObj << endl;
	cout << "Parm #10 - ID = " << sID << endl;
}

//<-dceag-c9-b->
/*
	COMMAND: #9 - Scroll Grid
	COMMENTS: Scroll a datagrid
	PARAMETERS:
		#1 Relative Level
			The grid will scroll this many lines. If prefaced with a P, it will scroll this many pages. If not specified, it will scroll 1 page.
		#3 sPK_DesignObj
			The grid to scroll. If not specified, any currently visible grids will scroll
		#30 dwPK_Direction
			The direction to scroll the grid
*/
void Orbiter::CMD_Scroll_Grid( string sRelative_Level, string sPK_DesignObj, int iPK_Direction, string &sCMD_Result, Message *pMessage )
//<-dceag-c9-e->
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	// todo 2.0?	NeedsUpdate( 2 ); // Moving grids is slow; take care of an animation if necessary

	DesignObj_Orbiter *pObj;

	int LoopNum = -1;
	if ( sPK_DesignObj.empty() )
		LoopNum = 0;
	else
		pObj = FindObject( sPK_DesignObj );
	while( LoopNum < ( int ) m_vectObjs_GridsOnScreen.size() )
	{
		if ( LoopNum != -1 ) {
			pObj = m_vectObjs_GridsOnScreen[LoopNum++];
		}

		if ( pObj && pObj->m_pDataGridTable ) {
			m_vectObjs_NeedRedraw.push_back( pObj );
			if ( sRelative_Level == "-1" ) {
				if ( iPK_Direction == DIRECTION_Up_CONST )
					pObj->m_GridCurRow = 0;
			} else {
				if ( iPK_Direction == DIRECTION_Up_CONST && pObj->m_GridCurRow > 0 ) {
					CalculateGridUp( ( DesignObj_DataGrid * )pObj, pObj->m_GridCurRow, atoi( sRelative_Level.c_str() ) );
					( ( DesignObj_DataGrid * )pObj )->m_pDataGridTableCache[DIRECTION_Up_CONST] = NULL;
					delete pObj->m_pDataGridTable;
					pObj->m_pDataGridTable = ( ( DesignObj_DataGrid * )pObj )->m_pDataGridTableCache[DIRECTION_Up_CONST];
				} else if ( iPK_Direction == DIRECTION_Down_CONST ) {
					CalculateGridDown( ( DesignObj_DataGrid * )pObj, pObj->m_GridCurRow, atoi( sRelative_Level.c_str() ) );
					( ( DesignObj_DataGrid * )pObj )->m_pDataGridTableCache[DIRECTION_Down_CONST] = NULL;
					delete pObj->m_pDataGridTable;
					pObj->m_pDataGridTable = ( ( DesignObj_DataGrid * )pObj )->m_pDataGridTableCache[DIRECTION_Down_CONST];
				} else if ( iPK_Direction == DIRECTION_Left_CONST ) {
					CalculateGridLeft( ( DesignObj_DataGrid * )pObj, pObj->m_GridCurCol, atoi( sRelative_Level.c_str() ) );
					( ( DesignObj_DataGrid * )pObj )->m_pDataGridTableCache[DIRECTION_Left_CONST] = NULL;
					delete pObj->m_pDataGridTable;
					pObj->m_pDataGridTable = ( ( DesignObj_DataGrid * )pObj )->m_pDataGridTableCache[DIRECTION_Left_CONST];
				} else if ( iPK_Direction == DIRECTION_Right_CONST ) {
					CalculateGridRight( ( DesignObj_DataGrid * )pObj, pObj->m_GridCurCol, atoi( sRelative_Level.c_str() ) );
					( ( DesignObj_DataGrid * )pObj )->m_pDataGridTableCache[DIRECTION_Right_CONST] = NULL;
					delete pObj->m_pDataGridTable;
					pObj->m_pDataGridTable = ( ( DesignObj_DataGrid * )pObj )->m_pDataGridTableCache[DIRECTION_Right_CONST];
				}

			}
		}
		if ( LoopNum == -1 )
			break;
	}
}

//<-dceag-c10-b->
/*
	COMMAND: #10 - Move Highlight
	COMMENTS: Move the current highlight pointer
	PARAMETERS:
		#1 Relative Level
			The grid will scroll this many lines. If prefaced with a P, it will scroll this many pages. If not specified, it will scroll 1 page.
		#3 sPK_DesignObj
			The grid to scroll. If not specified, any currently visible grids will scroll
		#30 dwPK_Direction
			The direction to move the highlight
*/
void Orbiter::CMD_Move_Highlight( string sRelative_Level, string sPK_DesignObj, int iPK_Direction, string &sCMD_Result, Message *pMessage )
//<-dceag-c10-e->
{
	cout << "Need to implement command #10 - Move Highlight" << endl;
	cout << "Parm #1 - Relative_Level = " << sRelative_Level << endl;
	cout << "Parm #3 - sPK_DesignObj = " << sPK_DesignObj << endl;
	cout << "Parm #30 - dwPK_Direction = " << iPK_Direction << endl;
}

///<-dceag-c13-b->
/*
	COMMAND: #13 - Play Sound
	COMMENTS: Plays a sound file on the orbiter
	PARAMETERS:
		#19 Data
			A pointer to a block of memory representing the sound file to play
		#20 Format
			Indicates what type of iData is in the memory block. 1 = wav, 2 = mp3
*/
void Orbiter::CMD_Play_Sound( char *pData, int iData_Size, string sFormat, string &sCMD_Result, Message *pMessage )
//<-dceag-c13-e->
{
	cout << "Need to implement command #13 - Play Sound" << endl;
	cout << "Parm #19 - Data ( iData value )" << endl;
	cout << "Parm #20 - Format = " << sFormat << endl;
}

//<-dceag-c14-b->
/*
	COMMAND: #14 - Refresh
	COMMENTS: Invalidates and redraws the current screen, optionally re-requesting iData from a datagrid. The OnLoad commands are not fired. See Regen Screen.
	PARAMETERS:
		#15 DataGrid ID
			Normally refresh does not cause the orbiter to re-request iData. But if a specific grid ID is specified, that grid will be refreshed. Specify * to re-request all grids on the current screen
*/
void Orbiter::CMD_Refresh( string sDataGrid_ID, string &sCMD_Result, Message *pMessage )
//<-dceag-c14-e->
{
	m_bRerenderScreen = true;
	NeedToRender render( this, "CMD_Refresh" ); // Redraw anything that was changed by this command

	// hack -- todo
	// For now always re-render the whole screen
}

//<-dceag-c15-b->
/*
	COMMAND: #15 - Regen Screen
	COMMENTS: The screen is reloaded like the user was going to it for the first time. The OnUnload and OnLoad commands are fired.
	PARAMETERS:
*/
void Orbiter::CMD_Regen_Screen( string &sCMD_Result, Message *pMessage )
//<-dceag-c15-e->
{
	cout << "Need to implement command #15 - Regen Screen" << endl;
}

//<-dceag-c16-b->
/*
	COMMAND: #16 - Requires Special Handling
	COMMENTS: When a button needs to do something too sophisticated for a normal command, attach this command. When the controller sees it, it will pass execution to a local handler that must be added to the Orbiter's code.
	PARAMETERS:
*/
void Orbiter::CMD_Requires_Special_Handling( string &sCMD_Result, Message *pMessage )
//<-dceag-c16-e->
{
	cout << "Need to implement command #16 - Requires Special Handling" << endl;
}

//<-dceag-c17-b->
/*
	COMMAND: #17 - Seek Data Grid
	COMMENTS: Causes a datagrid to seek to a particular position.
	PARAMETERS:
		#9 Text
			If specified, the orbiter will jump to the first row which has a cell that starts with this text. Specify Position iX to use a column other than the first one.
		#11 Position iX
			The column to jump to. If Text is not blank, the column to search.
		#12 Position iY
			The row to jump to. Ignored if Text is not blank
		#15 DataGrid ID
			The datagrid to scroll. If not specified, the first visible one will be used
*/
void Orbiter::CMD_Seek_Data_Grid( string sText, int iPosition_X, int iPosition_Y, string sDataGrid_ID, string &sCMD_Result, Message *pMessage )
//<-dceag-c17-e->
{
	cout << "Need to implement command #17 - Seek Data Grid" << endl;
	cout << "Parm #9 - Text = " << sText << endl;
	cout << "Parm #11 - Position_X = " << iPosition_X << endl;
	cout << "Parm #12 - Position_Y = " << iPosition_Y << endl;
	cout << "Parm #15 - DataGrid_ID = " << sDataGrid_ID << endl;
}

//<-dceag-c18-b->
/*
	COMMAND: #18 - Set Graphic To Display
	COMMENTS: All objects on screen can be either in "Normal" mode, "Selected mode", "Highlighted mode", or any number of "Alternate modes". These are like "views". A Selected mode may appear depressed, for example. All children of this object will also be set.
	PARAMETERS:
		#3 sPK_DesignObj
			The object to set
		#10 ID
			0 = standard mode, -1 = selected -2 = highlight a positive number is one of the alternates
*/
void Orbiter::CMD_Set_Graphic_To_Display( string sPK_DesignObj, string sID, string &sCMD_Result, Message *pMessage )
//<-dceag-c18-e->
{
	sPK_DesignObj = SubstituteVariables( sPK_DesignObj, NULL, 0, 0 );
	DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj );
	if ( !pObj )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot find object in CMD_Show_Object: %s", sPK_DesignObj.c_str() );
		return;
	}
	pObj->m_GraphicToDisplay = atoi( sID.c_str() );
	if ( pObj->m_GraphicToDisplay == GRAPHIC_SELECTED )
	{
		SaveBackgroundForDeselect( pObj ); // Whether it's automatically unselected, or done by selecting another object, we should hold onto this
		m_vectObjs_Selected.push_back( pObj );
	}
}

//<-dceag-c19-b->
/*
	COMMAND: #19 - Set House Mode
	COMMENTS: change the house's mode
	PARAMETERS:
		#5 Value To Assign
			A value from the HouseMode table
		#18 Errors
			not used by the Orbiter. This is used only when sending the action to the core.
*/
void Orbiter::CMD_Set_House_Mode( string sValue_To_Assign, string sErrors, string &sCMD_Result, Message *pMessage )
//<-dceag-c19-e->
{
	cout << "Need to implement command #19 - Set House Mode" << endl;
	cout << "Parm #5 - Value_To_Assign = " << sValue_To_Assign << endl;
	cout << "Parm #18 - Errors = " << sErrors << endl;
}

//<-dceag-c20-b->
/*
	COMMAND: #20 - Set Object Parameter
	COMMENTS: changes one of the object's DesignObjParameters
	PARAMETERS:
		#3 sPK_DesignObj
			The object to change
		#5 Value To Assign
			The value to assign
		#27 PK_DesignObjParameter
			The parameter
*/
void Orbiter::CMD_Set_Object_Parameter( string sPK_DesignObj, string sValue_To_Assign, int iPK_DesignObjParameter, string &sCMD_Result, Message *pMessage )
//<-dceag-c20-e->
{
	cout << "Need to implement command #20 - Set Object Parameter" << endl;
	cout << "Parm #3 - sPK_DesignObj = " << sPK_DesignObj << endl;
	cout << "Parm #5 - Value_To_Assign = " << sValue_To_Assign << endl;
	cout << "Parm #27 - PK_DesignObjParameter = " << iPK_DesignObjParameter << endl;
}

//<-dceag-c21-b->
/*
	COMMAND: #21 - Set Object Position
	COMMENTS: Change an objects's position on the screen
	PARAMETERS:
		#3 sPK_DesignObj
			The object to move. Can be a fully qualified object ( iX.Y.z ), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position iX
			
		#12 Position iY
			
*/
void Orbiter::CMD_Set_Object_Position( string sPK_DesignObj, int iPosition_X, int iPosition_Y, string &sCMD_Result, Message *pMessage )
//<-dceag-c21-e->
{
	cout << "Need to implement command #21 - Set Object Position" << endl;
	cout << "Parm #3 - sPK_DesignObj = " << sPK_DesignObj << endl;
	cout << "Parm #11 - Position_X = " << iPosition_X << endl;
	cout << "Parm #12 - Position_Y = " << iPosition_Y << endl;
}

//<-dceag-c22-b->
/*
	COMMAND: #22 - Set Object Size
	COMMENTS: Change an object's size
	PARAMETERS:
		#3 sPK_DesignObj
			The object to move. Can be a fully qualified object ( iX.Y.z ), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position iX
			
		#12 Position iY
			
*/
void Orbiter::CMD_Set_Object_Size( string sPK_DesignObj, int iPosition_X, int iPosition_Y, string &sCMD_Result, Message *pMessage )
//<-dceag-c22-e->
{
	cout << "Need to implement command #22 - Set Object Size" << endl;
	cout << "Parm #3 - sPK_DesignObj = " << sPK_DesignObj << endl;
	cout << "Parm #11 - Position_X = " << iPosition_X << endl;
	cout << "Parm #12 - Position_Y = " << iPosition_Y << endl;
}

//<-dceag-c23-b->
/*
	COMMAND: #23 - Set Pos Rel To Parent
	COMMENTS: Like Set Object Position, but the iX and iY coordinates are assumed to be relative to the parent rather than absolute
	PARAMETERS:
		#3 sPK_DesignObj
			The object to move. Can be a fully qualified object ( iX.Y.z ), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position iX
			
		#12 Position iY
			
*/
void Orbiter::CMD_Set_Pos_Rel_To_Parent( string sPK_DesignObj, int iPosition_X, int iPosition_Y, string &sCMD_Result, Message *pMessage )
//<-dceag-c23-e->
{
	cout << "Need to implement command #23 - Set Pos Rel To Parent" << endl;
	cout << "Parm #3 - sPK_DesignObj = " << sPK_DesignObj << endl;
	cout << "Parm #11 - Position_X = " << iPosition_X << endl;
	cout << "Parm #12 - Position_Y = " << iPosition_Y << endl;
}

//<-dceag-c24-b->
/*
	COMMAND: #24 - Set Size Rel To Parent
	COMMENTS: Change an object's size, relative to it's parent object
	PARAMETERS:
		#3 sPK_DesignObj
			The object to move. Can be a fully qualified object ( iX.Y.z ), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position iX
			The percentage of the parent object's iWidth. 100 = the parent's full iWidth.
		#12 Position iY
			The percentage of the parent object's height. 100 = the parent's full height.
*/
void Orbiter::CMD_Set_Size_Rel_To_Parent( string sPK_DesignObj, int iPosition_X, int iPosition_Y, string &sCMD_Result, Message *pMessage )
//<-dceag-c24-e->
{
	cout << "Need to implement command #24 - Set Size Rel To Parent" << endl;
	cout << "Parm #3 - sPK_DesignObj = " << sPK_DesignObj << endl;
	cout << "Parm #11 - Position_X = " << iPosition_X << endl;
	cout << "Parm #12 - Position_Y = " << iPosition_Y << endl;
}

//<-dceag-c25-b->
/*
	COMMAND: #25 - Set Text
	COMMENTS: Change the text within a text object on the fly
	PARAMETERS:
		#3 sPK_DesignObj
			The Design Object which contains the text object. Can be a fully qualified object ( iX.Y.z ), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#9 Text
			The text to assign
		#25 PK_Text
			The text object in which to store the current input
*/
void Orbiter::CMD_Set_Text( string sPK_DesignObj, string sText, int iPK_Text, string &sCMD_Result, Message *pMessage )
//<-dceag-c25-e->
{
	DesignObj_Orbiter *pObj = ( m_pScreenHistory_Current ? m_pScreenHistory_Current->m_pObj : NULL );
	if ( sPK_DesignObj.length() )
		pObj = FindObject( sPK_DesignObj );

	DesignObjText *pText = FindText( pObj, iPK_Text );
	if ( !pText )
		g_pPlutoLogger->Write( LV_CRITICAL, "SetText: cannot find object %s text %d", sPK_DesignObj.c_str(), iPK_Text );
	else
		pText->m_sText = sText;

	if ( pObj->m_bOnScreen )
		m_vectObjs_NeedRedraw.push_back( pObj );

}

//<-dceag-c26-b->
/*
	COMMAND: #26 - Set User Mode
	COMMENTS: Changes a user's mode
	PARAMETERS:
		#5 Value To Assign
			A Value from the UserMode table
		#17 PK_Users
			The User to change
*/
void Orbiter::CMD_Set_User_Mode( string sValue_To_Assign, int iPK_Users, string &sCMD_Result, Message *pMessage )
//<-dceag-c26-e->
{
	cout << "Need to implement command #26 - Set User Mode" << endl;
	cout << "Parm #5 - Value_To_Assign = " << sValue_To_Assign << endl;
	cout << "Parm #17 - PK_Users = " << iPK_Users << endl;
}

//<-dceag-c27-b->
/*
	COMMAND: #27 - Set Variable
	COMMENTS: Change the value of a variable
	PARAMETERS:
		#4 PK_Variable
			The variable to change
		#5 Value To Assign
			The value to assign
*/
void Orbiter::CMD_Set_Variable( int iPK_Variable, string sValue_To_Assign, string &sCMD_Result, Message *pMessage )
//<-dceag-c27-e->
{
	m_mapVariable[iPK_Variable] = sValue_To_Assign;
}

//<-dceag-c28-b->
/*
	COMMAND: #28 - Simulate Keypress
	COMMENTS: Simulates that a key has been touched. Touchable keys on screen can use this command to allow for simultaneous operation with keyboard or mouse. Also works with the "Capture Keyboard to Variable" command.
	PARAMETERS:
		#26 dwPK_Button
			What key to simulate being pressed
		#50 Name
			The application to send the keypress to. If not specified, it goes to the DCE device.
*/
void Orbiter::CMD_Simulate_Keypress( int idwPK_Button, string sName, string &sCMD_Result, Message *pMessage )
//<-dceag-c28-e->
{
	ButtonDown( idwPK_Button );
}

//<-dceag-c29-b->
/*
	COMMAND: #29 - Simulate Mouse Click
	COMMENTS: Simulates a mouse click or touch on the indicated iX & iY coordinates
	PARAMETERS:
		#11 Position iX
			
		#12 Position iY
			
*/
void Orbiter::CMD_Simulate_Mouse_Click( int iPosition_X, int iPosition_Y, string &sCMD_Result, Message *pMessage )
//<-dceag-c29-e->
{
	cout << "Need to implement command #29 - Simulate Mouse Click" << endl;
	cout << "Parm #11 - Position_X = " << iPosition_X << endl;
	cout << "Parm #12 - Position_Y = " << iPosition_Y << endl;
}

//<-dceag-c30-b->
/*
	COMMAND: #30 - Stop Sound
	COMMENTS: If a sound file is being played, it will be stopped.
	PARAMETERS:
*/
void Orbiter::CMD_Stop_Sound( string &sCMD_Result, Message *pMessage )
//<-dceag-c30-e->
{
	cout << "Need to implement command #30 - Stop Sound" << endl;
}

//<-dceag-c31-b->
/*
	COMMAND: #31 - Store Variables
	COMMENTS: The orbiter will store a snapshot of the variables at this moment, and if the user returns to this screen with a go back, it will restore the variables to this value.
	PARAMETERS:
*/
void Orbiter::CMD_Store_Variables( string &sCMD_Result, Message *pMessage )
//<-dceag-c31-e->
{
	cout << "Need to implement command #31 - Store Variables" << endl;
}

//<-dceag-c32-b->
/*
	COMMAND: #32 - Update Object Image
	COMMENTS: Changes the background image within an object
	PARAMETERS:
		#3 sPK_DesignObj
			The object in which to put the bitmap
		#14 Type
			1 = bmp, 2 = jpg, 3 = png
		#19 Data
			The contents of the bitmap, like reading from the file into a memory buffer
		#23 Disable Aspect Lock
			If 1, the image will be stretched to fit the object
*/
void Orbiter::CMD_Update_Object_Image( string sPK_DesignObj, string sType, char *pData, int iData_Size, string sDisable_Aspect_Lock, string &sCMD_Result, Message *pMessage )
//<-dceag-c32-e->
{
	DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj );
	if ( !pObj )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Got update object image but cannot find: %s", sPK_DesignObj.c_str() );
		return;
	}

	// todo -- need a command mutex here too?? PLUTO_SAFETY_LOCK( cm, m_bCommandMutex );

	if ( pObj )
	{
		int PriorWidth = 0, PriorHeight = 0;
		if ( pObj->m_pCurrentGraphic )
			pObj->m_pCurrentGraphic = NULL;
		if ( pObj->m_pGraphic ) {
			PriorWidth = pObj->m_pGraphic->Width;
			PriorHeight = pObj->m_pGraphic->Height;
		}
		delete pObj->m_pGraphic;
		if ( iData_Size == 0 ) {
			pObj->m_pGraphic = NULL;
			return;
		}
		WinGraphic *pWinGraphic = new WinGraphic( this );
		pObj->m_pGraphic = pWinGraphic;
		pWinGraphic->m_pCompressedImage = pData;
		pWinGraphic->m_CompressedImageLength = iData_Size;

		// We won't know the real size until after the graphic is rendered. If this a DVD menu
		// that is being touched, the touch will fail if it happens in between, so assume the
		// new image is the same size as the old until we render it.
		pWinGraphic->Width = PriorWidth;
		pWinGraphic->Height = PriorHeight;
		if ( pWinGraphic->m_CompressedImageLength > 500000 ) {
			g_pPlutoLogger->Write( LV_CRITICAL, "Load Graphic( 3 ), length is %d %s", pWinGraphic->m_CompressedImageLength, pWinGraphic->m_Filename.c_str() );
		}
		// what was this for??? Doesn't the framework delete this? It makes it impossible to call from within Orbiter -- todo m_pThisMessage->m_mapData_Parameters.clear();
		pWinGraphic->m_GraphicFormat = ( eGraphicFormat )atoi( sType.c_str() );
		pWinGraphic->m_GraphicManagement = GR_DYNAMIC;
		if ( sDisable_Aspect_Lock.length() )
			pObj->m_bDisableAspectLock = ( sDisable_Aspect_Lock == "1" ) ? true : false;
		m_vectObjs_NeedRedraw.push_back( pObj );
	}
}


//<-dceag-c66-b->
/*
	COMMAND: #66 - Select Object
	COMMENTS: The same as clicking on an object.
	PARAMETERS:
		#3 sPK_DesignObj
			The object to select.
*/
void Orbiter::CMD_Select_Object( string sPK_DesignObj, string &sCMD_Result, Message *pMessage )
//<-dceag-c66-e->
{
	DesignObj_Orbiter *pDesignObj_Orbiter = FindObject( sPK_DesignObj );
	if ( !pDesignObj_Orbiter )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Got select object %s, but can't find it", sPK_DesignObj.c_str() );
		return;
	}
	SelectedObject( pDesignObj_Orbiter );
}

//<-dceag-c72-b->
/*
	COMMAND: #72 - Surrender to OS
	COMMENTS: Let the O/S take over. This is useful with the Orbiter running on the media director's desktop as a full screen app, and media is inserted, or the user starts a computer application on the mobile phone. The orbiter will then let the other application ta
	PARAMETERS:
		#8 On/Off
			1 = Hide and let the OS take over. 0 = The orbiter comes up again.
		#54 Fully release keyboard
			Only applies if on/off is 1. If this is false, the orbiter will still filter keystrokes, looking for macros to implement, and only pass on keys that it doesn't catch. If true, it will pass all keys.
*/
void Orbiter::CMD_Surrender_to_OS( string sOnOff, bool bFully_release_keyboard, string &sCMD_Result, Message *pMessage )
//<-dceag-c72-e->
{
	m_bYieldScreen = ( sOnOff == "1" );
	m_bYieldInput = bFully_release_keyboard;

	if ( m_bYieldScreen )
		RenderDesktop( NULL, PlutoRectangle( 0, 0, -1, -1 ) ); // Full screen
	else
		RenderDesktop( NULL, PlutoRectangle( 0, 0, 0, 0 ) );
}


// Radu -- implemented scrolling into Orbiter -- Up/Down for now
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
DesignObjText *Orbiter::FindText( DesignObj_Orbiter *pObj, int iPK_Text )
{
	DesignObjText *pText = NULL;
	vector < DesignObjText * > ::iterator it_text;
	for( it_text = pObj->m_vectDesignObjText.begin();
			it_text != pObj->m_vectDesignObjText.end();
			it_text++
	   ) {
		DesignObjText *pText_Temp = * it_text;

		if ( pText_Temp->m_PK_Text == iPK_Text ) {
			pText = pText_Temp;
			break;
		}
	}

	if ( pText )
		return pText;

	// Otherwise, search all our children
	DesignObj_DataList::iterator iHao;
	for( iHao = pObj->m_ChildObjects.begin(); iHao != pObj->m_ChildObjects.end(); ++iHao ) {
		pText = FindText( ( DesignObj_Orbiter * )*iHao, iPK_Text );
		if ( pText )
			return pText;
	}
	return NULL;
}
//------------------------------------------------------------------------------------------------------------
bool Orbiter::BuildCaptureKeyboardParams( string sPK_DesignObj, int iPK_Variable, string sOnOff, string sType, string sReset, int iPK_Text, bool bDataGrid )
{
	//on/off parameter
	m_bCaptureKeyboard_bOnOff = sOnOff == "0" ? false : true;

	//normal/pin paramter
	m_bCaptureKeyboard_bTypePin = sType != "2";

	//reset parameter
	m_bCaptureKeyboard_bReset = sReset != "0";

	//iData grid
	m_bCaptureKeyboard_bDataGrid = bDataGrid;

	//set var
	m_dwPK_CaptureKeyboard_Variable = iPK_Variable;

	if ( iPK_Variable )
		m_mapVariable[iPK_Variable] = "";

	//find the parent object
	DesignObj_Orbiter *pObj;
	if ( sPK_DesignObj.length() > 0 )
		pObj = FindObject( sPK_DesignObj );
	else
		if ( NULL == m_pScreenHistory_Current ) {
			g_pPlutoLogger->Write( LV_CONTROLLER, "CMD_Capture_Keyboard_To_Variable: m_pScreenHistory_Current is null!" );
			return true;
		} else
			pObj = m_pScreenHistory_Current->m_pObj;

	//find the text object
	m_pCaptureKeyboard_Text = FindText( pObj, iPK_Text );

	if ( iPK_Variable )
		m_pCaptureKeyboard_Text->m_sText = m_mapVariable[iPK_Variable];

	//build the text string
	m_sCaptureKeyboard_Text = "";
	if ( NULL == m_pCaptureKeyboard_Text ) {
		g_pPlutoLogger->Write( LV_CONTROLLER, "CMD_Capture_Keyboard_To_Variable: text object is null!" );
		return true;
	} else {
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.X ) + ", ";
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.Y ) + ", ";
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.Width ) + ", ";
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.Height );
	}

	return true;
}
//------------------------------------------------------------------------------------------------------------
bool Orbiter::CaptureKeyboard_EditText_DeleteLastChar()
{
	string OldValue = m_mapVariable[m_dwPK_CaptureKeyboard_Variable];
	string NewValue = "";

	if ( OldValue.size() )
		NewValue = OldValue.substr( 0, OldValue.size() - 1 );

	m_sCaptureKeyboard_InternalBuffer = NewValue;
	m_mapVariable[m_dwPK_CaptureKeyboard_Variable] = NewValue;

	if ( NULL != m_pCaptureKeyboard_Text ) {
		m_pCaptureKeyboard_Text->m_sText = NewValue;
		m_vectObjs_NeedRedraw.push_back( m_pCaptureKeyboard_Text->m_pObject );
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool Orbiter::CaptureKeyboard_EditText_AppendChar( char ch )
{
	string OldValue = m_mapVariable[m_dwPK_CaptureKeyboard_Variable];
	string NewValue = OldValue + ch;

	m_sCaptureKeyboard_InternalBuffer = NewValue;
	m_mapVariable[m_dwPK_CaptureKeyboard_Variable] = NewValue;

	if ( NULL != m_pCaptureKeyboard_Text ) {
		m_pCaptureKeyboard_Text->m_sText = NewValue;
		m_vectObjs_NeedRedraw.push_back( m_pCaptureKeyboard_Text->m_pObject );
		return true;
	}

	return false;
}

//<-dceag-sample-b->!
//<-dceag-c85-b->
/*
	COMMAND: #85 - Rest Highlight
	COMMENTS: Resets the currently highlighted object. Do this when you hide or unhide blocks that have tab stops.
	PARAMETERS:
*/
void Orbiter::CMD_Rest_Highlight( string &sCMD_Result, Message *pMessage )
//<-dceag-c85-e->
{
	HighlightFirstObject();
}

bool Orbiter::ReceivedMessage( class Message *pMessageOriginal )
{
	NeedToRender render( this, "ReceivedMessage" ); // Redraw anything that was changed by this command
	bool bResult = Orbiter_Command::ReceivedMessage( pMessageOriginal );
	return bResult;
}

void Orbiter::CaptureKeyboard_UpdateVariableAndText( int iVariable, string sVariable ) //for capture keyboard cmd
{
	m_mapVariable[iVariable] = sVariable;

	/*
	if ( m_pCaptureKeyboard_Text->m_pObject )
	{
		NeedToRender render( this, "Capture update var and text" );
		m_pCaptureKeyboard_Text->m_sText = sVariable;
		m_vectObjs_NeedRedraw.push_back( m_pCaptureKeyboard_Text->m_pObject );
	}*/
}
//<-dceag-c58-b->
/* 
	COMMAND: #58 - Set Current User
	COMMENTS: Sets what user is currently using the orbiter.
	PARAMETERS:
		#17 PK_Users
			The user currently using the orbiter.
*/
void Orbiter::CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage)
//<-dceag-c58-e->
//<-dceag-c88-b->
/* 
	COMMAND: #88 - Set Current Location
	COMMENTS: Sets the location the orbiter is in.  The location is a combination of room and entertainment area.
	PARAMETERS:
		#65 LocationID
			The location ID is a sequential number created by OrbiterGen which defines a combination of room and entertainment area.
*/
void Orbiter::CMD_Set_Current_Location(int iLocationID,string &sCMD_Result,Message *pMessage)
//<-dceag-c88-e->
