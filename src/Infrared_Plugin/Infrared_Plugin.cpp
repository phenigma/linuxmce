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

#ifndef WIN32
	#include <unistd.h>
#endif

#include "Orbiter_Plugin/Orbiter_Plugin.h" 

#include "DCE/DataGrid.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_InfraredGroup.h"
#include "pluto_main/Table_InfraredGroup_Command.h"
#include "pluto_main/Table_InfraredGroup_Command_Preferred.h"
#include "pluto_main/Table_DeviceTemplate_InfraredGroup.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_DeviceTemplate_AV.h"
#include "pluto_main/Table_DeviceTemplate_Input.h"
#include "pluto_main/Table_Manufacturer.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_Variable.h"

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
	delete m_pDatabase_pluto_main;
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Infrared_Plugin::Register()
//<-dceag-reg-e->
{
	m_pDatagrid_Plugin=NULL;
	ListCommand_Impl *pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Datagrid_Plugin_CONST);

	if( !pListCommand_Impl || pListCommand_Impl->size()!=1 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"File grids cannot find datagrid handler %s",(pListCommand_Impl ? "There were more than 1" : ""));
		return false;
	}

	m_pDatagrid_Plugin=(Datagrid_Plugin *) pListCommand_Impl->front();

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::DevicesGrid)),
		DATAGRID_Devices_by_Room_CONST);

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::CommandsGrid)),
		DATAGRID_Commmands_By_Device_CONST);

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::Manufacturers)),
		DATAGRID_Manufacturers_CONST);

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::IRGroupCategories)),
		DATAGRID_Infrared_Group_Categories_CONST);

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::InfraredGroups)),
		DATAGRID_Infrared_Groups_CONST);

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Infrared_Plugin::InfraredCodes)),
		DATAGRID_Infrared_Codes_CONST);

    ListCommand_Impl *pListCommand_Impl_2nd = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Orbiter_Plugin_CONST );
    if( !pListCommand_Impl_2nd || pListCommand_Impl_2nd->size( )!=1 )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Infrared plug in cannot find orbiter handler %s", ( pListCommand_Impl_2nd ? "There were more than 1" : "" ) );
        return false;
    }

    m_pOrbiter_Plugin=( Orbiter_Plugin * ) pListCommand_Impl_2nd->front( );

	return Connect(PK_DeviceTemplate_get()); 
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

class DataGridTable *Infrared_Plugin::DevicesGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	int Type = atoi(Parms.c_str());
	int iRow=0;

    for(map<int,Room *>::const_iterator it=m_pRouter->m_mapRoom_get()->begin();it!=m_pRouter->m_mapRoom_get()->end();++it)
    {
		Room *pRoom = (*it).second;
		bool bFirst=true;

		for(list<class DeviceData_Router *>::iterator itD=pRoom->m_listDevices.begin();itD!=pRoom->m_listDevices.end();++itD)
		{
			class DeviceData_Router *pDeviceData_Router = *itD;
			if( Type==1 && !pDeviceData_Router->WithinCategory(DEVICECATEGORY_AV_CONST) )
				continue;

			pCell = new DataGridCell( (bFirst ? pRoom->m_sDescription + "\n  " : "  ") + pDeviceData_Router->m_sDescription,
				StringUtils::itos(pDeviceData_Router->m_dwPK_Device) );
			pDataGrid->SetData(0,iRow++,pCell);
			bFirst=false;
		}
	}

	return pDataGrid;
}

