//<-dceag-d-b->
#include "Text_To_Speech.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Text_To_Speech::Text_To_Speech(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Text_To_Speech_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Text_To_Speech::~Text_To_Speech()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Text_To_Speech::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
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
void Text_To_Speech::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void Text_To_Speech::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void Text_To_Speech::SomeFunction()
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
/*
char *Text_To_Speech::CreateWAV(string sText,int &Size)
{
	// do it
	FILE *fp;
	string cmd;

	char *pBuffer, *pData;
	unsigned int count, size;

	pBuffer = pData = NULL;
#ifdef WIN32
	cmd = "iisc /ttw \"" + sText + "\" sample.wav";
	system(cmd.c_str());
#else
	cmd = "flite -t \"" + sText + "\" sample.wav";
	system(cmd.c_str());
#endif

	count = 0;
	pData = (char *)malloc(1);
	fp = fopen("sample.wav", "rb");
	fseek( fp, 0L, SEEK_SET );
	if(fp != NULL) {
		while(feof(fp)==0) {
			char c;
			fscanf( fp, "%c", &c );
			count++;
		}
		fclose(fp);
	}
	size = count;
	pBuffer = new char[size];
	size--;
//Oricare Os este pana aici avem fisierul WAV;
	fp = fopen("sample.wav", "rb");
	fseek( fp, 0L, SEEK_SET );
	count = 0;
	if(fp != NULL) {
		while(feof(fp)==0) {
			fscanf(fp, "%c", &pBuffer[count]);
			count++;
		}
		fclose(fp);
	} 
	else {
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to open WAV file");
	}
//Am citit buferul shi il returnam;
	Size = size;

#ifdef WIN32
	cmd = "del sample.wav";
	system(cmd.c_str());
#else
	cmd = "rm sample.wav";
	system(cmd.c_str());
#endif

	return pBuffer;
}

*/

//<-dceag-c253-b->

	/** @brief COMMAND: #253 - Send Audio To Device */
	/** Will convert the text to an audio file, and send it to the device with the "Play Media" Command. */
		/** @param #9 Text */
			/** What to say */
		/** @param #103 PK_Device_List */
			/** A comma delimited list of the devices to send it to */

void Text_To_Speech::CMD_Send_Audio_To_Device(string sText,string sPK_Device_List,string &sCMD_Result,Message *pMessage)
//<-dceag-c253-e->
{
	int Size;
	char *pBuffer = NULL;//CreateWAV(sText,Size);
	if( !pBuffer )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to create WAV");
	}
	else
	{
		DCE::CMD_Play_Sound_DL CMD_Play_Sound_DL(m_dwPK_Device,sPK_Device_List,pBuffer,Size,"WAV");
		SendCommand(CMD_Play_Sound_DL);
	}
	delete [] pBuffer;
}

//<-dceag-c256-b->

	/** @brief COMMAND: #256 - Text To Wave */
	/** This will take a text and convert it into wave file */
		/** @param #9 Text */
			/** The text to say */
		/** @param #19 Data */
			/** This is the wave file */

void Text_To_Speech::CMD_Text_To_Wave(string sText,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c256-e->
{
	int Size;

//	Size = Find_Exact_Size(sText);
	*pData = new char[Size];
	*iData_Size=Size;
	cout << "the size is " << Size << endl;
	*pData = NULL; //CreateWAV(sText,Size);

}
/*
unsigned int Text_To_Speech::Find_Exact_Size(string sText)
{
	unsigned int count;
	FILE *fp;
	char c;
	string cmd;

#ifdef WIN32
	cmd = "iisc /ttw \"" + sText + "\" sample.wav";
	system(cmd.c_str());
#else
	cmd = "flite -t \"" + sText + "\" sample.wav";
	system(cmd.c_str());
#endif

	count = 0;
	fp = fopen("sample.wav", "rb");
	fseek( fp, 0L, SEEK_SET );
	if(fp != NULL) {
		while(feof(fp)==0) {
			char c;
			fscanf( fp, "%c", &c );
			count++;
		}
		fclose(fp);
	}
	count--;

#ifdef WIN32
	cmd = "del sample.wav";
	system(cmd.c_str());
#else
	cmd = "rm sample.wav";
	system(cmd.c_str());
#endif
	return count;
}
*/

//<-dceag-createinst-b->!
