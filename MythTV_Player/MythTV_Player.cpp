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

#include <sstream>
#include <qsqldatabase.h>

#include <libmythtv/programinfo.h>
#include <libmythtv/remoteencoder.h>
#include <libmythtv/remoteutil.h>

#include <pthread.h>
#include "MythMainWindowResizable.h"

#include "utilities/linux/RatpoisonHandler.h"

#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "X11/keysym.h"
#include <X11/extensions/XTest.h>

#define MYTH_WINDOW_NAME "mythfrontend"

class RatPoisonWrapper : public RatpoisonHandler<RatPoisonWrapper>
{
    Display *display;

public:
    RatPoisonWrapper(Display *display) : display(display) {}
    Display *getDisplay() { return display; }
    void commandRatPoison(string command) { RatpoisonHandler<RatPoisonWrapper>::commandRatPoison(command); }
};

MythContext *gContext;

// extern int print_verbose_messages;

//<-dceag-const-b->
MythTV_Player::MythTV_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
    LaunchMythFrontend();
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

bool MythTV_Player::LaunchMythFrontend()
{
//     char *const frontendCommand[] = { "/usr/bin/mythfrontend" };
//     forkAndWait( frontendCommand, 3 ); // make a process and wait 3 secondss
    system("`which " MYTH_WINDOW_NAME "`&");
    sleep(5);

    m_pRatWrapper = new RatPoisonWrapper(XOpenDisplay(getenv("DISPLAY")));

    locateMythTvFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay()));
}

