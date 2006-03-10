//<-dceag-d-b->
#include "USB_UIRT_0038.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "PlutoUtils/LinuxSerialUSB.h"

USB_UIRT_0038 *g_pUsbUirt;


#include "uuirtdrv.h"

#ifdef __linux
#include <dlfcn.h>
static void *		hinstLib = NULL; 
#else
 HINSTANCE		hinstLib = NULL; 
#endif

// Driver handle for UUIRT device
HUUHANDLE		hDrvHandle;

unsigned int	drvVersion;

// Globals to hold last-learned IR-Code and its format...
char	gIRCode[2048] = "0000 0048 0001 0011 0017 0163 0017 00a3 0016 00a3 0017 00a3 0016 0062 0017 0061 0016 00a3 0017 00a3 0016 00a3 0017 00a3 0017 00a3 0016 00a3 0017 00a3 0016 00a3 0017 00a3 0016 00a3 0017 00a3 0017 0163";
int		gIRCodeFormat = UUIRTDRV_IRFMT_PRONTO;

// Globals used during Learning...
char	gLearnBuffer[2048];
int		gLearnFormat;

// UUIRT .dll function pointers. These will be assigned when calling LoadDLL()
pfn_UUIRTOpenEx				fnUUIRTOpenEx;
pfn_UUIRTClose				fnUUIRTClose;
pfn_UUIRTGetDrvInfo			fn_UUIRTGetDrvInfo;
pfn_UUIRTGetUUIRTInfo		fn_UUIRTGetUUIRTInfo;
pfn_UUIRTGetUUIRTConfig		fn_UUIRTGetUUIRTConfig;
pfn_UUIRTSetUUIRTConfig		fn_UUIRTSetUUIRTConfig;
pfn_UUIRTSetReceiveCallback	fn_UUIRTSetReceiveCallback;
pfn_UUIRTTransmitIR			fn_UUIRTTransmitIR;
pfn_UUIRTLearnIR			fn_UUIRTLearnIR;

#ifdef __linux

#define DEVICE_PATH "/dev/ttyUSB0"

void *LoadLibrary(const char* LibName)
{
	char namePath[256];
	strcpy(namePath,"./");
	strcat(namePath, LibName);
	strcat(namePath,".so");
	return dlopen(namePath, RTLD_NOW);
};

bool FreeLibrary(void* handle)
{
	return dlclose(handle) == 0;	
};

void *GetProcAddress(void* handle, const char* Name)
{
	return dlsym(handle,Name);	
};

#endif 

/*****************************************************************************/
/* unLoadDLL: Disconnects from .DLL and unloads it from memory				 */
/*																			 */
/* returns: none															 */
/*																			 */
/*****************************************************************************/
void unLoadDLL(void)
{
	if (hinstLib)
		FreeLibrary(hinstLib);
	hinstLib = NULL;
}

/*****************************************************************************/
/* loadDLL: Establish contact with the UUIRTDRV dll and assign function      */
/*			entry points													 */
/*																			 */
/* returns: TRUE on success, FALSE on failure								 */
/*																			 */
/*****************************************************************************/
BOOL loadDLL(void)
{
    // Get a handle to the DLL module.
 
#ifdef WIN32
	hinstLib = LoadLibrary("uuirtdrv");
#else
    hinstLib = LoadLibrary("../lib/uuirtdrv"); 
#endif
 
    // If the handle is valid, try to get the function address.
 
    if (hinstLib != NULL) 
    { 
        fnUUIRTOpenEx = (pfn_UUIRTOpenEx) GetProcAddress(hinstLib, "UUIRTOpenEx");
        fnUUIRTClose = (pfn_UUIRTClose) GetProcAddress(hinstLib, "UUIRTClose");
		fn_UUIRTGetDrvInfo  = (pfn_UUIRTGetDrvInfo) GetProcAddress(hinstLib, "UUIRTGetDrvInfo");
		fn_UUIRTGetUUIRTInfo = (pfn_UUIRTGetUUIRTInfo) GetProcAddress(hinstLib, "UUIRTGetUUIRTInfo");
		fn_UUIRTGetUUIRTConfig = (pfn_UUIRTGetUUIRTConfig) GetProcAddress(hinstLib, "UUIRTGetUUIRTConfig");
		fn_UUIRTSetUUIRTConfig = (pfn_UUIRTSetUUIRTConfig) GetProcAddress(hinstLib, "UUIRTSetUUIRTConfig");
		fn_UUIRTSetReceiveCallback = (pfn_UUIRTSetReceiveCallback) GetProcAddress(hinstLib, "UUIRTSetReceiveCallback");
		fn_UUIRTTransmitIR = (pfn_UUIRTTransmitIR) GetProcAddress(hinstLib, "UUIRTTransmitIR");
		fn_UUIRTLearnIR = (pfn_UUIRTLearnIR) GetProcAddress(hinstLib, "UUIRTLearnIR");

		if (!fnUUIRTOpenEx || 
			!fnUUIRTClose || 
			!fn_UUIRTGetDrvInfo || 
			!fn_UUIRTGetUUIRTInfo || 
			!fn_UUIRTGetUUIRTConfig || 
			!fn_UUIRTSetUUIRTConfig || 
			!fn_UUIRTSetReceiveCallback || 
			!fn_UUIRTTransmitIR || 
			!fn_UUIRTLearnIR)
		{
			unLoadDLL();
			return FALSE;
		}

		return TRUE;
	}
	return FALSE;
}

