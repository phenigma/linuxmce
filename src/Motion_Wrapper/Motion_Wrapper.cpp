//<-dceag-d-b->
#include "Motion_Wrapper.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

#define MOTION_CONF_PATH	"/etc/motion/"
#define MOTION_CONF_FILE	"motion.conf"

#define SNAPSHOT_SLEEP_TIME	100

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Motion_Wrapper::Motion_Wrapper(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Motion_Wrapper_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
/*	int i;
	FILE *fp;
	string sLine, sLine2;
	size_t size;
	string sPortNumber, sPath;
	string FilePath, sData, sRep;
	unsigned long int isize, iPortNumber, pid;
	
    
	int iVideoStandard = DATA_Get_Video_Standard();
	int iNumberOfPorts = DATA_Get_Number_of_ports();
	int iVideoInputType = DATA_Get_Video_Input_Type();

	g_pPlutoLogger->Write(LV_STATUS, "Using Generic Analog Capture Card with parameters: VideoStandard=%d Number_of_Ports=%d Video_Input_Type=%d",
				iVideoStandard,iNumberOfPorts,iVideoInputType);
	
	if(!FileUtils::FileExists(MOTION_CONF_FILE) == true) {
		g_pPlutoLogger->Write(LV_STATUS, "Motion package not installed. Please install...");
		exit(1);
	}
	
	fp = fopen("/etc/motion/motion.conf","wt+");
	//main config
//	fprintf(fp,"videodevice /dev/video0\n");
//	fprintf(fp,"input %d\n",iVideoInputType);	//0 for video/tv and 8 for usb cammera
	fprintf(fp,"norm %d\n",iVideoStandard);	//pal/secam
	fprintf(fp,"frequency 0\n");
	fprintf(fp,"rotate 0\n");
	fprintf(fp,"width 320\n");
	fprintf(fp,"height 240\n");
	fprintf(fp,"framerate 2\n");
	fprintf(fp,"auto_brightness off\n");
	fprintf(fp,"brightness 0\n");
	fprintf(fp,"contrast 0\n");
	fprintf(fp,"saturation 0\n");
	fprintf(fp,"hue 0\n");
// filters
	fprintf(fp,"\ndespeckle EedDl \n");
	fprintf(fp,"lightswitch 50 \n");
	fprintf(fp,"minimum_motion_frames 5 \n");
	fprintf(fp,"night_compensate on \n");
	fprintf(fp,"smart_mask_speed 5\n");
// output
	fprintf(fp,"\nalways_changes on\n");
	fprintf(fp,"locate on\n");
	fprintf(fp,"post_capture 5 \n");
	fprintf(fp,"webcam_quality 50 \n");
	fprintf(fp,"webcam_maxrate 5\n");
	fprintf(fp,"text_changes on\n");
	sLine = "text_right %d-%m-%Y\\n%T";
	fprintf(fp,"%s\n",sLine.c_str());
//	fprintf(fp,"mail_address ...\n");


// movies

	fprintf(fp,"\nffmpeg_cap_new on\n");
	fprintf(fp,"ffmpeg_timelapse 60\n");
	fprintf(fp,"ffmpeg_timelapse_mode daily\n");
	fprintf(fp,"ffmpeg_video_codec mpeg1\n");




//snapshot config
	fprintf(fp,"\nsnapshot_interval 60\n");
	sLine = "snapshot_filename %Y/%m/%d/%H/%M_%S";
	fprintf(fp,"%s\n",sLine.c_str());
	sLine = "jpeg_filename %Y/%m/%d/%H/%M_%S";
	fprintf(fp,"%s\n",sLine.c_str());
	sLine = "ffmpeg_filename %Y/%m/%d/%H/%M_%S";
	fprintf(fp,"%s\n",sLine.c_str());
	sLine = "timelapse_filename %Y/%m/%d-timelapse";
	fprintf(fp,"%s\n",sLine.c_str());

	fprintf(fp,"\n\n");
	//Output Directory
	g_pPlutoLogger->Write(LV_STATUS, "Wrote default config");

	for(i = 0 ; i < iNumberOfPorts+1 ; i++) {
		fprintf(fp,"#thread /etc/motion/thread%d.conf\n",i);
	}
	fprintf(fp,"\n\n");

	fclose(fp);
//---------------------------------
	for(size_t i=0;i<m_pData->m_vectDeviceData_Impl_Children.size();++i) {
                DeviceData_Impl *pDeviceData_Impl = m_pData->m_vectDeviceData_Impl_Children[i];
                int PK_Device = pDeviceData_Impl->m_dwPK_Device;
                string sPort = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
                string sMotion = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_Motion_Option_CONST);
       	  string sSensitivity = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_Sensitivity_CONST);
		  string sDevice =  pDeviceData_Impl->mapParameters_Find(DEVICEDATA_Device_CONST);
		  string sDescription =  pDeviceData_Impl->m_sDescription ;
                g_pPlutoLogger->Write(LV_STATUS, "Using child device %d from Generic Analog Capture Card ",i);
		  
		iPortNumber = i;
		sPortNumber = StringUtils::itos(iPortNumber);
		
		sPath = "/etc/motion/thread" + StringUtils::itos(iPortNumber) + ".conf";
		FilePath = sPath;
		g_pPlutoLogger->Write(LV_STATUS, "Looking for camera config file");
		if(FileUtils::FileExists(sPath) == true) {
			g_pPlutoLogger->Write(LV_STATUS, "File found, modifing...");
			sPath = "rm -f " + sPath;
 			system(sPath.c_str());
		} else {
			g_pPlutoLogger->Write(LV_STATUS, "Camera config file not found, writing new one");
		}
		fp = fopen(FilePath.c_str(),"wt+");
		//main config
		sLine = "videodevice /dev/video" + sDevice+ "\n";  //ADDED device parameter ...
		fprintf(fp,sLine.c_str());
		sLine = "input " + sPort + "\n";
		fprintf(fp,sLine.c_str());

		sLine = "800" + StringUtils::itos(iPortNumber);
		fprintf(fp,"webcam_port %s\n",sLine.c_str());
		sData = " ";
		sRep = "_";
 		sLine = StringUtils::Replace(sDescription,sData,sRep);
		fprintf(fp,"text_left %s\\nRoomName\n\n\n",sLine.c_str()); // Real Room Name could be added !!!!!!

		//Output Directory
		sLine = "/home/cameras/" + StringUtils::itos(PK_Device);
		g_pPlutoLogger->Write(LV_STATUS, "Looking for camera output directory");
		if(FileUtils::DirExists(sLine) == false) {
			g_pPlutoLogger->Write(LV_STATUS, "Camera output directory not found, creating it");
			FileUtils::MakeDir(sLine);
		} else {
			g_pPlutoLogger->Write(LV_STATUS, "Camera output found");
		}
		fprintf(fp,"target_dir %s\n",sLine.c_str());

		//snapshot config
		fprintf(fp,"\nsnapshot_interval 60\n");
		sLine = "snapshot_filename %Y/%m/%d/%H/%M_%S";
		fprintf(fp,"%s\n",sLine.c_str());
		sLine = "jpeg_filename %Y/%m/%d/%H/%M_%S";
		fprintf(fp,"%s\n",sLine.c_str());
		sLine = "ffmpeg_filename %Y/%m/%d/%H/%M_%S";
		fprintf(fp,"%s\n",sLine.c_str());
		sLine = "timelapse_filename %Y/%m/%d-timelapse";
		fprintf(fp,"%s\n",sLine.c_str());

		fclose(fp);

		g_pPlutoLogger->Write(LV_STATUS, "Reading Main configuration file of the motion server");
		sPath = "/etc/motion/motion.conf";
		isize = FileUtils::FileSize(sPath);
		char *pData = new char(isize+1);
		pData[isize] = '\0';
		pData = FileUtils::ReadFileIntoBuffer(sPath,(size_t &)isize);
		sLine = "#thread /etc/motion/thread" + sPortNumber + ".conf";
		sRep = "thread /etc/motion/thread" + sPortNumber + ".conf";
	
		std::string std = pData;
		StringUtils::Replace(&std,sLine,sRep);

		g_pPlutoLogger->Write(LV_STATUS, "Adding child camera %d to main config file",i);
		strcpy(pData,std.c_str());
		FileUtils::WriteBufferIntoFile(sPath,pData,isize-1);
       }
//---------------------------------	
	system("rm -f motion.temp");
	g_pPlutoLogger->Write(LV_STATUS, "Starting motion server");
	system("/usr/bin/motion | grep Failed > motion.temp");
	const char *ptr = FileUtils::ReadFileIntoBuffer("motion.temp",size);
	if(strstr(ptr,"Failed") != NULL ) {
		g_pPlutoLogger->Write(LV_STATUS, "Motion Server failed to start, exiting");
		exit(1);
	}
*/	
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Motion_Wrapper::Motion_Wrapper(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Motion_Wrapper_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Motion_Wrapper::~Motion_Wrapper()
//<-dceag-dest-e->
{
	if(motionpid_) {
		g_pPlutoLogger->Write(LV_STATUS, "Killing motion process: %d...", motionpid_);
		kill(motionpid_, SIGKILL);
		g_pPlutoLogger->Write(LV_STATUS, "Waiting for motion process to finish: %d...", motionpid_);
		wait(NULL);
		g_pPlutoLogger->Write(LV_STATUS, "Done.");
	}
}

bool Motion_Wrapper::Connect(int iPK_DeviceTemplate) {
	g_pPlutoLogger->Write(LV_STATUS, "Generating Motion configuration...");
	
	string sPath = MOTION_CONF_PATH MOTION_CONF_FILE;
	if(!FileUtils::FileExists(sPath.c_str()) == true) {
		g_pPlutoLogger->Write(LV_STATUS, "Motion package not installed. Please install...");
		return false;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Writing configuration to: %s", MOTION_CONF_FILE);
	
	std::ofstream mconffile;
	mconffile.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );	
	try {	
		mconffile.open(sPath.c_str(), fstream::out | fstream::trunc);
		string sVideoStandard = m_pData->mapParameters_Find(DEVICEDATA_Video_Standard_CONST);
		
		mconffile 	<< "norm " << sVideoStandard << endl 
					<< "frequency 0" <<  endl
					<< "rotate 0" <<  endl
					<< "width 320" <<  endl
					<< "height 240" <<  endl
					<< "framerate 2" <<  endl
					<< "auto_brightness on" <<  endl
					<< "brightness 0" <<  endl
					<< "contrast 0" <<  endl
					<< "saturation 0" <<  endl
					<< "hue 0" <<  endl
					/* filters*/ << endl
					<< "despeckle EedDl" << endl
					<< "lightswitch 50" << endl
					<< "minimum_motion_frames 5" << endl
					<< "night_compensate on" << endl
					<< "noise_tune on" << endl
					<< "smart_mask_speed 5" << endl
					/* output */ << endl
					<< "always_changes on" << endl
					<< "locate on" << endl
					<< "post_capture 5" << endl
					<< "webcam_quality 50" << endl
					<< "webcam_maxrate 5" << endl
					<< "text_changes on" << endl
					<< "text_right %d-%m-%Y\\n%T" << endl
//					/* movies */ << endl
					<< "ffmpeg_cap_new on" << endl
					<< "ffmpeg_timelapse 60" << endl
					<< "ffmpeg_timelapse_mode daily" << endl
					<< "ffmpeg_video_codec msmpeg4" << endl
					/* snapshot config */ << endl
					<< "snapshot_interval 60" << endl
					<< "snapshot_filename %Y/%m/%d/%H/%M_%S" << endl
					<< "jpeg_filename %Y/%m/%d/%H/%M_%S" << endl
					<< "ffmpeg_filename movies/%d_%m_%Y_%H_%M_%S" << endl
					<< "timelapse_filename %Y/%m/%d-timelapse" << endl
					<< endl << endl;
	} catch(ifstream::failure e) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot open %s for writing...", sPath.c_str());
		return false;
	}
				
	bool bFirstAdded = true;
	for(size_t i = 0; i < m_pData->m_vectDeviceData_Impl_Children.size(); ++i) {
        DeviceData_Impl *pDeviceData_Impl = m_pData->m_vectDeviceData_Impl_Children[i];
		if(!bFirstAdded) {
			if(AddChildDeviceToConfigFile(mconffile, pDeviceData_Impl, i)) {
				bFirstAdded = true;
			}
			mconffile << endl << endl;
		} else {
			char cthread = '0' + i;
			sPath = string(MOTION_CONF_PATH "thread") + string(1, cthread) + ".conf";
			ofstream tconffile;
			tconffile.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );	
			try {
				tconffile.open(sPath.c_str(), fstream::out | fstream::trunc);
				if(AddChildDeviceToConfigFile(tconffile, pDeviceData_Impl, i)) {
					mconffile << "thread " << sPath << endl;
				}
			} catch(ifstream::failure e) {
				g_pPlutoLogger->Write(LV_CRITICAL, "Cannot open %s for writing...", sPath.c_str());
				continue;
			}
		}
	}

	g_pPlutoLogger->Write(LV_STATUS, "Forking to launch motion process...");
	motionpid_ = fork();
	if(motionpid_) {
		g_pPlutoLogger->Write(LV_STATUS, "In parent process.");
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "In child process.");
		g_pPlutoLogger->Write(LV_STATUS, "Launching motion...");
		if(execl("/usr/bin/motion", NULL, NULL)) {
			g_pPlutoLogger->Write(LV_CRITICAL, "Could not launch motion motion!");
			exit(1);
		}
	}
							
	return Motion_Wrapper_Command::Connect(iPK_DeviceTemplate);
}


