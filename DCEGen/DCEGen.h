#ifndef DCE_Gen_h
#define DCE_Gen_h

#include "PlutoUtils/MySQLHelper.h"
#include "../pluto_main/Database_pluto_main.h"
#include "../pluto_main/Table_Command.h"

class CommandInfo
{
public:
	class Row_Command *m_pRow_Command;
	class Row_DeviceCommandGroup_Command *m_pRow_DeviceCommandGroup_Command;
	string m_sParmsWithType_In,m_sParmsWithType_Out,m_sParmsWithTypePointers_Out,m_sAssignParmToLocal_In,m_sAssignParmToLocal_Out,m_sParmsWithNoType_In,m_sParmsWithNoType_Out,m_sPassingToMessage_In,m_sPassingToMessage_Out,m_sAssignLocalToParm;
	vector<Row_Command_CommandParameter *> m_vectRow_Command_CommandParameter_In,m_vectRow_Command_CommandParameter_Out;


	string CPPName() { return FileUtils::ValidCPPName(m_pRow_Command->Description_get()); }
};

class DataInfo
{
public:
	class Row_DeviceData *m_pRow_DeviceData;
	bool m_bCanSet;
};

class DeviceInfo
{
public:
	map<int,class CommandInfo *> m_mapCommandInfo;

	list<class Row_Event *> m_listRow_Event;
	map<int,string> m_mapEventDeclarations;
	map<int,string> m_mapEventParms;
	map<int,class Row_DeviceTemplate_Event *> m_mapRow_DeviceTemplate_Event;
	map<int,class DataInfo *> m_mapDataInfo;
	class Row_DeviceTemplate *m_pRow_DeviceTemplate;
	DeviceInfo(class Row_DeviceTemplate *pRow_DeviceTemplate) { m_pRow_DeviceTemplate=pRow_DeviceTemplate; }
};

class DCEGen : public MySqlHelper
{
	map<int,int> m_mapGeneratedDevices;  // Keep track of the devices we've already generated
	string m_sGeneratedOutput,m_sTemplateInput,m_sTemplateOutput;
	int m_iPK_DeviceTemplate;

public:
	Database_pluto_main m_dce;

	DCEGen(int PK_DeviceTemplate,string GeneratedOutput,string TemplateInput,string TemplateOutput,string DBHost,string DBUser,string DBPassword,string DBName,int DBPort);

	void GenerateDevice(int PK_DeviceTemplate,bool bTemplates);
	void AddChildrenByCategory(class Row_DeviceCategory *p_Row_DeviceCategory,map<int,class Row_DeviceTemplate *> *p_mapRow_MasterDevice_Children);
	void CreateDeviceFile(class Row_DeviceTemplate *p_Row_DeviceTemplate,map<int,class Row_DeviceTemplate *> *p_mapRow_MasterDevice_Children,bool bTemplates=true);
	void SearchAndReplace(string InputFile,string OutputFile,string Classname,class DeviceInfo *pDeviceInfo,string sMergeTo,string sDesription);
	void WriteGlobals(); 

	// sParmsWithType: int Parm1, string Parm2, etc.
	// sAssignParmsToLocal: int Parm1=pMessage->m_mapParameters[3]; \n string Parm2= ..., etc.
	// sParmsWithNoType: Parm1, Parm2, etc.
	void CreateFunctionParms(Row_Command *pRow_Command,CommandInfo *pCommandInfo);
	void CreateFunctionParms(class Row_Event *pRow_Event,int &ParmCount,string &sParmsWithType,string &sAssignParmToLocal,string &sParmsWithNoType,string &sPassingToMessage);
	void CreateFunctionParms(class Row_Request *pRow_Request,string &sParmsWithType,string &sAssignParmToLocal,string &sParmsWithNoType,string &sPassingToMessage);
	// bByReference is used for the Requests where some parameters are passed by reference instead
	void CreateFunctionParms(int iParameterID,int iParameterType,string ParameterName,string &sParmsWithType,string &sAssignParmToLocal,string &sParmsWithNoType,string &sPassingToMessage,bool bByReference=false);
	string GetPrefix(int PK_ParameterType); // Get the prefix to put on local variables based on this type of parameter
	string CastTypeToChar(string s,int PK_ParameterType); // assume s is a string, cast it as the correct type
	string CastStringToType(string s,int PK_ParameterType); // assume s is the given type, cast it to a const char
};

#endif
