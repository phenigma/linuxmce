#ifndef __POPULATE_LIST_IN_VMC_H__
#define __POPULATE_LIST_IN_VMC_H__

#include <string>
using namespace std;

bool PopulateListsInVMC(string sSourceVMC, string sDestVMC, long dwPKDevice,
						class Database_pluto_main *pDatabase_pluto_main);

class VIPMenuCollection *LoadMenuCollection(string sVMCFile);
bool SaveMenuCollection(class VIPMenuCollection *pMenuCollection, string sVMCFile);

void PopulateStandardScenariosList(class VIPMenuCollection *pMenuCollection, 
								   class Database_pluto_main *pDatabase_pluto_main);

void AddResolutionsForSetHouseMode(VIPMenuCollection *pMenuCollection, Database_pluto_main *pDatabase_pluto_main);

#endif //__POPULATE_LIST_IN_VMC_H__