/*
    MythTV_Player

    Copyright (C) 2004 Pluto, Inc., a Florida Corporation

    www.plutohome.com

    Phone: +1 (877) 758-8648

    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.
*/


/**
 *
 * @file MythTV_Player.cpp
 * @brief source file for the MythTV_Player class
 *
 */

 //<-dceag-d-b->
#include "MythTV_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <qsqldatabase.h>

#include <libmythtv/programinfo.h>
#include <libmythtv/remoteencoder.h>
#include <libmythtv/remoteutil.h>

#include <pthread.h>
#include "MythMainWindowResizable.h"


#include "utilities/linux/RatpoisonHandler.h"

class RatPoisonWrapper : public RatpoisonHandler<RatPoisonWrapper>
{
    Display *display;

public:
    RatPoisonWrapper(Display *display) : display(display) {}
    Display *getDisplay() { return display; }
};

MythContext *gContext;

// extern int print_verbose_messages;

//<-dceag-const-b->
MythTV_Player::MythTV_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{

}

bool MythTV_Player::InitMythTvStuff()
{
    int argc = 1;
    char *argv[] = { "", "", "" };


    g_pPlutoLogger->Write( LV_STATUS, "Passing params: %d", argc );

    m_pQApplication = new QApplication( argc, argv );

    if ( ! InitMythTvGlobalContext() )
    return false;

    QSqlDatabase *db = QSqlDatabase::addDatabase( "QMYSQL3" );
    if ( !db )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Could not connect to mysql database" );
        return false;
    }

    if ( !gContext->OpenDatabase( db ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Could not open mysql database" );
        return false;
    }

    m_pMythMainWindow = new MythMainWindowResizable();

    m_pMythMainWindow->setCaption("mythtv-playback-window");
    m_pRatWrapper = new RatPoisonWrapper(XOpenDisplay(NULL));

    m_pMythMainWindow->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    m_pMythMainWindow->setMinimumSize( 0, 0 );
    m_pMythMainWindow->show();

    gContext->SetMainWindow( m_pMythMainWindow );

    TV::InitKeys();

    m_pMythTV = new TV();
    m_pMythTV->Init();

//     m_pMythMainWindow->setEventFilterOnChild(m_pMythTV);

    if ( pthread_create(&m_qApplicationThreadId, NULL, ProcessQApplicationEventThreadFunction, this) != 0 )
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Could not create the video event processing thread. Bailing out!");
        return false;
    }

    pthread_detach(m_qApplicationThreadId);

    g_pPlutoLogger->Write(LV_STATUS, "TV object initialized.");
    return true;
}
//<-dceag-dest-b->
MythTV_Player::~MythTV_Player()
//<-dceag-dest-e->
{
}

void *MythTV_Player::ProcessQApplicationEventThreadFunction(void *data)
{
    MythTV_Player *m_pPlayer = (MythTV_Player *)data;

    m_pPlayer->m_pQApplication->exec();
}

bool MythTV_Player::InitMythTvGlobalContext()
{
    gContext = new MythContext( MYTH_BINARY_VERSION, false ); /** create a non GUI version to test the server connectivity. */

    if ( ! ( gContext->ConnectToMasterServer() && gContext->IsConnectedToMaster() ) )
    {
        this->EVENT_Error_Occured( "The player wasn't able to connect to the master server!" );
        delete gContext;
        return false;
    }

    delete gContext;

    gContext = new MythContext( MYTH_BINARY_VERSION );
    gContext->LoadQtConfig();

    return true;
}


