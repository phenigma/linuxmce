//<-dceag-d-b->
#include "Infrared_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_InfraredGroup_Command.h"
#include "pluto_main/Table_DeviceTemplate_InfraredGroup.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Infrared_Plugin::Infrared_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Infrared_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDatabase_pluto_main = new Database_pluto_main();
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit=true;
		return;
	}

}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Infrared_Plugin::~Infrared_Plugin()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Infrared_Plugin::Register()
//<-dceag-reg-e->
{
	return Connect(); 
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Infrared_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Infrared_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c188-b->

	/** @brief COMMAND: #188 - Get Infrared Codes */
	/** Retrieves all the infrared codes for a given device. */
		/** @param #2 PK_Device */
			/** The device to retrieve the infrared codes for. */
		/** @param #5 Value To Assign */
			/** A tab delimited list of all the commands and infrared codes for the device.  The format is:
\t{\tData} */

void Infrared_Plugin::CMD_Get_Infrared_Codes(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c188-e->
{
	int i;
	size_t Count = 0;
	Table_InfraredGroup_Command * pTable_InfraredGroup_Command = m_pDatabase_pluto_main->InfraredGroup_Command_get();
	vector<Row_InfraredGroup_Command *> vectRow_InfraredGroup_Command[3];

	long FK_DeviceTemplate = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device)->FK_DeviceTemplate_get();
	
	vector<Row_DeviceTemplate_InfraredGroup *> vectRow_DeviceTemplate_InfraredGroup;
	m_pDatabase_pluto_main->DeviceTemplate_InfraredGroup_get()->GetRows("FK_DeviceTemplate=" + StringUtils::itos(iPK_Device),
		&vectRow_DeviceTemplate_InfraredGroup);
	
	vector<Row_DeviceTemplate_InfraredGroup *>::iterator it_RDTIG;
	for (it_RDTIG = vectRow_DeviceTemplate_InfraredGroup.begin(); it_RDTIG != vectRow_DeviceTemplate_InfraredGroup.end(); it_RDTIG++)
	{
		Row_DeviceTemplate_InfraredGroup * pRow_DeviceTemplate_InfraredGroup = *it_RDTIG;
		pTable_InfraredGroup_Command->GetRows("FK_InfraredGroup=" + StringUtils::itos(pRow_DeviceTemplate_InfraredGroup->FK_InfraredGroup_get()),
			&vectRow_InfraredGroup_Command[0]);
	}
	pTable_InfraredGroup_Command->GetRows("FK_DeviceTemplate=" + StringUtils::itos(FK_DeviceTemplate),
		&vectRow_InfraredGroup_Command[1]);
	pTable_InfraredGroup_Command->GetRows("FK_Device=" + StringUtils::itos(iPK_Device),
		&vectRow_InfraredGroup_Command[2]);


	for (i = 0; i < 3; i++)
		Count += vectRow_InfraredGroup_Command[i].size();
	
	// Format: <Count> \t (<Command ID> \t <IRData> \t){<Count> times}
	* sValue_To_Assign = StringUtils::ltos(Count) + "\t";

	for (i = 0; i < 3; i++)
	{
		vector<Row_InfraredGroup_Command *>::iterator it_vRIGC;
		for (it_vRIGC = vectRow_InfraredGroup_Command[i].begin(); it_vRIGC != vectRow_InfraredGroup_Command[i].end(); it_vRIGC++)
		{
			Row_InfraredGroup_Command * pRow_InfraredGroup_Command = * it_vRIGC;
//			cout << (*j)->Description_get() << ": " << (*j)->IRData_get() << endl;
			* sValue_To_Assign += StringUtils::ltos(pRow_InfraredGroup_Command->FK_Command_get()) + "\t" +
				pRow_InfraredGroup_Command->IRData_get() + "\t";
		}
	}
	sCMD_Result = "OK";
}
//<-dceag-c250-b->

	/** @brief COMMAND: #250 - Store Infrared Code */
	/** Store a learned infrared code for a "Device" + "Command" pair */
		/** @param #2 PK_Device */
			/** Device this code was learned for */
		/** @param #5 Value To Assign */
			/** IR code in Philips pronto format */
		/** @param #71 PK_Command_Input */
			/** Command this code launches */

void Infrared_Plugin::CMD_Store_Infrared_Code(int iPK_Device,string sValue_To_Assign,int iPK_Command_Input,string &sCMD_Result,Message *pMessage)
//<-dceag-c250-e->
{
	Table_InfraredGroup_Command * pTable_InfraredGroup_Command = m_pDatabase_pluto_main->InfraredGroup_Command_get();

	vector<Row_InfraredGroup_Command *> vectRow_InfraredGroup_Command;
	pTable_InfraredGroup_Command->GetRows("FK_Device=" + StringUtils::itos(iPK_Device) +
		" AND FK_Command=" + StringUtils::itos(iPK_Command_Input), &vectRow_InfraredGroup_Command);
	if (vectRow_InfraredGroup_Command.size() == 0)
	{
		// we found no existing entry for the Command+Device pair; adding a new one
		Row_InfraredGroup_Command * pRow_InfraredGroup_Command = pTable_InfraredGroup_Command->AddRow();
		pRow_InfraredGroup_Command->FK_Device_set(iPK_Device);
		pRow_InfraredGroup_Command->FK_Command_set(iPK_Command_Input);
		pRow_InfraredGroup_Command->FK_DeviceTemplate_set(pRow_InfraredGroup_Command->FK_Device_getrow()->FK_DeviceTemplate_get());
		pRow_InfraredGroup_Command->IRData_set(sValue_To_Assign);
	}
	else
	{
		// we found an entry; updating it
		vectRow_InfraredGroup_Command[0]->IRData_set(sValue_To_Assign);
	}
	pTable_InfraredGroup_Command->Commit();

	sCMD_Result = "OK";
}
