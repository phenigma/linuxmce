#ifndef WizardLogic_h
#define WizardLogic_h

//-----------------------------------------------------------------------------------------------------
#include "PlutoUtils/MySQLHelper.h"
//-----------------------------------------------------------------------------------------------------

class Row_Device;

class WizardLogic : public MySqlHelper
{
private:
	friend class ScreenHandler;
	friend class OSDScreenHandler;
	long m_nPK_Device_TVProvider_External,m_nPK_Device_TV,m_nPK_Device_Receiver,
		m_nPK_Command_Input_Video_On_TV;
	bool m_bUsingReceiverForVideo;

	long m_nPK_Device_Last_AV, m_nPK_Device_AV_Installed;

	long m_nPK_Device_AlarmPanel,m_nPK_Device_ZWave;  // Monster specific
	long m_nAlarmDeviceTimeout;
	bool m_bAlarmPanelDetectionStarted;
	bool m_bAlarmPanelCommandReceived;
	bool m_bAlarmPanelIsOk;

	string GetDeviceStatus(long nPK_Device);
	deque< pair<int,string> > m_dequeNumLights;

	//AV Wizard
	string m_ManufacturerName, m_AVTemplateName;
	string m_AVTemplateNumericEntry;

	bool m_bAVTogglePower,m_bAVToggleDSP,m_bAVToggleInput;

	long m_nPKDeviceCategory,m_nPKAVTemplate, m_nPKManufacuter;
	long m_nPKIRGroupPower,m_nPKIRGroup;
	long m_nPKDevice,m_nPKParentDevice;
	long m_nPKMediaType,m_nPKConnectorType;
protected:
	class Orbiter *m_pOrbiter;
	class UserUtils *m_pUserUtils;
public:
	list< pair<int,string> > m_listRoomTypes;  // In the order to display them
	map<int,string> m_mapRoomTypes; // For fast lookup

	//AV Wizard
	map<int,string> m_mapAVInputs;
	list<string> m_listDSPModes;
	string m_AVInputsId[3];
	string m_firstId,m_secondId; 
	string m_firstPos,m_secondPos;
	string m_SelectId;

	map<int,string> m_mapIRCommands;
	set<int> m_listIRGroups;

	WizardLogic(Orbiter *pOrbiter);
	virtual ~WizardLogic();

	bool Setup();

	/*
		SETUP USERS
	*/
	bool AlreadyHasUsers();
	int AddUser(string sUsername);
	void RemoveUser(int PK_User);

	/*
		SETUP LOCATION
	*/
	int GetCountry();
	void SetCountry(int PK_Country);
	string GetCityRegion();
	bool SetPostalCode(string PostalCode);
	int GetPostalCode();


	/*
		SETUP ROOMS
	*/
	// Get a map of RoomTypes to number of Rooms of that Type, and the total number of rooms
  string GetRoomTypeName(int PK_RoomType);
	int PreSeedRoomInfo( map<int, int > &mapRooms ); // Call this to pre-populate the room info
	void ProcessUpdatedRoomInfo( map<int, int > &mapRooms ); // Call this to process the updated info
	void RemoveRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired ); // Helper function
	void AddRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired); // Helper function
	void AddRoomOfType(int PK_RoomType); //Helper function
	void RemoveRoomOfType(int PK_RoomType); //Helper function
	void ChangeRoomName(int PK_Room,string sName);
	int WhatRoomIsThisDeviceIn(int PK_Device);

	/*
		SETUP A/V EQUIPMENT
	*/
	void SetManufacturer(string name){ m_ManufacturerName = name; }
	void SetAVTemplateName(string name){ m_AVTemplateName = name; }
	string GetAVTemplateName(){ return m_AVTemplateName; }
	string GetManufactureName() { return m_ManufacturerName; }

	void SetAVTemplateID(long id){ m_nPKAVTemplate = id; }
	long GetAVTemplateId(){ return m_nPKAVTemplate; }
	void SetManufacurerID(long id){ m_nPKManufacuter = id; }
	long GetManufacturerId(){ return m_nPKManufacuter; }
	void SetDeviceId(long id){ m_nPKDevice = id; }
	void SetDeviceCategory(long id){ m_nPKDeviceCategory = id; }
	long GetDeviceCategory(){ return m_nPKDeviceCategory; }
	void SetIRGroupPower(long id){m_nPKIRGroupPower = id; }
	long GetIRGroupPower(){ return m_nPKIRGroupPower; }

	void SetAVTemplateTogglePower(bool state){ m_bAVTogglePower = state;}
	bool GetAVTemplateTogglePower(){ return m_bAVTogglePower; }
	void SetAVToggleDSP(bool state){ m_bAVToggleDSP = state; }
	bool GetAVToggleDSP(){ return m_bAVToggleDSP; }
	void SetAVToggleInput(bool state){ m_bAVToggleInput = state; }
	bool GetAVToggleInput(){ return m_bAVToggleInput; }

	void SetAVTemplateNumericEntry(string value){ m_AVTemplateNumericEntry = value; }
	void SetPKAVTemplate(long PKTemplate){ m_nPKAVTemplate = PKTemplate; }
	long GetPKAVTemplate(){ return m_nPKAVTemplate; }

	void SetPKConnectorType(long id){ m_nPKConnectorType = id; }
	void SetPKMediaType(long id){ m_nPKMediaType = id;}

	// return value 0 unknown,1 toggle,2 discret
	int GetAVIRCodesType();
	int GetAVInputType();
	int GetParentDevice();
	bool IsIRCodes(string commands="");

	int AddAVDeviceTemplate();
	void UpdateAVTemplateDelays(string IR_PowerDelay,string IR_ModeDelay,string DigitDelay);
	void UpdateAVTemplateSettings();
	void UpdateAVTemplateToggle();
	//AV Wizard Inputs
	void AddAVMediaType();
	void AddAVDeviceInput();
	void InsertDSPModes();
	void ChangeDSPOrder();
	void ChangeInputOrder();
	void CreateIRGroup();

	int AddDevice(int PK_DeviceTemplate, string sDeviceDataList = "", long PK_Device_ControlledVia = 0);
	void SetAvPath(int PK_Device_From,int PK_Device_To,int PK_Pipe,int PK_Command_Input);
	void AddExternalTuner(int PK_Device_Tuner);

	/*
		SETUP LIGHTING
	*/
	int GetNumLights(int &iNumLightsUnassigned);
	void LookForZWave();

	/*
		UTILITIES
	*/

	string Installation_get();
	void DeleteDevicesInThisRoomOfType(int PK_DeviceCategory);
	void AddExternalTuner(long something) {}
	void DeleteDevicesInThisRoomOfType(long something) {}

	bool ExistManufacturer(string name);
	/*
		ALARM PANEL
	*/

	string GetDeviceName(string sPK_Device);
	void SetDeviceName(string sPK_Device, string sName);

	long GetDeviceTemplateForDevice(string sPK_Device);
	void ChangeDeviceTemplateForDevice(string sPK_Device, string sFK_DeviceTemplate);

	long GetRoomForDevice(string sPK_Device);
	void SetRoomForDevice(string sPK_Device, string sFK_Room);

	int GetTopMostDevice(int PK_Device);
};
//-----------------------------------------------------------------------------------------------------
#endif
// WizardLogic_h