//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Motion_Wrapper::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Motion_Wrapper_Command *Create_Motion_Wrapper(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Motion_Wrapper(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Motion_Wrapper::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "OK";
	
	g_pPlutoLogger->Write(LV_STATUS, "Command %d received for child.", pMessage->m_dwID);
	
	
	// find child device
	DeviceData_Impl* pDeviceData_Impl = NULL;
	
	g_pPlutoLogger->Write(LV_STATUS, "Finding child device...");

	VectDeviceData_Impl& vDeviceData = m_pData->m_vectDeviceData_Impl_Children;
	for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++) {
		if(vDeviceData[i]->m_dwPK_Device == pMessage->m_dwPK_Device_To) {
			pDeviceData_Impl = vDeviceData[i];
			break;
		}
	}
	
	if(!pDeviceData_Impl) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Child device %d not found.", pMessage->m_dwPK_Device_To);
		return;
	}
	
	g_pPlutoLogger->Write(LV_STATUS, "Child device %d found.", pMessage->m_dwPK_Device_To);

	switch(pMessage->m_dwID) {
		case COMMAND_Get_Video_Frame_CONST: {
				string sPortNumber = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
				
				g_pPlutoLogger->Write(LV_STATUS, "Taking Snapshot...");
				if(kill(motionpid_, SIGALRM)) {
					g_pPlutoLogger->Write(LV_WARNING, "Could not take snapshot, fail sending SIGALRM signal...");
				} else {
					usleep(SNAPSHOT_SLEEP_TIME * 1000);
					g_pPlutoLogger->Write(LV_STATUS, "Checking file existance...");
					string FilePath = "/home/cameras/" + StringUtils::itos(pMessage->m_dwPK_Device_To) + "/lastsnap.jpg";
					if(FileUtils::FileExists(FilePath)) {
						g_pPlutoLogger->Write(LV_STATUS, "File exists. Reading and sending snapshot picture...");
						size_t nDataLength;
						char *pFileData = FileUtils::ReadFileIntoBuffer(FilePath, nDataLength);
						
						g_pPlutoLogger->Write(LV_STATUS, "Sending Reply message to sender.");
						
                            pMessage->m_bRespondedToMessage=true;

							Message *pMessageOut=new Message(pMessage->m_dwPK_Device_To, pMessage->m_dwPK_Device_From,
																	PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 0);
							pMessageOut->m_mapData_Parameters[19]=pFileData; 
							pMessageOut->m_mapData_Lengths[19]=nDataLength;
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						
						g_pPlutoLogger->Write(LV_STATUS, "Sent %d bytes to command sender.", nDataLength);
					} else {
						g_pPlutoLogger->Write(LV_WARNING, "Missing snapshot file: %s.", FilePath.c_str());
					}
				}
			}break;
		default:
			g_pPlutoLogger->Write(LV_CRITICAL, "Unknown command %d received.", pMessage->m_dwID);
	}
}


