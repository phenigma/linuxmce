#ifndef UpdateEntArea_h
#define UpdateEntArea_h

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_EntertainArea.h"

class UpdateEntArea 
{
	int m_iPK_Installation;
	int m_dwPK_Device_MediaPlugIn;
    Database_pluto_main *m_pDatabase_pluto_main ;

public:
	UpdateEntArea(int PK_Installation,string host, string user, string pass, string db_name, int port=3306);

	void DoIt();
	void PutMDsChildrenInRoom(Row_Device *pRow_Device);
	void DeleteEntertainArea(Row_EntertainArea *pRow_EntertainArea);
	void AddDefaultCommandsToEntArea(Row_EntertainArea *pRow_EntertainArea);
	int FindCommandGroupByTemplate(Row_EntertainArea *pRow_EntertainArea,int PK_Template,string sDescription);
	void AddCommand(int PK_CommandGroup,int PK_Device,int PK_Command,int NumParms,...);
	void AddMDsDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea);
	void AddAVDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea);
};
#endif
