#include <iostream>
#include <fstream>

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "OrbiterGen.h"

#include "DCE/Logger.h"
#include "DesignObj_Generator.h"
#include "OGCommand.h"
#include "OGArray.h"
#include "SerializeClass/ShapesColors.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Text_LS.h"
#include "pluto_main/Table_Image.h"
#include "pluto_main/Table_CachedScreens.h"
#include "pluto_main/Table_Criteria.h"
#include "pluto_main/Table_DesignObj.h"
#include "pluto_main/Table_DesignObjVariation.h"
#include "pluto_main/Table_DesignObjVariation_Text_Skin_Language.h"
#include "pluto_main/Table_DesignObjVariation_Text.h"
#include "pluto_main/Table_DesignObjVariation_Zone.h"
#include "pluto_main/Table_DesignObjVariation_DesignObj.h"
#include "pluto_main/Table_DesignObjVariation_DesignObjParameter.h"
#include "pluto_main/Table_Size.h"
#include "pluto_main/Table_Skin.h"
#include "pluto_main/Table_Language.h"
#include "pluto_main/Table_Style.h"
#include "pluto_main/Table_Variable.h"
#include "pluto_main/Table_StyleVariation.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_Orbiter_Variable.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_Text_LS_AltVersions.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_Icon.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DesignObjParameter.h"
#include "pluto_main/Table_Installation_Users.h"
#include "pluto_main/Table_DesignObjType.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_Variable.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Event.h"
#include "pluto_main/Table_EventHandler.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CommandGroup_Command.h"

#define  VERSION "<=version=>"

namespace DCE
{
	class Logger *g_pPlutoLogger;
}

static bool LocationComparer(LocationInfo *x, LocationInfo *y)
{
	return x->Description<y->Description;
}

// For some reason windows won't compile with this in the same file???
void DoRender(string font, string output,int width,int height,bool bAspectRatio,class DesignObj_Generator *ocDesignObj);


#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
{

	//	WSADATA wsaData;
	//	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
#else
int main(int argc, char *argv[])
{
	setenv("SDL_VIDEODRIVER", "dummy", 1); // force SDL to use its dummy video driver (removed a dependency on the X server)
#endif
	g_pPlutoLogger=new FileLogger(stdout);

	string DBHost="dce_router",DBUser="root",DBPassword="",DBName="pluto_main";
#ifdef WIN32
	string GraphicsFiles="/pluto/graphics/skins",FontFiles="/windows/fonts",OutputFiles="/pluto";
#else
	string GraphicsFiles="/usr/pluto/skins",FontFiles="/usr/share/fonts/truetype/msttcorefonts",OutputFiles="/usr/pluto";
#endif
	int DBPort=3306,PK_Orbiter=0;
	bool bRegen=false;

	bool bError=false; // An error parsing the command line
	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			cerr << "Unknown option " << argv[optnum] << endl;
			exit(1);
		}

		c=argv[optnum][1];
		switch (c)
		{
		case 'h':
			DBHost = argv[++optnum];
			break;
		case 'u':
			DBUser = argv[++optnum];
			break;
		case 'p':
			DBPassword = argv[++optnum];
			break;
		case 'D':
			DBName = argv[++optnum];
			break;
		case 'P':
			DBPort = atoi(argv[++optnum]);
			break;
		case 'd':
			PK_Orbiter = atoi(argv[++optnum]);
			break;
		case 'g':
			GraphicsFiles = argv[++optnum];
			break;
		case 'f':
			FontFiles = argv[++optnum];
			break;
		case 'o':
			OutputFiles = argv[++optnum];
			break;
		case 'r':
			bRegen=true;
			break;
		default:
			bError=true;
			break;
		};
	}

	if (bError || GraphicsFiles.length()==0 || FontFiles.length()==0 || OutputFiles.length()==0 || !PK_Orbiter)
	{
		cout << "OrbiterGen, v." << VERSION << endl
			<< "Usage: OrbiterGen -d Orbiter -r [-g graphics files] [-f font files]" << endl
				<< "[-o output path] [-h hostname] [-u username] [-p password] [-D database]" << endl
				<< "[-P mysql port]" << endl
			<< "-d orbiter   -- the ID of the orbiter to generate" << endl
			<< "-r           -- Regenerate all screens--ignore cache" << endl
			<< "-g graphics  -- path to find the input (source) graphics files" << endl
			<< "-f fonts     -- path to find the font files" << endl
			<< "-o output    -- path to put all output files" << endl
			<< "-h hostname  -- address or DNS of database host, default is `localhost`" << endl
			<< "-u username  -- username for database connection" << endl
			<< "-p password  -- password for database connection, default is `` (empty)" << endl
			<< "-D database  -- database name" << endl
			<< "-P port      -- port for database connection, default is 3306" << endl;
		exit(0);
	}

	// Windows will work with / also
	GraphicsFiles = StringUtils::Replace(GraphicsFiles,"\\","/");
	FontFiles = StringUtils::Replace(FontFiles,"\\","/");
	OutputFiles = StringUtils::Replace(OutputFiles,"\\","/");

	if( GraphicsFiles.length() && GraphicsFiles[GraphicsFiles.length()-1]!='/' )
		GraphicsFiles+="/";
	if( FontFiles.length() && FontFiles[FontFiles.length()-1]!='/' )
		FontFiles+="/";
	if( OutputFiles.length() && OutputFiles[OutputFiles.length()-1]!='/' )
		OutputFiles+="/";

	int iResult=0;

	if( !FileUtils::FileExists(GraphicsFiles + "Basic") )
	{
		cerr << "Cannot find 'Basic' skin directory under " << GraphicsFiles << ".  Specify the directory with -g" << endl;
		exit(1);
	}
	if( !FileUtils::FileExists(FontFiles + "arial.ttf") )
	{
		cerr << "Cannot find 'fonts' -- looked for " << FontFiles << "arial.ttf.  Specify the directory with -f" << endl;
		exit(1);
	}
	try
	{
		OrbiterGenerator cg(GraphicsFiles,FontFiles,OutputFiles,PK_Orbiter,DBHost,DBUser,DBPassword,DBName,DBPort);
		if( bRegen )
			cg.m_bOrbiterChanged=true;
		iResult = cg.DoIt();
	}
	catch(const char *error)
	{
		cerr << "Error: " << error;
		Sleep(2000);
	}
	catch(string error)
	{
		cerr << "Error: " << error;
		Sleep(2000);
	}
	cout << "Finished!" << endl;
	return iResult;
}

