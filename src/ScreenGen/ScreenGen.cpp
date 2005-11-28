#include <iostream>
#include <fstream>

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Screen.h"
#include "pluto_main/Table_Screen_CommandParameter.h"

//uncomment next line to see if AllScreens.h is compilable
//#include "Gen_Devices/AllScreens.h"

#define  VERSION "<=version=>"
//-----------------------------------------------------------------------------------------------------
namespace DCE
{
	class Logger *g_pPlutoLogger;
}
//-----------------------------------------------------------------------------------------------------
static string g_sAppPath;
static const string csTemplateFile = "AllScreensTemplate.h";
//-----------------------------------------------------------------------------------------------------
class ScreenGenerator
{
	Database_pluto_main *m_pDatabase_pluto_main;
	string m_sOutputPath;

	string GenerateClasses();
	string GenerateMethods();
	string GenerateSwitchBlock();

	string GetClassOrMethod(Row_Screen *pRow_Screen);
	string GetParamName(Row_Screen_CommandParameter *pCommandParameter);
	string GetParamPair(Row_Screen_CommandParameter *pCommandParameter);

public:
	ScreenGenerator(Database_pluto_main *pDatabase_pluto_main, string sOutputPath);
	bool GenerateScreensFile();
};
//-----------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	g_sAppPath = FileUtils::BasePath(argv[0]); 
	g_pPlutoLogger=new FileLogger(stdout);

	string DBHost="dcerouter",DBUser="root",DBPassword="",DBName="pluto_main";	
	int DBPort=3306;
	string OutputPath = "Gen_Devices";

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
		case 'o':
			OutputPath = argv[++optnum];
			break;
		default:
			bError=true;
			break;
		};
	}

	if (bError)
	{
		cout << "ScreenGen, v." << VERSION << endl
			<< "Usage: ScreenGen [-o output path] [-h hostname] [-u username] [-p password] [-D database] [-P mysql port]" << endl
			<< "-o output    -- path to put output file" << endl
			<< "-h hostname  -- address or DNS of database host, default is `localhost`" << endl
			<< "-u username  -- username for database connection" << endl
			<< "-p password  -- password for database connection, default is `` (empty)" << endl
			<< "-D database  -- database name" << endl
			<< "-P port      -- port for database connection, default is 3306" << endl;
		exit(0);
	}

	Database_pluto_main mds; 
	if(!mds.Connect(DBHost, DBUser, DBPassword, DBName, DBPort))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed to connect to database (host %s, user %s, password %s, dbname %s, "
			"port %d", DBHost.c_str(), DBUser.c_str(), DBPassword.c_str(), DBName.c_str(), DBPort);
		return 1;
	}

	auto_ptr<ScreenGenerator> psScreenGenerator(new ScreenGenerator(&mds, OutputPath));
	if(!psScreenGenerator->GenerateScreensFile())
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed to generate 'AllScreens.h' file. Aborting...");
		return 2;
	}

	return 0;
}
//-----------------------------------------------------------------------------------------------------
ScreenGenerator::ScreenGenerator(Database_pluto_main *pDatabase_pluto_main, string sOutputPath) 
{
	m_pDatabase_pluto_main = pDatabase_pluto_main;
	m_sOutputPath = sOutputPath;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenGenerator::GenerateScreensFile() 
{
	string sTemplateFile = csTemplateFile;
	string sGeneratedData;

	if(!FileUtils::FileExists(sTemplateFile))
	{
		sTemplateFile = FileUtils::BasePath(FileUtils::IncludeTrailingSlash(g_sAppPath)) + "ScreenGen/" + sTemplateFile;

		if(!FileUtils::FileExists(sTemplateFile))
		{
			sTemplateFile = "../" + sTemplateFile;

			if(!FileUtils::FileExists(sTemplateFile))
			{
				g_pPlutoLogger->Write(LV_CRITICAL, "Unable to find template file '%s'", csTemplateFile.c_str());
				return false;
			}
		}
	}

	if(FileUtils::ReadTextFile(sTemplateFile, sGeneratedData))
	{
		string sClasses = GenerateClasses();
		string sMethods = GenerateMethods();
		string sSwitchBlock = GenerateSwitchBlock();

		sGeneratedData = StringUtils::Replace(sGeneratedData, "$GEN_CLASSES$", sClasses);
		sGeneratedData = StringUtils::Replace(sGeneratedData, "$GEN_METHODS$", sMethods);
		sGeneratedData = StringUtils::Replace(sGeneratedData, "$GEN_SWITCH_BLOCK$", sSwitchBlock);
	}

	if(!FileUtils::DirExists(m_sOutputPath))
	{
		m_sOutputPath = FileUtils::IncludeTrailingSlash(FileUtils::BasePath(g_sAppPath)) + "Gen_Devices";
		if(!FileUtils::DirExists(m_sOutputPath))
		{
			m_sOutputPath = "../Gen_Devices";
			if(!FileUtils::DirExists(m_sOutputPath))
			{
				g_pPlutoLogger->Write(LV_CRITICAL, "Unable to save generated file. Cannot find 'Gen_Devices' folder");    
				return false;
			}
		}
	}

	string sOutputFile = FileUtils::IncludeTrailingSlash(m_sOutputPath) + "AllScreens.h";
	if(!FileUtils::WriteTextFile(sOutputFile, sGeneratedData))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot write file '%s'", sOutputFile.c_str());    
		return false;
	}
    
	return true;
}
//-----------------------------------------------------------------------------------------------------
string ScreenGenerator::GenerateClasses()
{
	g_pPlutoLogger->Write(LV_WARNING, "Generating classes...");

	string sGeneratedClassesData;
	vector<Row_Screen *> vectRow_Screen;
	m_pDatabase_pluto_main->Screen_get()->GetRows("1=1", &vectRow_Screen);

	for(vector<Row_Screen *>::iterator it = vectRow_Screen.begin(); it != vectRow_Screen.end(); it++)
	{
		string sClass;
		Row_Screen *pRow_Screen = *it;
		string sClassName = GetClassOrMethod(pRow_Screen);

		g_pPlutoLogger->Write(LV_STATUS, "Generating class %s...", sClassName.c_str());

		string sConstructorParams;
		string sParams;

		long nMessageIndex = 1;
		vector<Row_Screen_CommandParameter *> vectRow_Screen_CommandParameter;
		m_pDatabase_pluto_main->Screen_CommandParameter_get()->GetRows("FK_Screen = " + 
			StringUtils::ltos(pRow_Screen->PK_Screen_get()), &vectRow_Screen_CommandParameter);

		for(vector<Row_Screen_CommandParameter *>::iterator itp = vectRow_Screen_CommandParameter.begin(); 
			itp != vectRow_Screen_CommandParameter.end(); itp++)
		{
			Row_Screen_CommandParameter *pCommandParameter = *itp;
			sConstructorParams += "string " + GetParamName(pCommandParameter);
			sParams += GetParamPair(pCommandParameter);

			if(nMessageIndex < long(vectRow_Screen_CommandParameter.size()))
			{
				sConstructorParams += ", ";
				sParams += ", ";
			}
			nMessageIndex++;
		}

		string sPK_Screen = StringUtils::ltos(pRow_Screen->PK_Screen_get());

		//normal command
		string sNormalClassData = 
			"\tclass " + sClassName + " : public PreformedCommand\r\n"
			"\t{\r\n"
			"\tpublic:\r\n"
			"\t\t" + sClassName + "(long DeviceIDFrom, long DeviceIDTo" + (vectRow_Screen_CommandParameter.size() ? ",\r\n" : "") +
			(vectRow_Screen_CommandParameter.size() ? "\t\t\t" + sConstructorParams + ")" : ")") + "\r\n"
			"\t\t{\r\n"
			"\t\t\tm_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, " + 
				StringUtils::ltos(long(vectRow_Screen_CommandParameter.size()) + 1) + ", \r\n" 
				"\t\t\t\tCOMMANDPARAMETER_EK_Screen_CONST, \"" + sPK_Screen + "\" /* screen ID */" + 
			(vectRow_Screen_CommandParameter.size() ? ",\r\n\t\t\t\t" + sParams : "") + ");\r\n"
			"\t\t}\r\n"
			"\t};\r\n"
			"\r\n";

		//_DL command
		string sDLClassData = 
			"\tclass " + sClassName + "_DL : public PreformedCommand\r\n"
			"\t{\r\n"
			"\tpublic:\r\n"
			"\t\t" + sClassName + "_DL(long DeviceIDFrom, string sDeviceIDTo" + (vectRow_Screen_CommandParameter.size() ? ",\r\n" : "") +
			(vectRow_Screen_CommandParameter.size() ? "\t\t\t" + sConstructorParams + ")" : ")") + "\r\n"
			"\t\t{\r\n"
			"\t\t\tm_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, " + 
			StringUtils::ltos(long(vectRow_Screen_CommandParameter.size()) + 1) + ", \r\n" 
			"\t\t\t\tCOMMANDPARAMETER_EK_Screen_CONST, \"" + sPK_Screen + "\" /* screen ID */" + 
			(vectRow_Screen_CommandParameter.size() ? ",\r\n\t\t\t\t" + sParams : "") + ");\r\n"
			"\t\t}\r\n"
			"\t};\r\n"
			"\r\n";

		//_DT command
		string sDTClassData = 
			"\tclass " + sClassName + "_DT : public PreformedCommand\r\n"
			"\t{\r\n"
			"\tpublic:\r\n"
			"\t\t" + sClassName + "_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB" + (vectRow_Screen_CommandParameter.size() ? ",\r\n" : "") +
			(vectRow_Screen_CommandParameter.size() ? "\t\t\t" + sConstructorParams + ")" : ")") + "\r\n"
			"\t\t{\r\n"
			"\t\t\tm_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, " + 
			StringUtils::ltos(long(vectRow_Screen_CommandParameter.size()) + 1) + ", \r\n" 
			"\t\t\t\tCOMMANDPARAMETER_EK_Screen_CONST, \"" + sPK_Screen + "\" /* screen ID */" + 
			(vectRow_Screen_CommandParameter.size() ? ",\r\n\t\t\t\t" + sParams : "") + ");\r\n"
			"\t\t}\r\n"
			"\t};\r\n"
			"\r\n";

		//_Cat command
		string sCatClassData = 
			"\tclass " + sClassName + "_Cat : public PreformedCommand\r\n"
			"\t{\r\n"
			"\tpublic:\r\n"
			"\t\t" + sClassName + "_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB" + (vectRow_Screen_CommandParameter.size() ? ",\r\n" : "") +
			(vectRow_Screen_CommandParameter.size() ? "\t\t\t" + sConstructorParams + ")" : ")") + "\r\n"
			"\t\t{\r\n"
			"\t\t\tm_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, " + 
			StringUtils::ltos(long(vectRow_Screen_CommandParameter.size()) + 1) + ", \r\n" 
			"\t\t\t\tCOMMANDPARAMETER_EK_Screen_CONST, \"" + sPK_Screen + "\" /* screen ID */" + 
			(vectRow_Screen_CommandParameter.size() ? ",\r\n\t\t\t\t" + sParams : "") + ");\r\n"
			"\t\t}\r\n"
			"\t};\r\n"
			"\r\n";

		sGeneratedClassesData += sNormalClassData;
		sGeneratedClassesData += sDLClassData;
		sGeneratedClassesData += sDTClassData;
		sGeneratedClassesData += sCatClassData;
	}

	return sGeneratedClassesData;
}
//-----------------------------------------------------------------------------------------------------
string ScreenGenerator::GenerateMethods()
{
	g_pPlutoLogger->Write(LV_WARNING, "Generating methods...");

	string sGeneratedMethodsData;

	vector<Row_Screen *> vectRow_Screen;
	m_pDatabase_pluto_main->Screen_get()->GetRows("1=1", &vectRow_Screen);

	for(vector<Row_Screen *>::iterator it = vectRow_Screen.begin(); it != vectRow_Screen.end(); it++)
	{
		string sClass;
		Row_Screen *pRow_Screen = *it;
		string sMethodName = GetClassOrMethod(pRow_Screen);
		string sPK_Screen = StringUtils::ltos(pRow_Screen->PK_Screen_get());

		g_pPlutoLogger->Write(LV_STATUS, "Generating method %s...", sMethodName.c_str());

		string sParams;

		long nMessageIndex = 1;
		vector<Row_Screen_CommandParameter *> vectRow_Screen_CommandParameter;
		m_pDatabase_pluto_main->Screen_CommandParameter_get()->GetRows("FK_Screen = " + 
			StringUtils::ltos(pRow_Screen->PK_Screen_get()), &vectRow_Screen_CommandParameter);

		sParams = string("long PK_Screen") + (vectRow_Screen_CommandParameter.size() ? ", " : ""); 
		for(vector<Row_Screen_CommandParameter *>::iterator itp = vectRow_Screen_CommandParameter.begin(); 
			itp != vectRow_Screen_CommandParameter.end(); itp++)
		{
			Row_Screen_CommandParameter *pCommandParameter = *itp;

			sParams += "string " + GetParamName(pCommandParameter);
			if(nMessageIndex < long(vectRow_Screen_CommandParameter.size()))
				sParams += ", ";

			nMessageIndex++;
		}

		string sMethodData = "\t\tvirtual void " + sMethodName + "(" + sParams + ")"
			"{ GotoScreen(PK_Screen); }\r\n";

		sGeneratedMethodsData += sMethodData;
	}

	return sGeneratedMethodsData;
}
//-----------------------------------------------------------------------------------------------------
string ScreenGenerator::GenerateSwitchBlock()
{
	g_pPlutoLogger->Write(LV_WARNING, "Generating cases for switch block...");

	string sGeneratedCasesData;

	vector<Row_Screen *> vectRow_Screen;
	m_pDatabase_pluto_main->Screen_get()->GetRows("1=1", &vectRow_Screen);

	for(vector<Row_Screen *>::iterator it = vectRow_Screen.begin(); it != vectRow_Screen.end(); it++)
	{
		string sClass;
		Row_Screen *pRow_Screen = *it;
		string sMethodName = GetClassOrMethod(pRow_Screen);
		string sPK_Screen = StringUtils::ltos(pRow_Screen->PK_Screen_get());

		g_pPlutoLogger->Write(LV_STATUS, "Generating case for %s...", sPK_Screen.c_str());

		string sParams;
		string sMessageParamsLines;

		long nMessageIndex = 1;
		vector<Row_Screen_CommandParameter *> vectRow_Screen_CommandParameter;
		m_pDatabase_pluto_main->Screen_CommandParameter_get()->GetRows("FK_Screen = " + 
			StringUtils::ltos(pRow_Screen->PK_Screen_get()), &vectRow_Screen_CommandParameter);

		sParams = string("nPK_Screen") + (vectRow_Screen_CommandParameter.size() ? ", " : ""); 
		for(vector<Row_Screen_CommandParameter *>::iterator itp = vectRow_Screen_CommandParameter.begin(); 
			itp != vectRow_Screen_CommandParameter.end(); itp++)
		{
			Row_Screen_CommandParameter *pCommandParameter = *itp;

			sParams += GetParamName(pCommandParameter);
			sMessageParamsLines += "\t\t\t\t\tstring " + GetParamName(pCommandParameter) + 
				" = pMessage->m_mapParameters[" +
				StringUtils::ltos(pCommandParameter->FK_CommandParameter_get()) + "];\r\n";
			if(nMessageIndex < long(vectRow_Screen_CommandParameter.size()))
				sParams += ", ";

			nMessageIndex++;
		}

		string sCaseData = 
			"\t\t\t\tcase " + sPK_Screen + ":\r\n" + 
			"\t\t\t\t{\r\n" +
			sMessageParamsLines + 
			"\t\t\t\t\t" + sMethodName + "(" + sParams + ");\r\n"
			"\t\t\t\t\tbreak;\r\n"
			"\t\t\t\t}\r\n";

		sGeneratedCasesData += sCaseData;
	}

	return sGeneratedCasesData;
}
//-----------------------------------------------------------------------------------------------------
string ScreenGenerator::GetClassOrMethod(Row_Screen *pRow_Screen)
{
	string sClassName = "SCREEN_" + StringUtils::Replace(pRow_Screen->Description_get(), " ", "_");
	sClassName = StringUtils::Replace(sClassName, "-", "");
	sClassName = StringUtils::Replace(sClassName, "/", "_");

	return sClassName;
}
//-----------------------------------------------------------------------------------------------------
string ScreenGenerator::GetParamName(Row_Screen_CommandParameter *pCommandParameter)
{
	return "s" + StringUtils::Replace(pCommandParameter->Description_get(), " ", "");
}
//-----------------------------------------------------------------------------------------------------
string ScreenGenerator::GetParamPair(Row_Screen_CommandParameter *pCommandParameter)
{
	string sParamPair = 
		StringUtils::ltos(pCommandParameter->FK_CommandParameter_get()) + 
		" /* " + pCommandParameter->Description_get() + " */, " + GetParamName(pCommandParameter) + ".c_str()";

	return sParamPair;
}
//-----------------------------------------------------------------------------------------------------