bool 
Motion_Wrapper::AddChildDeviceToConfigFile(std::ofstream& conffile, DeviceData_Impl* pDeviceData,size_t i) {
	int PK_Device = pDeviceData->m_dwPK_Device;
	g_pPlutoLogger->Write(LV_STATUS, "Using child device %d from Generic Analog Capture Card ", PK_Device);
	
	string sMotion = pDeviceData->mapParameters_Find(DEVICEDATA_Motion_Option_CONST);
			
		  

	//video device
	string sDevice = pDeviceData->mapParameters_Find(DEVICEDATA_Device_CONST);

	if(!sDevice.empty()) {
		conffile 	<< "videodevice /dev/video" << sDevice << endl;
	} else {
		g_pPlutoLogger->Write(LV_WARNING, "Device number not specified for device: %d.", PK_Device);
		return false;
	}

	//input port
	string sPort = pDeviceData->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
	if(!sPort.empty()) {
		conffile 	<< "input " << sPort << endl;
	} else {
		g_pPlutoLogger->Write(LV_WARNING, "Port not specified for device: %d.", PK_Device);
		return false;
	}
	
	string sNoiseLevel = pDeviceData->mapParameters_Find(DEVICEDATA_Noise_CONST);
	if(!sNoiseLevel.empty()) {
		conffile	<< "noise_level " << sNoiseLevel << endl;
	}

	
	//input port
	string sDescription =  pDeviceData->m_sDescription ;
	if(!sDescription.empty()) {

///////    RoomName should be replaced with name of room, where the camera is ....
		conffile	<< "text_left " << StringUtils::Replace(&sDescription, " ", "_") << "\\nRoomName" << endl << endl << endl;
	}

	//web port (+8000) and child number
	string sPortNumber = StringUtils::itos(i+8000);
	if(!sPortNumber.empty()) {
		conffile	<< "webcam_port " << sPortNumber << endl;
	}

					
	//Check Output Directory
	string sPath = "/home/cameras/" + StringUtils::itos(PK_Device);
	g_pPlutoLogger->Write(LV_STATUS, "Looking for camera output directory: %s...", sPath.c_str());
	if(FileUtils::DirExists(sPath) == false) {
		g_pPlutoLogger->Write(LV_STATUS, "Camera output directory not found, creating it");
		FileUtils::MakeDir(sPath);
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "Camera output found");
	}
		
	conffile 	<< "target_dir " << sPath << endl;
	return true;
}