/*****************************************************************************/
/* IRReceiveCallback: Receive IR Callback Procedure						     */
/*																			 */
/* This procedure is called by the UUIRT .dll whenever an IRcode is received */
/* The IRcode is passed to the callback in UIR format.						 */
/*																			 */
/*****************************************************************************/
void WINAPI IRReceiveCallback (char *IREventStr, void *userData)
{
	g_pUsbUirt->OurCallback(IREventStr);

	g_pPlutoLogger->Write(LV_STATUS, "<IR Receive: Code = %s, UserData = %08x!!!\n", IREventStr, (UINT32)userData);
}

/*****************************************************************************/
/* IRLearnCallback: Learn IR Callback Procedure						         */
/*																			 */
/* This procedure is called by the UUIRT .dll during the LEARN process		 */
/* to allow user feedback on Learn progress, signal quality and (if needed)  */
/* carrier frequency.														 */
/*																			 */
/*****************************************************************************/
void WINAPI IRLearnCallback (unsigned int progress, unsigned int sigQuality, unsigned long carrierFreq, void *userData)
{
	g_pPlutoLogger->Write(LV_STATUS,"<Learn Progress: %d%%, Signal = %d%%, Freq = %ld, UserData = %08x!!!\n", progress, sigQuality & 0xff, carrierFreq, (UINT32)userData);
}


/*****************************************************************************/
/* LearnThread: Learn IR Thread function									 */
/*																			 */
/* This function executes as a separate thread which calls the UUIRTLearnIR  */
/* function.  In this example, the UUIRTLearnIR function is called from this */
/* separate thread to allow the main console thread to continue monitoring   */
/* the keyboard so that the user may abort the learn process. Depending on   */
/* the application, the UUIRTLearnIR may be called from the main thread if   */
/* an asynchronous method (such as a timer) is available to monitor user     */
/* input.																	 */
/*																			 */
/*****************************************************************************/
void LearnThread( void *lpParameter )
{
	USB_UIRT_0038 *pUIRT = (USB_UIRT_0038 *)lpParameter;

	pUIRT->LearningThread();
}



