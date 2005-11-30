#include "ScreenMap.h"
#include "PlutoUtils/CommonIncludes.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Screen.h"
#include "pluto_main/Table_Screen_DesignObj.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_UI.h"
#include "pluto_main/Table_Skin.h"

void PopulateScreenMap(Database_pluto_main *pDatabase_pluto_main, map<int,int>& mapDesignObj,
					   Row_UI *pRow_UI, Row_Skin *pRow_Skin, Row_Device *pRow_Device)
{
	string SQL="(FK_UI=" + StringUtils::itos(pRow_UI->PK_UI_get()) + " OR FK_UI IS NULL) AND "
		"(FK_Skin=" + StringUtils::itos(pRow_Skin->PK_Skin_get()) + " OR FK_Skin IS NULL) AND "
		"(FK_DeviceTemplate=" + StringUtils::itos(pRow_Device->FK_DeviceTemplate_get()) + " OR FK_DeviceTemplate IS NULL) "
		"ORDER BY FK_Screen,FK_DeviceTemplate DESC, FK_UI DESC, FK_Skin DESC";

	vector<Row_Screen_DesignObj *> vectRow_Screen_DesignObj;
	pDatabase_pluto_main->Screen_DesignObj_get()->GetRows(SQL,&vectRow_Screen_DesignObj);

	int PK_Screen_Last=0;  // Keep track of the last one, we only 1 want design obj per screen, the first one
	for(size_t s=0;s<vectRow_Screen_DesignObj.size();++s)
	{
		Row_Screen_DesignObj *pRow_Screen_DesignObj = vectRow_Screen_DesignObj[s];
		if( pRow_Screen_DesignObj->FK_Screen_get()==PK_Screen_Last )
			continue;
		PK_Screen_Last=pRow_Screen_DesignObj->FK_Screen_get();
		mapDesignObj[PK_Screen_Last] = pRow_Screen_DesignObj->FK_DesignObj_get();
	}	
}