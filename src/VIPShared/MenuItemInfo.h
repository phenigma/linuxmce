#ifndef _MENU_ITEMINFO_H_
#define _MENU_ITEMINFO_H_

//---------------------------------------------------------------------------------------------------------
#include <string>
using namespace std;
//---------------------------------------------------------------------------------------------------------
#include "../SerializeClass/SerializeClass.h"
//---------------------------------------------------------------------------------------------------------


class MenuItemInfo {
public:
	unsigned char m_nImageType;
	char *m_pImage;
	unsigned long m_nImageSize;
	string m_sScenName;
	int m_nRoomId;
	string m_sObjectId;

	typedef enum { stNone=0, stRooms, stScenarios, stSubScenarios } ItemType;

	MenuItemInfo(){
		m_nImageType = 0;
		m_pImage = NULL;
		m_nImageSize = 0;
	}
	MenuItemInfo( const MenuItemInfo& Item ) { 
		m_nImageType = 0;
		m_pImage = NULL;
		m_nImageSize = 0;
		*this = Item; 
	}
	MenuItemInfo& operator=( const MenuItemInfo& Item );
	~MenuItemInfo();

	void write( unsigned char nItemType, SerializeClass &sc );
	void read( unsigned char nItemType, SerializeClass &sc );
	
	void SetImage( unsigned char ImageType,unsigned long ImageSize,const char *pImage );
};


#endif.