//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
USB_UIRT_0038::USB_UIRT_0038(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: USB_UIRT_0038_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, IRReceiverBase(this), m_UIRTMutex("UIRT")
{
	g_pUsbUirt=this; // Used for the callback

}

//<-dceag-const2-b->!

//<-dceag-dest-b->
USB_UIRT_0038::~USB_UIRT_0038()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool USB_UIRT_0038::GetConfig()
{
	if( !USB_UIRT_0038_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	if( !m_Virtual_Device_Translator.GetConfig(m_pData) )
		return false;

	IRBase::setCommandImpl(this);
	IRBase::setAllDevices(&(GetData()->m_AllDevices));
	IRReceiverBase::GetConfig(m_pData);
	DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Infrared_Plugins_CONST);
	if( pDevice )
		m_dwPK_Device_IRPlugin = pDevice->m_dwPK_Device;
	else
		m_dwPK_Device_IRPlugin = 0;

	m_iRepeat=1; // DATA_Get_Repeat();

	// Find all our sibblings that are remote controls 
	for(Map_DeviceData_Base::iterator itD=m_pData->m_AllDevices.m_mapDeviceData_Base.begin();
		itD!=m_pData->m_AllDevices.m_mapDeviceData_Base.end();++itD)
	{
		DeviceData_Base *pDevice = itD->second;
		if( pDevice->m_dwPK_Device_ControlledVia==m_pData->m_dwPK_Device_ControlledVia &&
			pDevice->m_dwPK_DeviceCategory==DEVICECATEGORY_USBUIRT_Remote_Controls_CONST )
		{
			g_pPlutoLogger->Write(LV_STATUS,"Using remote %d %s",pDevice->m_dwPK_Device,pDevice->m_sDescription.c_str());
			string sType;
			DCE::CMD_Get_Device_Data_Cat CMD_Get_Device_Data_Cat2(m_dwPK_Device,DEVICECATEGORY_General_Info_Plugins_CONST,true,BL_SameHouse,
				pDevice->m_dwPK_Device,DEVICEDATA_Remote_Layout_CONST,true,&sType);
			SendCommand(CMD_Get_Device_Data_Cat2);

			string sConfiguration;
			DCE::CMD_Get_Device_Data_Cat CMD_Get_Device_Data_Cat(m_dwPK_Device,DEVICECATEGORY_General_Info_Plugins_CONST,true,BL_SameHouse,
				pDevice->m_dwPK_Device,DEVICEDATA_Configuration_CONST,true,&sConfiguration);

			if( SendCommand(CMD_Get_Device_Data_Cat) && sConfiguration.size() )
			{
				vector<string> vectCodes;
				StringUtils::Tokenize(sConfiguration,"\r\n",vectCodes);
				for(size_t s=0;s<vectCodes.size();++s)
				{
					string::size_type pos=0;
					string sButton = StringUtils::Tokenize(vectCodes[s]," ",pos);
					while(pos<vectCodes[s].size())
					{
						string sCode = StringUtils::Tokenize(vectCodes[s]," ",pos);
						m_mapCodesToButtons[sCode] = make_pair<string,int> (sButton,pDevice->m_dwPK_Device);
						// Jon -- sCode will the code in whatever format you want.  sButton is the button name it corresponds to
						g_pPlutoLogger->Write(LV_STATUS,"Code: %s will fire button %s",sCode.c_str(),sButton.c_str());
					}
				}
			}
		}
	}

#ifdef __linux
	char devicePath[81];
	// TODO: Does this need to be a config option?
	strcpy(devicePath, DEVICE_PATH);
#endif

	if (!loadDLL())
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ERROR: Unable to load uuirtdrv,dll!\n");
		return 0;
	}

	if (!fn_UUIRTGetDrvInfo(&drvVersion))
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ERROR: Unable to retrieve uuirtdrv version!\n");
		unLoadDLL();
		return 0;
	}

	if (drvVersion != 0x0100)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ERROR: Invalid uuirtdrv version!\n");
		unLoadDLL();
		return 0;
	}

	string sComPortOnPC = DATA_Get_COM_Port_on_PC();
	g_pPlutoLogger->Write(LV_STATUS,"In start IR Server %s",sComPortOnPC.c_str());

	char TTYPort[255];
	TTYPort[0]=0;
#ifndef WIN32
	if( sComPortOnPC.size() && sComPortOnPC.size()<255 )
		strcpy(TTYPort,TranslateSerialUSB(sComPortOnPC).c_str());
	
	hDrvHandle = fnUUIRTOpenEx(TTYPort,0,0,0);
#else
	hDrvHandle = fnUUIRTOpenEx("USB-UIRT",0,0,0);
