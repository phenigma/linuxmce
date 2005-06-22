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
	, m_TTSMutex( "tts" )
{
	// Be sure there's no temporary files in the directory
	FileUtils::DelDir(FILE_PATH);
	FileUtils::MakeDir(FILE_PATH);
	m_TTSMutex.Init(NULL);

	m_dwID=0;

	m_dwPK_Device_MediaPlugin=0;
	for(Map_DeviceData_Base::iterator it=m_pData->m_AllDevices.m_mapDeviceData_Base.begin();it!=m_pData->m_AllDevices.m_mapDeviceData_Base.end();++it)
	{
		DeviceData_Base *pDeviceData_Base = it->second;
		if( pDeviceData_Base->m_dwPK_DeviceCategory==DEVICECATEGORY_Media_Plugins_CONST )
		{
			m_dwPK_Device_MediaPlugin=pDeviceData_Base->m_dwPK_Device;
			break;
		}
	}

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

//<-dceag-sample-b->!

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
	PLUTO_SAFETY_LOCK(tm,m_TTSMutex);

	string sFile = "/home/public/data/tts/" + StringUtils::itos(m_dwID++) + ".wav";
	string sCmd = "flite -t \"" + sText + ".\" -o " + sFile;
	system(sCmd.c_str());
	if( FileUtils::FileExists(sFile) )
	{
		string::size_type pos=0;
		while( pos<sPK_Device_List.size() && pos!=string::npos )
		{
			int PK_Device = atoi(StringUtils::Tokenize(sPK_Device_List,",",pos).c_str());
			if( PK_Device )
			{
				DCE::CMD_MH_Play_Media CMD_MH_Play_Media(m_dwPK_Device,m_dwPK_Device_MediaPlugin,PK_Device,"",sFile,0,0,"",true,false);
				SendCommand(CMD_MH_Play_Media);
			}
		}
		m_mapOutstandingFiles[time(NULL)]=sFile;
	}

	g_pPlutoLogger->Write(LV_STATUS,"Processed file: %s outstanding: %d",sCmd.c_str(),(int) m_mapOutstandingFiles.size());
	for(map<time_t,string>::iterator it=m_mapOutstandingFiles.begin();it!=m_mapOutstandingFiles.end();)
	{
		if( it->first + 300 < time(NULL) )
		{
			FileUtils::DelFile(it->second);
			m_mapOutstandingFiles.erase(it++);
		}
		else
			it++;
	}
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

//<-dceag-createinst-b->!