int OrbiterGenerator::DoIt()
{
	MySQLConnect(); // Connect our helper
	// and also the sql2cpp classes
	if( !mds.Connect(m_sMySQLHost,m_sMySQLUser,m_sMySQLPass,m_sMySQLDBName,m_iMySQLPort) )
	{
		cout << "Failed to connect to database" << endl;
		exit(1);
	}

	m_pRow_Device = mds.Device_get()->GetRow(m_iPK_Orbiter);
	if( !m_pRow_Device )
		throw "No orbiter info for device: "; 

	m_pRow_Orbiter = mds.Orbiter_get()->GetRow(m_iPK_Orbiter);
	if( !m_pRow_Orbiter )
	{
		m_pRow_Orbiter = mds.Orbiter_get()->AddRow();
		m_pRow_Orbiter->PK_Orbiter_set(m_iPK_Orbiter);
		m_pRow_Orbiter->Table_Orbiter_get()->Commit();
	}

	cout << "Generating: #" << m_pRow_Device->PK_Device_get() << " " << m_pRow_Device->Description_get() << endl;

	if( m_sOutputPath[ m_sOutputPath.length()-1 ]!='/' )
		m_sOutputPath += "/";

	if( m_sFontPath[ m_sFontPath.length()-1 ]!='/' )
		m_sFontPath += "/";

	m_sOutputPath += "C" + StringUtils::itos(m_iPK_Orbiter) + "/";
	FileUtils::MakeDir(m_sOutputPath);

	cout << "Building orbiter: " << m_iPK_Orbiter << " installation " << m_pRow_Device->FK_Installation_get() << endl;

#ifdef WIN32
	system(("mkdir \"" + m_sOutputPath + "\" 2> null").c_str());
#else
	system(("mkdir -p " + m_sOutputPath + " 2>/dev/null").c_str());
#endif

	m_pRow_DesignObj_MainMenu = NULL;
	Row_Device_DeviceData *pRow_Device_DeviceData = mds.Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_PK_DesignObj_MainMenu_CONST);
	if( pRow_Device_DeviceData )
		m_pRow_DesignObj_MainMenu = mds.DesignObj_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );

	if( !m_pRow_DesignObj_MainMenu )
		m_pRow_DesignObj_MainMenu = mds.DesignObj_get()->GetRow(DESIGNOBJ_mnuMain_CONST);

	if( !m_pRow_DesignObj_MainMenu )
	{
		cerr << "Cannot find Orbiter's Main Menu: " << endl;
		exit(1);
	}

	m_pRow_DesignObj_Sleeping = NULL;
	pRow_Device_DeviceData = mds.Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_PK_DesignObj_Sleeping_CONST);
	if( pRow_Device_DeviceData )
		m_pRow_DesignObj_Sleeping = mds.DesignObj_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );

	if( !m_pRow_DesignObj_Sleeping )
		m_pRow_DesignObj_Sleeping = m_pRow_DesignObj_MainMenu;

	m_pRow_Skin = NULL;
	pRow_Device_DeviceData = mds.Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_PK_Skin_CONST);
	if( pRow_Device_DeviceData )
		m_pRow_Skin = mds.Skin_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );

	if( !m_pRow_Skin )
	{
		cerr << "Cannot find Orbiter's Skin" << endl;
		exit(1);
	}

	// Get the language
	m_pRow_Language = NULL;
	pRow_Device_DeviceData = mds.Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_PK_Language_CONST);
	if( pRow_Device_DeviceData )
		m_pRow_Language = mds.Language_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );

	if( !m_pRow_Language )
	{
		cerr << "Cannot find Orbiter's Language" << endl;
		exit(1);
	}

	// Get the no effects flag
	pRow_Device_DeviceData = mds.Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_No_Effects_CONST);
	if( pRow_Device_DeviceData )
		m_bNoEffects = atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str())!=1;

	// See if there's a default entertainment area for this orbiter
	Row_EntertainArea *pRow_EntertainArea_Default = NULL;
	vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
	m_pRow_Device->Device_EntertainArea_FK_Device_getrows(&vectRow_Device_EntertainArea);
	if( vectRow_Device_EntertainArea.size() )
		pRow_EntertainArea_Default = vectRow_Device_EntertainArea[0]->FK_EntertainArea_getrow();

	vector<Row_Variable *> vectrv;
	mds.Variable_get()->GetRows("1=1",&vectrv);
	for(size_t s=0;s<vectrv.size();++s)
	{
		Row_Variable *drVariable = vectrv[s];
		Row_Orbiter_Variable *drCV = mds.Orbiter_Variable_get()->GetRow(m_pRow_Orbiter->PK_Orbiter_get(),drVariable->PK_Variable_get());
		if( drCV && !drCV->Value_isNull() )
		{
			m_mapVariable[drVariable->PK_Variable_get()]=drCV->Value_get();
		}
		else
			m_mapVariable[drVariable->PK_Variable_get()]="";
	}

	Row_Device_DeviceData *drD_C_DP_DefaultUser = mds.Device_DeviceData_get()->GetRow(m_pRow_Orbiter->PK_Orbiter_get(),DEVICEDATA_PK_Users_CONST);

	if( !drD_C_DP_DefaultUser )
		throw "No default user set for orbiter: " + StringUtils::itos(m_pRow_Orbiter->PK_Orbiter_get());

	Row_Users *drUsers_Default = NULL;

	Row_Installation_Users *pRow_Installation_Users=mds.Installation_Users_get()->GetRow(m_pRow_Device->FK_Installation_get(),atoi(drD_C_DP_DefaultUser->IK_DeviceData_get().c_str()));
	if( pRow_Installation_Users )
		drUsers_Default = pRow_Installation_Users->FK_Users_getrow();
	if( !drUsers_Default)
	{
		throw "Default user is invalid for orbiter: " + StringUtils::itos(m_pRow_Orbiter->PK_Orbiter_get());
	}

	m_dwPK_Users = drUsers_Default->PK_Users_get();

	if( m_pRow_Device->FK_Room_isNull() )
		throw "no room for orbiter: " + StringUtils::itos(m_pRow_Device->PK_Device_get());

	//	map<int,listDesignObj_Generator *> htGeneratedScreens;
	//	map<int,string> htUsedDevices;
	//	map<int,string> m_mapUsedOrbiterCriteria;
	m_GraphicsBasePath=m_sGraphicsBasePath;
	//	m_htGeneratedScreens=&htGeneratedScreens;
	//	m_htUsedStyleVariation=&htUsedStyleVariation;
	//	m_htUsedVariables=htUsedVariables;
	//	m_htUsedDevices=htUsedDevices;
	//	m_m_mapUsedOrbiterCriteria = &m_mapUsedOrbiterCriteria;
	//	m_swErrors=swErrors;
	//	m_htVariables=m_mapVariable;
	if( !m_bOrbiterChanged )  // It may have already been set to true if -r regen was specified
	{
		time_t lModDate = StringUtils::SQLDateTime(m_pRow_Orbiter->psc_mod_get());
		time_t lModDate_LastGen = StringUtils::SQLDateTime(m_pRow_Orbiter->Modification_LastGen_get());
		m_bOrbiterChanged = (lModDate!=lModDate_LastGen);
	}

	m_pRow_Size = NULL;
	pRow_Device_DeviceData = mds.Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_PK_Size_CONST);
	if( pRow_Device_DeviceData )
		m_pRow_Size = mds.Size_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );

	if( !m_pRow_Size )
		throw "Cannot determine the size";

	m_sizeScreen = new PlutoSize(m_pRow_Size->Width_get() * 1000 / m_pRow_Size->ScaleX_get(),m_pRow_Size->Height_get() * 1000 / m_pRow_Size->ScaleY_get());
	m_iPK_DesignObj_Screen = m_pRow_DesignObj_MainMenu->PK_DesignObj_get();

	int i=0;

	/*  This is tricky with Mysql since there are no full joins or sub-queries.  We have to do it in multiple passes
	string sql = string("SELECT PK_Room,Room.Description AS RoomDescription,") \
	+	"PK_Device,X.Description AS DeviceDescription,FK_Icon FROM Room " \
	+	"FULL OUTER JOIN " \
	+	"(SELECT * FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(DeviceTemplate_Entertain_Unit_CONST) + ") AS X " \
	+	"ON FK_Room=PK_Room " \
	+	"WHERE (Room.FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()) + " Or Room.FK_Installation IS NULL) " \
	+	"AND (X.FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()) + " Or X.FK_Installation IS NULL)";
	*/

	// First put this in a list.  We really need a deque so we can access the members by their numeric
	// position, but we also need to sort it, and deque's don't implement sorting
	list<LocationInfo *> listLocationInfo;

	vector<Row_Room *> vectRow_Room;
	mds.Room_get()->GetRows("FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()),&vectRow_Room);
	for(size_t s=0;s<vectRow_Room.size();++s)
	{
		Row_Room *pRow_Room = vectRow_Room[s];
		LocationInfo *li = new LocationInfo();
		li->Description = pRow_Room->Description_get();
		li->PK_EntertainArea = 0;
		li->PK_Room = pRow_Room->PK_Room_get();
		li->drIcon=NULL;
		if( !pRow_Room->FK_Icon_isNull() )
			li->drIcon = pRow_Room->FK_Icon_getrow();
		listLocationInfo.push_back(li);
	}

	m_sMainMenu="";

	vector<Row_Device *> vectRow_Device;
	mds.Device_get()->GetRows(DEVICE_FK_INSTALLATION_FIELD + string("=") + StringUtils::itos(m_pRow_Device->FK_Installation_get()),&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		switch(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get())
		{
		case DEVICECATEGORY_DCE_Router_CONST:
			m_dwPK_Device_Router = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Lighting_Plugins_CONST:
			m_dwPK_Device_LightingPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Climate_Plugins_CONST:
			m_dwPK_Device_ClimatePlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Media_Plugins_CONST:
			m_dwPK_Device_MediaPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Telecom_Plugins_CONST:
			m_dwPK_Device_TelecomPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Security_Plugins_CONST:
			m_dwPK_Device_SecurityPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Datagrid_Plugins_CONST:
			m_dwPK_Device_DatagridPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_General_Info_Plugins_CONST:
			m_dwPK_Device_GeneralInfoPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Orbiter_Plugins_CONST:
			m_dwPK_Device_OrbiterPlugIn = pRow_Device->PK_Device_get();
			break;

		};
	}

	for(size_t s=0;s<vectRow_Room.size();++s)
	{
		Row_Room *pRow_Room = vectRow_Room[s];
		vector<Row_EntertainArea *> vectRow_EntertainArea;
		pRow_Room->EntertainArea_FK_Room_getrows(&vectRow_EntertainArea);
		LocationInfo *li=NULL;
		for(size_t s=0;s<vectRow_EntertainArea.size();++s)
		{
			Row_EntertainArea *pRow_EntertainArea = vectRow_EntertainArea[s];

			// Find the entry for the room
			list<LocationInfo *>::iterator it;
			for(it=listLocationInfo.begin();it!=listLocationInfo.end();++it)
			{
				li = (*it);
				if( li->PK_Room==pRow_EntertainArea->FK_Room_get() )
				{
					if( li->PK_EntertainArea==0 )
					{
						break;
					}
				}
				li = NULL; // This wasn't a valid match
			}

			if( !li )
			{
				li = new LocationInfo();
				li->PK_Room = pRow_EntertainArea->FK_Room_get();
				li->drIcon=NULL;
				listLocationInfo.push_back(li);
			}

			li->Description = pRow_EntertainArea->Description_get();
			li->PK_EntertainArea = pRow_EntertainArea->PK_EntertainArea_get();

			Row_Device *pRow_Device_MediaDirector = NULL;
			vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
			pRow_EntertainArea->Device_EntertainArea_FK_EntertainArea_getrows(&vectRow_Device_EntertainArea);
			for(size_t s=0;s<vectRow_Device_EntertainArea.size();++s)
			{
				Row_Device_EntertainArea *pRow_Device_EntertainArea = vectRow_Device_EntertainArea[s];
				if( pRow_Device_EntertainArea->FK_Device_getrow()->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST )
				{
					pRow_Device_MediaDirector = pRow_Device_EntertainArea->FK_Device_getrow();
					break;
				}
			}

			// There's a media director here.  Match up all the virtual devices
			if( pRow_Device_MediaDirector )
			{
				li->m_dwPK_Device_MediaDirector = pRow_Device_MediaDirector->PK_Device_get();
				MatchChildDevicesToLocation(li,pRow_Device_MediaDirector);
			}
		}
	}

	listLocationInfo.sort(LocationComparer);

	// First add the location numbers so they're available from the beginning, and put them in 
	// the deque.  We only used a list up till now so we could sort it.
	list<LocationInfo *>::iterator it;
	for(it=listLocationInfo.begin();it!=listLocationInfo.end();++it)
	{
		LocationInfo *li = (*it);
		li->iLocation=i++;
		m_dequeLocation.push_back(li);
	}

	if( m_dequeLocation.size()==0 )
	{
		throw "No entertainment areas in this installation";
		/*
		cout << "Warning: No entertainment areas and no rooms" << endl;
	
		Row_EntertainArea *pRow_EntertainArea = vectRow_EntertainArea[0];

		// We've got an empty house with no rooms and no entertainment areas.  Just add something
		LocationInfo *li = new LocationInfo();
		li->iLocation=0;
		li->Description = "Anywhere";
		li->PK_EntertainArea = pRow_EntertainArea->PK_EntertainArea_get();
		li->PK_Room = pRow_EntertainArea->FK_Room_get();
		li->drIcon=NULL;
		m_dequeLocation.push_back(li);					
*/
	}

	m_iLocation_Initial=0;
	DequeLocationInfo::iterator itd;
	for(itd=m_dequeLocation.begin();itd!=m_dequeLocation.end();++itd)
	{
		LocationInfo *li = (*itd);
		if( (pRow_EntertainArea_Default && li->PK_EntertainArea==pRow_EntertainArea_Default->PK_EntertainArea_get()) ||
			(!pRow_EntertainArea_Default && li->PK_Room==m_pRow_Device->FK_Room_get()) || m_dequeLocation.size()==1 )
		{
			m_sMainMenu = StringUtils::itos(m_pRow_DesignObj_MainMenu->PK_DesignObj_get()) + "." + StringUtils::itos(li->iLocation) + ".0";
			m_iLocation_Initial = li->iLocation;
		}

		m_pRow_Room = mds.Room_get()->GetRow(li->PK_Room);
		m_pRow_EntertainArea = li->PK_EntertainArea>0 ? mds.EntertainArea_get()->GetRow(li->PK_EntertainArea) : NULL;
		m_iLocation = li->iLocation;
		DesignObj_Generator *ocDesignObj = new DesignObj_Generator(this,m_pRow_DesignObj_MainMenu,PlutoRectangle(0,0,0,0),NULL,true,false);

		map<int,class Row_StyleVariation *> htStylesUsedInOrbiterText;
	}

	if( m_sMainMenu=="" )
	{
		cerr << "WARNING: No valid main menu found.  Check that a default room and entertainment area are specified in the Orbiter table." << endl
				<< "Using the first entry as the main menu." << endl;
		
		LocationInfo *li = m_dequeLocation.front();
		m_sMainMenu = StringUtils::itos(m_pRow_DesignObj_MainMenu->PK_DesignObj_get()) + "." + StringUtils::itos(li->iLocation) + ".0";
		m_iLocation_Initial = li->iLocation;
	}

	list<Row_DesignObj *> alNewDesignObjsToGenerate;  // Have to cache the list because I can't modify htgeneratedscreens while enumerating

	map<int,listDesignObj_Generator *>::iterator itgs;
	for(itgs=m_htGeneratedScreens.begin();itgs!=m_htGeneratedScreens.end();++itgs)
	{
		listDesignObj_Generator *o = (*itgs).second;
		m_pRow_DesignObj_MainMenu=NULL;

		if( o->size()>0 )
		{
			DesignObj_Generator *oco = o->front();
			if( oco->m_bUsingCache )
			{
				// We're going to have to lookup the Row_DesignObj manually, and we're going to need to search for goto's in all the children
				// since this was built from cache, and so the children were not created when the object was created
				m_pRow_DesignObj_MainMenu = mds.DesignObj_get()->GetRow(atoi(oco->m_ObjectID.c_str()));
				SearchForGotos(oco);
			}
			else
				m_pRow_DesignObj_MainMenu = oco->m_pRow_DesignObj;
		}

		if( m_pRow_DesignObj_MainMenu!=NULL)
		{
			vector<Row_DesignObj *> vectros;
			m_pRow_DesignObj_MainMenu->DesignObj_FK_DesignObj_IncludeIfOtherIncluded_getrows(&vectros);
			for(size_t s=0;s<vectros.size();++s)
			{
				Row_DesignObj *m_pRow_DesignObjDependancy = vectros[s];
				alNewDesignObjsToGenerate.push_back(m_pRow_DesignObjDependancy);
			}
		}
	}


	string sql = string("SELECT ") + COMMANDGROUP_COMMAND_COMMANDPARAMETER_IK_COMMANDPARAMETER_TABLE_FIELD + " FROM " +
		EVENTHANDLER_TABLE + " JOIN " + COMMANDGROUP_COMMAND_TABLE + " ON " + COMMANDGROUP_COMMAND_FK_COMMANDGROUP_TABLE_FIELD + "=" +
		EVENTHANDLER_FK_COMMANDGROUP_TABLE_FIELD + " JOIN " + COMMANDGROUP_COMMAND_COMMANDPARAMETER_TABLE + " ON " +
		COMMANDGROUP_COMMAND_COMMANDPARAMETER_FK_COMMANDGROUP_COMMAND_FIELD + "=" + COMMANDGROUP_COMMAND_PK_COMMANDGROUP_COMMAND_FIELD +
		" WHERE (" + EVENTHANDLER_FK_INSTALLATION_TABLE_FIELD + " IS NULL OR " + EVENTHANDLER_FK_INSTALLATION_TABLE_FIELD + "=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()) + ") AND " + COMMANDGROUP_COMMAND_FK_COMMAND_FIELD + 
		"=" + StringUtils::itos(COMMAND_Goto_Screen_CONST) + " AND " + COMMANDGROUP_COMMAND_COMMANDPARAMETER_FK_COMMANDPARAMETER_FIELD + "=" + StringUtils::itos(COMMANDPARAMETER_PK_DesignObj_CONST);

	PlutoSqlResult result_set3;
	MYSQL_ROW row;
	if( (result_set3.r=mysql_query_result(sql)) )
	{
		while ((row = mysql_fetch_row(result_set3.r)))
		{
			try
			{
				Row_DesignObj *drNewDesignObj = mds.DesignObj_get()->GetRow(atoi(row[0]));
				alNewDesignObjsToGenerate.push_back(drNewDesignObj);
			}
			catch(...)
			{}
		}
	}

	sql = string("select DeviceTemplate_MediaType_DesignObj.FK_DesignObj,Device.FK_DesignObj") + 
		" FROM Device " +
		" INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate " +
		" INNER JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate=PK_DeviceTemplate " + 
		" INNER JOIN DeviceTemplate_MediaType_DesignObj ON FK_DeviceTemplate_MediaType=PK_DeviceTemplate_MediaType " +
		" WHERE FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get());

	PlutoSqlResult result_set4;
	if( (result_set4.r=mysql_query_result(sql)) )
	{
		while ((row = mysql_fetch_row(result_set4.r)))
		{
			try
			{
				if( row[0] )
				{
					Row_DesignObj *drNewDesignObj = mds.DesignObj_get()->GetRow(atoi(row[0]));
					if( !drNewDesignObj )
						cerr << "Cannot find devicetempate_mediatype_designobj: " << row[0] << endl;
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
				}
				if( row[1] )
				{
					Row_DesignObj *drNewDesignObj = mds.DesignObj_get()->GetRow(atoi(row[1]));
					if( !drNewDesignObj )
						cerr << "Cannot find device.designobj: " << row[1] << endl;
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
				}
			}
			catch(...)
			{}
		}
	}


	list<Row_DesignObj *>::iterator itno;
	for(itno=alNewDesignObjsToGenerate.begin();itno!=alNewDesignObjsToGenerate.end();++itno)
	{
		Row_DesignObj *m_pRow_DesignObjDependancy = *itno;
		if( m_htGeneratedScreens.find(m_pRow_DesignObjDependancy->PK_DesignObj_get())==m_htGeneratedScreens.end() )
		{
			m_iPK_DesignObj_Screen = m_pRow_DesignObjDependancy->PK_DesignObj_get();
			DesignObj_Generator *ocDesignObj = new DesignObj_Generator(this,m_pRow_DesignObjDependancy,PlutoRectangle(0,0,0,0),NULL,true,false);
		}
	}
	int NumScreens=0;

	for(itgs=m_htGeneratedScreens.begin();itgs!=m_htGeneratedScreens.end();++itgs)
	{
		listDesignObj_Generator *o = (*itgs).second;
		for(listDesignObj_Generator::iterator itlcgo=o->begin();itlcgo!=o->end();++itlcgo)
		{
			DesignObj_Generator *oco = (*itlcgo);
			if( !oco->m_bUsingCache )
			{
				// The scaling process will also let us know if we have any arrays within our child objects.  If so
				// we know we cannot cache this screen.  So we pass in the top-most object throughout
				oco->ScaleAllValues(m_pRow_Size->ScaleX_get(),m_pRow_Size->ScaleY_get(),oco);
			}
			if( oco->m_alMPArray.size()<=1 )
				NumScreens++;
			else
				NumScreens += (int) oco->m_alMPArray.size();
		}
	}

	m_Width = m_pRow_Size->Width_get();
	m_Height = m_pRow_Size->Height_get();
	m_AnimationStyle = 1; // TODO -- is this used anymore? m_pRow_Orbiter->FK_Skin_getrow()->FK_AnimationStyle_get();


	// temp -- use of styles isn't yet well defined.  just hack in something to get a style variation for each style right now
	vector<Row_Style *> vectRow_Style;

	mds.Style_get()->GetRows("1=1",&vectRow_Style); // all styles
	for(size_t style=0;style<vectRow_Style.size();++style)
	{
		Row_Style *pRow_Style = vectRow_Style[style];

		vector<Row_StyleVariation *> vectrsv;
		pRow_Style->StyleVariation_FK_Style_getrows(&vectrsv);

		if( vectrsv.size()==0 )
		{
			cerr << "WARNING! No variation for style: " << StringUtils::itos(pRow_Style->PK_Style_get()) << " reverting to 1" << endl;
			pRow_Style = pRow_Style->Table_Style_get()->GetRow(1);
			pRow_Style->StyleVariation_FK_Style_getrows(&vectrsv);
			if( vectrsv.size()==0 )
				throw "No variation for style: " + StringUtils::itos(pRow_Style->PK_Style_get());
		}

		TextStyle *pTextStyle = DesignObj_Generator::PickStyleVariation(vectrsv,this,0);
		if( pTextStyle )
		{
			pTextStyle->m_iVersion=0;
			m_mapTextStyle[pRow_Style->PK_Style_get()]=pTextStyle;
		}
	}


	for(itgs=m_htGeneratedScreens.begin();itgs!=m_htGeneratedScreens.end();++itgs)
	{
		listDesignObj_Generator *o = (*itgs).second;
		for(listDesignObj_Generator::iterator itlcgo=o->begin();itlcgo!=o->end();++itlcgo)
		{
			DesignObj_Generator *oco = (*itlcgo);
			if( !oco->m_bUsingCache )
			{
				OutputScreen(oco);
				oco->m_rPosition.Right(m_Width);
				oco->m_rPosition.Bottom(m_Height);
				oco->m_rBackgroundPosition.Right(m_Width);
				oco->m_rBackgroundPosition.Bottom(m_Height);
				cout << "Rendering screen " << oco->m_ObjectID << endl;
				try
				{
					DoRender(m_sFontPath,m_sOutputPath,m_Width,m_Height,m_pRow_Size->PreserveAspectRatio_get()==1,oco);
				}
				catch(string s)
				{
					throw "Failed to render screen " + oco->m_ObjectID + " error: " + s ;
				}
				if( !oco->m_bContainsArrays )
				{
					// We can cache this the next time
					Row_CachedScreens *pdrCachedScreen = mds.CachedScreens_get()->GetRow(m_pRow_Orbiter->PK_Orbiter_get(),oco->m_pRow_DesignObj->PK_DesignObj_get(),oco->m_iVersion);
					if( !pdrCachedScreen )
					{
						pdrCachedScreen = mds.CachedScreens_get()->AddRow();
						pdrCachedScreen->FK_Orbiter_set(m_pRow_Orbiter->PK_Orbiter_get());
						pdrCachedScreen->FK_DesignObj_set(oco->m_pRow_DesignObj->PK_DesignObj_get());
						pdrCachedScreen->Version_set(oco->m_iVersion);

					}
					string Filename = m_sOutputPath + "screen " + StringUtils::itos(m_pRow_Orbiter->PK_Orbiter_get()) + "." + 
						StringUtils::itos(oco->m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(oco->m_iVersion) + "." + 
						StringUtils::itos((int) StringUtils::SQLDateTime(oco->m_pRow_DesignObj->psc_mod_get())) + ".cache";

					oco->SerializeWrite(Filename);
					pdrCachedScreen->Modification_LastGen_set(oco->m_pRow_DesignObj->psc_mod_get());
					mds.CachedScreens_get()->Commit();
				}
			}
			else
			{
				cout << "Using cached version of screen " << oco->m_ObjectID << endl;
				// Got to look for any used styles
			}
			m_ScreenMap[ oco->m_ObjectID ] = oco;
		}
	}
	/*
	fstr_Orbiter << StringUtils::itos((int) m_mapUsedOrbiterCriteria.size()) << "|";
	for(map<int,int>::iterator itucc=m_mapUsedOrbiterCriteria.begin();itucc!=m_mapUsedOrbiterCriteria.end();++itucc)
	{
	Row_Criteria * drCriteria = mds.Criteria_get()->GetRow((*itucc).first);
	fstr_Orbiter << drCriteria->FK_CriteriaList_get() << "|" << drCriteria->PK_Criteria_get() << "|";
	OutputCriteriaNest(drCriteria->FK_CriteriaParmNesting_getrow());
	}
	*/
	/*
	// Lastly, we should now have a complete list of the variables that we used in m_htVariablesUsed.  Go through and fill in all the defaults
	for(map<int,int>::iterator ituv=htUsedVariables.begin();ituv!=htUsedVariables.end();++ituv)
	{
	ocOrbiter->m_alUsedVariables.push_back(new OCUsedVariables((*ituv).first,m_mapVariable[(*ituv).first]));
	}

	fstr_Orbiter << StringUtils::itos((int) ocOrbiter->m_alUsedVariables.size()) << "|";
	for(list<OCUsedVariables *>::iterator ituv=ocOrbiter->m_alUsedVariables.begin();ituv!=ocOrbiter->m_alUsedVariables.end();++ituv)
	{
	OCUsedVariables *uv = (*ituv);
	fstr_Orbiter << uv->m_iPK_Variable << "|" << (uv->m_oInitialValue.length()>0 ? uv->m_oInitialValue : "") << "|";
	}
	*/
/* todo 2.0
	// Be sure to add any styles that are forced to be included
	vector<Row_Style *> vectStyles = mds.Style_get()->GetRows(string(STYLE_ALWAYSINCLUDEONORBITER_FIELD) + "=1");
	for(size_t s=0;s<vectStyles.size();++s)
	{
		Row_StyleVariation *temp1,*temp2;
		Row_Style * drStyle = vectStyles[s];
		Row_StyleVariation * drSV = DesignObj_Generator::PickStyleVariation(drStyle,this,temp1,temp2);
		if( m_htStylesUsedInOrbiterText.find(drSV->FK_Style_get())==m_htStylesUsedInOrbiterText.end() )
			m_htStylesUsedInOrbiterText[drSV->FK_Style_get()]=drSV;
	}
*/
/*
	for(map<int,class Row_StyleVariation *>::iterator itsuic=m_htStylesUsedInOrbiterText.begin();itsuic!=m_htStylesUsedInOrbiterText.end();++itsuic)
	{
		Row_StyleVariation * drStyleVariation = (*itsuic).second;
		DesignObjStyle *pDesignObjStyle = new DesignObjStyle();
		pDesignObjStyle->m_iPK_Style = drStyleVariation->FK_Style_get();
		pDesignObjStyle->m_iPixelHeight = drStyleVariation->PixelHeight_get() * m_pRow_Size->Scale_get() / 1000;
		pDesignObjStyle->m_iShadowX = drStyleVariation->ShadowX_get();
		pDesignObjStyle->m_iShadowY = drStyleVariation->ShadowY_get();
		pDesignObjStyle->m_sFont = drStyleVariation->Font_get();
		pDesignObjStyle->m_PlutoColor = PlutoColor(drStyleVariation->Color_get());
		pDesignObjStyle->m_PlutoColor_Shadow = PlutoColor(drStyleVariation->ShadowColor_get());
		pDesignObjStyle->m_bBold = drStyleVariation->Bold_get()==1;
		pDesignObjStyle->m_bItalic = drStyleVariation->Italic_get()==1;
		pDesignObjStyle->m_bUnderline = drStyleVariation->Underline_get()==1;
		m_mapDesignObjStyle[pDesignObjStyle->m_iPK_Style]=pDesignObjStyle;
	}
*/
	// This will update the _LastGen without changing the Modification field
	sql = "UPDATE Orbiter SET FloorplanInfo='" + StringUtils::itos(m_iNumFloorplanItems) + m_sFloorPlanData +
		+ "',Modification_LastGen=psc_mod,psc_mod=psc_mod WHERE PK_Orbiter=" + StringUtils::itos(m_pRow_Orbiter->PK_Orbiter_get());
	threaded_mysql_query(sql);

	bool b=SerializeWrite(m_sOutputPath + "C" + StringUtils::itos(m_iPK_Orbiter) + ".info");

	if( b )
		cout << "Successfully finished" << endl;
	else
		cerr << "Could not create output file" << endl;

#ifdef _WIN32
	// no dumping leaks now	WSACleanup();
#endif

	return 0;
}

