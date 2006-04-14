#ifndef __SCREEN_MAP_H__
#define __SCREEN_MAP_H__

#include <map>
using namespace std;

void PopulateScreenMap(class Database_pluto_main *pDatabase_pluto_main, map<int,int>& mapDesignObj,
					   class Row_UI *pRow_UI, class Row_Skin *pRow_Skin, class Row_Device *pRow_Device);

#endif
