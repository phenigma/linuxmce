//<-dceag-d-b->
#include "MythTV_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <qsqldatabase.h>
MythContext *gContext;

//<-dceag-const-b->
MythTV_Player::MythTV_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
    int argc = 1;
    char *argv[] = { "", "", "" };

    g_pPlutoLogger->Write(LV_STATUS, "Passing params: %d", argc);

    m_pQApplication = new QApplication(argc, argv);

    if ( ! initMythTVGlobalContext() )
    {
        delete m_pQApplication;
        throw "Could not init Myth Global Context";
    }

    m_pMythMainWindow = new MythMainWindow();
    m_pMythMainWindow->Init();
    // make a window regardless of what the myth Tv conf wants.
    m_pMythMainWindow->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_pMythMainWindow->resize(QSize(800, 600));
    m_pMythMainWindow->showNormal();


    gContext->SetMainWindow(m_pMythMainWindow);

    TV::InitKeys();

    m_pMythTV = new TV();
    m_pMythTV->Init();
}

//<-dceag-dest-b->
MythTV_Player::~MythTV_Player()
//<-dceag-dest-e->
{
}

bool MythTV_Player::initMythTVGlobalContext()
{
    gContext = new MythContext(MYTH_BINARY_VERSION);
    gContext->LoadQtConfig();

    QSqlDatabase *db = QSqlDatabase::addDatabase("QMYSQL3");
    if (!db)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Could not connect to mysql database");
        return false;
    }

    if (!gContext->OpenDatabase(db))
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Could not open mysql database");
        return false;
    }
}
//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MythTV_Player::Register()
//<-dceag-reg-e->
{
    return Connect();
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

void MythTV_Player::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DERC:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Send Orbiters the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.

	// Send the message to a specific orbiter, identified by OrbiterID
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);

	// Send the message to orbiters 32898 and 27283
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,"32898,27283",55,77);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);

	// This time we want to wait to be sure the orbiter gets the message, and the thread will block until the orbiter receives the message
	string sResult="";
	bool bResult = SendMessageWithConfirm(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77),sResult);
	// If bResult is true, the message was received ok.  Otherwise it failed, and sResult contains an explanation of the failure

	// A request is like a command, except that it has both "in" and "out" parameters, and the 
	// thread blocks until the request is completed.  Note that requests are sent on a separate socket, so you can continue to receive commands and send
	// events while a thread is waiting for a request to complete.  However the SendMessageWithConfirm uses the same request socket, and will wait if
	// the socket is busy.  

	// We want to request the contents of a file.  This request takes 1 "in" parameter, the file name, and has 2 "out" parameters: the file's contents, and the date.
	// All out parameters are passed by reference.
	// Note that the "Data" parameter which contains the file is a binary parameter.  Whether in or out, binary parameters always take both a pointer to the object and the size
	// This command will be sent to the "Standard Plug-in" which implements this type of request.  We set the boardcast level to BL_SameHouse.  Note that since this is a
	// request, the router will find the first matching device (ie the first device "Standard Plug-in") and return that device's response.  If this was a command, not a request,
	// and there were multiple devices "Standard Plug-in" they would all get the message.
	PlutoDate plutoDate;
	char *FileContents;
	int FileSize;
	bool bResult = SendRequest(new DERC::REQ_File_Contents(m_dwPK_Device,DeviceTemplate_Standard_Plug_In_CONST,true,BL_SameHouse,"some_file_name",&FileContents,&FileSize,&plutoDate);

	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();


}
*/
//<-dceag-sample-e->

/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c75-b->
/* 
	COMMAND: #75 - Start TV
	COMMENTS: Start TV playback on this device.
	PARAMETERS:
*/
void MythTV_Player::CMD_Start_TV(string &sCMD_Result,Message *pMessage)
//<-dceag-c75-e->
{
    if ( m_pMythTV )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Starting Live TV Playback!");
        m_pMythTV->LiveTV(true);
    }
}

//<-dceag-c76-b->
/* 
	COMMAND: #76 - Stop TV
	COMMENTS: Stop TV playback on this device.
	PARAMETERS:
*/
void MythTV_Player::CMD_Stop_TV(string &sCMD_Result,Message *pMessage)
//<-dceag-c76-e->
{
    m_pMythTV->StopLiveTV();
}