class DataGridTable *Infrared_Plugin::CommandsGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	string::size_type pos=0;
	int PK_Device = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	if( !PK_Device )
		return pDataGrid;

	int PK_Orbiter = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int PK_Text = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());

	int iRow=0;
	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);

	vector<Row_DeviceTemplate_AV *> vectRow_DeviceTemplate_AV;
	pDevice->m_pRow_Device->FK_DeviceTemplate_getrow()->DeviceTemplate_AV_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_AV);
	Row_DeviceTemplate_AV *pRow_DeviceTemplate_AV = vectRow_DeviceTemplate_AV.size() ? vectRow_DeviceTemplate_AV[0] : NULL;
	bool bUsesIR = pRow_DeviceTemplate_AV && pRow_DeviceTemplate_AV->UsesIR_get()==1;

	for(map<int,string>::iterator it=pDevice->m_mapCommands.begin();it!=pDevice->m_mapCommands.end();++it)
	{
		// Handle some special cases
		if( (*it).first == COMMAND_Toggle_Power_CONST && bUsesIR && pRow_DeviceTemplate_AV->TogglePower_get()==0 )
		{
			// We don't toggle power, we have discrete on and off's
			pCell = new DataGridCell( "ON",	StringUtils::itos(COMMAND_Generic_On_CONST) );
			pCell->m_Colspan = 4;
			pCell->m_pMessage = new Message(PK_Orbiter,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_On_CONST,0);
			pDataGrid->SetData(0,iRow,pCell);

			pCell = new DataGridCell( "learn","" );
			DCE::CMD_Learn_IR CMD_Learn_IR(PK_Orbiter,pDevice->m_dwPK_Device,"1",PK_Text,COMMAND_Generic_On_CONST);
			pCell->m_pMessage = CMD_Learn_IR.m_pMessage;
			pDataGrid->SetData(4,iRow++,pCell);

			pCell = new DataGridCell( "OFF",	StringUtils::itos(COMMAND_Generic_Off_CONST) );
			pCell->m_Colspan = 4;
			pCell->m_pMessage = new Message(PK_Orbiter,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_Off_CONST,0);
			pDataGrid->SetData(0,iRow,pCell);

			pCell = new DataGridCell( "learn","" );
			DCE::CMD_Learn_IR CMD_Learn_IR2(PK_Orbiter,pDevice->m_dwPK_Device,"1",PK_Text,COMMAND_Generic_Off_CONST);
			pCell->m_pMessage = CMD_Learn_IR2.m_pMessage;
			pDataGrid->SetData(4,iRow++,pCell);
		}
		else if( (*it).first == COMMAND_Jump_Position_In_Playlist_CONST && bUsesIR )
		{
			for(int i=0;i<=9;++i)
			{
				pCell = new DataGridCell( StringUtils::itos(i),	StringUtils::itos(COMMAND_0_CONST + i) );
				pCell->m_Colspan = 4;
				pCell->m_pMessage = new Message(PK_Orbiter,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_0_CONST + i,0);
				pDataGrid->SetData(0,iRow,pCell);

				pCell = new DataGridCell( "learn","" );
				DCE::CMD_Learn_IR CMD_Learn_IR(PK_Orbiter,pDevice->m_dwPK_Device,"1",PK_Text,COMMAND_0_CONST + i);
				pCell->m_pMessage = CMD_Learn_IR.m_pMessage;
				pDataGrid->SetData(4,iRow++,pCell);
			}
			pCell = new DataGridCell( "Enter",	StringUtils::itos(COMMAND_Send_Generic_EnterGo_CONST) );
			pCell->m_Colspan = 4;
			pCell->m_pMessage = new Message(PK_Orbiter,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Send_Generic_EnterGo_CONST,0);
			pDataGrid->SetData(0,iRow,pCell);

			pCell = new DataGridCell( "learn","" );
			DCE::CMD_Learn_IR CMD_Learn_IR(PK_Orbiter,pDevice->m_dwPK_Device,"1",PK_Text,COMMAND_Send_Generic_EnterGo_CONST);
			pCell->m_pMessage = CMD_Learn_IR.m_pMessage;
			pDataGrid->SetData(4,iRow++,pCell);
		}
		else
		{
			pCell = new DataGridCell( (*it).second,	StringUtils::itos((*it).first) );
			pCell->m_Colspan = 4;
			pCell->m_pMessage = new Message(PK_Orbiter,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,(*it).first,0);
			pDataGrid->SetData(0,iRow,pCell);

			pCell = new DataGridCell( "learn","" );
			DCE::CMD_Learn_IR CMD_Learn_IR(PK_Orbiter,pDevice->m_dwPK_Device,"1",PK_Text,(*it).first);
			pCell->m_pMessage = CMD_Learn_IR.m_pMessage;
			pDataGrid->SetData(4,iRow++,pCell);
		}
	}

	// Add the inputs
	vector<Row_DeviceTemplate_Input *> vectRow_DeviceTemplate_Input;
	pDevice->m_pRow_Device->FK_DeviceTemplate_getrow()->DeviceTemplate_Input_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_Input);
	for(size_t s=0;s<vectRow_DeviceTemplate_Input.size();++s)
	{
		Row_DeviceTemplate_Input *pRow_DeviceTemplate_Input = vectRow_DeviceTemplate_Input[s];

		pCell = new DataGridCell( pRow_DeviceTemplate_Input->FK_Command_getrow()->Description_get(), StringUtils::itos(pRow_DeviceTemplate_Input->FK_Command_get()) );
		pCell->m_Colspan = 4;
		pCell->m_pMessage = new Message(PK_Orbiter,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,pRow_DeviceTemplate_Input->FK_Command_get(),0);
		pDataGrid->SetData(0,iRow,pCell);

		if( pRow_DeviceTemplate_AV && pRow_DeviceTemplate_AV->ToggleInput_get()==0 )
		{
			pCell = new DataGridCell( "learn","" );
			DCE::CMD_Learn_IR CMD_Learn_IR(PK_Orbiter,pDevice->m_dwPK_Device,"1",PK_Text,pRow_DeviceTemplate_Input->FK_Command_get());
			pCell->m_pMessage = CMD_Learn_IR.m_pMessage;
		}
		else
			pCell = new DataGridCell( "TOAD","" );

		pDataGrid->SetData(4,iRow++,pCell);
	}

	return pDataGrid;
}

