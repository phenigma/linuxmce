#ifndef DCEDEVICE_H
#define DCEDEVICE_H

#include "DeviceData_Impl.h"

namespace DCE
{

	// This indicates connectivity, that something is being sent somewhere else, such as audio/video connections
	// For example, the Movie Scaler is connected to the TV, using output ID 5 on the Scaler, and Input ID 3 on the TV
	// There would be 2 pipes, with a destination of the TV.  One an output with ID 5, one an Input with ID 3, both with Kind="Video"
	enum enum_PipeType { Audio=1, Video };

	class Pipe
	{
		class DeviceData_Router *m_pDevice;
		bool m_bIsOutput;
		enum_PipeType m_ePipeType;
	};

	class Command
	{
	public:
		Command(int PK_Command,string sDescription) { m_dwPK_Command=PK_Command; m_sDescription=sDescription; }
		int m_dwPK_Command;
		string m_sDescription;
		list<Pipe *> m_listPipe;
	};

	class CommandParameter
	{
	public:
		string m_sValue;
		int m_PK_CommandParameter;

		CommandParameter(int PK_CommandParameter,string sValue) : m_sValue(sValue), m_PK_CommandParameter(PK_CommandParameter) {}
	};

	class CommandGroup_Command
	{
	public:
		int m_dwPK_Command,m_PK_Device,m_PK_DeviceGroup,m_PK_C_Array;
		list<class Device_Routing *> m_listDevices;
		map<int,class CommandParameter *> m_mapCommandParameter;

		CommandGroup_Command(int PK_Command,int PK_Device,int PK_DeviceGroup,int PK_C_Array)
		{
			m_dwPK_Command = PK_Command;
			m_PK_Device = PK_Device;
			m_PK_DeviceGroup = PK_DeviceGroup;
			m_PK_C_Array = PK_C_Array;
		}

		~CommandGroup_Command();
	};

	class CommandGroup
	{
	public:
		int m_PK_CommandGroup;
		vector<class CommandGroup_Command *> m_vectCommands;
		string m_Description;
		int m_PK_C_Array;

		CommandGroup(int PK_CommandGroup,int PK_C_Array=0) : 
		m_PK_CommandGroup(PK_CommandGroup), m_PK_C_Array(PK_C_Array) {}

		~CommandGroup()
		{
			vector<class CommandGroup_Command *>::iterator iA;

			for (iA = m_vectCommands.begin(); iA != m_vectCommands.end(); ++iA)
			{
				delete (*iA); 
			}
		}
	};

	class DeviceGroup 
	{
	public:
		int m_PK_DeviceGroup;
		string m_sDescription;
		vector<class DeviceData_Router *> m_vectDevices;
	};

	class DCERoom
	{
	public:
		int m_PK_Room;
		string m_Description;
		int m_PK_C_RoomMode;
		bool m_bLightsLastOn,m_bClimateLastOn;
		class CommandGroup *m_pagLightsOff,*m_pagLightsOn,*m_pagClimateOff,*m_pagClimateOn;
		int PK_CommandGroup_LightsOff,PK_CommandGroup_LightsOn,PK_CommandGroup_ClimateOff,PK_CommandGroup_ClimateOn;
		int m_iTemperature;
		vector<class OCController *> m_vectController;
		vector<class CommandGroup *> m_vectCommandGroups;
		list<class DeviceData_Router *> m_listDevices;

		DCERoom()
		{	
			m_pagLightsOff=NULL; m_pagLightsOn=NULL; m_pagClimateOff=NULL; m_pagClimateOn=NULL;
			m_bLightsLastOn=false; m_bClimateLastOn=false; m_iTemperature=0;
		}
	};

	class DeviceRelation 
	{
	public:
		class DeviceData_Router *pDevice;
		string sParms;
	};

	class DeviceData_Router : public DeviceData_Impl
	{
	public:

		// **** SERIALIZED VALUES FROM THE CONFIGURATION FILE ****

		// General ID's for this Device
		// These are in the base-> int m_dwPK_Device,m_dwPK_Installation,m_dwPK_DeviceTemplate,m_dwPK_DeviceCategory,m_dwPK_Room,;
		// string m_sDescription;

		// If the device is running on a different installation, this will point to the IP address
		// of the Router to send the messages to
		string m_sForeignRouter;

