#include <iostream>
#include <fstream>

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "DCEGen.h"
#include "../pluto_main/Database_pluto_main.h"
#include "../pluto_main/Table_DeviceTemplate.h"
#include "../pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "../pluto_main/Table_DeviceCategory_DeviceData.h"
#include "../pluto_main/Table_DeviceCategory.h"
#include "../pluto_main/Table_DeviceData.h"
#include "../pluto_main/Table_EventParameter.h"
#include "../pluto_main/Table_ParameterType.h"
#include "../pluto_main/Table_DeviceTemplate_DeviceCommandGroup.h"
#include "../pluto_main/Table_DeviceCommandGroup_Command.h"
#include "../pluto_main/Table_CommandParameter.h"
#include "../pluto_main/Table_Command_CommandParameter.h"
#include "../pluto_main/Table_DeviceCommandGroup_Command.h"
#include "../pluto_main/Table_DeviceCommandGroup.h"
#include "../pluto_main/Table_Command.h"
#include "../pluto_main/Table_DeviceTemplate_Event.h"
#include "../pluto_main/Table_Event_EventParameter.h"
#include "../pluto_main/Table_Event.h"
#include "../pluto_main/Table_DeviceTemplate_DeviceTemplate_ControlledVia.h"
#include "../pluto_main/Table_DeviceTemplate_DeviceCategory_ControlledVia.h"
#include "../pluto_main/Table_Users.h"
#include "../pluto_main/Table_Package.h"

#define  VERSION "<=version=>"

namespace DCE
{
	class Logger *g_pPlutoLogger;
}

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
{

	//	WSADATA wsaData;
	//	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
#else
int main(int argc, char *argv[])
{
#endif

//	return result == 0;

	g_pPlutoLogger=new FileLogger("DCEGen.log");
	string DBHost="dcerouter",DBUser="root",DBPassword="",DBName="pluto_main";
	string TemplateInput="../DCEGen",TemplateOutput="",GeneratedOutput="../Gen_Devices";
	int DBPort=3306,PK_DeviceTemplate=0;

	bool bError=false; // An error parsing the command line
	bool bListAllDevices=false,bTemplates=true;
	string sSearchDeviceByName="";
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
		case 'd':
			PK_DeviceTemplate = atoi(argv[++optnum]);
			break;
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
		case 'o':
			GeneratedOutput = argv[++optnum];
			break;
		case 'i':
			TemplateInput = argv[++optnum];
			break;
		case 't':
			TemplateOutput = argv[++optnum];
			break;
		case 's':
			sSearchDeviceByName = argv[++optnum];
			break;
		case 'N':
			bTemplates=false;
			break;
		case 'a':
			bListAllDevices=true;
			break;
		default:
			bError=true;
			break;
		};
	}

	if (bError)
	{
		cout << "DCEGen, v." << VERSION << endl
			<< "Usage: DCEGen -d Device Template ID [-N] [-o Output Path] [-i Input for Templates] [-t Output for Templates] [-h hostname] [-u username] [-p password] [-D database] [-P mysql port] [-a] [-s word]" << endl
			<< "-d -- the Device ID to generate, from the table DeviceTemplate" << endl
			<< "-N -- don't create templates--only the generated files.  If this is specified, -d is optional, and if omitted, all devices will be generated." << endl
			<< "-o -- Where to put the generated files.  Defaults to ../Gen_Devices" << endl
			<< "-i -- Where to find the template files.  Defaults to ../DCEGen" << endl
			<< "-t -- Where to write the templates.  If not specified, defaults to ../[device name].  If that directory exists, it will delete and recreate a ../[device name]_temp." << endl
			<< "-h -- address or DNS of database host, default is dcerouter" << endl
			<< "-u -- username for database connection.  default is root " << endl
			<< "-p -- password for database connection, default is `` (empty)" << endl
			<< "-D -- database name.  default is pluto_main" << endl
			<< "-P -- port for database connection, default is 3306" << endl
			<< "-a -- List all devices by number" << endl
			<< "-s -- List all devices with this string in the name" << endl;
		exit(0);
	}

	GeneratedOutput = StringUtils::Replace(GeneratedOutput,"\\","/");
	TemplateInput = StringUtils::Replace(TemplateInput,"\\","/");
	TemplateOutput = StringUtils::Replace(TemplateOutput,"\\","/");

	if( GeneratedOutput.length() && GeneratedOutput[GeneratedOutput.length()-1]!='/' )
		GeneratedOutput+="/";
	if( TemplateInput.length() && TemplateInput[TemplateInput.length()-1]!='/' )
		TemplateInput+="/";
	if( TemplateOutput.length() && TemplateOutput[TemplateOutput.length()-1]!='/' )
		TemplateOutput+="/";

	if( !FileUtils::FileExists(GeneratedOutput) || !FileUtils::FileExists(TemplateInput + "DCE_Template.cpp") )
	{
		GeneratedOutput="Gen_Device";
		TemplateInput="DCEGen";
		TemplateOutput="./";
		if( !FileUtils::FileExists(GeneratedOutput) || !FileUtils::FileExists(TemplateInput + "DCE_Template.cpp") )
		{
			cerr << "Cannot find the templates, or output directory.  Confirm they exists." << endl;
			exit(1);
		}
		else
		{
			cout << "Could not find the standard template input, or output directories.  Using: " << GeneratedOutput << " instead." << endl;
		}
	}

	if( GeneratedOutput.length() )
		FileUtils::MakeDir(GeneratedOutput);
	if( TemplateOutput.length() )
		FileUtils::MakeDir(TemplateOutput);


	DCEGen gen(PK_DeviceTemplate,GeneratedOutput,TemplateInput,TemplateOutput,DBHost,DBUser,DBPassword,DBName,DBPort);
	gen.WriteGlobals(); // The global DCE namespace with class helpers for commands and requests
	if( bListAllDevices )
	{
		// todo
		exit(1);
	}
	if( sSearchDeviceByName.length() )
	{
		// todo 
		exit(1);
	}
	if( !PK_DeviceTemplate )
	{
		if( bTemplates && !AskYNQuestion("You didn't specify a device.  Do you want to rebuild all of them?",false) )
			return 0;

		vector<Row_DeviceTemplate *> vectRow_DeviceTemplate;
		gen.m_dce.DeviceTemplate_get()->GetRows( string(DEVICETEMPLATE_IMPLEMENTSDCE_FIELD) + "=1", &vectRow_DeviceTemplate);
		for(size_t s=0;s<vectRow_DeviceTemplate.size();++s)
		{
			Row_DeviceTemplate *pRow_DeviceTemplate = vectRow_DeviceTemplate[s];
			cout << "Recreating headers for " << pRow_DeviceTemplate->PK_DeviceTemplate_get() << " " << pRow_DeviceTemplate->Description_get() << endl;
			gen.m_mapGeneratedDevices.clear(); // Clear this since we're doing each device new
			gen.m_dwPK_DeviceTemplate = pRow_DeviceTemplate->PK_DeviceTemplate_get();
			gen.GenerateDevice(pRow_DeviceTemplate->PK_DeviceTemplate_get(),bTemplates);
		}
	}
	else
		gen.GenerateDevice(PK_DeviceTemplate,bTemplates);

	cout << "Done." << endl;
}

DCEGen::DCEGen(int PK_DeviceTemplate,string GeneratedOutput,string TemplateInput,string TemplateOutput,
				 string DBHost,string DBUser,string DBPassword,string DBName,int DBPort)
{
	m_dwPK_DeviceTemplate=PK_DeviceTemplate;  // Keep track of the main device we're generating
	m_sGeneratedOutput=GeneratedOutput;
	m_sTemplateInput=TemplateInput;
	m_sTemplateOutput=TemplateOutput;

	if( !m_dce.Connect(DBHost,DBUser,DBPassword,DBName,DBPort) )
	{
		cerr << "Failed to connect to database" << endl;
		exit(1);
	}
}

void DCEGen::GenerateDevice(int PK_DeviceTemplate,bool bTemplates)
{
	// This function will end up recursively calling itself to generate all it's child devices as well
	// Because there may be mutual relationships, keep track of what devices we've generated this 
	// session and just return if we've already generated this device to avoid an endless loop
	if( m_mapGeneratedDevices.find(PK_DeviceTemplate)!=m_mapGeneratedDevices.end() )
		return;

	m_mapGeneratedDevices[PK_DeviceTemplate]=1;  // 1 is unimportant

	// First we're going to build a list of devices that this one
	// must be able to create because this is specified as a "ControlledVia" either directly
	// through DeviceTemplate_DeviceTemplate_ControlledVia, or by the category through
	// DeviceTemplate_DeviceCategory_ControlledVia
	map<int,Row_DeviceTemplate *> mapRow_MasterDevice_Children;

	Row_DeviceTemplate *p_Row_DeviceTemplate = m_dce.DeviceTemplate_get()->GetRow(PK_DeviceTemplate);
	if( !p_Row_DeviceTemplate )
	{
		cerr << "Device Template: " << PK_DeviceTemplate << " doesn't exist." << endl;
		exit(1);
	}

	cout << "Generating DCE Files for device " << p_Row_DeviceTemplate->PK_DeviceTemplate_get() << " " << p_Row_DeviceTemplate->Description_get() << endl;

	// First add ourselves to the map, since the classes will need to generate our own data/events also
	mapRow_MasterDevice_Children[p_Row_DeviceTemplate->PK_DeviceTemplate_get()]=p_Row_DeviceTemplate;

	// Now find all records which specify us in the master
	vector<Row_DeviceTemplate_DeviceTemplate_ControlledVia *> vectChildDevices;
	p_Row_DeviceTemplate->DeviceTemplate_DeviceTemplate_ControlledVia_FK_DeviceTemplate_ControlledVia_getrows(&vectChildDevices);
	for(size_t s=0;s<vectChildDevices.size();++s)
	{
		Row_DeviceTemplate_DeviceTemplate_ControlledVia *pChild = vectChildDevices[s];
		if( pChild->FK_DeviceTemplate_getrow()->ImplementsDCE_get() && pChild->FK_DeviceTemplate_getrow()->IsEmbedded_get() )
			mapRow_MasterDevice_Children[pChild->FK_DeviceTemplate_get()]=pChild->FK_DeviceTemplate_getrow();

	}

	// Now find all records which specify us indirectly by the category
	// This will have to be a recursive function because a device can specify not only
	// our direct category, but the parent's category as well
	AddChildrenByCategory(p_Row_DeviceTemplate->FK_DeviceCategory_getrow(),&mapRow_MasterDevice_Children);

	m_sTemplateOutput = "../" + FileUtils::ValidCPPName(p_Row_DeviceTemplate->Description_get()) + "/";  // They're going to be output within out directory

	// We put our master device in this map already, and the embedded children too
	map<int,Row_DeviceTemplate *>::iterator it;
	for(it=mapRow_MasterDevice_Children.begin();it!=mapRow_MasterDevice_Children.end();++it)
		CreateDeviceFile((*it).second,&mapRow_MasterDevice_Children,bTemplates);
}

void DCEGen::AddChildrenByCategory(class Row_DeviceCategory *p_Row_DeviceCategory,map<int,class Row_DeviceTemplate *> *p_mapRow_MasterDevice_Children)
{
	vector<Row_DeviceTemplate_DeviceCategory_ControlledVia *> vectChildDevices;
	p_Row_DeviceCategory->DeviceTemplate_DeviceCategory_ControlledVia_FK_DeviceCategory_getrows(&vectChildDevices);
	for(size_t s=0;s<vectChildDevices.size();++s)
	{
		Row_DeviceTemplate_DeviceCategory_ControlledVia *pChild = vectChildDevices[s];
		if( pChild->FK_DeviceTemplate_getrow()->ImplementsDCE_get()==1 && pChild->FK_DeviceTemplate_getrow()->IsEmbedded_get() )
			(*p_mapRow_MasterDevice_Children)[pChild->FK_DeviceTemplate_get()]=pChild->FK_DeviceTemplate_getrow();
	}

	// Add children of parent categories as well
	if( !p_Row_DeviceCategory->FK_DeviceCategory_Parent_isNull() )
		AddChildrenByCategory(p_Row_DeviceCategory->FK_DeviceCategory_Parent_getrow(),p_mapRow_MasterDevice_Children);
}

