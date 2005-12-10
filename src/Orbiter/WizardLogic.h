#ifndef WizardLogic_h
#define WizardLogic_h

//-----------------------------------------------------------------------------------------------------
#include "PlutoUtils/MySQLHelper.h"
//-----------------------------------------------------------------------------------------------------

class WizardLogic : public MySqlHelper
{
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
	string GetCityRegion(int PK_Country);
	bool SetPostalCode(string PostalCode);


	/*
		SETUP ROOMS
	*/
	// Get a map of RoomTypes to number of Rooms of that Type, and the total number of rooms
	int PreSeedRoomInfo( map<int, int > &mapRooms ); // Call this to pre-populate the room info
	void ProcessUpdatedRoomInfo( map<int, int > &mapRooms ); // Call this to process the updated info
	void RemoveRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired ); // Helper function
	void AddRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired); // Helper function
	void ChangeRoomName(int PK_Room,string sName) {};

	/*
		SETUP A/V EQUIPMENT
	*/
	int AddDevice(int PK_DeviceTemplate) { return 0; }
	void SetAvPath(int PK_Device_From,int PK_Device_To,int PK_Pipe,int PK_Command_Input) {};

	/*
		UTILITIES
	*/

	string Installation_get() { return StringUtils::itos(m_pOrbiter->m_pData->m_dwPK_Installation); }


};
//-----------------------------------------------------------------------------------------------------
#endif
