#include "PopulateListsInVMC.h"
#include "Orbiter_Plugin.h"

#include "DCE/Logger.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "pluto_main/Table_Array.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Define_Button.h"

enum MenuIds { miMain, miStandardScenarios, miSetHouseMode, miChooseMode, miSpeakInHouse };
#define liStandardScenarios 1
#define liSetHouseMode 1


static const string csRelativeURL = "/bubu.php?";

bool PopulateListsInVMC(string sSourceVMC, string sDestVMC, long dwPKDevice,
						Database_pluto_main *pDatabase_pluto_main)
{
	g_pPlutoLogger->Write(LV_STATUS, "Ready to populate lists in %s file", sSourceVMC.c_str());

	if(!FileUtils::FileExists(sSourceVMC))
	{
		g_pPlutoLogger->Write(LV_STATUS, "File %s not found!", sSourceVMC.c_str());
		return false;
	}

	//load the vmc file
	VIPMenuCollection *pMenuCollection = LoadMenuCollection(sSourceVMC);

	//standard scenarios list + add resolutios
	PopulateStandardScenariosList(pMenuCollection, pDatabase_pluto_main);
	AddResolutionsForSetHouseMode(pMenuCollection, pDatabase_pluto_main);

	//save the vmc file
	if(!SaveMenuCollection(pMenuCollection, sDestVMC))
	{
		g_pPlutoLogger->Write(LV_STATUS, "Unable to save %s!", sDestVMC.c_str());
		delete pMenuCollection;
        return false;
	}

	delete pMenuCollection;
	return true;
}

VIPMenuCollection *LoadMenuCollection(string sVMCFile)
{
	size_t iSize = 0;
	char *pData = FileUtils::ReadFileIntoBuffer(sVMCFile, iSize);

	if(!pData)
		return NULL;

	VIPMenuCollection *pMenuCollection = new VIPMenuCollection(long(iSize), pData);
	delete[] pData;

	return pMenuCollection;
}

bool SaveMenuCollection(VIPMenuCollection *pMenuCollection, string sVMCFile)
{
	pMenuCollection->ConvertToBinary();

	return FileUtils::WriteBufferIntoFile(sVMCFile, pMenuCollection->m_pBinary,
		pMenuCollection->m_iBinarySize);
}

void PopulateStandardScenariosList(VIPMenuCollection *pMenuCollection, Database_pluto_main *pDatabase_pluto_main)
{
	VIPMenu *pStandardScenariosMenu = pMenuCollection->m_mapMenus_Find(miStandardScenarios);
	VIPMenuElement *pStandardScenariosElement = pStandardScenariosMenu->m_mapMenuElements_Find(liStandardScenarios);

	VIPMenuElement_List *pStandardScenariosList = NULL;
	if(pStandardScenariosElement->MenuElementID() == MENU_ELEMENT_LIST)
		pStandardScenariosList = (VIPMenuElement_List *)pStandardScenariosElement;
	else
		return;

	vector <Row_CommandGroup *> vectRow_CommandGroup;
	pDatabase_pluto_main->CommandGroup_get()->GetRows("FK_Array = " + StringUtils::ltos(ARRAY_Mobile_Orbiter_Scenarios_CONST),
		&vectRow_CommandGroup);

	int iIndex = 0;
	pStandardScenariosList->m_sText = "";
	for(vector <Row_CommandGroup *>::iterator it = vectRow_CommandGroup.begin(); it != vectRow_CommandGroup.end(); it++)
	{
		pStandardScenariosList->m_sText += (*it)->Description_get() + "\r\n";

		string sID = StringUtils::ltos((*it)->PK_CommandGroup_get());
		VIPMenuResolution *pMenuResolution = new VIPMenuResolution("",0,"","", 0,0,0,0,0,"");
		pMenuResolution->m_sProgramName = csRelativeURL + "id=" + sID;
		pMenuResolution->m_sTerminatingKey += char(BUTTON_1_CONST + iIndex);

		pStandardScenariosMenu->AddResolution(pMenuResolution);
		iIndex++;
	}
	vectRow_CommandGroup.clear();
}

void AddResolutionsForSetHouseMode(VIPMenuCollection *pMenuCollection, Database_pluto_main *pDatabase_pluto_main)
{
	VIPMenu *pStandardScenariosMenu = pMenuCollection->m_mapMenus_Find(miChooseMode);
	VIPMenuElement *pStandardScenariosElement = pStandardScenariosMenu->m_mapMenuElements_Find(liSetHouseMode);

	VIPMenuResolution *pMenuResolution = new VIPMenuResolution("",0,"","", 0,0,0,0,0,"");
	pMenuResolution->m_sProgramName = csRelativeURL + "code=1";
	pMenuResolution->m_sTerminatingKey += char(BUTTON_1_CONST);
	pStandardScenariosMenu->AddResolution(pMenuResolution);

/*
	int iIndex = 0;

	string sID = StringUtils::ltos((*it)->PK_CommandGroup_get());
		VIPMenuResolution *pMenuResolution = new VIPMenuResolution("",0,"","", 0,0,0,0,0,"");
		pMenuResolution->m_sProgramName = csRelativeURL + "id=" + sID;
		pMenuResolution->m_sTerminatingKey += char(BUTTON_1_CONST + iIndex);

		pStandardScenariosMenu->AddResolution(pMenuResolution);
		iIndex++;
*/
}