void DCEGen::CreateDeviceFile(class Row_DeviceTemplate *p_Row_DeviceTemplate,map<int,class Row_DeviceTemplate *> *p_mapRow_MasterDevice_Children,bool bTemplates)
{
	string Name = FileUtils::ValidCPPName(p_Row_DeviceTemplate->Description_get());

	DeviceInfo deviceInfo(p_Row_DeviceTemplate);

	// First build some CommandInfo instances that list all our commmands
	vector<Row_DeviceTemplate_DeviceCommandGroup *> vectRow_DeviceTemplate_DeviceCommandGroup;
	p_Row_DeviceTemplate->DeviceTemplate_DeviceCommandGroup_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_DeviceCommandGroup);
	for(size_t i=0;i<vectRow_DeviceTemplate_DeviceCommandGroup.size();++i)
	{
		Row_DeviceTemplate_DeviceCommandGroup *pRow_DeviceTemplate_DeviceCommandGroup = vectRow_DeviceTemplate_DeviceCommandGroup[i];
		Row_DeviceCommandGroup *pRow_DeviceCommandGroup = pRow_DeviceTemplate_DeviceCommandGroup->FK_DeviceCommandGroup_getrow();

		vector<Row_DeviceCommandGroup_Command *> vectRow_DeviceCommandGroup_Command;
		pRow_DeviceCommandGroup->DeviceCommandGroup_Command_FK_DeviceCommandGroup_getrows(&vectRow_DeviceCommandGroup_Command);
		for(size_t i2=0;i2<vectRow_DeviceCommandGroup_Command.size();++i2)
		{
			Row_DeviceCommandGroup_Command *pRow_DeviceCommandGroup_Command = vectRow_DeviceCommandGroup_Command[i2];
			Row_Command *pRow_Command = pRow_DeviceCommandGroup_Command->FK_Command_getrow();
			if( deviceInfo.m_mapCommandInfo.find(pRow_Command->PK_Command_get())!=deviceInfo.m_mapCommandInfo.end() )
				continue;

			class CommandInfo *pCommandInfo = new CommandInfo();
			pCommandInfo->m_pRow_Command=pRow_Command;
			pCommandInfo->m_pRow_DeviceCommandGroup_Command=pRow_DeviceCommandGroup_Command;

			deviceInfo.m_mapCommandInfo[pRow_Command->PK_Command_get()] = pCommandInfo;

			CreateFunctionParms(pRow_Command,pCommandInfo);
		}
	}

	fstream fstr_DeviceCommand;

	fstr_DeviceCommand.open((m_sGeneratedOutput + Name + "Base.h").c_str(),fstream::out);
	fstr_DeviceCommand << "#ifndef "   <<  Name   <<  "Base_h" << endl;
	fstr_DeviceCommand << "#define "   <<  Name   <<  "Base_h" << endl;
	fstr_DeviceCommand << "#include \"DeviceData_Impl.h\"" << endl;
	fstr_DeviceCommand << "#include \"Message.h\"" << endl;
	fstr_DeviceCommand << "#include \"Command_Impl.h\"" << endl;
	fstr_DeviceCommand << endl << endl;
	fstr_DeviceCommand << "namespace DCE" << endl;
	fstr_DeviceCommand << "{" << endl;
	fstr_DeviceCommand << "//   OUR EVENT CLASS " << endl << endl;

	// ****EVENT****
	fstr_DeviceCommand << "class "  << Name <<  "_Event : public Event_Impl" << endl;
	fstr_DeviceCommand << "{" << endl;
	fstr_DeviceCommand << "public:" << endl;
	fstr_DeviceCommand << "\t" << Name << "_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID, ServerAddress, bConnectEventHandler) {};" << endl;
	fstr_DeviceCommand << "\t" << Name << "_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};" << endl;

	fstr_DeviceCommand << "\t//Events" << endl;
	fstr_DeviceCommand << "\tclass Event_Impl *CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice );"<< endl;

	vector<Row_DeviceTemplate_Event *> vectRow_DeviceTemplate_Event;
	p_Row_DeviceTemplate->DeviceTemplate_Event_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_Event);
	for(size_t i3=0;i3<vectRow_DeviceTemplate_Event.size();++i3)
	{
		Row_DeviceTemplate_Event *pRow_DeviceTemplate_Event = vectRow_DeviceTemplate_Event[i3];

		string EventDeclaration=FileUtils::ValidCPPName(pRow_DeviceTemplate_Event->FK_Event_getrow()->Description_get()) + "(";

		int ParmCount=0;
		string sParmsWithType="";
		string sAssignParmToLocal="";
		string sParmsWithNoType="";
		string sPassingToMessage="";

		CreateFunctionParms(pRow_DeviceTemplate_Event->FK_Event_getrow(),ParmCount,sParmsWithType,sAssignParmToLocal,sParmsWithNoType,sPassingToMessage);

		EventDeclaration += sParmsWithType + ")";
		fstr_DeviceCommand << "\tvirtual void " << EventDeclaration << endl << "\t{" << endl;
		deviceInfo.m_listRow_Event.push_back(pRow_DeviceTemplate_Event->FK_Event_getrow());
		deviceInfo.m_mapRow_DeviceTemplate_Event[pRow_DeviceTemplate_Event->FK_Event_get()]=pRow_DeviceTemplate_Event;
		deviceInfo.m_mapEventDeclarations[pRow_DeviceTemplate_Event->FK_Event_get()]=EventDeclaration;
		deviceInfo.m_mapEventParms[pRow_DeviceTemplate_Event->FK_Event_get()]=sParmsWithNoType;
		fstr_DeviceCommand << "\t\tSendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, "+StringUtils::itos(pRow_DeviceTemplate_Event->FK_Event_get())+",";
		fstr_DeviceCommand << sPassingToMessage;
		fstr_DeviceCommand << "));" << endl << "\t}" << endl << endl;
	}

	fstr_DeviceCommand << "};" << endl << endl << endl;

	fstr_DeviceCommand << "//   OUR DATA CLASS " << endl << endl;

	// Data requires special handling because we may or may not have a "set" method, depending on the value of SetByDevice.  We store this in DeviceInfo.m_mapDataCanSet.
	// And, Data can be linked directly to the device in DeviceTemplate_DeviceData, or to the device's category in DeviceTemplate_DeviceCategory
	vector<Row_DeviceTemplate_DeviceData *> vectMDLParms;
	p_Row_DeviceTemplate->DeviceTemplate_DeviceData_FK_DeviceTemplate_getrows(&vectMDLParms);
	for(size_t i2=0;i2<vectMDLParms.size();++i2)
	{
		Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = vectMDLParms[i2];
		if( deviceInfo.m_mapDataInfo.find(pRow_DeviceTemplate_DeviceData->FK_DeviceData_get())==deviceInfo.m_mapDataInfo.end() )
		{
			DataInfo *pDataInfo = new DataInfo();
			pDataInfo->m_pRow_DeviceData = pRow_DeviceTemplate_DeviceData->FK_DeviceData_getrow();
			pDataInfo->m_bCanSet = pRow_DeviceTemplate_DeviceData->SetByDevice_get()==1;
			deviceInfo.m_mapDataInfo[pRow_DeviceTemplate_DeviceData->FK_DeviceData_get()] = pDataInfo;
		}
	}
	vector<Row_DeviceCategory_DeviceData *> vectRow_DeviceCategory_DeviceData;
	p_Row_DeviceTemplate->FK_DeviceCategory_getrow()->DeviceCategory_DeviceData_FK_DeviceCategory_getrows(&vectRow_DeviceCategory_DeviceData);
	for(size_t i3=0;i3<vectRow_DeviceCategory_DeviceData.size();++i3)
	{
		Row_DeviceCategory_DeviceData *pRow_DeviceCategory_DeviceData = 
			vectRow_DeviceCategory_DeviceData[i3];

		if( deviceInfo.m_mapDataInfo.find(pRow_DeviceCategory_DeviceData->FK_DeviceData_get())==deviceInfo.m_mapDataInfo.end() )
		{
			DataInfo *pDataInfo = new DataInfo();
			pDataInfo->m_pRow_DeviceData = pRow_DeviceCategory_DeviceData->FK_DeviceData_getrow();
			pDataInfo->m_bCanSet = pRow_DeviceCategory_DeviceData->SetByDevice_get()==1;
		}
	}
	fstr_DeviceCommand << "class "  <<  Name  <<  "_Data : public DeviceData_Impl" << endl << "{" << endl;
	fstr_DeviceCommand << "public:" << endl;
	fstr_DeviceCommand << "\tvirtual ~"  << Name <<  "_Data() {};" << endl;
	fstr_DeviceCommand << "\tclass DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);" << endl;
	fstr_DeviceCommand << "\tvirtual int GetPK_DeviceList() { return "  <<  StringUtils::itos(p_Row_DeviceTemplate->PK_DeviceTemplate_get()) <<  "; } ;" << endl;
	fstr_DeviceCommand << "\tvirtual const char *GetDeviceDescription() { return \""  << FileUtils::ValidCPPName(p_Row_DeviceTemplate->Description_get()) <<  "\"; } ;" << endl;

	map<int,class DataInfo *>::iterator itDataInfo;
	for(itDataInfo=deviceInfo.m_mapDataInfo.begin();itDataInfo!=deviceInfo.m_mapDataInfo.end();++itDataInfo)
	{
		DataInfo *pDataInfo = (*itDataInfo).second;

		string pID = StringUtils::itos(pDataInfo->m_pRow_DeviceData->PK_DeviceData_get());
		string pDefine = FileUtils::ValidCPPName(pDataInfo->m_pRow_DeviceData->Description_get());
		fstr_DeviceCommand << "\t" << pDataInfo->m_pRow_DeviceData->FK_ParameterType_getrow()->Description_get()  << " Get_"  <<  
			pDefine  << "() { return ";
		fstr_DeviceCommand << CastStringToType("m_mapParameters[" + pID + "]",pDataInfo->m_pRow_DeviceData->FK_ParameterType_get()) << ";}" << endl;
		if(pDataInfo->m_bCanSet)
		{
			fstr_DeviceCommand << "\tvoid Set_" << pDefine << "(" << pDataInfo->m_pRow_DeviceData->FK_ParameterType_getrow()->Description_get() << 
				" Value) { SetParm(" << pID << "," << CastTypeToChar("Value",pDataInfo->m_pRow_DeviceData->FK_ParameterType_get()) << "); }" << endl;
		}
	}		

	fstr_DeviceCommand << "};" << endl;
	fstr_DeviceCommand << endl << endl << endl;
	fstr_DeviceCommand << "//   OUR COMMAND CLASS " << endl << endl;
	fstr_DeviceCommand << "class "   <<  Name   <<  "_Command : public Command_Impl" << endl;
	fstr_DeviceCommand << "{" << endl;
	fstr_DeviceCommand << "public:" << endl;
	fstr_DeviceCommand << "\t"  << Name   <<  "_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)" << endl;
	fstr_DeviceCommand << "\t: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)" << endl;
	fstr_DeviceCommand << "\t{" << endl;
	fstr_DeviceCommand << "\t\tif( m_bLocalMode )" << endl;
	fstr_DeviceCommand << "\t\t\treturn;" << endl;
	fstr_DeviceCommand << "\t\tm_pData=NULL;" << endl;
	fstr_DeviceCommand << "\t\tm_pEvent = new "  << Name  << "_Event(DeviceID, ServerAddress);" << endl;
	fstr_DeviceCommand << "\t\tint Size; char *pConfig = m_pEvent->GetConfig(Size);" << endl;
	fstr_DeviceCommand << "\t\tif( !pConfig )" << endl;
	fstr_DeviceCommand << "\t\t\tthrow \"Cannot get configuration data\";" << endl;
	fstr_DeviceCommand << "\t\tm_pData = new "  << Name  << "_Data();" << endl;
	fstr_DeviceCommand << "\t\tif( Size )" << endl;
	fstr_DeviceCommand << "\t\t\tm_pData->SerializeRead(Size,pConfig);" << endl;

	fstr_DeviceCommand << "\t\tdelete pConfig;" << endl;
	fstr_DeviceCommand << "\t\tpConfig = m_pEvent->GetDeviceList(Size);" << endl;
	fstr_DeviceCommand << "\t\tm_pData->m_AllDevices.SerializeRead(Size,pConfig);" << endl;
	fstr_DeviceCommand << "\t\tdelete pConfig;" << endl;
	fstr_DeviceCommand << "\t\tm_pData->m_pEvent_Impl = m_pEvent;" << endl;
	fstr_DeviceCommand << "\t\tm_pcRequestSocket = new Event_Impl(DeviceID, ServerAddress);" << endl;
	fstr_DeviceCommand << "\t};" << endl;
	fstr_DeviceCommand << "\t"  << Name  << "_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};" << endl;
	fstr_DeviceCommand << "\tvirtual ~"  << Name  << "_Command() {};" << endl;

	fstr_DeviceCommand << "\t"  << Name  << "_Event *GetEvents() { return ("  << Name  << "_Event *) m_pEvent; };" << endl;
	fstr_DeviceCommand << "\t"  << Name  << "_Data *GetData() { return ("  << Name  << "_Data *) m_pData; };" << endl;
	fstr_DeviceCommand << "\tconst char *GetClassName() { return \""   <<  Name   <<  "_Command\"; };" << endl;
	fstr_DeviceCommand << "\tvirtual int PK_DeviceTemplate_get() { return " << p_Row_DeviceTemplate->PK_DeviceTemplate_get() <<  "; };" << endl;
	fstr_DeviceCommand << "\tstatic int PK_DeviceTemplate_get_static() { return " << p_Row_DeviceTemplate->PK_DeviceTemplate_get() <<  "; };" << endl;
	fstr_DeviceCommand << "\tvirtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage) { };" << endl;
	fstr_DeviceCommand << "\tvirtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };" << endl;

	fstr_DeviceCommand << "\tCommand_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);" << endl;
	fstr_DeviceCommand << "\t//Data accessors" << endl;

	for(itDataInfo=deviceInfo.m_mapDataInfo.begin();itDataInfo!=deviceInfo.m_mapDataInfo.end();++itDataInfo)
	{
		DataInfo *pDataInfo = (*itDataInfo).second;
		Row_DeviceData *pRow_DeviceData = pDataInfo->m_pRow_DeviceData;

		string pID = StringUtils::itos(pRow_DeviceData->PK_DeviceData_get());
		string pDefine = FileUtils::ValidCPPName(pRow_DeviceData->Description_get());
		fstr_DeviceCommand << "\t" << pRow_DeviceData->FK_ParameterType_getrow()->Description_get() << " DATA_Get_"  <<  pDefine  << "() { return GetData()->Get_" << pDefine << "(); }" << endl;
		if(pDataInfo->m_bCanSet)
		{
			fstr_DeviceCommand << "\tvoid DATA_Set_" << pDefine << "(" << pDataInfo->m_pRow_DeviceData->FK_ParameterType_getrow()->Description_get() << " Value) { GetData()->Set_" << pDefine << "(Value); }" << endl;
		}
	}		
	fstr_DeviceCommand << "\t//Event accessors" << endl;
	map<int,string>::iterator itEventDeclarations;
	for(itEventDeclarations=deviceInfo.m_mapEventDeclarations.begin();itEventDeclarations!=deviceInfo.m_mapEventDeclarations.end();++itEventDeclarations)
	{
		Row_Event *pRow_Event = m_dce.Event_get()->GetRow( (*itEventDeclarations).first );
		fstr_DeviceCommand << "\tvoid EVENT_" << (*itEventDeclarations).second << " { GetEvents()->" << FileUtils::ValidCPPName(pRow_Event->Description_get()) << "(" << deviceInfo.m_mapEventParms[(*itEventDeclarations).first] << "); }" << endl;
	}

	fstr_DeviceCommand << "\t//Commands - Override these to handle commands from the server" << endl;

	map<int,class CommandInfo *>::iterator it;
	for(it=deviceInfo.m_mapCommandInfo.begin();it!=deviceInfo.m_mapCommandInfo.end();++it)
	{
		class CommandInfo *pCommandInfo = (*it).second;
		fstr_DeviceCommand << "\tvirtual void CMD_"  << pCommandInfo->CPPName() <<  "("  << pCommandInfo->m_sParmsWithType_In << (pCommandInfo->m_sParmsWithType_In.length()>0 && pCommandInfo->m_sParmsWithTypePointers_Out.length()>0 ? "," : "") << pCommandInfo->m_sParmsWithTypePointers_Out << (pCommandInfo->m_sParmsWithType_In.length() || pCommandInfo->m_sParmsWithTypePointers_Out.length() ? "," : "") << "string &sCMD_Result,class Message *pMessage) {};" << endl;
	}
	fstr_DeviceCommand << "" << endl;