#endif
	if (hDrvHandle == INVALID_HANDLE_VALUE)
	{
		DWORD err;

#ifdef WIN32
		err = GetLastError();
#else
		err = errno;
#endif

		if (err == UUIRTDRV_ERR_NO_DLL)
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"ERROR: Unable to find USB-UIRT Driver. Please make sure driver is Installed!\n");
		}
		else if (err == UUIRTDRV_ERR_NO_DEVICE)
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"ERROR: Unable to connect to USB-UIRT device!  Please ensure device is connected to the computer!\n");
		}
		else if (err == UUIRTDRV_ERR_NO_RESP)
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"ERROR: Unable to communicate with USB-UIRT device on %s!  Please check connections and try again.  If you still have problems, try unplugging and reconnecting your USB-UIRT.  If problem persists, contact Technical Support!\n",TTYPort );
		}
		else if (err == UUIRTDRV_ERR_VERSION)
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"ERROR: Your USB-UIRT's firmware is not compatible with this API DLL. Please verify you are running the latest API DLL and that you're using the latest version of USB-UIRT firmware!  If problem persists, contact Technical Support!\n");
		}
		else
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"ERROR: Unable to initialize USB-UIRT (unknown error)!\n");
		}

		unLoadDLL();

		return 0;
	}

	printf("\n");

	// Register a callback function for IR receive...
	fn_UUIRTSetReceiveCallback(hDrvHandle, &IRReceiveCallback, (void *)0xA5A5A5A5);



	// Jon -- Initialize the USB UIRT here
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool USB_UIRT_0038::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->!
/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void USB_UIRT_0038::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	// Let the IR Base class try to handle the message
	if (IRBase::ProcessMessage(pMessage))
	{
		printf("Message processed by IRBase class\n");
		sCMD_Result = "OK";
		return;
	}
	
	if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && pMessage->m_dwID==COMMAND_Learn_IR_CONST )
	{
		// Jon -- We must start or stop the learning process here
		if( pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST]=="1" )
			StartLearning(pMessage->m_dwPK_Device_To,atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_Command_CONST].c_str()),
				pMessage->m_dwPK_Device_From,
				atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_Text_CONST].c_str()));
		else
			StopLearning();
		sCMD_Result = "OK";
		return;
	}

	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void USB_UIRT_0038::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void USB_UIRT_0038::SomeFunction()
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

void USB_UIRT_0038::SendIR(string Port, string IRCode)
{
	if( m_bLearningIR )
		StopLearning();

	char *pBuffer = (char *)IRCode.c_str();
	size_t size = IRCode.size();
	bool bDeleteBuffer=false;
	if( pBuffer[0]=='/' )
	{
		bDeleteBuffer=true;
		pBuffer = FileUtils::ReadFileIntoBuffer(pBuffer,size);
	}
	
	g_pPlutoLogger->Write(LV_STATUS,"UsbUirt Sending: %s",pBuffer);


	if (!fn_UUIRTTransmitIR(hDrvHandle,
										pBuffer /* IRCode */,
										gIRCodeFormat /* codeFormat */,
										10 /* repeatCount */,
										0 /* inactivityWaitTime */,
										NULL /* hEvent */,
										NULL /* reserved1 */,
										NULL /* reserved2 */
										))
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"UsbUirt transmit failed sending %s!",pBuffer);
	}

	// Jon this function needs to hook into your stuff to send the code contained in pBuffer
/*    int res = UsbUirt_transmit(m_iRepeat, 
                            -1, 
                            (const unsigned char *) pBuffer,
                            size);*/

//    if ( res != 0 ) 
//		g_pPlutoLogger->Write(LV_CRITICAL,"UsbUirt failed Sending(%d): %s",res,IRCode.c_str());

	if( bDeleteBuffer )
		delete pBuffer;
}

// Must override so we can call IRBase::Start() after creating children
void USB_UIRT_0038::CreateChildren()
{
	USB_UIRT_0038_Command::CreateChildren();
	Start();
}

void USB_UIRT_0038::OurCallback(const char *szButton)
{
	timespec ts_now;
	gettimeofday(&ts_now,NULL);

	if( m_sLastButton==szButton )
	{
		timespec ts_diff = ts_now-m_tsLastButton;
		if( ts_diff.tv_sec==0 && ts_diff.tv_nsec<500000000 )
			return;
	}
	else
	{
		m_sLastButton=szButton;
		m_tsLastButton=ts_now;
	}

	map<string,pair<string,int> >::iterator it=m_mapCodesToButtons.find(szButton);
	if( it==m_mapCodesToButtons.end() )
		g_pPlutoLogger->Write(LV_WARNING,"Cannot find anything for IR %s",szButton);
	else
		ReceivedCode(it->second.second,it->second.first.c_str());
}


