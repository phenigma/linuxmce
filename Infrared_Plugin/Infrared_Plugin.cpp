//<-dceag-d-b->
#include "Infrared_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_InfraredGroup.h"
#include "pluto_main/Table_InfraredCode.h"

//<-dceag-d-e->

//<-dceag-const-b->
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

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

void Infrared_Plugin::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DERC:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Send Orbiters the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.

	// Send the message to a specific orbiter, identified by OrbiterID
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);

	// Send the message to orbiters 32898 and 27283
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,"32898,27283",55,77);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);

	// This time we want to wait to be sure the orbiter gets the message, and the thread will block until the orbiter receives the message
	string sResult="";
	bool bResult = SendMessageWithConfirm(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77),sResult);
	// If bResult is true, the message was received ok.  Otherwise it failed, and sResult contains an explanation of the failure

	// A request is like a command, except that it has both "in" and "out" parameters, and the 
	// thread blocks until the request is completed.  Note that requests are sent on a separate socket, so you can continue to receive commands and send
	// events while a thread is waiting for a request to complete.  However the SendMessageWithConfirm uses the same request socket, and will wait if
	// the socket is busy.  

	// We want to request the contents of a file.  This request takes 1 "in" parameter, the file name, and has 2 "out" parameters: the file's contents, and the date.
	// All out parameters are passed by reference.
	// Note that the "Data" parameter which contains the file is a binary parameter.  Whether in or out, binary parameters always take both a pointer to the object and the size
	// This command will be sent to the "Standard Plug-in" which implements this type of request.  We set the boardcast level to BL_SameHouse.  Note that since this is a
	// request, the router will find the first matching device (ie the first device "Standard Plug-in") and return that device's response.  If this was a command, not a request,
	// and there were multiple devices "Standard Plug-in" they would all get the message.
	PlutoDate plutoDate;
	char *FileContents;
	int FileSize;
	bool bResult = SendRequest(new DERC::REQ_File_Contents(m_dwPK_Device,DeviceTemplate_Standard_Plug_In_CONST,true,BL_SameHouse,"some_file_name",&FileContents,&FileSize,&plutoDate);

	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();


}
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c188-b->
/* 
	COMMAND: #188 - Get Infrared Codes
	COMMENTS: Retrieves all the infrared codes for a given device.
	PARAMETERS:
		#2 PK_Device
			The device to retrieve the infrared codes for.
		#5 Value To Assign
			A tab delimited list of all the commands and infrared codes for the device.  The format is:
#commands\t{PK_Command\tData}
*/
void Infrared_Plugin::CMD_Get_Infrared_Codes(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c188-e->
{
	//cout << "Command #188 - Get Infrared Codes" << endl;
	//cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	//cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;

	vector<Row_Device *> vectpRow_Device;

	*sValue_To_Assign = "";

	Row_DeviceCategory * pRow_DeviceCategory = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device)->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow();
	vector<Row_InfraredGroup *> vectpRow_InfraredGroup;
	m_pDatabase_pluto_main->InfraredGroup_get()->GetRows("FK_DeviceCategory=" + StringUtils::ltos(pRow_DeviceCategory->PK_DeviceCategory_get()), &vectpRow_InfraredGroup);
	vector<Row_InfraredGroup *>::iterator i;
	for (i = vectpRow_InfraredGroup.begin(); i != vectpRow_InfraredGroup.end(); i++)
	{
		vector<Row_InfraredCode *> vectpRow_InfraredCode;
		m_pDatabase_pluto_main->InfraredCode_get()->GetRows("FK_InfraredGroup=" + StringUtils::ltos((*i)->PK_InfraredGroup_get()) + " LIMIT 1", &vectpRow_InfraredCode);
		vector<Row_InfraredCode *>::iterator j;
		* sValue_To_Assign = StringUtils::ltos((vectpRow_InfraredCode.size())) + "\t";
		for (j = vectpRow_InfraredCode.begin(); j != vectpRow_InfraredCode.end(); j++)
		{
//			cout << (*j)->Description_get() << ": " << (*j)->IRData_get() << endl;
			* sValue_To_Assign += StringUtils::ltos((*j)->FK_Command_get()) + "\t" + (*j)->IRData_get() + "\t";
		}
	}
	sCMD_Result = "OK";
}