bool MythTV_Player::Connect()
{
 return Command_Impl::Connect() && InitMythTvStuff();
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MythTV_Player::Register()
//<-dceag-reg-e->
{
 return Connect();
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void MythTV_Player::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void MythTV_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void MythTV_Player::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

void MythTV_Player::waitToFireMediaChanged()
{
    while ( m_pMythTV->GetState() == kState_ChangingState )
    {
        usleep(50000); //sleep for 1/2 sec;
    }

    if ( m_pMythTV->GetState() == kState_WatchingLiveTV )
    {
        m_pMythMainWindow->setNuppelPlayerToResize(m_pMythTV->activenvp);
        QString currentChannelName = m_pMythTV->activerecorder->GetCurrentChannel();

        QDateTime currentDateTime = QDateTime::currentDateTime();

        // m_pMythTV->DoInfo(); // This is error prone. It seems to cause crashed. Maybe a race somewhere

        g_pPlutoLogger->Write(LV_STATUS, "Channel:\"%s\"", currentChannelName.ascii());
        if ( currentChannelName != "" )
        {
            ProgramInfo *programInfo = ProgramInfo::GetProgramAtDateTime(
                QSqlDatabase::database(),
                    currentChannelName,
                    currentDateTime);

            if ( programInfo != NULL )
            {
                m_pRatWrapper->makeActive(m_pMythMainWindow->caption().ascii());

                EVENT_Playback_Info_Changed(programInfo->channame.ascii(), programInfo->title.ascii(), programInfo->description.ascii());
                delete programInfo;
            }
        }
        else
        {
            EVENT_Playback_Info_Changed("Not available", "Not available", "");
            EVENT_Error_Occured("The current channel is empty. This usually means that the mythtv database is inconsistent. You should try to setup the channel list again!");
        }
    }
}

/*

 COMMANDS TO IMPLEMENT

*/

//<-dceag-c75-b->

	/** @brief COMMAND: #75 - Start TV */
	/** Start TV playback on this device. */

void MythTV_Player::CMD_Start_TV(string &sCMD_Result,Message *pMessage)
//<-dceag-c75-e->
{
    if ( m_pMythTV )
    {
        if ( m_pMythTV->GetState() == kState_WatchingLiveTV || m_pMythTV->GetState() == kState_ChangingState )
        {
            g_pPlutoLogger->Write( LV_STATUS, "LiveTV is already started or is starting now" );
            return;
        }

        g_pPlutoLogger->Write( LV_STATUS, "Starting Live TV Playback!" );
        if ( m_pMythTV->LiveTV( false ) == 0 )
        {
            EVENT_Error_Occured( "We weren't able to start LiveTV." );
        }
        else
        {
            m_iControllingDevice = pMessage->m_dwPK_Device_From;
            waitToFireMediaChanged();
        }
    }
}


//<-dceag-c76-b->

	/** @brief COMMAND: #76 - Stop TV */
	/** Stop TV playback on this device. */

void MythTV_Player::CMD_Stop_TV(string &sCMD_Result,Message *pMessage)
//<-dceag-c76-e->
{
//  m_pRemoteEncoder->StopLiveTV();
 if ( m_pMythTV )
  m_pMythTV->Stop();
}

//<-dceag-c187-b->

	/** @brief COMMAND: #187 - Tune to channel */
	/** This will make the device to tune to a specific channel. */
		/** @param #68 ProgramID */
			/** The Program ID that we need to tune to. */

void MythTV_Player::CMD_Tune_to_channel(string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c187-e->
{
 if ( m_pMythTV == NULL )
 {
  EVENT_Error_Occured( "TV is not started yet" );
  return;
 }

 if ( m_pMythTV->GetState() != kState_WatchingLiveTV )
 {
  EVENT_Error_Occured( "Can't tune if not watchin live TV" );
  return;
 }

 ProgramInfo *programInfo;

 vector<string> numbers;

 StringUtils::Tokenize( sProgramID, "|", numbers );

 QString channelName = numbers[0];
 QDateTime startTime = QDateTime(
        QDate(
         atoi( numbers[1].c_str() ),
         atoi( numbers[2].c_str() ),
         atoi( numbers[3].c_str() ) ),

        QTime(
         atoi( numbers[4].c_str() ),
         atoi( numbers[5].c_str() ) ) );

 programInfo = ProgramInfo::GetProgramAtDateTime(
          QSqlDatabase::database(),
          channelName,
          startTime );

 g_pPlutoLogger->Write( LV_STATUS, "Tuning to channel %s", channelName.ascii() );

 m_pMythTV->ChangeChannelByString( channelName );

 if ( programInfo != NULL )
 {
    EVENT_Playback_Info_Changed(programInfo->channame.ascii(), programInfo->title.ascii(), programInfo->description.ascii());
    delete programInfo;
 }

 g_pPlutoLogger->Write( LV_STATUS, "Done" );
}
//<-dceag-c84-b->

	/** @brief COMMAND: #84 - Get Video Frame */
	/** Capture a Video frame */
		/** @param #19 Data */
			/** The video frame */
		/** @param #20 Format */
			/** One of the following: "jpg", "png" */
		/** @param #23 Disable Aspect Lock */
			/** If true, don't worry about the aspect ratio.  Try to get the requested width and height. */
		/** @param #41 StreamID */
			/** Optional.  For multi stream devices, like media players, this identifies the stream. */
		/** @param #60 Width */
			/** The desired width of the video frame.  The sender need not respect this. */
		/** @param #61 Height */
			/** The desired height of the video frame.  The sender need not respect this. */

void MythTV_Player::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
    g_pPlutoLogger->Write(LV_STATUS, "Method was called here");
    if ( m_pMythTV == NULL || m_pMythTV->GetState() != kState_WatchingLiveTV )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Invalid state.");
        EVENT_Error_Occured("Not playing TV at this time. Can't take a screen shot");
        return;
    }

    VideoFrame *grabbedFrame;
    VideoFrame actualFrame;

    grabbedFrame = m_pMythTV->activenvp->GetCurrentFrame(actualFrame.width, actualFrame.height);

    actualFrame.bpp = grabbedFrame->bpp;
    actualFrame.size = grabbedFrame->size;
    actualFrame.frameNumber = grabbedFrame->frameNumber;
    actualFrame.timecode = grabbedFrame->timecode;
    actualFrame.codec = grabbedFrame->codec;
    actualFrame.interlaced_frame = grabbedFrame->interlaced_frame;
    actualFrame.top_field_first = grabbedFrame->top_field_first;

    actualFrame.buf = new unsigned char[actualFrame.size];
    memcpy(actualFrame.buf, grabbedFrame->buf, actualFrame.size);

    m_pMythTV->activenvp->ReleaseCurrentFrame(grabbedFrame);

    g_pPlutoLogger->Write(LV_STATUS, "Got frame size %dx%d (%d) %d)",
                actualFrame.width, actualFrame.height,
                actualFrame.size, actualFrame.codec);

    delete actualFrame.buf;
    int x, y, nWidth, nHeight;
    m_pMythTV->activenvp->getVideoOutput()->GetDrawSize(x, y, nWidth, nHeight);

    g_pPlutoLogger->Write(LV_STATUS, "DrawSize: %dx%d %dx%d", x, y, nWidth, nHeight);

//     sCMD_Result = m_pMythTV->GetScreenFrame(sDisable_Aspect_Lock, iWidth, iHeight, pData, iData_Size, sFormat);
}


//<-dceag-c129-b->

	/** @brief COMMAND: #129 - PIP - Channel Up */
	/** Go the next channel */

void MythTV_Player::CMD_PIP_Channel_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c129-e->
{
    if ( m_pMythTV && m_pMythTV->GetState() == kState_WatchingLiveTV )
    {
        m_pMythTV->ChangeChannel(CHANNEL_DIRECTION_UP);
        waitToFireMediaChanged();
    }
}

//<-dceag-c130-b->

	/** @brief COMMAND: #130 - PIP - Channel Down */
	/** Go the previous channel. */

void MythTV_Player::CMD_PIP_Channel_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c130-e->
{
    if ( m_pMythTV && m_pMythTV->GetState() == kState_WatchingLiveTV )
    {
        m_pMythTV->ChangeChannel(CHANNEL_DIRECTION_DOWN);
        waitToFireMediaChanged();
    }
}