/*
	for(itStringString=mapRNameParms.begin();itStringString!=mapRNameParms.end();++itStringString)
	{
		fstr_DeviceCommand << "\tvirtual bool REQ_"  << (*itStringString).first <<  "("  << (*itStringString).second <<  ") { return false; };" << endl;
	}
	fstr_DeviceCommand << "" << endl;
*/
	fstr_DeviceCommand << "\t//This distributes a received message to your handler." << endl;
	fstr_DeviceCommand << "\tvirtual bool ReceivedMessage(class Message *pMessageOriginal)" << endl;
	fstr_DeviceCommand << "\t{" << endl;

	fstr_DeviceCommand << "\t\tif( Command_Impl::ReceivedMessage(pMessageOriginal) )" << endl;
	fstr_DeviceCommand << "\t\t\treturn true;" << endl;

	fstr_DeviceCommand << "\t\tint iHandled=0;" << endl;
	fstr_DeviceCommand << "\t\tfor(int s=-1;s<(int) pMessageOriginal->m_vectExtraMessages.size(); ++s)" << endl;
	fstr_DeviceCommand << "\t\t{" << endl;
	fstr_DeviceCommand << "\t\t\tMessage *pMessage = s>=0 ? pMessageOriginal->m_vectExtraMessages[s] : pMessageOriginal;" << endl;

	if (deviceInfo.m_mapCommandInfo.size()>0)
	{
		fstr_DeviceCommand << "\t\t\tif (pMessage->m_dwPK_Device_To==m_dwPK_Device && pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND)\n\t\t\t{" << endl;
		fstr_DeviceCommand << "\t\t\t\tswitch(pMessage->m_dwID)" << endl;
		fstr_DeviceCommand << "\t\t\t\t{" << endl;

		map<int,class CommandInfo *>::iterator it;
		for(it=deviceInfo.m_mapCommandInfo.begin();it!=deviceInfo.m_mapCommandInfo.end();++it)
		{
			class CommandInfo *pCommandInfo = (*it).second;
if( pCommandInfo->m_pRow_Command->PK_Command_get()==COMMAND_Request_Datagrid_Contents_CONST )
{
int k=2;
}
			fstr_DeviceCommand << "\t\t\t\tcase " << pCommandInfo->m_pRow_Command->PK_Command_get() << ":" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t{" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t\tstring sCMD_Result=\"OK\";" << endl;
			fstr_DeviceCommand << pCommandInfo->m_sAssignParmToLocal_In;
			fstr_DeviceCommand << pCommandInfo->m_sAssignParmToLocal_Out;
//			if( pCommandInfo->m_sParmsWithType_Out.length()>0 )
//				fstr_DeviceCommand << "\t\t\t\t\t\t" << StringUtils::Replace(pCommandInfo->m_sParmsWithType_Out,",",";") << ";" << endl;  // we want int i1;sting s2 not int i1,string s2
			fstr_DeviceCommand << "\t\t\t\t\t\tCMD_" << pCommandInfo->CPPName() <<  "("  << pCommandInfo->m_sParmsWithNoType_In << (pCommandInfo->m_sParmsWithNoType_In.length() ? "," : "") << "sCMD_Result,pMessage);" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t\tif( pMessage->m_eExpectedResponse==ER_ReplyMessage )" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t\t{" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t\t\tMessage *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);" << endl;
			fstr_DeviceCommand << pCommandInfo->m_sAssignLocalToParm;
			fstr_DeviceCommand << "\t\t\t\t\t\t\tSendMessage(pMessageOut);" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t\t}" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t\telse if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t\t\tSendString(sCMD_Result);" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t};" << endl;
			fstr_DeviceCommand << "\t\t\t\t\tiHandled++;" << endl;
			fstr_DeviceCommand << "\t\t\t\t\tcontinue;" << endl;
		}
		fstr_DeviceCommand << "\t\t\t\t}" << endl;
		fstr_DeviceCommand << "\t\t\t\tiHandled += Command_Impl::ReceivedMessage(pMessage);" << endl;
		fstr_DeviceCommand << "\t\t\t}" << endl;
	}