class DataGridTable *Infrared_Plugin::InfraredCodes(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	if( Parms.length()==0 )
		return pDataGrid;

	string::size_type pos=0;
	string sPK_Device = StringUtils::Tokenize(Parms,",",pos);
	string sPK_InfraredGroup = StringUtils::Tokenize(Parms,",",pos);

	Row_InfraredGroup *pRow_InfraredGroup = m_pDatabase_pluto_main->InfraredGroup_get()->GetRow( atoi(sPK_InfraredGroup.c_str()) );
	if( !pRow_InfraredGroup )
		return pDataGrid;

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow( atoi(sPK_Device.c_str()) );
	if( !pRow_Device )
		return pDataGrid;

	vector<Row_InfraredGroup_Command *> vectRow_InfraredGroup_Command;
	pRow_InfraredGroup->InfraredGroup_Command_FK_InfraredGroup_getrows(&vectRow_InfraredGroup_Command);

	if( vectRow_InfraredGroup_Command.size()==0 )
	{
		pCell = new DataGridCell( "No commands","" );
		pDataGrid->SetData(0,0,pCell);
	}

	for(size_t s=0;s<vectRow_InfraredGroup_Command.size();++s)
	{
		Row_InfraredGroup_Command *pRow_InfraredGroup_Command = vectRow_InfraredGroup_Command[s];
		pCell = new DataGridCell( pRow_InfraredGroup_Command->FK_Command_getrow()->Description_get(),"" );
		DCE::CMD_Send_Code CMD_Send_Code(m_dwPK_Device,pRow_Device->PK_Device_get(),pRow_InfraredGroup_Command->IRData_get());
		pCell->m_pMessage = CMD_Send_Code.m_pMessage;
		pDataGrid->SetData(0,s,pCell);
	}
	return pDataGrid;
}
class DataGridTable *Infrared_Plugin::Manufacturers(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
	Row_Device *pRow_Device = NULL;  // The initially selected manufacturer can be passed in as a device

	if( Parms.length() )
	{
		pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(atoi(Parms.c_str()));
		if( pRow_Device )
		{
			*iPK_Variable = VARIABLE_Misc_Data_1_CONST;
			*sValue_To_Assign = StringUtils::itos(pRow_Device->FK_DeviceTemplate_getrow()->FK_Manufacturer_get());
		}
	}
	vector<Row_Manufacturer *> vectRow_Manufacturer;
	m_pDatabase_pluto_main->Manufacturer_get()->GetRows("1=1 ORDER BY Description",&vectRow_Manufacturer);
	for(size_t s=0;s<vectRow_Manufacturer.size();++s)
	{
		Row_Manufacturer *pRow_Manufacturer = vectRow_Manufacturer[s];
		pCell = new DataGridCell( pRow_Manufacturer->Description_get(),StringUtils::itos(pRow_Manufacturer->PK_Manufacturer_get()) );
		pDataGrid->SetData(0,s,pCell);
	}

	return pDataGrid;
}

class DataGridTable *Infrared_Plugin::InfraredGroups(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	string::size_type pos=0;
	string sPK_Manufacturer = StringUtils::Tokenize(Parms,",",pos);
	string sPK_DeviceCategory = StringUtils::Tokenize(Parms,",",pos);

	if( !sPK_DeviceCategory.length() )
	{
		if( !sPK_Manufacturer.length() )
			return pDataGrid;
		// If we got in only 1 number, it's really a device
		Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(atoi(sPK_Manufacturer.substr(1).c_str()));
		if( !pRow_Device )
			return pDataGrid;
		sPK_Manufacturer = StringUtils::itos(pRow_Device->FK_DeviceTemplate_getrow()->FK_Manufacturer_get());
		sPK_DeviceCategory = StringUtils::itos(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get());
	}

	vector<Row_InfraredGroup *> vectRow_InfraredGroup;
	m_pDatabase_pluto_main->InfraredGroup_get()->GetRows("FK_Manufacturer=" + sPK_Manufacturer + " AND FK_DeviceCategory=" + sPK_DeviceCategory,&vectRow_InfraredGroup);

	for(size_t s=0;s<vectRow_InfraredGroup.size();++s)
	{
		Row_InfraredGroup *pRow_InfraredGroup = vectRow_InfraredGroup[s];
		pCell = new DataGridCell( pRow_InfraredGroup->Description_get(), StringUtils::itos(pRow_InfraredGroup->PK_InfraredGroup_get()) );
        pDataGrid->SetData( 0, s, pCell );
	}

	return pDataGrid;
}

