#ifndef _MENU_DATA_H_
#define _MENU_DATA_H_

#include <map>
#include <vector>
#include <string>
using namespace std;

#include "../SerializeClass/SerializeClass.h"
#include "../Orbiter/TextStyle.h"

#ifdef SMARTPHONE	
	class LRMenu;
	class LRMenuItem;
#endif


class MD_SubScenario {
protected:
	string m_sSubScenId;
	string m_sSubScenName;
public:
	MD_SubScenario( string sSubScenId="", string sSubScenName="" ) {
		m_sSubScenId = sSubScenId;
		m_sSubScenName = sSubScenName;
	}
	~MD_SubScenario(){}
	MD_SubScenario( const MD_SubScenario& subscen ){ *this = subscen; }
	MD_SubScenario& operator=( const MD_SubScenario& subscen ){
		m_sSubScenId = subscen.m_sSubScenId;
		m_sSubScenName = subscen.m_sSubScenName;
		return *this;
	}
	int operator==( const MD_SubScenario& subscen ){ return m_sSubScenId==subscen.m_sSubScenId; }

	void Write( SerializeClass& sc );
	void Read( SerializeClass& sc );

	string& GetName() { return m_sSubScenName; }
	string& GetId() { return m_sSubScenId; }
};

class MD_Scenario {
protected:
	string m_sScenarioName;
	vector<MD_SubScenario> m_vSubScenarios;
public:
	MD_Scenario( string sScenarioName="" ) {
		m_sScenarioName = sScenarioName;
	}
	~MD_Scenario() {}
	MD_Scenario( const MD_Scenario& scen ) { *this = scen; }
	MD_Scenario& operator=( const MD_Scenario& scen ) {
		m_sScenarioName = scen.m_sScenarioName;
		m_vSubScenarios = scen.m_vSubScenarios;
		return *this;
	}
	int operator==( const MD_Scenario& scen ){ return m_sScenarioName==scen.m_sScenarioName; }


	void AddSubScenario( MD_SubScenario& subscen ) { 
		m_vSubScenarios.push_back( subscen ); 
	}
	MD_SubScenario* GetSubScenario( string& sSubScenarioId );
	void Write( SerializeClass& sc );
	void Read( SerializeClass& sc );

	string& GetName() { return m_sScenarioName; }
	#ifdef SMARTPHONE
	void CreateMenu( LRMenuItem* pRoot );
	#endif

};

class MD_Room {
protected:
	int m_iRoomId;
	string m_sRoomName;
	vector<MD_Scenario> m_vScenarios;
public:
	MD_Room( int iRoomId=0, string sRoomName="" ) {
		m_iRoomId = iRoomId;
		m_sRoomName = sRoomName;
	}
	~MD_Room() {}
	MD_Room( const MD_Room& room ) { *this = room; }
	MD_Room& operator=( const MD_Room& room ) { 
		m_iRoomId = room.m_iRoomId;
		m_sRoomName = room.m_sRoomName;
		m_vScenarios = room.m_vScenarios;
		return *this;
	}
	int operator==( const MD_Room& room ){ return m_iRoomId==room.m_iRoomId; }
	
	void AddScenario( MD_Scenario& scen ) {
		m_vScenarios.push_back( scen );
	}
	MD_Scenario* GetScenario( string sScenarioName );
	void Write( SerializeClass& sc );
	void Read( SerializeClass& sc );

	string& GetName() { return m_sRoomName; }
	int GetId() { return m_iRoomId; }

	#ifdef SMARTPHONE
	void CreateMenu( LRMenuItem* pRoot );
	#endif
};


class MenuData {
protected: 
	typedef vector<MD_Room> MD_Rooms;
	MD_Rooms m_Rooms;
	TextStyle m_tsItemText, m_tsHlItemText;	
	int m_iCrtRoom;

public:
	MenuData(){}
	~MenuData(){}
	MenuData& operator=( MenuData& data ){
		m_Rooms = data.m_Rooms;
		m_tsItemText = data.m_tsItemText;
		m_tsHlItemText = data.m_tsHlItemText;
		m_iCrtRoom = data.m_iCrtRoom;
		return *this;
	}

	void Write( SerializeClass& sc );
	void Read( SerializeClass& sc );

	MD_Room* GetRoom( int iRoomId );
	void AddRoom( MD_Room& room ) { m_Rooms.push_back( room ); }
	void AddRoom( int iRoomId, string sRoomName );
	void AddScenario( int iRoomId, string sScenarioName );
	void AddSubScenario( int iRoomId, string sScenarioName, string& sSubId, string sSubName );

	void SetItemTextStyle( TextStyle &ts ) { m_tsItemText = ts; }
	void SetHlItemTextStyle( TextStyle &ts ) { m_tsHlItemText = ts; }
	void SetCrtRoom( int iCrtRoom ) { m_iCrtRoom = iCrtRoom; }

	static int GetItemStyleIndex() { return 17; }
	static int GetHlItemStyleIndex() { return 18; }

	#ifdef SMARTPHONE
		LRMenu* CreateMainMenu();
	#endif
};

#endif.