		// The command line to execute for this device
		string m_sCommandLine; 

		// If set to true, the first time this device connects we will send it a reload.  This is good for 
		// controllers which try to continually reconnect and may not realize the server has been restarted
		bool m_bForceReloadOnFirstConnect;

		list<class Pipe *> m_listPipe_Available; // The available pipes
		list<class Pipe *> m_listPipe_Active; // The currently activated pipes

		// A virtual device that doesn't really exist, but serves as a placeholder will have SlaveTo set
		// For example, a Television may have several tuners.  Each tuner must be a separate device so the user
		// can tune on any one of them.  They are all marked as SlaveTo the television itself.
		int m_dwPK_Device_SlaveTo;  

		// All the groups, parameters, inputs, etc.
		map<int,class DeviceRelation *> m_mapDeviceRelation;
		map<int,class DeviceGroup *> m_mapDeviceGroup;
		vector<DeviceData_Router *> m_vectDevices_SendingPipes;  // The devices that send this one something
		map<int,int> m_mapInputs,m_mapOutputs,m_mapDSPModes;  // map PK_Command,Input/Output/DSPMode

		// **** FLAGS AND INFORMATION SET AT RUNTIME ****

		bool m_bIsRegistered,m_bIsReady; // Some devices will set this by sending a ready message/event
		bool m_bBusy; // A device can be marked busy.  It's messages will be queued until this flag is cleared
		bool m_bAlert;  // A device with this flag set is having a problem
		string m_sStatus; // A device can set a generic status message, such as "triggered".  There is no predefined purpose
		// When a device received an on or an off, the state is set to 0 or -1.  A plug-in may override this
		// and assign a value of, for example, 1-99 for a percentage of brightness
		int m_iLastState;

		string m_IPAddr;
		time_t m_tCanReceiveNextCommand,m_tLastused;

		char *m_pMySerializedData;  // A copy of the device serialized so we don't have to do it for each request
		int m_iConfigSize;

		// **** POINTERS CREATED BY THE SERIALIZED ID'S ****

		class DCERoom *m_pRoom;
		class DeviceData_Router *m_pDevice_SlaveTo;
		class DeviceData_Router *m_pDevice_Audio,*m_pDevice_Video;

		DeviceData_Router(unsigned long  iPK_Device,unsigned long  iPK_Installation,unsigned long  iPK_DeviceTemplate,unsigned long  iPK_Device_ControlledVia, unsigned long m_dwPK_DeviceCategory, unsigned long iPK_Room,bool bImplementsDCE,bool bIsEmbedded,
			string sCommandLine,bool bIsPlugIn,string sDescription,string sIPAddress,string sMacAddress)
			: DeviceData_Impl(iPK_Device,iPK_Installation,iPK_DeviceTemplate,iPK_Device_ControlledVia,m_dwPK_DeviceCategory,iPK_Room,
			bImplementsDCE,bIsEmbedded,sCommandLine,bIsPlugIn,sDescription,sIPAddress,sMacAddress)
		{
			m_dwPK_Device_SlaveTo=0;
			m_iLastState=0;
			m_bForceReloadOnFirstConnect=m_bIsRegistered=m_bIsReady=m_bBusy=m_bAlert=false;
			m_tLastused=m_tCanReceiveNextCommand=0;

			m_pRoom=NULL;
			m_pDevice_ControlledVia=m_pDevice_SlaveTo=NULL;
			m_pDevice_Audio=m_pDevice_Video=NULL;
			m_pMySerializedData=NULL;
			m_iConfigSize=0;
		}

		~DeviceData_Router()
		{
			map<int,class DeviceRelation *>::iterator itMD;
			for(itMD=m_mapDeviceRelation.begin();itMD!=m_mapDeviceRelation.end();++itMD)
			{
				class DeviceRelation *pR = (*itMD).second;
				delete pR;
			}
		}

		DeviceRelation *mapDeviceRelation_Find(int PK_Device)
		{
			map<int,class DeviceRelation *>::iterator it = m_mapDeviceRelation.find(PK_Device);
			return it==m_mapDeviceRelation.end() ? NULL : (*it).second;
		}

		// We're not going to use this, since we're not creating actual devices.  Implement this pure virtual function from our base class
		class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition) { return NULL; };
	};

	typedef list<DeviceData_Router *> ListDeviceData_Router;
}

#endif

