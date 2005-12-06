#ifndef WizardLogic_h
#define WizardLogic_h

//-----------------------------------------------------------------------------------------------------
#include "PlutoUtils/MySQLHelper.h"
//-----------------------------------------------------------------------------------------------------

class WizardLogic : public MySqlHelper
{
protected:
	class Orbiter *m_pOrbiter;

public: 
	list< pair<int,string> > m_listRoomTypes;  // In the order to display them
	map<int,string> m_mapRoomTypes; // For fast lookup

	WizardLogic(Orbiter *pOrbiter);
	virtual ~WizardLogic();

	bool Setup();

	/*
		SETUP ROOMS
	*/
	// Get a map of RoomTypes to number of Rooms of that Type, and the total number of rooms
	int PreSeedRoomInfo( map<int, int > &mapRooms ); // Call this to pre-populate the room info
	void ProcessUpdatedRoomInfo( map<int, int > &mapRooms ); // Call this to process the updated info
	void RemoveRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired ); // Helper function
	void AddRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired); // Helper function


};
//-----------------------------------------------------------------------------------------------------
#endif