/*
	if (mapRNameParms.size()>0)
	{
		fstr_DeviceCommand << "\t\tif (pMessage->m_dwPK_Device_To==m_dwPK_Device && pMessage->m_dwMessage_Type == MESSAGETYPE_REQUEST)\n\t\t{" << endl;
		fstr_DeviceCommand << "\t\t\tswitch(pMessage->m_dwID)" << endl;
		fstr_DeviceCommand << "\t\t\t{" << endl;

		for(itStringString=mapRNameParms.begin();itStringString!=mapRNameParms.end();++itStringString)
		{
			Row_Request *pRow_Request = m_dce.Request_get()->GetRow(  atoi(mapRNameRequestID[(*itStringString).first].c_str()));
			fstr_DeviceCommand << "\t\t\tcase " << mapRNameRequestID[(*itStringString).first] << ":" << endl;
			fstr_DeviceCommand << "\t\t\t\t{" << endl;
			vector<Row_Request_RequestParameter_In *> vectRow_Request_RequestParameter_In;
			pRow_Request->Request_RequestParameter_In_FK_Request_getrows(&vectRow_Request_RequestParameter_In);
			vector<Row_Request_RequestParameter_Out *> vectRow_Request_RequestParameter_Out;
			pRow_Request->Request_RequestParameter_Out_FK_Request_getrows(&vectRow_Request_RequestParameter_Out);

			for(size_t i3=0;i3<vectRow_Request_RequestParameter_In.size();++i3)
			{
				Row_Request_RequestParameter_In *pRow_Request_RequestParameter_In = vectRow_Request_RequestParameter_In[i3];
				string sParmsWithType,sAssignParmToLocal,sParmsWithNoType,sPassingToMessage;
				CreateFunctionParms(pRow_Request_RequestParameter_In->FK_RequestParameter_get(),
					pRow_Request_RequestParameter_In->FK_RequestParameter_getrow()->FK_ParameterType_get(),
					FileUtils::ValidCPPName(pRow_Request_RequestParameter_In->FK_RequestParameter_getrow()->Description_get()),
					sParmsWithType,sAssignParmToLocal,sParmsWithNoType,sPassingToMessage,false);  
				fstr_DeviceCommand << sAssignParmToLocal;
			}
			for(size_t i3=0;i3<vectRow_Request_RequestParameter_Out.size();++i3)
			{
				Row_Request_RequestParameter_Out *pRow_Request_RequestParameter_Out = vectRow_Request_RequestParameter_Out[i3];
				string sParmsWithType,sAssignParmToLocal,sParmsWithNoType,sPassingToMessage;
				CreateFunctionParms(pRow_Request_RequestParameter_Out->FK_RequestParameter_get(),
					pRow_Request_RequestParameter_Out->FK_RequestParameter_getrow()->FK_ParameterType_get(),
					FileUtils::ValidCPPName(pRow_Request_RequestParameter_Out->FK_RequestParameter_getrow()->Description_get()),
					sParmsWithType,sAssignParmToLocal,sParmsWithNoType,sPassingToMessage,false);  
				fstr_DeviceCommand << "\t\t\t\t\t\t" << StringUtils::Replace(sParmsWithType,",",";") << ";" << endl;  // Data parms will come in as a char and int separated with a ,
			}
/*
			fstr_DeviceCommand << mapRNameAssignParmToLocal[(*itStringString).first];
			if ((mapRNameAssignParmToLocal[(*itStringString).first]).size() > 0)
				fstr_DeviceCommand << "\t";

			fstr_DeviceCommand << "\t\t\t\tbool bResult = REQ_" << (*itStringString).first <<  "("  <<  mapRNameCallParms[(*itStringString).first]  <<  ");" << endl;
			fstr_DeviceCommand << "\t\t\t\t\tif( bResult )" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t{" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t\tMessage *pMessageOut = new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,MESSAGETYPE_REPLY,PRIORITY_NORMAL,1,0);" << endl;

			for(size_t i3=0;i3<vectRow_Request_RequestParameter_Out.size();++i3)
			{
				Row_Request_RequestParameter_Out *pRow_Request_RequestParameter_Out = vectRow_Request_RequestParameter_Out[i3];
				string sParmsWithType,sAssignParmToLocal,sParmsWithNoType,sPassingToMessage;
				CreateFunctionParms(pRow_Request_RequestParameter_Out->FK_RequestParameter_get(),
					pRow_Request_RequestParameter_Out->FK_RequestParameter_getrow()->FK_ParameterType_get(),
					FileUtils::ValidCPPName(pRow_Request_RequestParameter_Out->FK_RequestParameter_getrow()->Description_get()),
					sParmsWithType,sAssignParmToLocal,sParmsWithNoType,sPassingToMessage,true);  // Inverse since we're doing by reference
				fstr_DeviceCommand << "\t\t\t\t\t\t" << sAssignParmToLocal << endl;
			}
			fstr_DeviceCommand << "\t\t\t\t\t\tSendMessage(pMessageOut);" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t}" << endl;
			fstr_DeviceCommand << "\t\t\t\t\telse" << endl;
			fstr_DeviceCommand << "\t\t\t\t\t\tSendMessage(new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,MESSAGETYPE_REPLY,PRIORITY_NORMAL,0,0));" << endl;
						
			if ((mapRNameAssignParmToLocal[(*itStringString).first]).length() > 0)
			{
				fstr_DeviceCommand << "\t\t\t\t};" << endl;
			}
			fstr_DeviceCommand << "\t\t\t\treturn true;" << endl;
		}
		fstr_DeviceCommand << "\t\t\t}" << endl << "\t\t}" << endl;
	}
*/
	fstr_DeviceCommand << "\t\t\t" << (deviceInfo.m_mapCommandInfo.size()>0 ? "else" : "") << " if( pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND )" << endl;
	fstr_DeviceCommand << "\t\t\t{" << endl;
	fstr_DeviceCommand << "\t\t\t\tMapCommand_Impl::iterator it = m_mapCommandImpl_Children.find(pMessage->m_dwPK_Device_To);" << endl;
	fstr_DeviceCommand << "\t\t\t\tif( it!=m_mapCommandImpl_Children.end() && !(*it).second->m_bGeneric )" << endl;
	fstr_DeviceCommand << "\t\t\t\t{" << endl;
	fstr_DeviceCommand << "\t\t\t\t\tCommand_Impl *pCommand_Impl = (*it).second;" << endl;
	fstr_DeviceCommand << "\t\t\t\t\tiHandled += pCommand_Impl->ReceivedMessage(pMessage);" << endl;
	fstr_DeviceCommand << "\t\t\t}" << endl;
	fstr_DeviceCommand << "\t\t\telse" << endl;
	fstr_DeviceCommand << "\t\t\t{" << endl;
	fstr_DeviceCommand << "\t\t\t\tDeviceData_Base *pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pMessage->m_dwPK_Device_To);" << endl;
	fstr_DeviceCommand << "\t\t\t\tstring sCMD_Result=\"UNHANDLED\";" << endl;
	fstr_DeviceCommand << "\t\t\t\tif( pDeviceData_Base && pDeviceData_Base->IsChildOf(m_pData) )" << endl;
	fstr_DeviceCommand << "\t\t\t\t\tReceivedCommandForChild(pDeviceData_Base,sCMD_Result,pMessage);" << endl;
	fstr_DeviceCommand << "\t\t\t\telse" << endl;
	fstr_DeviceCommand << "\t\t\t\t\tReceivedUnknownCommand(sCMD_Result,pMessage);" << endl;
	fstr_DeviceCommand << "\t\t\t\t\tif( pMessage->m_eExpectedResponse==ER_ReplyMessage )" << endl;
	fstr_DeviceCommand << "\t\t\t\t\t{" << endl;
	fstr_DeviceCommand << "\t\t\t\t\t\tMessage *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);" << endl;
	fstr_DeviceCommand << "\t\t\t\t\t\tSendMessage(pMessageOut);" << endl;
	fstr_DeviceCommand << "\t\t\t\t\t}" << endl;
	fstr_DeviceCommand << "\t\t\t\t\telse if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )" << endl;
	fstr_DeviceCommand << "\t\t\t\t\t\tSendString(sCMD_Result);" << endl;
	fstr_DeviceCommand << "\t\t\t\t\tif( sCMD_Result!=\"UNHANDLED\" )" << endl;
	fstr_DeviceCommand << "\t\t\t\t\t\tiHandled++;" << endl;
	fstr_DeviceCommand << "\t\t\t\t}" << endl;
	fstr_DeviceCommand << "\t\t\t}" << endl;
	fstr_DeviceCommand << "\t\t}" << endl;
	fstr_DeviceCommand << "\t\treturn iHandled!=0;" << endl;
	fstr_DeviceCommand << "\t}" << endl;
	fstr_DeviceCommand << "}; // end class" << endl;
	fstr_DeviceCommand << endl << endl;

	fstr_DeviceCommand << "}" << endl;
	fstr_DeviceCommand << "#endif" << endl;
	fstr_DeviceCommand.close();

	fstr_DeviceCommand.open((m_sGeneratedOutput + Name + "Base.cpp").c_str(),fstream::out);

	fstr_DeviceCommand << "#include \"" + Name + "Base.h\"" << endl;
	fstr_DeviceCommand << "#include \"DeviceData_Impl.h\"" << endl;
	fstr_DeviceCommand << "#include \"Logger.h\"" << endl << endl;
	fstr_DeviceCommand << "using namespace DCE;" << endl;
	
	map<int,class Row_DeviceTemplate *>::iterator itMDL;
	for(itMDL=p_mapRow_MasterDevice_Children->begin();itMDL!=p_mapRow_MasterDevice_Children->end();++itMDL)
	{
		Row_DeviceTemplate *p_Row_DeviceTemplate = (*itMDL).second;

		fstr_DeviceCommand << "#include \"" + m_sTemplateOutput + FileUtils::ValidCPPName(p_Row_DeviceTemplate->Description_get()) + ".h\"" << endl;
	}

	fstr_DeviceCommand << "DeviceData_Impl *" + Name + "_Data::CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)" << endl;
	fstr_DeviceCommand << "{" << endl;
	fstr_DeviceCommand << "\t// Peek ahead in the stream.  We're going to pass in the above pointers anyway so it won't affect the position" << endl;
	fstr_DeviceCommand << "\tSerializeClass b;" << endl;
	fstr_DeviceCommand << "\tb.ResumeReadWrite(pDataBlock,AllocatedSize,CurrentPosition);" << endl;
	fstr_DeviceCommand << "\tint iPK_Device = b.Read_unsigned_long();" << endl;
	fstr_DeviceCommand << "\tint iPK_Installation = b.Read_unsigned_long();" << endl;
	fstr_DeviceCommand << "\tint iPK_DeviceTemplate = b.Read_unsigned_long();" << endl;

	if( p_mapRow_MasterDevice_Children->size() )
	{
		fstr_DeviceCommand << "\tswitch(iPK_DeviceTemplate) {" << endl;

		for(itMDL=p_mapRow_MasterDevice_Children->begin();itMDL!=p_mapRow_MasterDevice_Children->end();++itMDL)
		{
			Row_DeviceTemplate *p_Row_DeviceTemplate = (*itMDL).second;

			fstr_DeviceCommand << "\t\tcase " << StringUtils::itos(p_Row_DeviceTemplate->PK_DeviceTemplate_get()) << ":" << endl;
			fstr_DeviceCommand << "\t\t\treturn new " << FileUtils::ValidCPPName(p_Row_DeviceTemplate->Description_get()) << "_Data();" << endl;
		}

		fstr_DeviceCommand << "\t};" << endl;
	}
	fstr_DeviceCommand << "\tg_pPlutoLogger->Write(LV_STATUS, \"Got CreateData for unknown type %d.\", iPK_DeviceTemplate);"<< endl;
	fstr_DeviceCommand << "\treturn NULL;" << endl;
	fstr_DeviceCommand << "}" << endl << endl;

	fstr_DeviceCommand << "Event_Impl *" + Name + "_Event::CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice )" << endl << "{"<< endl;
	if( p_mapRow_MasterDevice_Children->size() )
	{
		fstr_DeviceCommand << "\tswitch(dwPK_DeviceTemplate) {"<< endl;

		for(itMDL=p_mapRow_MasterDevice_Children->begin();itMDL!=p_mapRow_MasterDevice_Children->end();++itMDL)
		{
			Row_DeviceTemplate *p_Row_DeviceTemplate = (*itMDL).second;
			fstr_DeviceCommand << "\t\tcase " + StringUtils::itos(p_Row_DeviceTemplate->PK_DeviceTemplate_get()) + ":"<< endl;
			fstr_DeviceCommand << "\t\t\treturn (Event_Impl *) new " + FileUtils::ValidCPPName(p_Row_DeviceTemplate->Description_get()) + "_Event(pOCClientSocket, dwDevice);"<< endl;
		}
		fstr_DeviceCommand << "\t};"<< endl;
	}
	fstr_DeviceCommand << "\tg_pPlutoLogger->Write(LV_STATUS, \"Got CreateEvent for unknown type %d.\", dwPK_DeviceTemplate);"<< endl;
	fstr_DeviceCommand << "\treturn NULL;"<< endl;
	fstr_DeviceCommand << "}"<< endl;

	fstr_DeviceCommand << "Command_Impl  *" + Name + "_Command::CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent)" << endl;
	fstr_DeviceCommand << "{" << endl;
	if( p_mapRow_MasterDevice_Children->size()>1 )  // We will always have on for ourselves
	{
		fstr_DeviceCommand << "\tswitch(PK_DeviceTemplate)" << endl;
		fstr_DeviceCommand << "\t{" << endl;

		for(itMDL=p_mapRow_MasterDevice_Children->begin();itMDL!=p_mapRow_MasterDevice_Children->end();++itMDL)
		{
			Row_DeviceTemplate *p_Row_DeviceTemplate = (*itMDL).second;
			if( p_Row_DeviceTemplate->PK_DeviceTemplate_get()!=m_dwPK_DeviceTemplate )  // Don't do this for ourselves since we can't have a child of the same type
			{
				fstr_DeviceCommand << "\t\tcase " << StringUtils::itos(p_Row_DeviceTemplate->PK_DeviceTemplate_get()) << ":" << endl;
				fstr_DeviceCommand << "\t\t\treturn (Command_Impl *) new " << FileUtils::ValidCPPName(p_Row_DeviceTemplate->Description_get()) << "(pPrimaryDeviceCommand, pData, pEvent, m_pRouter);" << endl;
			}
		}
		fstr_DeviceCommand << "\t};" << endl;
	}
	fstr_DeviceCommand << "\tg_pPlutoLogger->Write(LV_STATUS, \"Got CreateCommand for unknown type %d.\", PK_DeviceTemplate);"<< endl;
	fstr_DeviceCommand << "\treturn NULL;"<< endl;
	fstr_DeviceCommand << "}"<< endl;
	fstr_DeviceCommand.close();

	if( !bTemplates )
		return;

	string sMergeTo=""; // If we will merge, this will be the directory to merge into

	string sTemplateOutput=m_sTemplateOutput;
	// If a template was specified, and we're generating the main device, build the template
	if( m_sTemplateInput.length() && (m_dwPK_DeviceTemplate==p_Row_DeviceTemplate->PK_DeviceTemplate_get()) )
	{
		if( sTemplateOutput=="" )
			sTemplateOutput="../" + Name + "/";

		if( FileUtils::FileExists(sTemplateOutput + "Main.cpp") || FileUtils::FileExists(sTemplateOutput + Name + ".cpp") || 
			FileUtils::FileExists(sTemplateOutput + Name + ".cpp") || FileUtils::FileExists(sTemplateOutput + Name + ".vcproj") ||
			FileUtils::FileExists(sTemplateOutput + Name + "_DLL.vcproj") || FileUtils::FileExists(sTemplateOutput + "Makefile") ||
			FileUtils::FileExists(sTemplateOutput + Name + ".def") )  // We already built, 
		{
			cout << "There is already a project in the directory: " << sTemplateOutput << endl
				<< "If you have not already started to implement this devices, you" << endl
				<< "could choose to overwrite those files.  NOTE: This would wipe out" << endl
				<< "anything you've done so far.  Or I can leave them in a temporary directory" << endl
				<< sTemplateOutput + "_temp/ for you to merge manually." << endl
				<< "The normal selection is to merge in automatically.  I will" << endl
				<< "go through your source files and replace the function declarations" << endl
				<< "without touching your code.";
			char c = AskMCQuestion("What would you like to do?  Merge, Overwrite, Leave in temp directory, Abort","Mola");
			if( c=='m' || c=='l' )
			{
				if( c=='m' )
					sMergeTo = sTemplateOutput;
				string NoSlash = sTemplateOutput.substr(0, sTemplateOutput.length()-1);
				sTemplateOutput = NoSlash + "_temp/";
			}
			else if( c=='a' )
				return;

			// No matter what we need to clear the directory so we can output the files
			FileUtils::MakeDir(sTemplateOutput);
			cout << "Outputing to directory: " << sTemplateOutput << endl;
#ifdef WIN32
			system( ("del \"" + sTemplateOutput + "/*.*\" 2> null").c_str() );
#else
			system( ("rm -rf \"" + sTemplateOutput + "/\" 2>/dev/null").c_str() );
#endif
		}

		FileUtils::MakeDir(sTemplateOutput);

		string sDescription = "DCE Implemenation for #" + StringUtils::itos(p_Row_DeviceTemplate->PK_DeviceTemplate_get()) + " " + p_Row_DeviceTemplate->Description_get();
		SearchAndReplace(m_sTemplateInput + "Main_Template.cpp", sTemplateOutput + "Main.cpp", Name, &deviceInfo,sMergeTo,sDescription);
		SearchAndReplace(m_sTemplateInput + "DCE_Template.cpp", sTemplateOutput + Name + ".cpp", Name, &deviceInfo,sMergeTo,sDescription);
		SearchAndReplace(m_sTemplateInput + "DCE_Template.h", sTemplateOutput + Name + ".h", Name, &deviceInfo,sMergeTo,sDescription);
		SearchAndReplace(m_sTemplateInput + "DCE_Template.vcproj", sTemplateOutput + Name + ".vcproj", Name, &deviceInfo,sMergeTo,sDescription);
		SearchAndReplace(m_sTemplateInput + "DCE_Template_DLL.vcproj", sTemplateOutput + Name + "_DLL.vcproj", Name, &deviceInfo,sMergeTo,sDescription);
		SearchAndReplace(m_sTemplateInput + "Makefile_Template", sTemplateOutput + "Makefile", Name, &deviceInfo,sMergeTo,sDescription);
		SearchAndReplace(m_sTemplateInput + "DCE_Template.def", sTemplateOutput + Name + ".def", Name, &deviceInfo,sMergeTo,sDescription);
		SearchAndReplace(m_sTemplateInput + "Package.Info", sTemplateOutput + "Package.Info", Name, &deviceInfo,sMergeTo,sDescription);
		SearchAndReplace(m_sTemplateInput + "make_package.sh", sTemplateOutput + "make_package.sh", Name, &deviceInfo,sMergeTo,sDescription);
		SearchAndReplace(m_sTemplateInput + "post_make.sh", sTemplateOutput + "post_make.sh", Name, &deviceInfo,sMergeTo,sDescription);
	}
	else if( m_sTemplateInput.length() )
	{
		if( FileUtils::FileExists(sTemplateOutput + Name + ".cpp") )  // We already built, 
		{
			cout << "The embedded device " << Name << " already exists in " << sTemplateOutput << endl
				<< "If you have not already started to implement this devices, you" << endl
				<< "could choose to overwrite those files.  NOTE: This would wipe out" << endl
				<< "anything you've done so far.  Or I can leave them in a temporary directory" << endl
				<< sTemplateOutput + "_temp/ for you to merge manually." << endl
				<< "The normal selection is to merge in automatically.  I will" << endl
				<< "go through your source files and replace the function declarations" << endl
				<< "without touching your code.";
			char c = AskMCQuestion("What would you like to do?  Merge, Overwrite, Leave in temp directory, Abort","Mola");
			if( c=='m' || c=='l' )
			{
				if( c=='m' )
					sMergeTo = sTemplateOutput;
				string NoSlash = sTemplateOutput.substr(0, sTemplateOutput.length()-1);
				sTemplateOutput = NoSlash + "_temp/";
			}
			else if( c=='a' )
				return;

			// No matter what we need to clear the directory so we can output the files
			FileUtils::MakeDir(sTemplateOutput);
			cout << "Outputing to directory: " << sTemplateOutput << endl;
		}

		FileUtils::MakeDir(sTemplateOutput);

		string sDescription = "DCE Implemenation for #" + StringUtils::itos(p_Row_DeviceTemplate->PK_DeviceTemplate_get()) + " " + p_Row_DeviceTemplate->Description_get();
		SearchAndReplace(m_sTemplateInput + "DCE_Template.cpp", sTemplateOutput + Name + ".cpp", Name, &deviceInfo,sMergeTo,sDescription);
		SearchAndReplace(m_sTemplateInput + "DCE_Template.h", sTemplateOutput + Name + ".h", Name, &deviceInfo,sMergeTo,sDescription);
	}
}

