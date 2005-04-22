#include "PopulateListsInVMC.h"
//------------------------------------------------------------------------------------------------------
#include "Orbiter_Plugin.h"
#include "DCE/Logger.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "PlutoUtils/PlutoDefs.h"
#include "pluto_main/Table_Array.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Define_Button.h"
//------------------------------------------------------------------------------------------------------
enum MenuIds { miMain, miStandardScenarios, miSetHouseMode, miChooseMode, miSpeakInHouse };
//------------------------------------------------------------------------------------------------------
#define liStandardScenarios 1
#define liSetHouseMode 1
//------------------------------------------------------------------------------------------------------
static const string csRelativeURL = "/check.php?";
//------------------------------------------------------------------------------------------------------
PopulateListsInVMC::PopulateListsInVMC(string sSourceTemplateVMC, string sDestinationVMC, long dwPKDevice,
						Database_pluto_main *pDatabase_pluto_main, long dwInstallation)
{
    m_pMenuCollection = NULL;
    m_pDatabase_pluto_main = pDatabase_pluto_main;
    m_sSourceTemplateVMC = sSourceTemplateVMC;
    m_sDestinationVMC = sDestinationVMC;
    m_dwPKDevice = dwPKDevice;
    m_dwInstallation = dwInstallation;

    g_pPlutoLogger->Write(LV_STATUS, "<VMC> Ready to create VMC file '%s' from template '%s' file for device %d", 
        m_sSourceTemplateVMC.c_str(), m_sDestinationVMC.c_str(), m_dwPKDevice);
}
//------------------------------------------------------------------------------------------------------
PopulateListsInVMC::~PopulateListsInVMC()
{
    //save the vmc file
    if(!PopulateListsInVMC::SaveMenuCollection(m_pMenuCollection, m_sDestinationVMC))
        g_pPlutoLogger->Write(LV_STATUS, "Unable to save %s!", m_sDestinationVMC.c_str());

    PLUTO_SAFE_DELETE(m_pMenuCollection);
}
//------------------------------------------------------------------------------------------------------
bool PopulateListsInVMC::DoIt()
{
    //check if the file exists
    if(!FileUtils::FileExists(m_sSourceTemplateVMC))
    {
        g_pPlutoLogger->Write(LV_STATUS, "File %s not found!", m_sSourceTemplateVMC.c_str());
        return false;
    }

    //load the template vmc file
    m_pMenuCollection = PopulateListsInVMC::LoadMenuCollection(m_sSourceTemplateVMC);

    //do it!
    PopulateStandardScenariosList();
    //AddResolutionsForSetHouseMode(m_pMenuCollection, m_pDatabase_pluto_main);

    return true;
}
//------------------------------------------------------------------------------------------------------
/*static*/ VIPMenuCollection *PopulateListsInVMC::LoadMenuCollection(string sVMCFile)
{
	size_t iSize = 0;
	char *pData = FileUtils::ReadFileIntoBuffer(sVMCFile, iSize);

	if(!pData)
		return NULL;

	VIPMenuCollection *pMenuCollection = new VIPMenuCollection(long(iSize), pData);
	delete[] pData;

	return pMenuCollection;
}
//------------------------------------------------------------------------------------------------------
/*static*/ bool PopulateListsInVMC::SaveMenuCollection(VIPMenuCollection *pMenuCollection, string sVMCFile)
{
	pMenuCollection->ConvertToBinary();

	return FileUtils::WriteBufferIntoFile(sVMCFile, pMenuCollection->m_pBinary,
		pMenuCollection->m_iBinarySize);
}
//------------------------------------------------------------------------------------------------------
bool PopulateListsInVMC::PopulateStandardScenariosList()
{
	//get 'Standard Scenarios' menu page
	VIPMenu *pStandardScenariosMenu = m_pMenuCollection->m_mapMenus_Find(miStandardScenarios);
	if(!pStandardScenariosMenu)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, 
            "Couldn't get 'Standard scenarios' menu with id %d in vmc file", 
            miStandardScenarios);
		return false;
	}

	//get the list object from the menu
	VIPMenuElement *pStandardScenariosElement = pStandardScenariosMenu->m_mapMenuElements_Find(liStandardScenarios);
	if(!pStandardScenariosElement)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, 
            "Couldn't get the object with id %d from 'Standard scenarios' menu from vmc template file.", 
			liStandardScenarios);
		return false;
	}

	VIPMenuElement_List *pStandardScenariosList = NULL;
	if(pStandardScenariosElement->MenuElementID() == MENU_ELEMENT_LIST)
		pStandardScenariosList = (VIPMenuElement_List *)pStandardScenariosElement;
	else
	{
		g_pPlutoLogger->Write(LV_CRITICAL, 
            "The object with id %d 'Standard scenarios' menu is not a list.", liStandardScenarios);
		return false;
	}

	vector <Row_CommandGroup *> vectRow_CommandGroup;
	m_pDatabase_pluto_main->CommandGroup_get()->GetRows(
        "FK_Array = " + StringUtils::ltos(ARRAY_Mobile_Orbiter_Scenarios_CONST) + " AND " + 
        "FK_Installation = " + StringUtils::ltos(m_dwInstallation),
		&vectRow_CommandGroup);

    g_pPlutoLogger->Write(LV_STATUS, "<VMC> About to populate the list with standard scenarios, items %d", 
        vectRow_CommandGroup.size());

	int iIndex = 0;
	pStandardScenariosList->m_sText = "";
    vector <Row_CommandGroup *>::iterator it;
	for(it = vectRow_CommandGroup.begin(); it != vectRow_CommandGroup.end(); it++)
	{
        string sDescription = (*it)->Description_get();
        int iKeyCode = BUTTON_1_CONST + iIndex;

		pStandardScenariosList->m_sText += sDescription + "\r\n";

		string sID = StringUtils::ltos((*it)->PK_CommandGroup_get());
		VIPMenuResolution *pMenuResolution = new VIPMenuResolution("",0,"","", 0,0,0,0,0,"");
		pMenuResolution->m_sProgramName = csRelativeURL + "cgid=" + sID;
		pMenuResolution->m_sTerminatingKey += char(iKeyCode);

        g_pPlutoLogger->Write(LV_STATUS, 
            "<VMC> Added '%s' in the list with standard scenarios, path '%s', key %d", 
            sDescription.c_str(), pMenuResolution->m_sProgramName.c_str(), iKeyCode);

		pStandardScenariosMenu->AddResolution(pMenuResolution);
		iIndex++;
	}
	vectRow_CommandGroup.clear();
    return true;
}
//------------------------------------------------------------------------------------------------------
bool PopulateListsInVMC::AddResolutionsForSetHouseMode()
{
	VIPMenu *pStandardScenariosMenu = m_pMenuCollection->m_mapMenus_Find(miChooseMode);
	VIPMenuElement *pStandardScenariosElement = pStandardScenariosMenu->m_mapMenuElements_Find(liSetHouseMode);

	//pDatabase_pluto_main

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
    return true;
}
//------------------------------------------------------------------------------------------------------