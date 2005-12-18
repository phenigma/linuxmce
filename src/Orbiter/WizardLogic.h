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

	long m_nPK_Device_AlarmPanel,m_nPK_Device_ZWave;  // Monster specific
	long m_nAlarmDeviceTimeout;
	bool m_bAlarmPanelDetectionStarted;

	string GetDeviceStatus(long nPK_Device);

protected:
	class Orbiter *m_pOrbiter;
	class UserUtils *m_pUserUtils;
public: 
	list< pair<int,string> > m_listRoomTypes;  // In the order to display them
	map<int,string> m_mapRoomTypes; // For fast lookup

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


	/*
		SETUP ROOMS
	*/
	// Get a map of RoomTypes to number of Rooms of that Type, and the total number of rooms
	int PreSeedRoomInfo( map<int, int > &mapRooms ); // Call this to pre-populate the room info
	void ProcessUpdatedRoomInfo( map<int, int > &mapRooms ); // Call this to process the updated info
	void RemoveRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired ); // Helper function
	void AddRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired); // Helper function
	void AddRoomOfType(int PK_RoomType); //Helper function
	void RemoveRoomOfType(int PK_RoomType); //Helper function
	void ChangeRoomName(int PK_Room,string sName);

	/*
		SETUP A/V EQUIPMENT
	*/
	int AddDevice(int PK_DeviceTemplate);
	void SetAvPath(int PK_Device_From,int PK_Device_To,int PK_Pipe,int PK_Command_Input);
	void AddExternalTuner(int PK_Device_Tuner);

	/*
		UTILITIES
	*/

	string Installation_get() { return StringUtils::itos(m_pOrbiter->m_pData->m_dwPK_Installation); }
	void DeleteDevicesInThisRoomOfType(int PK_DeviceCategory);
	void AddExternalTuner(long something) {}
	void DeleteDevicesInThisRoomOfType(long something) {}
};
//-----------------------------------------------------------------------------------------------------
#endif