void DCEGen::SearchAndReplace(string InputFile,string OutputFile,string Classname,class DeviceInfo *pDeviceInfo,string sMergeTo,string sDesription)
{
	FILE *file = fopen(OutputFile.c_str(),"rb");
	if( file )
	{
		fclose(file);
		cout << "Aborting: File " << OutputFile << " already exists." << endl;
		throw "Output file exists";
	}

	long total_size=0;
	file = fopen(InputFile.c_str(),"rb");
	if( !file )
	{
		cerr << "Cannot open file " << InputFile << endl;
		return;
	}

	fseek(file, 0L, SEEK_END);
	total_size=ftell(file);
	fseek(file, 0L, SEEK_SET);
	void *buf = malloc(total_size+1);
	if( fread(buf,1,total_size,file)!=total_size )
	{
		fclose(file);
		cerr << "Couldn't read file " << InputFile << endl;
		return;
	}
	char *term = (char *) buf;
	term[total_size]=0;
	fclose(file);

	string sBuffer((char *) buf);

	string::size_type pos;
// I don't think we're going to use the package info from here.  It limits our Make to Debian
	while( (pos=sBuffer.find("DCE_Template"))!=string::npos )
		sBuffer.replace(pos,12,Classname);

//#warning !!!! COMMENTED THIS LINE OUT SO IT WOULD COMPILE !!!!
//	while( (pos=sBuffer.find("DCE_DestinationPackage"))!=string::npos )
//		sBuffer.replace(pos,22,pDeviceInfo->m_pRow_DeviceTemplate->DestinationPackage_get());

	while( (pos=sBuffer.find("DCE_DestinationDir"))!=string::npos )
		sBuffer.replace(pos,18,pDeviceInfo->m_pRow_DeviceTemplate->DestinationDir_get());

	while( (pos=sBuffer.find("DCE_Description"))!=string::npos )
		sBuffer.replace(pos,15,sDesription);

	while( (pos=sBuffer.find("DCE_MaintainerName"))!=string::npos )
		sBuffer.replace(pos,14,pDeviceInfo->m_pRow_DeviceTemplate->FK_Users_Maintainer_getrow()->UserName_get());

	while( (pos=sBuffer.find("DCE_MaintainerEmail"))!=string::npos )
		sBuffer.replace(pos,19,pDeviceInfo->m_pRow_DeviceTemplate->FK_Users_Maintainer_getrow()->ForwardEmail_get());

	while ((pos = sBuffer.find("DCE_DevTemplateID")) != string::npos)
		sBuffer.replace(pos, 9, StringUtils::ltos(pDeviceInfo->m_pRow_DeviceTemplate->PK_DeviceTemplate_get()));
/*
	string sDependencies="x";
	vector<Row_DeviceTemplate_Package *> m_vectRow_DeviceTemplate_Package;
	pDeviceInfo->m_pRow_DeviceTemplate->DeviceTemplate_Package_FK_DeviceTemplate_getrows( &m_vectRow_DeviceTemplate_Package );
	for(size_t s=0;s<m_vectRow_DeviceTemplate_Package.size();++s)
	{
		Row_DeviceTemplate_Package *pRow_DeviceTemplate_Package = m_vectRow_DeviceTemplate_Package[s];
		sDependencies += (sDependencies.length() ? "," : "") + 	pRow_DeviceTemplate_Package->FK_Package_getrow()->Description_get() + "," 
			+ pRow_DeviceTemplate_Package->FK_Package_getrow()->Version_get();
	}

	while( (pos=sBuffer.find("DCE_Dependencies"))!=string::npos )
		sBuffer.replace(pos,16,sDependencies);
*/

	file = fopen(OutputFile.c_str(),"wb");
	if( !file )
	{
		cerr << "Cannot write to file " << OutputFile << endl;
		free(buf);
		return;
	}

	// There are 2 special tokens to search for
	bool bDefinition=false;
	pos = sBuffer.find("\t// FUNCTION_DEC");
	if( pos==string::npos )
	{
		bDefinition=true;
		pos = sBuffer.find("// FUNCTION_DEFN");
	}

	if( pos==string::npos )
	{
		// Just write it out
		fwrite(sBuffer.c_str(),1,sBuffer.length(),file);
		free(buf);
	}
	else
	{
		free(buf);

		// Write out the first part
		buf = (void *) sBuffer.c_str();
		fwrite(buf,1,pos,file);
		pos+=17; // Skip the comment

		if( !bDefinition )
		{
			fputs("\t/*\n\t\t\t*****DATA***** accessors inherited from base class\n",file);

			map<int,class DataInfo *>::iterator itDataInfo;
			for(itDataInfo=pDeviceInfo->m_mapDataInfo.begin();itDataInfo!=pDeviceInfo->m_mapDataInfo.end();++itDataInfo)
			{
				DataInfo *pDataInfo = (*itDataInfo).second;
				Row_DeviceData *pRow_DeviceData = pDataInfo->m_pRow_DeviceData;

				string pID = StringUtils::itos(pRow_DeviceData->PK_DeviceData_get());
				string pDefine = FileUtils::ValidCPPName(pRow_DeviceData->Description_get());
				fputs(("\t" + pRow_DeviceData->FK_ParameterType_getrow()->Description_get() + " DATA_Get_" + pDefine  + "();\n").c_str(),file);
				if(pDataInfo->m_bCanSet)
				{
					fputs(("\tvoid DATA_Set_" + pDefine  + "(" + pRow_DeviceData->FK_ParameterType_getrow()->Description_get() + " Value);\n").c_str(),file);
				}
			}		

			fputs("\n\t\t\t*****EVENT***** accessors inherited from base class\n",file);
			map<int,string>::iterator itEventDeclarations;
			for(itEventDeclarations=pDeviceInfo->m_mapEventDeclarations.begin();itEventDeclarations!=pDeviceInfo->m_mapEventDeclarations.end();++itEventDeclarations)
			{
				Row_Event *pRow_Event = m_dce.Event_get()->GetRow( (*itEventDeclarations).first );
				fputs(("\tvoid EVENT_" + (*itEventDeclarations).second + ";\n").c_str(),file);
			}

			fputs("\n\t\t\t*****COMMANDS***** we need to implement\n",file);
			fputs("\t*/\n\n",file);
		}

		map<int,class CommandInfo *>::iterator it;
		for(it=pDeviceInfo-> m_mapCommandInfo.begin();it!=pDeviceInfo-> m_mapCommandInfo.end();++it)
		{
			CommandInfo *pCommandInfo = (*it).second;

			Row_Command *pRow_Command = pCommandInfo->m_pRow_Command;
			Row_DeviceCommandGroup_Command *pRow_DeviceCommandGroup_Command = pCommandInfo->m_pRow_DeviceCommandGroup_Command;

			if( bDefinition )
				fputs(("//<-dceag-c" + StringUtils::itos(pRow_Command->PK_Command_get()) + "-b->\n").c_str(),file);
			fputs(("\n\t/** @brief COMMAND: #" + StringUtils::itos(pRow_Command->PK_Command_get()) + " - " + pRow_Command->Description_get() + " */\n").c_str(),file);
			fputs(("\t/** " + pRow_DeviceCommandGroup_Command->Description_get() + " */\n").c_str(),file);
			vector<Row_Command_CommandParameter *> vectRow_Command_CommandParameter;
			pRow_Command->Command_CommandParameter_FK_Command_getrows(&vectRow_Command_CommandParameter);
			for(size_t i3=0;i3<vectRow_Command_CommandParameter.size();++i3)
			{
				Row_Command_CommandParameter *pRow_Command_CommandParameter = vectRow_Command_CommandParameter[i3];
				fputs(("\t\t/** @param #" + StringUtils::itos(pRow_Command_CommandParameter->FK_CommandParameter_get()) + " " + 
					pRow_Command_CommandParameter->FK_CommandParameter_getrow()->Description_get() + " */\n").c_str(),file);
				if( !pRow_Command_CommandParameter->Description_isNull() )
					fputs(("\t\t\t/** " + pRow_Command_CommandParameter->Description_get() + " */\n").c_str(),file);
			}
			fputs("\n",file);

			if( bDefinition )
			{
				fputs(("void " + Classname + "::CMD_" + pCommandInfo->CPPName() + "(" + pCommandInfo->m_sParmsWithType_In + 
					(pCommandInfo->m_sParmsWithType_In.length()>0 && pCommandInfo->m_sParmsWithTypePointers_Out.length()>0 ? "," : "") + pCommandInfo->m_sParmsWithTypePointers_Out + (pCommandInfo->m_sParmsWithType_In.length() || pCommandInfo->m_sParmsWithTypePointers_Out.length() ? "," : "") + "string &sCMD_Result,Message *pMessage)\n").c_str(),file);
				fputs(("//<-dceag-c" + StringUtils::itos(pRow_Command->PK_Command_get()) + "-e->\n").c_str(),file);
				fputs("{\n",file);
				fputs(("\tcout << \"Need to implement command #" + StringUtils::itos(pRow_Command->PK_Command_get()) + 
					" - " + pRow_Command->Description_get() + "\" << endl;\n").c_str(),file);
				for(size_t i3=0;i3<vectRow_Command_CommandParameter.size();++i3)
				{
					Row_Command_CommandParameter *pRow_Command_CommandParameter = vectRow_Command_CommandParameter[i3];
					fputs(("\tcout << \"Parm #" + StringUtils::itos(pRow_Command_CommandParameter->FK_CommandParameter_get()) + " - " + 
						FileUtils::ValidCPPName(pRow_Command_CommandParameter->FK_CommandParameter_getrow()->Description_get())).c_str(),file);
					string Prefix=GetPrefix(pRow_Command_CommandParameter->FK_CommandParameter_getrow()->FK_ParameterType_get());
					if (pRow_Command_CommandParameter->FK_CommandParameter_getrow()->FK_ParameterType_get() != PARAMETERTYPE_Data_CONST)
					{
						fputs(("=\" << " + Prefix + FileUtils::ValidCPPName(pRow_Command_CommandParameter->FK_CommandParameter_getrow()->Description_get())).c_str(),file);
					}
					else
						fputs("  (data value)\"",file);
					fputs(" << endl;\n",file);
				}
				fputs("}\n\n",file);
			}
			else
			{
				fputs(("\tvirtual void CMD_" + pCommandInfo->CPPName() + "(" + pCommandInfo->m_sParmsWithType_In + (pCommandInfo->m_sParmsWithType_In.length()>0 && pCommandInfo->m_sParmsWithTypePointers_Out.length()>0 ? "," : "") 
					+ pCommandInfo->m_sParmsWithTypePointers_Out + 
					+ ") { string sCMD_Result; CMD_" + pCommandInfo->CPPName() + "(" + StringUtils::Replace(pCommandInfo->m_sParmsWithNoType_In,"&","")
					+ (pCommandInfo->m_sParmsWithNoType_In.length() ? "," : "") + "sCMD_Result,NULL);};\n").c_str(),file);
				fputs(("\tvirtual void CMD_" + pCommandInfo->CPPName() + "(" + pCommandInfo->m_sParmsWithType_In + (pCommandInfo->m_sParmsWithType_In.length()>0 && pCommandInfo->m_sParmsWithTypePointers_Out.length()>0 ? "," : "") + pCommandInfo->m_sParmsWithTypePointers_Out + (pCommandInfo->m_sParmsWithType_In.length() || pCommandInfo->m_sParmsWithTypePointers_Out.length() ? "," : "") + "string &sCMD_Result,Message *pMessage);\n\n").c_str(),file);
			}
		}

		const char *second_half = (const char *) buf;
		second_half += pos;
		fwrite((void *) second_half,sBuffer.length()-pos,1,file);
	}
	fclose(file);

	if( sMergeTo.length() )
	{
		// We need to merge the auto-generated section with the files in the sMergeTo directory
		// Read the file we wrote back into a string
		string sHandwrittenFilename = sMergeTo + FileUtils::FilenameWithoutPath(OutputFile);
		string::size_type pos=0;  // Use it temporarily for the file sizes, which we don't care about
		char *buffer = FileUtils::ReadFileIntoBuffer(OutputFile,pos);
		if( !buffer )
		{
			cerr << "Could not find " << OutputFile << ".  Aborting merge." << endl;
			return;
		}
		string sGeneratedFile( buffer );
			
		buffer = FileUtils::ReadFileIntoBuffer(sHandwrittenFilename,pos);
		if( !buffer )
		{
			cerr << "Could not find " << sHandwrittenFilename << ".  Copying instead." << endl;
			FILE *file = fopen(sHandwrittenFilename.c_str(),"wb");
			if( file )
			{
				if( fwrite(sGeneratedFile.c_str(),1,sGeneratedFile.length(),file)!=sGeneratedFile.length() )
					throw "Failed to write to file " + sHandwrittenFilename;
				fclose(file);
				return;
			}
		}
		string sHandwrittenFile( buffer );

		bool bModifications=false;

		pos=0;
		while( (pos=sGeneratedFile.find("<-dceag-",pos))!=string::npos )
		{
string foo = sGeneratedFile.substr(pos,999);
			string::size_type posTerminateBegin = sGeneratedFile.find("->",pos);
			if( posTerminateBegin==string::npos || sGeneratedFile[posTerminateBegin-1]!='b' )
			{
				cerr << "The file " << OutputFile << " has a corrupted <-dceag argument.  Aborting merge.";
				fclose(file);
				return;
			}

			string Token = sGeneratedFile.substr(pos,posTerminateBegin-pos-1);

			string::size_type posTerminateEnd,posEnd = sGeneratedFile.find("<-dceag-",posTerminateBegin);
			if( posEnd==string::npos || (posTerminateEnd=sGeneratedFile.find("->",posEnd))==string::npos || sGeneratedFile.substr(posEnd,posTerminateEnd-posEnd-1)!=Token )
			{
				cerr << "The file " << OutputFile << " has a corrupted token " << Token << ".  Aborting merge." << endl;
				fclose(file);
				return;
			}

			bModifications=true;
			string::size_type posEndBlock,posStartBlock = sHandwrittenFile.find(Token + "b->");
			// Found the block in the handwritten file.  If the token is in the format xxx-b->! the ! means don't replace
			if( posStartBlock==string::npos || sHandwrittenFile[posStartBlock + Token.length() + 3] !='!' )
			{
				if( posStartBlock==string::npos || (posEndBlock = sHandwrittenFile.find(Token + "e->",posStartBlock))==string::npos )
					sHandwrittenFile += "//" + sGeneratedFile.substr(pos,posTerminateEnd-pos) + "->\n";				// It's not found in the handwritten file.  Just append it.
				else
					sHandwrittenFile.replace(posStartBlock,posEndBlock-posStartBlock+Token.length()+1,sGeneratedFile.substr(pos,posTerminateEnd-pos));
			}
			pos = posTerminateEnd; // Move past this one
		}
		if( bModifications )
		{
			FILE *file = fopen(sHandwrittenFilename.c_str(),"wb");
			if( !file )
			{
				throw "Cannot open " + sHandwrittenFilename + " for writing.";
			}
			if( fwrite(sHandwrittenFile.c_str(),1,sHandwrittenFile.length(),file)!=sHandwrittenFile.length() )
				throw "Failed to write to file " + sHandwrittenFilename;
			fclose(file);
		}
	}
	return;
}

