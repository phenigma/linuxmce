#ifndef OrbiterGenerator_h
#define OrbiterGenerator_h

#include <stdlib.h>

#include "pluto_main/Database_pluto_main.h"
#include "DesignObj_Generator.h"
#include "Orbiter/OrbiterData.h"
#include "PlutoUtils/MySqlHelper.h"

class OrbiterGenerator : public OrbiterData, public MySqlHelper
{
public:
	Database_pluto_main mds;

	class Row_Orbiter * m_pRow_Orbiter;
	class Row_Device * m_pRow_Device;
	class Row_Room * m_drRoom;
	class Row_Device * m_drDevice_EntGroup;
	int m_iLocation;
	map<int,int> m_htUsedStyleVariation;
	map<int,int> m_mapUsedOrbiterCriteria;
	map<int,listDesignObj_Generator *> m_htGeneratedScreens;
	map<int,class TextStyle *> m_htStylesUsedInOrbiterText;
//	Hashtable m_htUsedDevices;
//	StringWriter m_swErrors;
	bool m_bOrbiterChanged;
	string m_iID_Array;
	string m_GraphicsBasePath;
	int m_iPK_DesignObj_Screen;
	int m_iPK_CommandGroup; // If this is an array and we are supposed to execute an action group, this will be non-zero.  Arrays are always customer created command groups
	int m_iFloorplanPage;
	string m_sDesc_Array;
	class Row_Icon * m_drIcon;
	class PlutoSize *m_sizeScreen;
	class Row_Size * m_drSize;
//	string m_sMySQLHost,m_sMySQLUser,m_sMySQLPass,m_sMySQLDBName;
	string m_sOutputPath,m_sGraphicsBasePath,m_sFontPath;
//	int m_iMySQLPort;

	OrbiterGenerator(string sGraphicsFiles,string sFontFiles,string sOutputFiles,int PK_Orbiter,string DBHost,string DBUser,string DBPassword,string DBName,int DBPort)
		: MySqlHelper(DBHost, DBUser, DBPassword, DBName, DBPort)
	{
		m_bOrbiterChanged=false;
		m_sGraphicsBasePath=sGraphicsFiles;
		m_sFontPath=sFontFiles;
		m_sOutputPath=sOutputFiles;
		m_iPK_Orbiter=PK_Orbiter;
/*		m_sMySQLHost=DBHost;
		m_sMySQLUser=DBUser;
		m_sMySQLPass=DBPassword;
		m_sMySQLDBName=DBName;
		m_iMySQLPort=DBPort;
*/

		m_sFloorPlanData="";
		m_iNumFloorplanItems=0;
		Counter=0;
		m_iID_Array="0";
		m_iPK_DesignObj_Screen=0;
		m_iPK_CommandGroup=0; // If this is an array and we are supposed to execute an action group, this will be non-zero
		m_iFloorplanPage=0;
		m_drIcon=NULL;
	}
	int DoIt();

private:
	int m_iPK_Orbiter;
	map<int,string> htDevicePages;
	string m_sFloorPlanData;
	int m_iNumFloorplanItems;
	long Counter;

	void OutputScreen(class DesignObj_Generator *ocDesignObj);
	void OutputDesignObjs(class DesignObj_Generator *ocDesignObj,int ArrayPage,bool bIsChild,string ParentScreen);
	void OutputText(class DesignObj_Generator *ocDesignObj,class CGText *ot,bool bPrerender);
	void OutputCriteriaNest(class Row_CriteriaParmNesting *row);
	void SearchForGotos(class DesignObj_Data *pDesignObj_Data);
	void SearchForGotos(class DesignObj_Data *pDesignObj_Data,DesignObjCommandList *alCommands);
	void MatchChildDevicesToLocation(LocationInfo *li,Row_Device *pRow_Device);
};

#endif