void OrbiterGenerator::SearchForGotos(DesignObj_Data *pDesignObj_Data)
{
if( pDesignObj_Data->m_ObjectID.find("2753")!=string::npos )
{
int k=2;
}

	SearchForGotos(pDesignObj_Data,&pDesignObj_Data->m_Action_LoadList);
	SearchForGotos(pDesignObj_Data,&pDesignObj_Data->m_Action_UnloadList);
	SearchForGotos(pDesignObj_Data,&pDesignObj_Data->m_Action_StartupList);
	SearchForGotos(pDesignObj_Data,&pDesignObj_Data->m_Action_TimeoutList);
	DesignObjZoneList::iterator itZone;
	for(itZone=pDesignObj_Data->m_ZoneList.begin();itZone!=pDesignObj_Data->m_ZoneList.end();++itZone)
	{
		CGZone *oz = (CGZone *) *itZone;
		SearchForGotos(pDesignObj_Data,&oz->m_Commands);
	}
	DesignObj_DataList::iterator it;
	for(it=pDesignObj_Data->m_ChildObjects.begin();it!=pDesignObj_Data->m_ChildObjects.end();++it)
		SearchForGotos(*it);

}

void OrbiterGenerator::SearchForGotos(DesignObj_Data *pDesignObj_Data,DesignObjCommandList *alCommands)
{
	DesignObjCommandList::iterator itActions;
	for(itActions=alCommands->begin();itActions!=alCommands->end();++itActions)
	{
		CGCommand *oca = (CGCommand *) *itActions;
		if( oca->m_PK_Command == COMMAND_Goto_Screen_CONST )
		{
			map<int, string>::iterator itParm;
			for(itParm=oca->m_ParameterList.begin();itParm!=oca->m_ParameterList.end();++itParm)
			{
				if( (*itParm).first == COMMANDPARAMETER_PK_DesignObj_CONST )
				{
					Row_DesignObj *p_m_pRow_DesignObj = mds.DesignObj_get()->GetRow(atoi((*itParm).second.c_str()));
					if( p_m_pRow_DesignObj && m_htGeneratedScreens.find(p_m_pRow_DesignObj->PK_DesignObj_get())==m_htGeneratedScreens.end() )
					{
						m_iPK_DesignObj_Screen = p_m_pRow_DesignObj->PK_DesignObj_get();
						DesignObj_Generator *ocDesignObj = new DesignObj_Generator(this,p_m_pRow_DesignObj,PlutoRectangle(0,0,0,0),NULL,true,false);
						SearchForGotos(ocDesignObj);
					}
				}
			}
		}
	}
}