void DCEGen::CreateFunctionParms(Row_Command *pRow_Command,CommandInfo *pCommandInfo)
{
	vector<Row_Command_CommandParameter *> vectRow_Command_CommandParameter;
	pRow_Command->Command_CommandParameter_FK_Command_getrows(&vectRow_Command_CommandParameter);

	// First all the in's
	for(size_t i3=0;i3<vectRow_Command_CommandParameter.size();++i3)
	{
		Row_Command_CommandParameter *pRow_Command_CommandParameter = vectRow_Command_CommandParameter[i3];
		if( pRow_Command_CommandParameter->IsOut_get() )
		{
			// Call this just to get m_sParmsWithType_Out
			string foo;
			CreateFunctionParms(pRow_Command_CommandParameter->FK_CommandParameter_get(),
				pRow_Command_CommandParameter->FK_CommandParameter_getrow()->FK_ParameterType_get(),
				FileUtils::ValidCPPName(pRow_Command_CommandParameter->FK_CommandParameter_getrow()->Description_get()),
				pCommandInfo->m_sParmsWithType_Out,pCommandInfo->m_sAssignParmToLocal_Out,pCommandInfo->m_sParmsWithNoType_Out,foo);
			continue;
		}
		CreateFunctionParms(pRow_Command_CommandParameter->FK_CommandParameter_get(),
			pRow_Command_CommandParameter->FK_CommandParameter_getrow()->FK_ParameterType_get(),
			FileUtils::ValidCPPName(pRow_Command_CommandParameter->FK_CommandParameter_getrow()->Description_get()),
			pCommandInfo->m_sParmsWithType_In,pCommandInfo->m_sAssignParmToLocal_In,pCommandInfo->m_sParmsWithNoType_In,pCommandInfo->m_sPassingToMessage_In);
		pCommandInfo->m_vectRow_Command_CommandParameter_In.push_back(pRow_Command_CommandParameter);
	}

	// Now do all the outs
	for(size_t i3=0;i3<vectRow_Command_CommandParameter.size();++i3)
	{
		Row_Command_CommandParameter *pRow_Command_CommandParameter = vectRow_Command_CommandParameter[i3];
		if( !pRow_Command_CommandParameter->IsOut_get() )
			continue;
		CreateFunctionParms(pRow_Command_CommandParameter->FK_CommandParameter_get(),
			pRow_Command_CommandParameter->FK_CommandParameter_getrow()->FK_ParameterType_get(),
			FileUtils::ValidCPPName(pRow_Command_CommandParameter->FK_CommandParameter_getrow()->Description_get()),
			pCommandInfo->m_sParmsWithTypePointers_Out,pCommandInfo->m_sAssignLocalToParm,pCommandInfo->m_sParmsWithNoType_In,pCommandInfo->m_sPassingToMessage_Out,true);
		pCommandInfo->m_vectRow_Command_CommandParameter_Out.push_back(pRow_Command_CommandParameter);
	}
}
/*
void DCEGen::CreateFunctionParms(Row_Request *pRow_Request,string &sParmsWithType,string &sAssignParmToLocal,string &sParmsWithNoType,string &sPassingToMessage)
{
	vector<Row_Request_RequestParameter_In *> vectRow_Request_RequestParameter_In;
	pRow_Request->Request_RequestParameter_In_FK_Request_getrows(&vectRow_Request_RequestParameter_In);
	sPassingToMessage = StringUtils::itos((int) vectRow_Request_RequestParameter_In.size());
	for(size_t i3=0;i3<vectRow_Request_RequestParameter_In.size();++i3)
	{
		Row_Request_RequestParameter_In *pRow_Request_RequestParameter_In = vectRow_Request_RequestParameter_In[i3];
		CreateFunctionParms(pRow_Request_RequestParameter_In->FK_RequestParameter_get(),
			pRow_Request_RequestParameter_In->FK_RequestParameter_getrow()->FK_ParameterType_get(),
			FileUtils::ValidCPPName(pRow_Request_RequestParameter_In->FK_RequestParameter_getrow()->Description_get()),
			sParmsWithType,sAssignParmToLocal,sParmsWithNoType,sPassingToMessage);
	}

	string tmpAssignParmToLocal,tmpsParmsWithType;
	vector<Row_Request_RequestParameter_Out *> vectRow_Request_RequestParameter_Out;
	pRow_Request->Request_RequestParameter_Out_FK_Request_getrows(&vectRow_Request_RequestParameter_Out);
	sPassingToMessage = StringUtils::itos((int) vectRow_Request_RequestParameter_Out.size());
	for(size_t i3=0;i3<vectRow_Request_RequestParameter_Out.size();++i3)
	{
		Row_Request_RequestParameter_Out *pRow_Request_RequestParameter_Out = vectRow_Request_RequestParameter_Out[i3];
		CreateFunctionParms(pRow_Request_RequestParameter_Out->FK_RequestParameter_get(),
			pRow_Request_RequestParameter_Out->FK_RequestParameter_getrow()->FK_ParameterType_get(),
			FileUtils::ValidCPPName(pRow_Request_RequestParameter_Out->FK_RequestParameter_getrow()->Description_get()),
			sParmsWithType,sAssignParmToLocal,sParmsWithNoType,sPassingToMessage,true);  // Inverse since we're doing by reference
	}
}
*/
void DCEGen::CreateFunctionParms(Row_Event *pRow_Event,int &ParmCount,string &sParmsWithType,string &sAssignParmToLocal,string &sParmsWithNoType,string &sPassingToMessage)
{
	vector<Row_Event_EventParameter *> vectRow_Event_EventParameter;
	pRow_Event->Event_EventParameter_FK_Event_getrows(&vectRow_Event_EventParameter);
	ParmCount = (int) vectRow_Event_EventParameter.size();
	sPassingToMessage = StringUtils::itos(ParmCount);
	for(int i4=0;i4<ParmCount;++i4)
	{
		Row_Event_EventParameter *pRow_Event_EventParameter = vectRow_Event_EventParameter[i4];
		CreateFunctionParms(pRow_Event_EventParameter->FK_EventParameter_get(),
			pRow_Event_EventParameter->FK_EventParameter_getrow()->FK_ParameterType_get(),
			FileUtils::ValidCPPName(pRow_Event_EventParameter->FK_EventParameter_getrow()->Description_get()),
			sParmsWithType,sAssignParmToLocal,sParmsWithNoType,sPassingToMessage);
	}
}