/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Motion_Wrapper::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void Motion_Wrapper::SomeFunction()
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

/*

	COMMANDS TO IMPLEMENT

*/

DeviceData_Router* Motion_Wrapper::find_Device(int iPK_Device) {
    /*search device by id*/
	return m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
}


/*

this came from generic_analog_camera

	string sPortNumber, sPath, sLine;
	FILE *fp;
	string FilePath, sData, sRep;
	unsigned long int size, iPortNumber, pid;

	iPortNumber = DATA_Get_Port_Number();
	iPortNumber++;
	sPath = "/etc/motion/thread" + StringUtils::itos(iPortNumber) + ".conf";
	FilePath = sPath;
	g_pPlutoLogger->Write(LV_STATUS, "Looking for camera config file");
	if(FileUtils::FileExists(sPath) == true) {
		g_pPlutoLogger->Write(LV_STATUS, "File found, modifing...");
		sPath = "rm -f " + sPath;
 		system(sPath.c_str());
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "Camera config file not found, writing new one");
	}
	fp = fopen(FilePath.c_str(),"wt+");
	//main config
	sLine = "videodevice /dev/video" + StringUtils::itos(iPortNumber) + "\n";
	fprintf(fp,sLine.c_str());
	fprintf(fp,"input 8\n");
	fprintf(fp,"norm 0\n");	//pal/secam
	fprintf(fp,"frequncy 0\n");
	fprintf(fp,"rotate 0\n");
	fprintf(fp,"width 320\n");
	fprintf(fp,"height 240\n");
	fprintf(fp,"framerate 2\n");
	fprintf(fp,"auto_brightness off\n");
	fprintf(fp,"brightness 0\n");
	fprintf(fp,"contrast 0\n");
	fprintf(fp,"saturation 0\n");
	fprintf(fp,"hue 0\n");
	//snapshot config
	fprintf(fp,"snapshot_interval 3600\n");
	//Output Directory
	sLine = "/home/cameras/" + StringUtils::itos(DeviceID);
	g_pPlutoLogger->Write(LV_STATUS, "Looking for camera output directory");
	if(FileUtils::DirExists(sLine) == false) {
		g_pPlutoLogger->Write(LV_STATUS, "Camera output directory not found, creating it");
		FileUtils::MakeDir(sLine);
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "Camera output found");
	}
	fprintf(fp,"target_dir %s\n",sLine.c_str());
	sLine = "snapshot_filename %Y/%m/%d/%H/%M_%S";
	fprintf(fp,"%s\n",sLine.c_str());
	sLine = "jpeg_filename %Y/%m/%d/%H/%M_%S";
	fprintf(fp,"%s\n",sLine.c_str());
	fclose(fp);

	g_pPlutoLogger->Write(LV_STATUS, "Reading Main configuration file of the motion server");
	sPath = "/etc/motion/motion.conf";
	size = FileUtils::FileSize(sPath);
	char *pData = new char(size+1);
	pData[size] = '\0';
	pData = FileUtils::ReadFileIntoBuffer(sPath,(size_t &)size);
	sLine = "#thread /etc/motion/thread" + StringUtils::itos(iPortNumber) + ".conf";
	sRep = "thread /etc/motion/thread" + StringUtils::itos(iPortNumber) + ".conf";
	
	g_pPlutoLogger->Write(LV_STATUS, "Replacing %s",sLine.c_str());
	g_pPlutoLogger->Write(LV_STATUS, "With %s",sRep.c_str());
	std::string std = pData;
	std = StringUtils::Replace(&std,sLine,sRep);

	g_pPlutoLogger->Write(LV_STATUS, "Adding child camera to main config file");
	strcpy(pData,std.c_str());
	FileUtils::WriteBufferIntoFile(sPath,pData,size);

	g_pPlutoLogger->Write(LV_STATUS, "Getting PID of the motion server");
	sLine = "ps -e | grep motion | awk '{print $1}' > camera_card.temp";
	system(sLine.c_str());
	size_t s;
	const char *ptr = FileUtils::ReadFileIntoBuffer("camera_card.temp",s);
	pid = atoi(ptr);
	if(pid == 0) {
		g_pPlutoLogger->Write(LV_STATUS, "Motion Server is not running, exiting");
		system("rm -f camera_card.temp");
		exit(1);
	}
	g_pPlutoLogger->Write(LV_STATUS, "Rehashing motion server with PID: %d",pid);

	sPath = "kill -s SIGHUP " + StringUtils::itos(pid);
	system(sPath.c_str());

*/