class DataGridTable *Infrared_Plugin::IRGroupCategories(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;	

	if( Parms.length()==0 )
		return pDataGrid;

	Row_Device *pRow_Device = NULL;
	// The first time the populate is called when the screen appears we don't know the category yet.  The initial options for this
	// grid pass in "D" + the device number, and we fill in the category automatically.  After that we're passed in the manufacturer
	if( Parms.length()>1 && Parms[0]=='D' )
	{
		pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(atoi(Parms.substr(1).c_str()));
		if( pRow_Device )
		{
			*iPK_Variable = VARIABLE_Misc_Data_2_CONST;
			*sValue_To_Assign = StringUtils::itos(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get());
			Parms = StringUtils::itos(pRow_Device->FK_DeviceTemplate_getrow()->FK_Manufacturer_get());  // We're going to expect this to be the manufacturer
		}
	}

	string sql = "SELECT DISTINCT PK_DeviceCategory,DeviceCategory.Description FROM InfraredGroup JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory WHERE FK_Manufacturer=" + Parms;

	PlutoSqlResult result;
    MYSQL_ROW row;
    int RowCount=0;

    if( ( result.r=m_pDatabase_pluto_main->mysql_query_result( sql ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            pCell = new DataGridCell( row[1], row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
		}
	}
	return pDataGrid;
}

//<-dceag-sample-b->!

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

	// We're going to go through the list twice.  First we'll get the stock codes where FK_DeviceTemplate IS NULL.  Any 
	// Codes the user learned, or specifically wants to use instead will be in InfraredGroup_Command_Preferred, so we'll
	// go through a second time to get the codes that are in there.
	vector<Row_InfraredGroup_Command *> vectRow_InfraredGroup_Command[2];

	long FK_DeviceTemplate = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device)->FK_DeviceTemplate_get();
	
	vector<Row_DeviceTemplate_InfraredGroup *> vectRow_DeviceTemplate_InfraredGroup;
	m_pDatabase_pluto_main->DeviceTemplate_InfraredGroup_get()->GetRows("FK_DeviceTemplate=" + StringUtils::itos(iPK_Device),
		&vectRow_DeviceTemplate_InfraredGroup);
	
	vector<Row_DeviceTemplate_InfraredGroup *>::iterator it_RDTIG;
	for (it_RDTIG = vectRow_DeviceTemplate_InfraredGroup.begin(); it_RDTIG != vectRow_DeviceTemplate_InfraredGroup.end(); it_RDTIG++)
	{
		Row_DeviceTemplate_InfraredGroup * pRow_DeviceTemplate_InfraredGroup = *it_RDTIG;
		pTable_InfraredGroup_Command->GetRows("FK_InfraredGroup=" + StringUtils::itos(pRow_DeviceTemplate_InfraredGroup->FK_InfraredGroup_get()) + " AND FK_DeviceTemplate IS NULL",
			&vectRow_InfraredGroup_Command[0]);
	}
	pTable_InfraredGroup_Command->GetRows("JOIN InfraredGroup_Command_Preferred ON FK_InfraredGroup_Command=PK_InfraredGroup_Command WHERE FK_DeviceTemplate=" + StringUtils::itos(FK_DeviceTemplate),
		&vectRow_InfraredGroup_Command[1]);

	for (i = 0; i < 2; i++)
{
g_pPlutoLogger->Write(LV_STATUS,"Found %d codes for device %d",(int) vectRow_InfraredGroup_Command[i].size(),iPK_Device);
		Count += vectRow_InfraredGroup_Command[i].size();
}
	
	// Format: <Count> \t (<Command ID> \t <IRData> \t){<Count> times}
	* sValue_To_Assign = StringUtils::ltos(Count) + "\t";

	for (i = 0; i < 2; i++)
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
g_pPlutoLogger->Write(LV_STATUS,"StoreIR Code.  Found %d rows for this already",(int) vectRow_InfraredGroup_Command.size());
	bool bNew = false;
	Row_InfraredGroup_Command *pRow_InfraredGroup_Command=NULL;
	if (vectRow_InfraredGroup_Command.size() == 0)
	{
		// we found no existing entry for the Command+Device pair; adding a new one
		pRow_InfraredGroup_Command = pTable_InfraredGroup_Command->AddRow();
		pRow_InfraredGroup_Command->FK_Device_set(iPK_Device);
		pRow_InfraredGroup_Command->FK_Command_set(iPK_Command_Input);
		pRow_InfraredGroup_Command->FK_DeviceTemplate_set(pRow_InfraredGroup_Command->FK_Device_getrow()->FK_DeviceTemplate_get());
		pRow_InfraredGroup_Command->IRData_set(sValue_To_Assign);
		bNew = true;
	}
	else
	{
		// we found an entry; updating it
		pRow_InfraredGroup_Command = vectRow_InfraredGroup_Command[0];
		pRow_InfraredGroup_Command->IRData_set(sValue_To_Assign);
	}
	pTable_InfraredGroup_Command->Commit();
g_pPlutoLogger->Write(LV_STATUS,"In the database as PK_IRG_C %d",pRow_InfraredGroup_Command->PK_InfraredGroup_Command_get());

	Row_InfraredGroup_Command_Preferred *pRow_InfraredGroup_Command_Preferred = NULL;
	if( !bNew )
		pRow_InfraredGroup_Command_Preferred = m_pDatabase_pluto_main->InfraredGroup_Command_Preferred_get()->GetRow( 
			pRow_InfraredGroup_Command->PK_InfraredGroup_Command_get(), m_pRouter->iPK_Installation_get() );

	if( !pRow_InfraredGroup_Command_Preferred )
	{
g_pPlutoLogger->Write(LV_STATUS,"adding the record to the preferred table for %d",pRow_InfraredGroup_Command->PK_InfraredGroup_Command_get());
		pRow_InfraredGroup_Command_Preferred =  m_pDatabase_pluto_main->InfraredGroup_Command_Preferred_get()->AddRow();
		pRow_InfraredGroup_Command_Preferred->FK_InfraredGroup_Command_set(pRow_InfraredGroup_Command->PK_InfraredGroup_Command_get());
		pRow_InfraredGroup_Command_Preferred->FK_Installation_set(m_pRouter->iPK_Installation_get());
		m_pDatabase_pluto_main->InfraredGroup_Command_Preferred_get()->Commit();
	}
else
g_pPlutoLogger->Write(LV_STATUS,"it's already in the preferred table for %d",pRow_InfraredGroup_Command_Preferred->FK_InfraredGroup_Command_get());

	sCMD_Result = "OK";
}
//<-dceag-createinst-b->!
//<-dceag-c276-b->

	/** @brief COMMAND: #276 - Add GC100 */
	/** Add a GC100 Device */