void OrbiterGenerator::OutputScreen(DesignObj_Generator *ocDesignObj)
{
	// Always output the first page, even if there's no other
	for(size_t i=0;i<ocDesignObj->m_alMPArray.size() || i==0;++i)
	{
		string ParentScreen = StringUtils::itos(ocDesignObj->m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(ocDesignObj->m_iVersion) + "." + StringUtils::itos((int) i);
		OutputDesignObjs(ocDesignObj,(int) i,false,ParentScreen);
	}
}

void OrbiterGenerator::OutputDesignObjs(DesignObj_Generator *ocDesignObj,int ArrayPage,bool bIsChild,string ParentScreen)
{
	ocDesignObj->m_bChild=bIsChild;

	int PK_Orbiter = ocDesignObj->m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get();
	Row_Orbiter * m_pRow_Orbiter = ocDesignObj->m_pOrbiterGenerator->m_pRow_Orbiter;

	if( ocDesignObj->m_bChild )
	{
		if( ocDesignObj->m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
		{
			size_t FirstDot = ParentScreen.find('.');
			size_t SecondDot = ParentScreen.find('.',FirstDot+1);
			string NewParent = ParentScreen.substr(0,SecondDot) + "." + StringUtils::itos(ocDesignObj->m_iFloorplanPage) + ".";
			ocDesignObj->m_ObjectID = NewParent + StringUtils::itos(ocDesignObj->m_pRow_DesignObjVariation->FK_DesignObj_get()) + "." + StringUtils::itos(Counter++);
		}
		else
		{
			ocDesignObj->m_ObjectID = ParentScreen + "." + StringUtils::itos(ocDesignObj->m_pRow_DesignObjVariation->FK_DesignObj_get()) + "." + StringUtils::itos(Counter++);
		}
	}
	else
		ocDesignObj->m_ObjectID = ParentScreen;

	if( ocDesignObj->m_rBackgroundPosition.Right()>m_Width )
		ocDesignObj->m_rBackgroundPosition.Right(m_Width);
	if( ocDesignObj->m_rPosition.Right()>m_Width )
		ocDesignObj->m_rPosition.Right(m_Width);
	if( ocDesignObj->m_rBackgroundPosition.Bottom()>m_Height )
		ocDesignObj->m_rBackgroundPosition.Bottom(m_Height);
	if( ocDesignObj->m_rPosition.Bottom()>m_Height )
		ocDesignObj->m_rPosition.Bottom(m_Height);

	if( ocDesignObj->m_iFloorplanPage>0 && ocDesignObj->m_iFloorplanDevice>0 )
	{
		string PageList = "";
		if( htDevicePages.find(ocDesignObj->m_iFloorplanDevice)!=htDevicePages.end() )
			PageList = htDevicePages[ocDesignObj->m_iFloorplanDevice];
		if( PageList.find(StringUtils::itos(ocDesignObj->m_iFloorplanPage) + ",")!=string::npos )
		{
			throw "Floorplan device " + StringUtils::itos(ocDesignObj->m_iFloorplanDevice) + " Page " + StringUtils::itos(ocDesignObj->m_iFloorplanPage) + " already in the system";
		}
		PageList += StringUtils::itos(ocDesignObj->m_iFloorplanPage) + ",";
		htDevicePages[ocDesignObj->m_iFloorplanDevice] = PageList;
		m_sFloorPlanData += "\t" + StringUtils::itos(ocDesignObj->m_iFloorplanDevice) + "\t" + StringUtils::itos(ocDesignObj->m_iFloorplanPage) + "\t" + ocDesignObj->m_ObjectID + "\t" + StringUtils::itos(ocDesignObj->m_pFloorplanFillPoint.X) + "\t" + StringUtils::itos(ocDesignObj->m_pFloorplanFillPoint.Y);
		m_iNumFloorplanItems++;
	}

	ocDesignObj->m_ObjectType=ocDesignObj->m_pRow_DesignObj->FK_DesignObjType_get();
	ocDesignObj->m_bCanBeHidden = bIsChild && ocDesignObj->m_bCanBeHidden;
	//	ocDesignObj->m_bDontMergeBackground =  
	//		(ocDesignObj->m_bHideByDefault ? "1" : "0") << "|" <<
	ocDesignObj->m_bDontResetState = ocDesignObj->m_pRow_DesignObjVariation->DontResetSelectedState_get()==1;
	ocDesignObj->m_bCantGoBack = ocDesignObj->m_pRow_DesignObj->CantGoBack_get()==1;

	//		StringUtils::itos(ocDesignObj->m_bChildrenBeforeText) << "|" << 
	//StringUtils::itos(ocDesignObj->m_bChildrenBehind) << "|" << 
	ocDesignObj->m_bProcessActionsAtServer = ocDesignObj->m_pRow_DesignObj->CommandsProcessedAtServer_get()==1;
	ocDesignObj->m_dwTimeoutSeconds = ocDesignObj->m_pRow_DesignObj->TimeoutSeconds_isNull() ? 0 : ocDesignObj->m_pRow_DesignObj->TimeoutSeconds_get();
	ocDesignObj->m_Priority=ocDesignObj->m_pRow_DesignObj->Priority_get();

	if( !ocDesignObj->m_pRow_DesignObjVariation->FK_Criteria_Orbiter_isNull() )
		ocDesignObj->m_iPK_Criteria = ocDesignObj->m_pRow_DesignObjVariation->FK_Criteria_Orbiter_get();
	else
		ocDesignObj->m_iPK_Criteria = 0;

	ocDesignObj->m_bAnimate = ocDesignObj->m_pRow_DesignObj->Animate_get()==1;
	ocDesignObj->m_bTabStop = ocDesignObj->m_bTabStop;

	/*
	if( ocDesignObj->m_sBackgroundFile.length()==0 )
	fstr_Orbiter << "|";
	else
	fstr_Orbiter << ocDesignObj->m_sBackgroundFile << "|";

	if( ocDesignObj->m_sSelectedFile.length()==0 )
	fstr_Orbiter << "|";
	else
	fstr_Orbiter << ocDesignObj->m_sSelectedFile << "|";

	if( ocDesignObj->m_sAltFile.length()==0 )
	fstr_Orbiter << "|";
	else
	fstr_Orbiter << ocDesignObj->m_sAltFile << "|";
	*/

	/*
	fstr_Orbiter << ocDesignObj->m_rBackgroundPosition.X << "|" << ocDesignObj->m_rBackgroundPosition.Y << "|" << \
	ocDesignObj->m_rBackgroundPosition.Width << "|" << ocDesignObj->m_rBackgroundPosition.Height << "|";
	fstr_Orbiter << ocDesignObj->m_rPosition.X << "|" << ocDesignObj->m_rPosition.Y << "|" << \
	ocDesignObj->m_rPosition.Width << "|" << ocDesignObj->m_rPosition.Height << "|";
	fstr_Orbiter << ocDesignObj->m_sOriginalSize.Width << "|" << ocDesignObj->m_sOriginalSize.Height << "|";
	*/
	int NumParms=0;

	vector<Row_DesignObjVariation_DesignObjParameter *> vectovp;
	ocDesignObj->m_pRow_DesignObjVariation->DesignObjVariation_DesignObjParameter_FK_DesignObjVariation_getrows(&vectovp);
	for(size_t s=0;s<vectovp.size();++s)
	{
		Row_DesignObjVariation_DesignObjParameter * drOVCP = vectovp[s];
		if( drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Graphic_Filename_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Selected_Graphic_Fil_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Alt_Graphic_File_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_X_Position_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Y_Position_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Width_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Height_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Page_CONST )
			NumParms++;
	}

	if( ocDesignObj->m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
		NumParms++;

	if( ocDesignObj->m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
	{
		ocDesignObj->m_mapObjParms[DESIGNOBJPARAMETER_Page_CONST]=ocDesignObj->m_iFloorplanPage;
	}

	for(size_t s=0;s<vectovp.size();++s)
	{
		Row_DesignObjVariation_DesignObjParameter * drOVCP = vectovp[s];
		if( drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Graphic_Filename_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Selected_Graphic_Fil_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Alt_Graphic_File_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_X_Position_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Y_Position_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Width_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Height_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Page_CONST )
		{
			string Value="";
			if( drOVCP->Value_isNull() || drOVCP->Value_get()=="" )
			{
				Row_DesignObjVariation_DesignObjParameter * drOVCP2 = mds.DesignObjVariation_DesignObjParameter_get()->GetRow(ocDesignObj->m_pRow_DesignObjVariation_Standard->PK_DesignObjVariation_get(),drOVCP->FK_DesignObjParameter_get());
				if( drOVCP2 )
					Value = drOVCP2->Value_get();
			}
			else
				Value = drOVCP->Value_get();

			// See if this is one of the parameters that we need to scale
			if( drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Fixed_Column_Width_CONST ||
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Column_Spacing_CONST ||
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_First_Column_Width_CONST )
			{
				if( Value=="" )
					ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]="";
				else
				{
					ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]=StringUtils::itos((atoi(Value.c_str()) * ocDesignObj->m_pOrbiterGenerator->m_pRow_Size->ScaleX_get() / 1000));
				}
			}
			if( drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Fixed_Row_Height_CONST || 
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Row_Spacing_CONST || 
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_First_Row_Height_CONST )
			{
				if( Value=="" )
					ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]="";
				else
				{
					ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]=StringUtils::itos((atoi(Value.c_str()) * ocDesignObj->m_pOrbiterGenerator->m_pRow_Size->ScaleY_get() / 1000));
				}
			}
/*  use plutocolor all the time
			else if( (drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_First_Row_Color_CONST || drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Cell_Color_CONST ||
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_First_Column_Color_CONST || drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Cell_Color_CONST ||
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Alt_Cell_Color_CONST) && Value.length()>0 )
			{
				string Output="";
				size_t Pos=0,LastPos=0;
				for(;;)
				{
					LastPos=Pos;
					Pos = Value.find(",",LastPos);
					PlutoColor c(atoi(Pos!=string::npos ? Value.substr(LastPos,Pos-LastPos).c_str() : Value.substr(LastPos).c_str()));
					if( Output.length()>0 )
						Output+=",";
					Output += StringUtils::itos(c.R()) + "," + StringUtils::itos(c.G()) + "," + StringUtils::itos(c.B());
					if( Pos==string::npos )
						break;
					else
						Pos++;
				}
				ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]=Output;
			}
*/
			else if( drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_PK_Style_CONST || drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_PK_Style_FirstRow_CONST ||
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_PK_Style_FirstColumn_CONST || drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_PK_Style_Selected_CONST ||
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_PK_Style_Alt_CONST )
			{
				int Style = atoi(Value.c_str());
/* todo, I'm just going to grab all styles now.  This is a problem when it doesn't regen a cached screen, it doesn't get the styles
				vector<Row_StyleVariation *> vectrsv;
				Row_Style *pRow_Style = mds.Style_get()->GetRow(Style);
				if( pRow_Style )
				{
					pRow_Style->StyleVariation_FK_Style_getrows(&vectrsv);
					if( vectrsv.size()==0 )
						throw "No variation for style; " + StringUtils::itos(pRow_Style->PK_Style_get());

					TextStyle *pTextStyle = DesignObj_Generator::PickStyleVariation(vectrsv,this,0);
					if( pTextStyle )
					{
						pTextStyle->m_iVersion=0;
						m_mapTextStyle[Style]=pTextStyle;
					}
				}
*/
				ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]=Value;
			}
			else
				ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]=StringUtils::Replace(Value,"<%=!%>",StringUtils::itos(PK_Orbiter));
		}
	}

	if( !ocDesignObj->m_pRow_DesignObjVariation->FK_Button_isNull() )
		ocDesignObj->m_iPK_Button = ocDesignObj->m_pRow_DesignObjVariation->FK_Button_get();
	else if( !ocDesignObj->m_pRow_DesignObjVariation_Standard->FK_Button_isNull() )
		ocDesignObj->m_iPK_Button = ocDesignObj->m_pRow_DesignObjVariation_Standard->FK_Button_get();
	else
		ocDesignObj->m_iPK_Button = 0;

	if( ocDesignObj->m_ZoneList.size()==0 && ocDesignObj->m_sDesignObjGoto.length()>0 )
	{
		// There's an object goto, but no zones.  This can happen with the arrays forward and back
		// Just output 1 zone that is the full object size with a nav_goto

		DesignObjZone *p_DesignObjZone = new DesignObjZone(ocDesignObj->m_rPosition.X,ocDesignObj->m_rPosition.Y,ocDesignObj->m_rPosition.Width,ocDesignObj->m_rPosition.Height);
		DesignObjCommand *p_DesignObjCommand = new DesignObjCommand(COMMAND_Goto_Screen_CONST,0,true,0,0);
		p_DesignObjZone->m_Commands.push_back(p_DesignObjCommand);
		p_DesignObjCommand->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST] = ocDesignObj->m_sDesignObjGoto;
		ocDesignObj->m_ZoneList.push_back(p_DesignObjZone);
	}
	else
	{
		DesignObjZoneList::iterator itZone;
		for(itZone=ocDesignObj->m_ZoneList.begin();itZone!=ocDesignObj->m_ZoneList.end();++itZone)
		{
			CGZone *oz = (CGZone *) *itZone;

			if( oz->m_Rect.Width==0 || oz->m_Rect.Height==0 )
			{
				oz->m_Rect.X=ocDesignObj->m_rPosition.X;
				oz->m_Rect.Y=ocDesignObj->m_rPosition.Y;
			}

			DesignObjCommandList::iterator itActions;
			for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
			{
				CGCommand *oa = (CGCommand *) *itActions;

				map<int, string>::iterator itParm;
				for(itParm=oa->m_ParameterList.begin();itParm!=oa->m_ParameterList.end();++itParm)
				{
					if( oa->m_PK_Command==COMMAND_Goto_Screen_CONST && (*itParm).first==COMMANDPARAMETER_PK_DesignObj_CONST )
					{
						string Value="";
						if( ocDesignObj->m_sDesignObjGoto.length()>0 )  // This must be going to another page in an array
							Value = ocDesignObj->m_sDesignObjGoto;
						else if( ocDesignObj->m_DesignObj_GeneratorGoto!=NULL )
							Value = StringUtils::itos(ocDesignObj->m_DesignObj_GeneratorGoto->m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(ocDesignObj->m_DesignObj_GeneratorGoto->m_iVersion) + ".0";
//						else if( (*itParm).second.find("<%=")!=string::npos ) // ?? todo -- this means that screens, like the main menu that got skipped in handle goto were ignored
						else
							Value = (*itParm).second;
						oa->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST]=Value;
					}
					else
						oa->m_ParameterList[(*itParm).first] = StringUtils::Replace((*itParm).second,"<%=!%>",StringUtils::itos(PK_Orbiter));
				}
			}
		}
	}


	// It probably makes more sense to just leave all the action groups attached to buttons within the orbiters, and only export action groups
	// associated with events to event manager.  Perhaps then all the fkid_object_goto's should be eliminated, and it is all just actions??/
	// Or leave fkid_object_goto in the database, but just make it an additional local action when it gets exported.
	// I WAS TRYING TO MOVE THE LOCAL ACTION INTO ACTION GROUPS, BUT THE PROBLEM IS THAT WHEN YOU TOUCH A BUTTON, A LOCAL ACTION IS NEVER SENT!!!
	// issues with removing fkid_action from objectvariation; action groups attached to an object with fkid_installation=null
	// will not get output.
	//			/* this isn't needed anymore since I'm always adding the action to the group, right?
	// The basic action applied to this button
	/*
	Row_Command * drCommand=null;
	if( !ocDesignObj->m_pRow_DesignObjVariation->FK_Command_isNull() || !ocDesignObj->m_pRow_DesignObjVariation_Standard->FK_Command_isNull())
	{
	DataRow[] drAPs;

	if( !ocDesignObj->m_pRow_DesignObjVariation->FK_Command_isNull() )
	{
	drCommand = ocDesignObj->m_pRow_DesignObjVariation->FK_Command_getrow();
	drAPs = ocDesignObj->m_pRow_DesignObjVariation.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_CommandParameter_FK_DesignObjVariation);
	}
	else
	{
	drCommand = ocDesignObj->m_pRow_DesignObjVariation->FK_Command_getrow();
	drAPs = ocDesignObj->m_pRow_DesignObjVariation_Standard.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_CommandParameter_FK_DesignObjVariation);
	}

	// If this action is handled locally, we won't put it here, we put it in the local actions later
	if( drCommand->HandledByOrbiter_get() )
	fstr_Orbiter << "0|0|");
	else
	{
	fstr_Orbiter << drCommand->PK_Command_get() + "|");
	fstr_Orbiter << drAPs.length() + "|");
	foreach(DataRow drAP in drAPs)
	{
	DesignObjVariation_Row_CommandParameter * drOVCAP = new DesignObjVariation_Row_CommandParameter *(drAP);
	fstr_Orbiter << drOVCAP->FK_CommandParameter_get() + "|" + (drOVCAP->FK_Variable_isNull() ? 0 : drOVCAP->FK_Variable_get()) + "|" + drOVCAP->Value_get() + "|");
	}
	}
	}
	else
	fstr_Orbiter << "0|0|");

	Row_CommandGroup * drAG = (ocDesignObj->m_iPK_CommandGroup==0 ? null : mds.tCommandGroup[ocDesignObj->m_iPK_CommandGroup]);

	fstr_Orbiter <<  ocDesignObj->m_iPK_CommandGroup==0 ? "" : ocDesignObj->m_iPK_CommandGroup.ToString() );
	fstr_Orbiter << "|");
	**************************   was action|action parms|action group
	*/
	/*			
	string sDesignObjGoto="";
	if( ocDesignObj->m_sDesignObjGoto!=null )
	sDesignObjGoto=ocDesignObj->m_sDesignObjGoto;
	else if( ocDesignObj->m_DesignObj_GeneratorGoto!=null )
	sDesignObjGoto = ocDesignObj->m_DesignObj_GeneratorGoto->m_pRow_DesignObj->PK_DesignObj_get() + "." + ocDesignObj->m_DesignObj_GeneratorGoto->m_iVersion + ".0";

	// We're pointing to a valid action group, look if there are any variable assign actions in there
	if( drAG!=null )
	{
	DataRow[] drAs = drAG.dr.GetChildRows(MyDataSet.MyRelations.CommandGroup_Command_FK_CommandGroup);
	foreach(DataRow dr in drAs)
	{
	Row_CommandGroup_Command * drAGA = new Row_CommandGroup_Command *(dr);
	if( drAGA->FK_Command_get() == COMMAND_Set_Variable_CONST )
	{
	int PK_Variable=0;
	string sValue=null;
	DataRow[] drAGAPs = drAGA.dr.GetChildRows(MyDataSet.MyRelations.CommandGroup_Command_CommandParameter_FK_CommandGroup_Command);
	foreach(DataRow dragap in drAGAPs)
	{
	Row_CommandGroup_Command_CommandParameter * drAGAP = new Row_CommandGroup_Command_CommandParameter *(dragap);
	if( drAGAP->FK_CommandParameter_get()==COMMANDPARAMETER_PK_Variable_CONST )
	{
	if( !drAGAP->FK_Variable_isNull() )
	PK_Variable = Convert.ToInt32(ocDesignObj->m_htVariables[drAGAP->FK_Variable_get()]);
	else if( !drAGAP->Value_isNull() )
	PK_Variable = Convert.ToInt32(drAGAP->Value_get());
	}
	else if( drAGAP->FK_CommandParameter_get()==COMMANDPARAMETER_Value_To_Assign_CONST )
	{
	if( !drAGAP->FK_Variable_isNull() )
	sValue = ocDesignObj->m_htVariables[drAGAP->FK_Variable_get()].ToString();
	else if( !drAGAP->Value_isNull() )
	sValue = drAGAP->Value_get();
	}
	}
	if( PK_Variable>0 && sValue!=null )
	{
	VariableAssignment va = new VariableAssignment(PK_Variable,sValue);
	ocDesignObj->m_alVariableAssignments.Add(va);
	if( !ocDesignObj->m_pOrbiterGenerator->m_htUsedVariables.ContainsKey(PK_Variable) )
	ocDesignObj->m_pOrbiterGenerator->m_htUsedVariables.Add(PK_Variable,1);
	}
	}
	}
	}
	*/
	/*			fstr_Orbiter << ocDesignObj->m_alVariableAssignments.Count + "|");
	foreach(VariableAssignment va in ocDesignObj->m_alVariableAssignments)
	{
	fstr_Orbiter << va.m_iPK_Variable + "|" + va.m_sValue + "|");
	}

	ArrayList alLocalCommands = new ArrayList();
	foreach(CGZone oz in ocDesignObj->m_alZones)
	{
	foreach(CGCommand oa in oz->m_alCommands)
	{
	// Don't add the goto's, we did this already
	if( oa->m_bLocalCommand || 
	(oa->drCommand->HandledByOrbiter_get() && oa->PK_Command!=COMMAND_Goto_Screen_CONST && oa->PK_Command!=COMMAND_NAV_GOTO_DEVICE_CONST) )
	alLocalCommands.Add(oa);
	}
	}

	if( drCommand!=null && drCommand->HandledByOrbiter_get() )
	{
	if( !ocDesignObj->m_pRow_DesignObjVariation->FK_Command_isNull() )
	{
	CGCommand oa = new CGCommand(ocDesignObj->m_pRow_DesignObjVariation,ocDesignObj);
	alLocalCommands.Add(oa);
	}
	else
	{
	CGCommand oa = new CGCommand(ocDesignObj->m_pRow_DesignObjVariation_Standard,ocDesignObj);
	alLocalCommands.Add(oa);
	}
	}
	*/
	/*			foreach(CGCommand oa in alLocalCommands)
	{
	fstr_Orbiter << oa->PK_Command + "|" + oa->m_PK_C_ExecTime + "|" + oa->m_alParms.Count + "|");
	foreach(CGCommandParm op in oa->m_alParms)
	{
	fstr_Orbiter << op.PK_CommandParameter + "|" + op.sValue + "|");
	}
	}

	fstr_Orbiter <<  sDesignObjGoto + "|");
	*/

	// This is the text that is not prerendered, going to the orbiter: pDesignObj_Generator->m_TextList
	// m_alText has all the text objects
	for(size_t s=0;s<ocDesignObj->m_vectDesignObjText.size();++s)
	{
		CGText *pDesignObjText = (CGText *) ocDesignObj->m_vectDesignObjText[s];
		OutputText(ocDesignObj,pDesignObjText,pDesignObjText->m_bPreRender);
		if( pDesignObjText->m_bPreRender )
			continue;

		for(MapTextStyle::iterator it=pDesignObjText->m_mapTextStyle.begin();it!=pDesignObjText->m_mapTextStyle.end();++it)
		{
			TextStyle *pTextStyle = (*it).second;;
			m_mapTextStyle[pTextStyle->m_iPK_Style]=pTextStyle;
		}
	}

	/*
	int Children = (int) ocDesignObj->m_alChildDesignObjs.size();
	for(size_t s=0;s<ocDesignObj->m_alNonMPArrays.size();++s)
	{
	CGArray *oca = ocDesignObj->m_alNonMPArrays[s];
	Children += (int) oca->m_alChildDesignObjs.size();
	}

	// ArrayPage is 0 based, count is 1 based, so output whenever count>arraypage
	if( ( (int)ocDesignObj->m_alMPArray.size())>ArrayPage )
	Children += (int) (ocDesignObj->m_alMPArray[ArrayPage])->m_alChildDesignObjs.size();
	*/
	for(size_t s=0;s<ocDesignObj->m_alChildDesignObjs.size();++s)
	{
		DesignObj_Generator *oco = ocDesignObj->m_alChildDesignObjs[s];
		ocDesignObj->m_ChildObjects.push_back(oco);
		OutputDesignObjs(oco,0,true,ParentScreen);
	}

	for(size_t s=0;s<ocDesignObj->m_alNonMPArrays.size();++s)
	{
		CGArray *oca = ocDesignObj->m_alNonMPArrays[s];
		for(size_t s=0;s<oca->m_alChildDesignObjs.size();++s)
		{
			DesignObj_Generator *oco = oca->m_alChildDesignObjs[s];
			ocDesignObj->m_ChildObjects.push_back(oco);
			OutputDesignObjs(oco,0,true,ParentScreen);
		}
	}

	if( ((int)ocDesignObj->m_alMPArray.size())>ArrayPage )
	{
		CGArray *oca = ocDesignObj->m_alMPArray[ArrayPage];
		for(size_t s=0;s<oca->m_alChildDesignObjs.size();++s)
		{
			DesignObj_Generator *oco = oca->m_alChildDesignObjs[s];
			ocDesignObj->m_ChildObjects.push_back(oco);
			OutputDesignObjs(oco,ArrayPage,true,ParentScreen);
		}
	}
}