bool MythTV_Player::Connect()
{
    return Command_Impl::Connect(); //  && InitMythTvStuff();
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
    get all commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void MythTV_Player::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->Window
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

//<-dceag-sample-b->!
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

bool MythTV_Player::forkAndWait(char *const args[], int waitTime)
{
    switch ( fork() )
    {
        case -1:
            g_pPlutoLogger->Write(LV_WARNING, "There was an error trying to fork.");
            break;
        case 0:
        {
            g_pPlutoLogger->Write(LV_STATUS, "In Child process");
            stringstream commandLine;

            int arg = 0;
            while ( args[arg] != 0 )
                commandLine << " " << args[arg++] << "";

            g_pPlutoLogger->Write(LV_STATUS, "Spawning process: %s", commandLine.str().c_str());

            int result;
            if ( (result = execv(args[0], args)) == -1 )
            {
                g_pPlutoLogger->Write(LV_WARNING, "Exec failed %d", errno);
                perror("Execve error: ");
                execl("/bin/false", NULL); // Cause the image to owerwritten so that an exit will not close the XServerDisplay
            }
        }
//             break; // it doesn't reach here anyway.
        default:
            // i'm in the parent here
            g_pPlutoLogger->Write(LV_STATUS, "Fork was succesfull. Waiting %d", waitTime);
            usleep(waitTime * 1000 * 1000);
    }

    return true;
}

void MythTV_Player::selectWindow()
{
    m_pRatWrapper->commandRatPoison(":select " MYTH_WINDOW_NAME);
}

void MythTV_Player::processKeyBoardInputRequest(int iXKeySym)
{
    Window oldWindow;
    int oldRevertBehaviour;

    if ( ! checkWindowName(m_iMythFrontendWindowId, MYTH_WINDOW_NAME) )
        LaunchMythFrontend();

    XGetInputFocus( m_pRatWrapper->getDisplay(), &oldWindow, &oldRevertBehaviour);
    XSetInputFocus( m_pRatWrapper->getDisplay(), (Window)m_iMythFrontendWindowId, RevertToParent, CurrentTime );
    XTestFakeKeyEvent( m_pRatWrapper->getDisplay(), XKeysymToKeycode(m_pRatWrapper->getDisplay(), iXKeySym), True, 0 );
    XTestFakeKeyEvent( m_pRatWrapper->getDisplay(), XKeysymToKeycode(m_pRatWrapper->getDisplay(), iXKeySym), False, 0 );
    XSetInputFocus( m_pRatWrapper->getDisplay(), oldWindow, oldRevertBehaviour, CurrentTime );

    XFlush(m_pRatWrapper->getDisplay());

    selectWindow();
}

bool MythTV_Player::checkWindowName(long unsigned int window, string windowName)
{
    XTextProperty text;

    if ( XGetWMName (m_pRatWrapper->getDisplay(), window, &text) && windowName == string((const char*)text.value) )
        return true;

    return false;
}

bool MythTV_Player::locateMythTvFrontendWindow(long unsigned int window)
{
    Window child_win, parent_win, root_win, *child_windows;
    unsigned int num_child_windows;
    XTextProperty text;

    if ( checkWindowName(window, MYTH_WINDOW_NAME ) )
    {
        m_iMythFrontendWindowId = window;
        g_pPlutoLogger->Write(LV_STATUS, "Found window id: 0x%x", window );
        return true;
    }

    /* finally, make the query for the above values. */
    XQueryTree(m_pRatWrapper->getDisplay(), (Window)window, &root_win, &parent_win, &child_windows, &num_child_windows);

    for ( int i = 0; i < num_child_windows; i++ )
        if ( locateMythTvFrontendWindow(child_windows[i]) )
            return true;

    /* we need to free the list of child IDs, as it was dynamically allocated */
    /* by the XQueryTree function.                                            */
    XFree(child_windows);
    return false;
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
//     if ( m_pMythTV )
//     {
//         if ( m_pMythTV->GetState() == kState_WatchingLiveTV || m_pMythTV->GetState() == kState_ChangingState )
//         {
//             g_pPlutoLogger->Write( LV_STATUS, "LiveTV is already started or is starting now" );
//             return;
//         }
//
//         g_pPlutoLogger->Write( LV_STATUS, "Starting Live TV Playback!" );
//         if ( m_pMythTV->LiveTV( false ) == 0 )
//         {
//             EVENT_Error_Occured( "We weren't able to start LiveTV." );
//         }
//         else
//         {
//             m_iControllingDevice = pMessage->m_dwPK_Device_From;
//             waitToFireMediaChanged();
//         }
//     }

    selectWindow();
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
//  Changing the code to not wmbed anything but forward key strokes to the application
//
//     if ( m_pMythTV == NULL )
//     {
//         EVENT_Error_Occured( "TV is not started yet" );
//         return;
//     }
//
//     if ( m_pMythTV->GetState() != kState_WatchingLiveTV )
//     {
//     EVENT_Error_Occured( "Can't tune if not watchin live TV" );
//      return;
//  }

//     ProgramInfo *programInfo;
    vector<string> numbers;
    StringUtils::Tokenize( sProgramID, "|", numbers );

    string channelNumber = numbers[0];
    for( int i = 0; i < channelNumber.size(); i++ )
    {
        switch ( channelNumber[i] )
        {
            case '0': processKeyBoardInputRequest(XK_0); break;
            case '1': processKeyBoardInputRequest(XK_1); break;
            case '2': processKeyBoardInputRequest(XK_2); break;
            case '3': processKeyBoardInputRequest(XK_3); break;
            case '4': processKeyBoardInputRequest(XK_4); break;
            case '5': processKeyBoardInputRequest(XK_5); break;
            case '6': processKeyBoardInputRequest(XK_6); break;
            case '7': processKeyBoardInputRequest(XK_7); break;
            case '8': processKeyBoardInputRequest(XK_8); break;
            case '9': processKeyBoardInputRequest(XK_9); break;
            default:
                g_pPlutoLogger->Write(LV_STATUS, "Invalid character %c in channel identifier %s", channelNumber[i], channelNumber.c_str());
        }
    }
/*
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

 g_pPlutoLogger->Write( LV_STATUS, "Done" );*/
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
//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Enter was hit */

void MythTV_Player::CMD_EnterGo(string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
    processKeyBoardInputRequest(XK_Return);
}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Up */

void MythTV_Player::CMD_Move_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
    processKeyBoardInputRequest(XK_Up);
}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Down */

void MythTV_Player::CMD_Move_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
    processKeyBoardInputRequest(XK_Down);
}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Left */

void MythTV_Player::CMD_Move_Left(string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
    processKeyBoardInputRequest(XK_Left);
}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Right */

void MythTV_Player::CMD_Move_Right(string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
    processKeyBoardInputRequest(XK_Right);
}

//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void MythTV_Player::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
    processKeyBoardInputRequest(XK_0);
}

//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void MythTV_Player::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
    processKeyBoardInputRequest(XK_1);
}

//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void MythTV_Player::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
    processKeyBoardInputRequest(XK_2);
}

//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void MythTV_Player::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
    processKeyBoardInputRequest(XK_3);
}

//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void MythTV_Player::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
    processKeyBoardInputRequest(XK_4);
}

//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void MythTV_Player::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
    processKeyBoardInputRequest(XK_5);
}

//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void MythTV_Player::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
    processKeyBoardInputRequest(XK_6);
}

//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void MythTV_Player::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
    processKeyBoardInputRequest(XK_7);
}

//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void MythTV_Player::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
    processKeyBoardInputRequest(XK_8);
}

//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void MythTV_Player::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
    processKeyBoardInputRequest(XK_9);
}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back */
	/** Navigate back .. ( Escape ) */

void MythTV_Player::CMD_Back(string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
    processKeyBoardInputRequest(XK_Escape);
}