void DCEGen::CreateFunctionParms(int iParameterID,int iParameterType,string sParameterName,string &sParmsWithType,string &sAssignParmToLocal,string &sParmsWithNoType,string &sPassingToMessage,bool bByReference)
{
	string SizePrefix = "i"; // The prefix for the size of parms, like int *iData_Size
	string Prefix = GetPrefix(iParameterType); 

	if (iParameterType == PARAMETERTYPE_Data_CONST)
	{
		sParmsWithType+= string(sParmsWithType.length()==0 ? "" : ",") + "char *" + (bByReference ? "*" : "") + Prefix + sParameterName + ",int " + (bByReference ? "*" : "") + SizePrefix + sParameterName + "_Size";
		if( bByReference )
		{
			sAssignParmToLocal+= "\t\t\t\t\t\tpMessageOut->m_mapData_Parameters["+StringUtils::itos(iParameterID)+"]=" + Prefix + sParameterName + "; ";
			sAssignParmToLocal+= "pMessageOut->m_mapData_Lengths["+StringUtils::itos(iParameterID)+"]=" +SizePrefix + sParameterName + "_Size;\n";
		}
		else
		{
			sAssignParmToLocal+= "\t\t\t\t\tchar *" + Prefix + sParameterName + "=pMessage->m_mapData_Parameters["+StringUtils::itos(iParameterID)+"];\n";
			sAssignParmToLocal+= string("\t\t\t\t\tint ") +SizePrefix + sParameterName + "_Size=pMessage->m_mapData_Lengths["+StringUtils::itos(iParameterID)+"];\n";
		}
		if( bByReference )
			sParmsWithNoType+= string(sParmsWithNoType.length()==0 ? "" : ",") + "&" + Prefix + sParameterName+",&"+SizePrefix+sParameterName+"_Size";
		else
			sParmsWithNoType+= string(sParmsWithNoType.length()==0 ? "" : ",") + Prefix + sParameterName+","+SizePrefix+sParameterName+"_Size";
		if( bByReference )
			sPassingToMessage+=",-" + StringUtils::itos(iParameterID)+",*"+ Prefix + sParameterName + ",*"+SizePrefix+sParameterName + "_Size";
		else
			sPassingToMessage+=",-" + StringUtils::itos(iParameterID)+","+ Prefix + sParameterName + ","+SizePrefix+sParameterName + "_Size";
	}
	else if (iParameterType == PARAMETERTYPE_int_CONST)
	{
		sParmsWithType+= string(sParmsWithType.length()==0 ? "" : ",") + "int " + (bByReference ? "*" : "") + Prefix + sParameterName;
		if( bByReference )
			sAssignParmToLocal+= "\t\t\t\t\t\tpMessageOut->m_mapParameters["+StringUtils::itos(iParameterID)+"]=StringUtils::itos(" + Prefix + sParameterName + ");\n";
		else
			sAssignParmToLocal+= "\t\t\t\t\tint " + Prefix + sParameterName + "=atoi(pMessage->m_mapParameters["+StringUtils::itos(iParameterID)+"].c_str());\n";
		if( bByReference )
			sParmsWithNoType+= string(sParmsWithNoType.length()==0 ? "" : ",") + "&" + Prefix + sParameterName;
		else
			sParmsWithNoType+= string(sParmsWithNoType.length()==0 ? "" : ",") + Prefix + sParameterName;
		if( bByReference )
			sPassingToMessage+="," + StringUtils::itos(iParameterID)+",StringUtils::itos(*"+ Prefix + sParameterName + ").c_str()";
		else
			sPassingToMessage+="," + StringUtils::itos(iParameterID)+",StringUtils::itos("+ Prefix + sParameterName + ").c_str()";
	}
	else if (iParameterType == PARAMETERTYPE_bool_CONST)
	{
		sParmsWithType+= string(sParmsWithType.length()==0 ? "" : ",") + "bool " + (bByReference ? "*" : "") + Prefix + sParameterName;
		if( bByReference )
			sAssignParmToLocal+= "\t\t\t\t\t\tpMessageOut->m_mapParameters["+StringUtils::itos(iParameterID)+"]=(" + Prefix + sParameterName + " ? \"1\" : \"0\");\n";
		else
			sAssignParmToLocal+= "\t\t\t\t\tbool " + Prefix + sParameterName + "=(pMessage->m_mapParameters["+StringUtils::itos(iParameterID)+"]==\"1\" ? true : false);\n";
		if( bByReference )
			sParmsWithNoType+= string(sParmsWithNoType.length()==0 ? "" : ",") + "&" + Prefix + sParameterName;
		else
			sParmsWithNoType+= string(sParmsWithNoType.length()==0 ? "" : ",") + Prefix + sParameterName;
		if( bByReference )
			sPassingToMessage+="," + StringUtils::itos(iParameterID)+",(*"+ Prefix + sParameterName + " ? \"1\" : \"0\")";
		else
			sPassingToMessage+="," + StringUtils::itos(iParameterID)+",("+ Prefix + sParameterName + " ? \"1\" : \"0\")";
	}
	else if (iParameterType == PARAMETERTYPE_PlutoColor_CONST)
	{
		sParmsWithType+= string(sParmsWithType.length()==0 ? "" : ",") + "PlutoColor " + (bByReference ? "*" : "") + Prefix + sParameterName;
		if( bByReference )
			sAssignParmToLocal+= "\t\t\t\t\t\tpMessageOut->m_mapParameters["+StringUtils::itos(iParameterID)+"]=StringUtils::itos("+Prefix + sParameterName +");\n";
		else
			sAssignParmToLocal+="\t\t\t\t\tpMessage->m_mapParameters["+StringUtils::itos(iParameterID)+"]=StringUtils::itos("+Prefix + sParameterName+");\n";
		if( bByReference )
			sParmsWithNoType+= string(sParmsWithNoType.length()==0 ? "" : ",") + "&" + Prefix + sParameterName;
		else
			sParmsWithNoType+= string(sParmsWithNoType.length()==0 ? "" : ",") + Prefix + sParameterName;
		if( bByReference )
			sPassingToMessage+="," + StringUtils::itos(iParameterID)+",StringUtils::itos(*"+ Prefix + sParameterName+".m_value).c_str()";
		else
			sPassingToMessage+="," + StringUtils::itos(iParameterID)+",StringUtils::itos("+ Prefix + sParameterName+".m_value).c_str()";
	}
	else 
	{
		sParmsWithType+= string(sParmsWithType.length()==0 ? "" : ",") + "string " + (bByReference ? "*" : "") + Prefix + sParameterName;
		if( bByReference )
			sAssignParmToLocal+= "\t\t\t\t\t\tpMessageOut->m_mapParameters["+StringUtils::itos(iParameterID)+"]=" + Prefix + sParameterName + ";\n";
		else
			sAssignParmToLocal+= "\t\t\t\t\tstring " + Prefix + sParameterName + "=pMessage->m_mapParameters["+StringUtils::itos(iParameterID)+"];\n";
		if( bByReference )
			sParmsWithNoType+= string(sParmsWithNoType.length()==0 ? "" : ",") + "&" + Prefix + sParameterName ;
		else
			sParmsWithNoType+= string(sParmsWithNoType.length()==0 ? "" : ",") + Prefix + sParameterName + ".c_str()";
		if( bByReference )
			sPassingToMessage+="," + StringUtils::itos(iParameterID)+",(*"+ Prefix + sParameterName+").c_str()";
		else
			sPassingToMessage+="," + StringUtils::itos(iParameterID)+","+ Prefix + sParameterName+".c_str()";
	}
}