void USB_UIRT_0038::LearningThread()
{
	g_pPlutoLogger->Write(LV_STATUS, "\nCalling LearnIR...");
	if (!fn_UUIRTLearnIR(hDrvHandle, UUIRTDRV_IRFMT_PRONTO, gLearnBuffer, IRLearnCallback, (void *)0x5a5a5a5a, &m_LrnAbort, 0, NULL, NULL))
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"*** ERROR calling UUIRTLearnIR! ***\n");
	}
	else
	{
		if (m_LrnAbort <= 0)
		{
			g_pPlutoLogger->Write(LV_STATUS,"Learned code device %d command %d orbiter %d text %d code %s",
						m_iPK_Device_Learning,m_iPK_Command_Learning,m_iPK_Orbiter,m_iPK_Text,gLearnBuffer);
			DCE::CMD_Store_Infrared_Code CMD_Store_Infrared_Code(m_dwPK_Device,m_dwPK_Device_IRPlugin,
					m_iPK_Device_Learning,gLearnBuffer,m_iPK_Command_Learning);
			SendCommand(CMD_Store_Infrared_Code);
			getCodeMap()[longPair(m_iPK_Device_Learning, m_iPK_Command_Learning)] = gLearnBuffer;
			if( m_iPK_Orbiter && m_iPK_Text )
			{
				DCE::CMD_Set_Text CMD_Set_Text(m_dwPK_Device,m_iPK_Orbiter,"",
					"Learned ok",m_iPK_Text);
				SendCommand(CMD_Set_Text);
			}
		}			
		else
		{
			g_pPlutoLogger->Write(LV_STATUS, "...*** LEARN ABORTED ***\n");
		}
	}
	m_LrnAbort = 1;
	pthread_exit(0);
}

void USB_UIRT_0038::StartLearning(int PK_Device,int PK_Command,int PK_Orbiter,int PK_Text)
{
	if( !PK_Device || !PK_Command || !m_dwPK_Device_IRPlugin )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot learn without a device, command & IR Plugin %d %d %d",
			PK_Device, PK_Command, m_dwPK_Device_IRPlugin);
		return;
	}
	m_iPK_Device_Learning=PK_Device; m_iPK_Command_Learning=PK_Command;
	m_iPK_Orbiter=PK_Orbiter; m_iPK_Text=PK_Text;

	g_pPlutoLogger->Write(LV_STATUS,"Start learning Command %d Device %d",
		PK_Command,PK_Device);

//	PLUTO_SAFETY_LOCK(tm,m_UIRTMutex);
	pthread_t LearnThreadHandle;
	m_LrnAbort = 0;
	if (pthread_create(&LearnThreadHandle, NULL, (void*(*)(void*))LearnThread, this) != 0)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"\n\t*** ERROR creating Learn Thread! ***\n");
	}
}

void USB_UIRT_0038::StopLearning()
{
//	PLUTO_SAFETY_LOCK(tm,m_UIRTMutex);
	m_LrnAbort = 1;

}


/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-c687-b->

	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

void USB_UIRT_0038::CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c687-e->
{
	m_cCurrentScreen=(char) iValue;
	g_pPlutoLogger->Write(LV_STATUS,"Screen type now %c",m_cCurrentScreen);
}


//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void USB_UIRT_0038::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
	SendIR("0", sText);
}

//<-dceag-c245-b->

	/** @brief COMMAND: #245 - Learn IR */
	/** The next IR code received is to be learned in Pronto format and fire a Store IR Code command to the I/R Plugin when done */
		/** @param #2 PK_Device */
			/** You can specify the device to learn for here, or you can send the command to the device itself and leave this blank */
		/** @param #8 On/Off */
			/** Turn IR Learning mode on or off
0, 1 */
		/** @param #25 PK_Text */
			/** If specified, the text object  which should contain the result of the learn command */
		/** @param #154 PK_Command */
			/** Command ID for which the learning is done for */

void USB_UIRT_0038::CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
		if( sOnOff=="1" )
		StartLearning(iPK_Device,iPK_Command,pMessage->m_dwPK_Device_From,iPK_Text);
	else
		StopLearning();
}
