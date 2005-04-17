#ifndef __Table_Device_H__
#define __Table_Device_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Device.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Device : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Device(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Device")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Device();

private:		
	friend class Row_Device;
	struct Key
	{
		friend class Row_Device;
		long int pk_PK_Device;

		
		Key(long int in_PK_Device);
	
		Key(class Row_Device *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Device::Key &key1, const Table_Device::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Device*> *rows);
	class Row_Device* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Device* GetRow(long int in_PK_Device);
	

private:	
	
		
	class Row_Device* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Device : public TableRow, public SerializeClass
	{
		friend struct Table_Device::Key;
		friend class Table_Device;
	private:
		Table_Device *table;
		
		long int m_PK_Device;
short int m_Disabled;
long int m_FK_Room;
long int m_FK_Installation;
long int m_FK_DesignObj;
string m_Description;
long int m_FK_DeviceTemplate;
long int m_FK_Device_ControlledVia;
string m_IPaddress;
string m_MACaddress;
short int m_IgnoreOnOff;
long int m_FK_Device_RouteTo;
short int m_NeedConfigure;
string m_State;
string m_Status;
short int m_PingTest;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[21];
	
	public:
		long int PK_Device_get();
short int Disabled_get();
long int FK_Room_get();
long int FK_Installation_get();
long int FK_DesignObj_get();
string Description_get();
long int FK_DeviceTemplate_get();
long int FK_Device_ControlledVia_get();
string IPaddress_get();
string MACaddress_get();
short int IgnoreOnOff_get();
long int FK_Device_RouteTo_get();
short int NeedConfigure_get();
string State_get();
string Status_get();
short int PingTest_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Device_set(long int val);
void Disabled_set(short int val);
void FK_Room_set(long int val);
void FK_Installation_set(long int val);
void FK_DesignObj_set(long int val);
void Description_set(string val);
void FK_DeviceTemplate_set(long int val);
void FK_Device_ControlledVia_set(long int val);
void IPaddress_set(string val);
void MACaddress_set(string val);
void IgnoreOnOff_set(short int val);
void FK_Device_RouteTo_set(long int val);
void NeedConfigure_set(short int val);
void State_set(string val);
void Status_set(string val);
void PingTest_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Room_isNull();
bool FK_Installation_isNull();
bool FK_DesignObj_isNull();
bool FK_DeviceTemplate_isNull();
bool FK_Device_ControlledVia_isNull();
bool IPaddress_isNull();
bool MACaddress_isNull();
bool FK_Device_RouteTo_isNull();
bool NeedConfigure_isNull();
bool State_isNull();
bool Status_isNull();
bool PingTest_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Room_setNull(bool val);
void FK_Installation_setNull(bool val);
void FK_DesignObj_setNull(bool val);
void FK_DeviceTemplate_setNull(bool val);
void FK_Device_ControlledVia_setNull(bool val);
void IPaddress_setNull(bool val);
void MACaddress_setNull(bool val);
void FK_Device_RouteTo_setNull(bool val);
void NeedConfigure_setNull(bool val);
void State_setNull(bool val);
void Status_setNull(bool val);
void PingTest_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Device(Table_Device *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Device *Table_Device_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Room* FK_Room_getrow();
class Row_Installation* FK_Installation_getrow();
class Row_DesignObj* FK_DesignObj_getrow();
class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_Device* FK_Device_ControlledVia_getrow();
class Row_Device* FK_Device_RouteTo_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Command_FK_Device_getrows(vector <class Row_CommandGroup_Command*> *rows);
void Device_FK_Device_ControlledVia_getrows(vector <class Row_Device*> *rows);
void Device_FK_Device_RouteTo_getrows(vector <class Row_Device*> *rows);
void Device_Command_FK_Device_getrows(vector <class Row_Device_Command*> *rows);
void Device_CommandGroup_FK_Device_getrows(vector <class Row_Device_CommandGroup*> *rows);
void Device_DeviceData_FK_Device_getrows(vector <class Row_Device_DeviceData*> *rows);
void Device_DeviceGroup_FK_Device_getrows(vector <class Row_Device_DeviceGroup*> *rows);
void Device_Device_Pipe_FK_Device_From_getrows(vector <class Row_Device_Device_Pipe*> *rows);
void Device_Device_Pipe_FK_Device_To_getrows(vector <class Row_Device_Device_Pipe*> *rows);
void Device_Device_Related_FK_Device_getrows(vector <class Row_Device_Device_Related*> *rows);
void Device_Device_Related_FK_Device_Related_getrows(vector <class Row_Device_Device_Related*> *rows);
void Device_EntertainArea_FK_Device_getrows(vector <class Row_Device_EntertainArea*> *rows);
void Device_HouseMode_FK_Device_getrows(vector <class Row_Device_HouseMode*> *rows);
void Device_Orbiter_FK_Device_getrows(vector <class Row_Device_Orbiter*> *rows);
void Device_StartupScript_FK_Device_getrows(vector <class Row_Device_StartupScript*> *rows);
void Device_Users_FK_Device_getrows(vector <class Row_Device_Users*> *rows);
void InfraredGroup_Command_FK_Device_getrows(vector <class Row_InfraredGroup_Command*> *rows);
void Package_Device_FK_Device_getrows(vector <class Row_Package_Device*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Device+ m_Disabled+ m_FK_Room+ m_FK_Installation+ m_FK_DesignObj+ m_Description+ m_FK_DeviceTemplate+ m_FK_Device_ControlledVia+ m_IPaddress+ m_MACaddress+ m_IgnoreOnOff+ m_FK_Device_RouteTo+ m_NeedConfigure+ m_State+ m_Status+ m_PingTest+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Device_asSQL();
string Disabled_asSQL();
string FK_Room_asSQL();
string FK_Installation_asSQL();
string FK_DesignObj_asSQL();
string Description_asSQL();
string FK_DeviceTemplate_asSQL();
string FK_Device_ControlledVia_asSQL();
string IPaddress_asSQL();
string MACaddress_asSQL();
string IgnoreOnOff_asSQL();
string FK_Device_RouteTo_asSQL();
string NeedConfigure_asSQL();
string State_asSQL();
string Status_asSQL();
string PingTest_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

