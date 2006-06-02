/*
 OrbiterBluetooth
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "OrbiterBluetooth.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/PlutoDefs.h"
#include "DCE/Logger.h"
#include "OrbiterBluetooth.h"
#include "SerializeClass/ShapesColors.h"
#include "BD/BDCommandProcessor.h"
#include "VIPShared/BD_CP_ShowImage.h"
#include "VIPShared/BD_CP_ShowList.h"
#include "VIPShared/BD_CP_CaptureKeyboard.h"
#include "VIPShared/BD_CP_SimulateEvent.h"
#include "VIPShared/PlutoPhoneCommands.h"

#include "Orbiter/ScreenHistory.h"
#include "Orbiter/SDL/JpegWrapper.h"

#define ADVANCED_OPTIONS_SCREEN "2022"
#define IMAGE_QUALITY_SCREEN    "1274"

#include "DataGrid.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
OrbiterBluetooth::OrbiterBluetooth(class BDCommandProcessor *pBDCommandProcessor, int DeviceID, 
    int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, bool bLocalMode, int nImageWidth, 
    int nImageHeight, pluto_pthread_mutex_t *pExternalScreenMutex)
: Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory, bLocalMode, nImageWidth, 
    nImageHeight, pExternalScreenMutex), m_pBDCommandProcessor(pBDCommandProcessor)
{
    m_bShowListSent = false;
    m_ImageQuality = 70;
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterBluetooth::GetConfig()
{
    if(!Orbiter::GetConfig())
        return false;

    m_ImageQuality = DATA_Get_ImageQuality();

    if(m_ImageQuality < 10 || m_ImageQuality > 100)
        m_ImageQuality = 70; //default

    m_bDisplayOn=true;  // Override the default behavior -- when the phone starts the display is already on

    return true;
}
//-----------------------------------------------------------------------------------------------------
OrbiterBluetooth::~OrbiterBluetooth()
{
	//don't destroy anything until maint thread exists
	KillMaintThread();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::RenderDataGrid(DesignObj_DataGrid *pObj, PlutoPoint point)
{
#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS, "Extraoptions in grid: %s", pObj->m_sExtraInfo.c_str());
#endif

#if (defined(PROFILING))
    clock_t clkStart = clock();
#endif

    //sending datagrid off command
    list<string> listGridClean;
    listGridClean.clear();
    BD_CP_ShowList *pBD_CP_ShowList_Off = new BD_CP_ShowList(0, 0, 0, 0, 0, listGridClean, false, false);
    if( m_pBDCommandProcessor )
        m_pBDCommandProcessor->AddCommand(pBD_CP_ShowList_Off);

    int iSelectedColumn = pObj->m_iInitialColNum;
	bool bUsePhoneGrid = false;

	//if 'c' - column  extraoption is specified, we'll send to phone the specified column
	size_t sPos;
	if((sPos = pObj->m_sExtraInfo.find( 'c' )) != string::npos)
	{
		bUsePhoneGrid=true;
		if(sPos + 1 < pObj->m_sExtraInfo.size())
			iSelectedColumn = pObj->m_sExtraInfo[sPos + 1] - '0';
	}

    //if 'F' option is specified, we'll let the base the render the grid
    if(pObj->m_sExtraInfo.find( 'F' ) != string::npos) 
    {
#ifdef DEBUG
        g_pPlutoLogger->Write(LV_WARNING, "OrbiterBluetooth: I won't render this grid on the phone");
#endif
        Orbiter::RenderDataGrid(pObj);
        return;
    }

	pObj->m_MaxRow = 0; //get all rows

    string Unused;
    Orbiter::PrepareRenderDataGrid(pObj, Unused);

	if(pObj->m_pDataGridTable)
    {
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_WARNING, "Got to render a datagrid with %d columns", pObj->m_pDataGridTable->m_ColumnCount);
#endif
		if(pObj->m_pDataGridTable->m_ColumnCount == 1)//we can render on column datagrid
			bUsePhoneGrid = true;

        if(!bUsePhoneGrid)
        {
            g_pPlutoLogger->Write(LV_WARNING, "OrbiterBluetooth: I won't render this grid on the phone");
			Orbiter::RenderDataGrid(pObj);
            return;
        }

		pObj->m_MaxRow = pObj->m_pDataGridTable->getTotalRowCount();

        int x       = pObj->m_rPosition.X;
        int y       = pObj->m_rPosition.Y;
        int Width   = pObj->m_rPosition.Width;
        int Height  = pObj->m_rPosition.Height;
        list<string> listGrid;
		bool bSendSelectedOnMove = false; 

        //if 'T' extraoptions is specified, then when user presses up/down buttons, PlutoMO will send a SelectedItem command
		if(pObj->m_sExtraInfo.find( 'T' ) != string::npos)
			bSendSelectedOnMove = true;

		bool bTurnOn = true;

        string sCurrentSelected = m_mapVariable[atoi(pObj->sSelVariable.c_str())];
        int iHighlightedRow = pObj->m_iHighlightedRow >= 0 ? pObj->m_iHighlightedRow : atoi(sCurrentSelected.c_str());

#ifdef DEBUG
		g_pPlutoLogger->Write(LV_WARNING, "About to send BD_CP_ShowList command, column %d, turnon %d, items count %d, selected item %d, send 'selected item' %d",
				iSelectedColumn, bTurnOn, pObj->m_pDataGridTable->getTotalRowCount(), iHighlightedRow, (int)bSendSelectedOnMove);
#endif

        for(int i = 0; i < pObj->m_pDataGridTable->getTotalRowCount(); i++)
        {
            DataGridCell * pCell = pObj->m_pDataGridTable->GetData(iSelectedColumn, i);
			string sItem = pCell != NULL ? pCell->GetText() : "<empty>";
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS, "Item %d : '%s'. Ignoring esc seq for now...", i, sItem.c_str());
#endif
            
            //ignoring esc seq
            size_t nPos = sItem.find("~S");
            if(nPos != string::npos)
            {
                size_t nPosNext = sItem.find("~", nPos + 1);
                if(nPosNext != string::npos)
                    sItem = sItem.substr(nPosNext + 1);
            }            

#ifdef DEBUG
            g_pPlutoLogger->Write(LV_STATUS, "Item %d : '%s'.", i, sItem.c_str());
#endif

            listGrid.push_back(sItem);
        }
        
        BD_CP_ShowList *pBD_CP_ShowList = new BD_CP_ShowList(x, y, Width, Height, iHighlightedRow, listGrid, 
			bSendSelectedOnMove, bTurnOn);

        if( m_pBDCommandProcessor )
        {
            m_pBDCommandProcessor->AddCommand(pBD_CP_ShowList);
            m_bShowListSent = true;
        }

        listGrid.clear();
    }
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage)
{
    if(
        Orbiter::BuildCaptureKeyboardParams(
            sPK_DesignObj,
            iPK_Variable,
            sOnOff,
            sType,
            sReset,
            iPK_Text,
            bDataGrid
        )
    )
    {
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_WARNING, "Sending BD_CP_CaptureKeyboard command with parameters: On: %d, Reset: %d, PinType: %d, DataGrid: %d, PK_Variable: %d, Text: %s",
			m_bCaptureKeyboard_OnOff, m_bCaptureKeyboard_Reset, m_iCaptureKeyboard_EditType,
			m_bCaptureKeyboard_DataGrid, m_iCaptureKeyboard_PK_Variable, m_sCaptureKeyboard_Text.c_str()
		);
#endif
		BD_CP_CaptureKeyboard *pBD_CP_CaptureKeyboard = new BD_CP_CaptureKeyboard(
				m_bCaptureKeyboard_OnOff,
				m_bCaptureKeyboard_DataGrid,
                m_bCaptureKeyboard_Reset,
				m_iCaptureKeyboard_EditType,
				m_iCaptureKeyboard_PK_Variable,
				m_sCaptureKeyboard_Text
		);
                
		if( m_pBDCommandProcessor )
			m_pBDCommandProcessor->AddCommand(pBD_CP_CaptureKeyboard);
    }
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::SimulateMouseClick(int x, int y)
{
	//do nothing
#ifndef WIN32
	printf("OrbiterBluetooth::SimulateMouseClick. The event will be ignored (we don't have mouse on the phone)\n");
#endif	
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::SimulateKeyPress(long key)
{
	BD_CP_SimulateEvent *pBD_CP_SimulateEvent = new BD_CP_SimulateEvent(0, key);

	if( m_pBDCommandProcessor )
		m_pBDCommandProcessor->AddCommand(pBD_CP_SimulateEvent);

#ifndef WIN32
	printf("OrbiterBluetooth::SimulateKeyPress with key code: %d\n", key);
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::OnReload()
{  
    g_pPlutoLogger->Write(LV_WARNING,"Orbiter reloading");

    //kill the connection. this will kill orbiter and then recreate it when the phone will be detected
    m_pBDCommandProcessor->m_bDead = true;
}  
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::SetImageQuality(unsigned long nImageQuality)
{
    m_ImageQuality = nImageQuality;
    DATA_Set_ImageQuality(nImageQuality, true);

    m_bRerenderScreen = true;
    RedrawObjects();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterBluetooth::ImageGenerated(const string& csImageFileName)
{
	bool bSignalStrengthScreen = false;
	bool bQualityImageScreen = false;
	if(NULL != m_pScreenHistory_Current)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Current screen: %s",  m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());
		bSignalStrengthScreen = m_pScreenHistory_Current->GetObj()->m_ObjectID.find(ADVANCED_OPTIONS_SCREEN) != string::npos;
	}

	if(NULL != m_pScreenHistory_Current)
	{
		bQualityImageScreen = m_pScreenHistory_Current->GetObj()->m_ObjectID.find(IMAGE_QUALITY_SCREEN) != string::npos;
	}

	string sRepeatedKeysList;
	GetRepeatedKeysForScreen(m_pScreenHistory_Current->GetObj(), sRepeatedKeysList);
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "Repeated keys list %s: ", sRepeatedKeysList.c_str());
#endif

	//load the image into a buffer and create 'BD_CP_ShowImage' command
	size_t iImageSize;
	char *pImage = FileUtils::ReadFileIntoBuffer(csImageFileName, iImageSize);

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "Ready to send a picture, size %d, reporting signal strength %d", iImageSize, bSignalStrengthScreen);
#endif

	BD_CP_ShowImage *pBD_CP_ShowImage = new BD_CP_ShowImage(0, (unsigned long)iImageSize, pImage, 
		(unsigned long)(sRepeatedKeysList.length()), sRepeatedKeysList.c_str(), bSignalStrengthScreen,
		bQualityImageScreen, m_ImageQuality);

	PLUTO_SAFE_DELETE_ARRAY(pImage);

	//hide any list if need it sending a 'BD_CP_ShowList' with Off option
	if( m_bShowListSent && !m_vectObjs_GridsOnScreen.size() )
	{
		list<string> listGrid;
		BD_CP_ShowList *pBD_CP_ShowList = new BD_CP_ShowList(0, 0, 0, 0, 0, listGrid, false, false);
		if( m_pBDCommandProcessor )
		{
			m_pBDCommandProcessor->AddCommand(pBD_CP_ShowList);
			m_bShowListSent = false;
		}
	}

	//finally, send the image
	if( m_pBDCommandProcessor )
		m_pBDCommandProcessor->AddCommand(pBD_CP_ShowImage);

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS, "ShowImage command added to the queue");
#endif

}
//-----------------------------------------------------------------------------------------------------