void OrbiterGenerator::OutputText(DesignObj_Generator *ocDesignObj,CGText *p_DesignObjText,bool bPrerender)
{
	p_DesignObjText->m_PK_Text = p_DesignObjText->m_pdrText_LS->FK_Text_get();
	if( !p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->FK_HorizAlignment_isNull() )
	{
		for(MapTextStyle::iterator it=p_DesignObjText->m_mapTextStyle.begin();it!=p_DesignObjText->m_mapTextStyle.end();++it)
		{
			TextStyle *pTextStyle = (*it).second;;
			pTextStyle->m_iPK_HorizAlignment = p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->FK_HorizAlignment_get();
		}
	}

	if( !p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->FK_VertAlignment_isNull() )
	{
		for(MapTextStyle::iterator it=p_DesignObjText->m_mapTextStyle.begin();it!=p_DesignObjText->m_mapTextStyle.end();++it)
		{
			TextStyle *pTextStyle = (*it).second;;
			pTextStyle->m_iPK_VertAlignment = p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->FK_VertAlignment_get();
		}
	}

//	p_DesignObjText->m_Rotate = p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->Rotate_get();
//	p_DesignObjText->m_Opacity = p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->Opacity_get();
	// If we force a background color, put it in the default style
	if( !p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->PlainBackgroundColor_isNull() )
	{
		PlutoColor cBackground(p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->PlainBackgroundColor_get());
		TextStyle *pTextStyle = p_DesignObjText->m_mapTextStyle_Find(0);
		pTextStyle->m_BackColor=cBackground;
	}

	if( !bPrerender )
	{
		map<int,string>::iterator itatv;
		for(itatv=p_DesignObjText->m_mapAltVersions.begin();itatv!=p_DesignObjText->m_mapAltVersions.end();++itatv)
		{
			p_DesignObjText->m_mapAltVersions[(*itatv).first] = (*itatv).second;
		}
	}
}