void Infrared_Plugin::CMD_Add_GC100(string &sCMD_Result,Message *pMessage)
//<-dceag-c276-e->
{
	string Command;
	char line[45];
	int returned, size;
	FILE *fp;

	Command = "/usr/pluto/bin/gc100-conf.pl";
	int iPK_Device_Orbiter = pMessage->m_dwPK_Device_From;
	m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"Finding GC100",false,30);

	returned = system(Command.c_str());

	if ( returned == -1) {
		g_pPlutoLogger->Write(LV_STATUS, "Failed Spawning configure script");
		m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Failed, Spwning config script",false,30);
	} else if( returned == 0) {
		size_t s;
		const char *ptr = FileUtils::ReadFileIntoBuffer("/var/log/pluto/gc100-conf.log",s);
		g_pPlutoLogger->Write(LV_STATUS, "The configure script returned with success.  log %d bytes: %s",s,ptr ? ptr : "NULL");
// The script will fire the new pp		m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 added with success",false,30);
	} else if( returned == 1) {
		g_pPlutoLogger->Write(LV_WARNING, "GC100 as default not found");
		m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Not Found as factory default",false,30);
	} else if( returned == 2) {
		g_pPlutoLogger->Write(LV_WARNING, "GC100 already exist in the databse");
		m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Allready exists",false,30);
	} else if( returned == 3) {
		g_pPlutoLogger->Write(LV_WARNING, "GC100 config did not found instalation number");
		m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Failed, invalid instalation number",false,30);
	} else if( returned == 4) {
		g_pPlutoLogger->Write(LV_WARNING, "GC100 config did not found Template number");
		m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Failed, invalid Template number",false,30);
	} else {
		g_pPlutoLogger->Write(LV_WARNING, "The config script returned weird error");
		m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"GC100 Failed, Weird answer",false,30);
	}
}
