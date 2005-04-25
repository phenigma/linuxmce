//<-dceag-d-b->
#ifndef EIB_h
#define EIB_h

//	DCE Implemenation for #49 EIB

#include "Gen_Devices/EIBBase.h"
//<-dceag-d-e->

#include "messagepool.h"
#include "duplicatefilterpoolinterceptor.h"

//<-dceag-decl-b->!
namespace DCE
{
	
	class EIB : public EIB_Command, public EIBBUS::DuplicateFilterPoolInterceptor
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		EIB(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~EIB();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
        virtual bool Connect(int iPK_DeviceTemplate);
//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		EIB(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

protected:
		/*callback methods from Message Pool*/
		virtual void handleUniqueTelegram(const EIBBUS::TelegramMessage *pt);

private:
		/*process telegram methods*/
		bool processTelegram(const EIBBUS::TelegramMessage *pt, DeviceData_Impl *pDevData);

		/*helper methods*/		
		/*
		static bool getParamsFromChannel(const std::string& sChannel, 
							unsigned int index, std::string& param);
		*/
		static int getParamsFromChannel(const std::string& sChannel, 
							std::vector<std::string>& params);
		static float getFloatFromUShort(unsigned short tempr);
		static unsigned short getUShortFromFloat( float tempr );
private:
	enum DATATYPE {
		DT_UNKNOWN				= 0,
		DT_SWITCH 				= 1, 		/*1 bit*/
		DT_DIMMING_CONTROL 		= 2,		/*4 bit*/
		DT_TIME			 		= 3,		/*3 Bytes*/
		DT_DATE			 		= 4,		/*3 Bytes*/
		DT_VALUE		 		= 5,		/*2 Bytes*/
		DT_SCALING		 		= 6,		/*1 Byte*/
		DT_DRIVE_CONTROL		= 7,		/*1 bit*/
		DT_FLOAT				= 9,		/*4 Bytes*/
		DT_COUNTER16			= 10,		/*2 Bytes*/
		DT_COUNTER32			= 11,		/*4 Bytes*/
		DT_ASCIICHAR			= 13,		/*1 Byte*/
		DT_COUNTER8				= 14,		/*1 Byte*/
		DT_STRING				= 15		/*23 Bytes*/
	};
	
	EIBBUS::MessagePool m_msgPool;

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Port();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #273 - EIB_Write */
	/** Peforms a WRITE request on the bus */
		/** @param #108 Address */
			/** EIB Group Address */
		/** @param #109 Data */
			/** EIB Data */
		/** @param #110 DataType */
			/** EIB Data Type:
[1]-switch (1b)
[2]-dimming-control (4b)
[3]-time (3B)
[4]-date (4B)
[5]-value (2B)
[6]-scaling (1B)
[7]-drive control
[9]-float
[10]-16bit counter
[11]-32bit counter
[13]-ASCII character
[14]-8bit counter
[15]-character string */

	virtual void CMD_EIB_Write(string sAddress,string sData,int iDataType) { string sCMD_Result; CMD_EIB_Write(sAddress.c_str(),sData.c_str(),iDataType,sCMD_Result,NULL);};
	virtual void CMD_EIB_Write(string sAddress,string sData,int iDataType,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #275 - EIB_Read */
	/** Performs a READ request on the bus */
		/** @param #108 Address */
			/** EIB Group Address */

	virtual void CMD_EIB_Read(string sAddress) { string sCMD_Result; CMD_EIB_Read(sAddress.c_str(),sCMD_Result,NULL);};
	virtual void CMD_EIB_Read(string sAddress,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

	
//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
