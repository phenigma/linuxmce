//<-dceag-d-b->
#ifndef Text_To_Speech_h
#define Text_To_Speech_h

//	DCE Implemenation for #57 Text To Speech

#include "Gen_Devices/Text_To_SpeechBase.h"
//<-dceag-d-e->
#define FILE_PATH "/home/public/data/tts"

//<-dceag-decl-b->
namespace DCE
{
	class Text_To_Speech : public Text_To_Speech_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Text_To_Speech(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Text_To_Speech();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		pluto_pthread_mutex_t m_TTSMutex; //this will protect needredraw vectors
		int m_dwPK_Device_MediaPlugin;
		int m_dwID;  // A numeric ID for the request
		map<time_t,string> m_mapOutstandingFiles; // So we can delete files more than 5 minutes old

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Name();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #253 - Send Audio To Device */
	/** Will convert the text to an audio file, and send it to the device with the "Play Media" Command. */
		/** @param #9 Text */
			/** What to say */
		/** @param #103 PK_Device_List */
			/** A comma delimited list of the devices to send it to */

	virtual void CMD_Send_Audio_To_Device(string sText,string sPK_Device_List) { string sCMD_Result; CMD_Send_Audio_To_Device(sText.c_str(),sPK_Device_List.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Audio_To_Device(string sText,string sPK_Device_List,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #256 - Text To Wave */
	/** This will take a text and convert it into wave file */
		/** @param #9 Text */
			/** The text to say */
		/** @param #19 Data */
			/** This is the wave file */

	virtual void CMD_Text_To_Wave(string sText,char **pData,int *iData_Size) { string sCMD_Result; CMD_Text_To_Wave(sText.c_str(),pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Text_To_Wave(string sText,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