void OrbiterGenerator::OutputCriteriaNest(Row_CriteriaParmNesting *row)
{
}

void OrbiterGenerator::MatchChildDevicesToLocation(LocationInfo *li,Row_Device *pRow_Device)
{
	vector<Row_Device *> vectChildren;
	pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectChildren);

	for(size_t s=0;s<vectChildren.size();++s)
	{
		Row_Device *pRow_Device_MDChild = vectChildren[s];
		switch( pRow_Device_MDChild->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get() )
		{
		case DEVICECATEGORY_App_Server_CONST:
			li->m_dwPK_Device_AppServer = pRow_Device_MDChild->PK_Device_get();
			MatchChildDevicesToLocation(li,pRow_Device_MDChild);  // These devices may be children of an app server also
			break;
		case DEVICECATEGORY_Disc_Drives_CONST:
			li->m_dwPK_Device_DiscDrive = pRow_Device_MDChild->PK_Device_get();
			break;
#pragma warning("there is not yet a web browser category")
//todo		case DEVICECATEGORY_WebBrowser_CONST:
//			li->m_dwPK_Device_WebBrowser = pRow_Device_MDChild->PK_Device_get();
//			break;
//todo		case DEVICECATEGORY_General_Info_CONST:
//			li->m_dwPK_Device_MediaBurner = pRow_Device_MDChild->PK_Device_get();
//			break;
//todo		case DEVICECATEGORY_General_Info_CONST:
//			li->m_dwPK_Device_VideoConf = pRow_Device_MDChild->PK_Device_get();
//			break;
//todo		case DEVICECATEGORY_General_Info_CONST:
//			li->m_dwPK_Device_CamcorderCapt = pRow_Device_MDChild->PK_Device_get();
//			break;
		case DEVICECATEGORY_Standard_Orbiter_CONST:
			li->m_dwPK_Device_Orbiter = pRow_Device_MDChild->PK_Device_get();
			MatchChildDevicesToLocation(li,pRow_Device_MDChild);  // These devices may be children of an orbiter also
			break;

		};
	}
}