void DCEGen::WriteGlobals()
{
	fstream fstr_DeviceCommand;
	fstr_DeviceCommand.open((m_sGeneratedOutput + "AllCommandsRequests.h").c_str(),fstream::out);
	fstr_DeviceCommand << "#ifndef AllCommandsRequest_h" << endl;
	fstr_DeviceCommand << "#define AllCommandsRequest_h" << endl;
	fstr_DeviceCommand << "#include \"Message.h\"" << endl;
	fstr_DeviceCommand << "#include \"PreformedCommand.h\"" << endl;
	fstr_DeviceCommand << "namespace DCE" << endl;
	fstr_DeviceCommand << "{" << endl;
	fstr_DeviceCommand << "\t //  Commands" << endl;

	vector<Row_Command *> vectRow_Command;
	m_dce.Command_get()->GetRows("1=1",&vectRow_Command);
	for(size_t s=0;s<vectRow_Command.size();++s)
	{
		Row_Command *pRow_Command = vectRow_Command[s];

		// Get the parameters
		CommandInfo commandInfo;
		commandInfo.m_pRow_Command=pRow_Command;
		CreateFunctionParms(pRow_Command,&commandInfo);
		string Name = commandInfo.CPPName();

		if( commandInfo.m_vectRow_Command_CommandParameter_Out.size() )
		{
			fstr_DeviceCommand << "\tclass RESP_" << Name << " : public PreformedCommandResponse {" << endl;
			// these come in as int *iFoo,string *sFoo.  Change to int *m_iFoo;string *m_sFoo;
			string decls=commandInfo.m_sParmsWithTypePointers_Out;
			StringUtils::Replace(decls," *"," *m_");
			StringUtils::Replace(decls,",",";");
			StringUtils::Replace(decls,"*m_*","**m_");  // for those char **
			fstr_DeviceCommand << "\t\t" << decls << ";" << endl;
			fstr_DeviceCommand << "\tpublic:" << endl;
			fstr_DeviceCommand << "\t\tRESP_" << Name << "(" << commandInfo.m_sParmsWithTypePointers_Out << ") { " << endl;

			// take the parms with no type, get rid of the c_str(), and assign everything to local variables
			string list = commandInfo.m_sParmsWithNoType_Out; string::size_type pos=0; string variable;
			StringUtils::Replace(list,".c_str()","");
			fstr_DeviceCommand << "\t\t";
			while( (variable=StringUtils::Tokenize(list,",",pos)).length()>0 )
				fstr_DeviceCommand << "m_" << variable << "=" << variable << "; ";

			fstr_DeviceCommand << "}" << endl;
			fstr_DeviceCommand << "\t\tvoid ParseResponse(Message *pMessage) {" << endl;

			// each of these is in the format \t\t\t\tint iX = pMessage->  convert to *iX = pMessage
			list = commandInfo.m_sAssignParmToLocal_Out;  pos=0;
			StringUtils::Replace(list,"\t","");
			fstr_DeviceCommand << "\t\t\t";
			while( (variable=StringUtils::Tokenize(list,"\n",pos)).length()>0 )
			{
				string::size_type space = variable.find(' ');
				if( variable[space+1]=='*' )
					space++;	// Skip over for passed in data values
				fstr_DeviceCommand << "*m_" << variable.substr(space+1) << " ";
			}
			fstr_DeviceCommand << "};\n\t};" << endl;
		}

		// Create a constructor with long DeviceIDFrom, long DeviceIDTo
		fstr_DeviceCommand << "\tclass CMD_" + Name + " : public PreformedCommand {" << endl;
		fstr_DeviceCommand << "\tpublic:" << endl;
		fstr_DeviceCommand << "\t\tCMD_" + Name + "(long DeviceIDFrom, long DeviceIDTo";
		if( commandInfo.m_sParmsWithType_In.length()>0 )
			fstr_DeviceCommand << "," << commandInfo.m_sParmsWithType_In;

		if( commandInfo.m_sParmsWithTypePointers_Out.length()>0 )
			fstr_DeviceCommand << "," << commandInfo.m_sParmsWithTypePointers_Out;

if( pRow_Command->PK_Command_get()==COMMAND_Request_Datagrid_Contents_CONST )
{
int k=2;
}
		fstr_DeviceCommand << ") {";
		fstr_DeviceCommand << " m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL," << pRow_Command->PK_Command_get() << "," << (int) (commandInfo.m_vectRow_Command_CommandParameter_In.size() + commandInfo.m_vectRow_Command_CommandParameter_Out.size()) << commandInfo.m_sPassingToMessage_In << commandInfo.m_sPassingToMessage_Out << ");";
		if( commandInfo.m_vectRow_Command_CommandParameter_Out.size() )
		{
			fstr_DeviceCommand << "\t\tm_pcResponse = new RESP_" << Name << "(" << StringUtils::Replace(commandInfo.m_sParmsWithNoType_Out,".c_str()","") << ");";
		}
		fstr_DeviceCommand << " }" << endl;
		fstr_DeviceCommand << "\t};\n"; // end class

		// Create a constructor with long DeviceIDFrom, string DeviceIDTo
		fstr_DeviceCommand << "\tclass CMD_" + Name + "_DL : public PreformedCommand {" << endl;
		fstr_DeviceCommand << "\tpublic:" << endl;
		fstr_DeviceCommand << "\t\tCMD_" + Name + "_DL(long DeviceIDFrom, string DeviceIDTo";
		if( commandInfo.m_sParmsWithType_In.length()>0 )
			fstr_DeviceCommand << "," << commandInfo.m_sParmsWithType_In;

		if( commandInfo.m_sParmsWithTypePointers_Out.length()>0 )
			fstr_DeviceCommand << "," << commandInfo.m_sParmsWithTypePointers_Out;

		fstr_DeviceCommand << ") {";
		fstr_DeviceCommand << " m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL," << pRow_Command->PK_Command_get() << "," << (int) (commandInfo.m_vectRow_Command_CommandParameter_In.size() + commandInfo.m_vectRow_Command_CommandParameter_Out.size()) << commandInfo.m_sPassingToMessage_In << commandInfo.m_sPassingToMessage_Out << ");";
		if( commandInfo.m_vectRow_Command_CommandParameter_Out.size() )
		{
			fstr_DeviceCommand << "\t\tm_pcResponse = new RESP_" << Name << "(" << StringUtils::Replace(commandInfo.m_sParmsWithNoType_Out,".c_str()","") << ");";
		}
		fstr_DeviceCommand << " }" << endl;
		fstr_DeviceCommand << "\t};\n"; // end class

		// Create a constructor with long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB
		fstr_DeviceCommand << "\tclass CMD_" + Name + "_DT : public PreformedCommand {" << endl;
		fstr_DeviceCommand << "\tpublic:" << endl;
		fstr_DeviceCommand << "\t\tCMD_" + Name + "_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB";
		if( commandInfo.m_sParmsWithType_In.length()>0 )
			fstr_DeviceCommand << "," << commandInfo.m_sParmsWithType_In;

		if( commandInfo.m_sParmsWithTypePointers_Out.length()>0 )
			fstr_DeviceCommand << "," << commandInfo.m_sParmsWithTypePointers_Out;

		fstr_DeviceCommand << ") {";
		fstr_DeviceCommand << " m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL," << pRow_Command->PK_Command_get() << "," << (int) (commandInfo.m_vectRow_Command_CommandParameter_In.size() + commandInfo.m_vectRow_Command_CommandParameter_Out.size()) << commandInfo.m_sPassingToMessage_In << commandInfo.m_sPassingToMessage_Out << ");";
		if( commandInfo.m_vectRow_Command_CommandParameter_Out.size() )
		{
			fstr_DeviceCommand << "\t\tm_pcResponse = new RESP_" << Name << "(" << StringUtils::Replace(commandInfo.m_sParmsWithNoType_Out,".c_str()","") << ");";
		}
		fstr_DeviceCommand << " }" << endl;
		fstr_DeviceCommand << "\t};\n"; // end class

		// 	Create a constructor with long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB
		fstr_DeviceCommand << "\tclass CMD_" + Name + "_Cat : public PreformedCommand {" << endl;
		fstr_DeviceCommand << "\tpublic:" << endl;
		fstr_DeviceCommand << "\t\tCMD_" + Name + "_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB";
		if( commandInfo.m_sParmsWithType_In.length()>0 )
			fstr_DeviceCommand << "," << commandInfo.m_sParmsWithType_In;

		if( commandInfo.m_sParmsWithTypePointers_Out.length()>0 )
			fstr_DeviceCommand << "," << commandInfo.m_sParmsWithTypePointers_Out;

		fstr_DeviceCommand << ") {";
		fstr_DeviceCommand << " m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL," << pRow_Command->PK_Command_get() << "," << (int) (commandInfo.m_vectRow_Command_CommandParameter_In.size() + commandInfo.m_vectRow_Command_CommandParameter_Out.size()) << commandInfo.m_sPassingToMessage_In << commandInfo.m_sPassingToMessage_Out << ");";
		if( commandInfo.m_vectRow_Command_CommandParameter_Out.size() )
		{
			fstr_DeviceCommand << "\t\tm_pcResponse = new RESP_" << Name << "(" << StringUtils::Replace(commandInfo.m_sParmsWithNoType_Out,".c_str()","") << ");";
		}
		fstr_DeviceCommand << " }" << endl;
		fstr_DeviceCommand << "\t};\n"; // end class

		// If there are out parameters, create commands that don't use them.  When attaching messages to cells, for example, we will never care about the return value
		if( commandInfo.m_vectRow_Command_CommandParameter_Out.size() )
		{
			// Create a constructor with long DeviceIDFrom, long DeviceIDTo
			fstr_DeviceCommand << "\tclass CMD_NOREP_" + Name + " : public PreformedCommand {" << endl;
			fstr_DeviceCommand << "\tpublic:" << endl;
			fstr_DeviceCommand << "\t\tCMD_NOREP_" + Name + "(long DeviceIDFrom, long DeviceIDTo";
			if( commandInfo.m_sParmsWithType_In.length()>0 )
				fstr_DeviceCommand << "," << commandInfo.m_sParmsWithType_In;

			fstr_DeviceCommand << ") {";
			fstr_DeviceCommand << " m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL," << pRow_Command->PK_Command_get() << "," << (int) commandInfo.m_vectRow_Command_CommandParameter_In.size() << commandInfo.m_sPassingToMessage_In << ");";
			fstr_DeviceCommand << " }" << endl;
			fstr_DeviceCommand << "\t};\n"; // end class

			// Create a constructor with long DeviceIDFrom, string DeviceIDTo
			fstr_DeviceCommand << "\tclass CMD_NOREP_" + Name + "_DL : public PreformedCommand {" << endl;
			fstr_DeviceCommand << "\tpublic:" << endl;
			fstr_DeviceCommand << "\t\tCMD_NOREP_" + Name + "_DL(long DeviceIDFrom, string DeviceIDTo";
			if( commandInfo.m_sParmsWithType_In.length()>0 )
				fstr_DeviceCommand << "," << commandInfo.m_sParmsWithType_In;
			fstr_DeviceCommand << ") {";
			fstr_DeviceCommand << " m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL," << pRow_Command->PK_Command_get() << "," << (int) commandInfo.m_vectRow_Command_CommandParameter_In.size() << commandInfo.m_sPassingToMessage_In << ");";
			fstr_DeviceCommand << " }" << endl;
			fstr_DeviceCommand << "\t};\n"; // end class

			// Create a constructor with long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB
			fstr_DeviceCommand << "\tclass CMD_NOREP_" + Name + "_DT : public PreformedCommand {" << endl;
			fstr_DeviceCommand << "\tpublic:" << endl;
			fstr_DeviceCommand << "\t\tCMD_NOREP_" + Name + "_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB";
			if( commandInfo.m_sParmsWithType_In.length()>0 )
				fstr_DeviceCommand << "," << commandInfo.m_sParmsWithType_In;

			fstr_DeviceCommand << ") {";
			fstr_DeviceCommand << " m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL," << pRow_Command->PK_Command_get() << "," << (int) commandInfo.m_vectRow_Command_CommandParameter_In.size() << commandInfo.m_sPassingToMessage_In << ");";
			fstr_DeviceCommand << " }" << endl;
			fstr_DeviceCommand << "\t};\n"; // end class

			// 	Create a constructor with long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB
			fstr_DeviceCommand << "\tclass CMD_NOREP_" + Name + "_Cat : public PreformedCommand {" << endl;
			fstr_DeviceCommand << "\tpublic:" << endl;
			fstr_DeviceCommand << "\t\tCMD_NOREP_" + Name + "_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB";
			if( commandInfo.m_sParmsWithType_In.length()>0 )
				fstr_DeviceCommand << "," << commandInfo.m_sParmsWithType_In;

			fstr_DeviceCommand << ") {";
			fstr_DeviceCommand << " m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL," << pRow_Command->PK_Command_get() << "," << (int) commandInfo.m_vectRow_Command_CommandParameter_In.size() << commandInfo.m_sPassingToMessage_In << ");";
			fstr_DeviceCommand << " }" << endl;
			fstr_DeviceCommand << "\t};\n"; // end class
		}
	}

	fstr_DeviceCommand << "}" << endl;
	fstr_DeviceCommand << "#endif" << endl;
}


string DCEGen::GetPrefix(int PK_ParameterType)
{
	if( PK_ParameterType == PARAMETERTYPE_Data_CONST )
		return "p";
	if( PK_ParameterType == PARAMETERTYPE_int_CONST )
		return "i";
	if( PK_ParameterType == PARAMETERTYPE_bool_CONST )
		return "b";
	if( PK_ParameterType == PARAMETERTYPE_PlutoColor_CONST )
		return "color";
	return "s";

}
string DCEGen::CastTypeToChar(string s,int PK_ParameterType)
{
	switch(PK_ParameterType)
	{
	case PARAMETERTYPE_string_CONST:
		return s + ".c_str()";
		break;
	case PARAMETERTYPE_int_CONST:
		return "StringUtils::itos(" + s + ").c_str()";
		break;
	case PARAMETERTYPE_bool_CONST:
		return "(" + s + " ? : \"1\" : \"0\")";
		break;
	case PARAMETERTYPE_Special_CONST:
		return s + ".c_str();";
		break;
	case PARAMETERTYPE_Data_CONST:
		return s + ".cantbedone_willcauseerror()";
		break;
	case PARAMETERTYPE_PlutoColor_CONST:
		return "StringUtils::itos(" + s + ").c_str()";
		break;
	case PARAMETERTYPE_PlutoDate_CONST:
		return "StringUtils::itos(" + s + ").c_str()";
		break;
	};
	return s;
}

string DCEGen::CastStringToType(string s,int PK_ParameterType)
{
	switch(PK_ParameterType)
	{
	case PARAMETERTYPE_string_CONST:
		return s;
		break;
	case PARAMETERTYPE_int_CONST:
		return "atoi(" + s + ".c_str())";
		break;
	case PARAMETERTYPE_bool_CONST:
		return "(" + s + "==\"1\" ? true : false)";
		break;
	case PARAMETERTYPE_Special_CONST:
		return s;
		break;
	case PARAMETERTYPE_Data_CONST:
		return "cantbedone()";
		break;
	case PARAMETERTYPE_PlutoColor_CONST:
		return "MyColor(atoi(" + s + ".c_str()))";
		break;
	case PARAMETERTYPE_PlutoDate_CONST:
		return "MyDate(atoi(" + s + ".c_str()))";
		break;
	};
	return s;
}

