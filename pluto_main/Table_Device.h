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

class DLL_EXPORT Table_Device
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

	map<Table_Device::Key, class Row_Device*, Table_Device::Key_Less> cachedRows;
	map<Table_Device::Key, class Row_Device*, Table_Device::Key_Less> deleted_cachedRows;
	vector<class Row_Device*> addedRows;
	vector<class Row_Device*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Device*> *rows);
	class Row_Device* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Device* GetRow(long int in_PK_Device);
	

private:	
	
		
	class Row_Device* FetchRow(Key &key);
		
			
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
long int m_FK_Device_Audio;
long int m_FK_Input_Audio;
long int m_FK_Device_Video;
long int m_FK_Input_Video;
long int m_FK_Device_ControlledVia;
long int m_FK_Interface;
string m_IPaddress;
string m_MACaddress;
short int m_IgnoreOnOff;
long int m_FK_Device_SlaveTo;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[21];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Device_get();
short int Disabled_get();
long int FK_Room_get();
long int FK_Installation_get();
long int FK_DesignObj_get();
string Description_get();
long int FK_DeviceTemplate_get();
long int FK_Device_Audio_get();
long int FK_Input_Audio_get();
long int FK_Device_Video_get();
long int FK_Input_Video_get();
long int FK_Device_ControlledVia_get();
long int FK_Interface_get();
string IPaddress_get();
string MACaddress_get();
short int IgnoreOnOff_get();
long int FK_Device_SlaveTo_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Device_set(long int val);
void Disabled_set(short int val);
void FK_Room_set(long int val);
void FK_Installation_set(long int val);
void FK_DesignObj_set(long int val);
void Description_set(string val);
void FK_DeviceTemplate_set(long int val);
void FK_Device_Audio_set(long int val);
void FK_Input_Audio_set(long int val);
void FK_Device_Video_set(long int val);
void FK_Input_Video_set(long int val);
void FK_Device_ControlledVia_set(long int val);
void FK_Interface_set(long int val);
void IPaddress_set(string val);
void MACaddress_set(string val);
void IgnoreOnOff_set(short int val);
void FK_Device_SlaveTo_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_Room_isNull();
bool FK_Installation_isNull();
bool FK_DesignObj_isNull();
bool FK_DeviceTemplate_isNull();
bool FK_Device_Audio_isNull();
bool FK_Input_Audio_isNull();
bool FK_Device_Video_isNull();
bool FK_Input_Video_isNull();
bool FK_Device_ControlledVia_isNull();
bool FK_Interface_isNull();
bool IPaddress_isNull();
bool MACaddress_isNull();
bool FK_Device_SlaveTo_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_Room_setNull(bool val);
void FK_Installation_setNull(bool val);
void FK_DesignObj_setNull(bool val);
void FK_DeviceTemplate_setNull(bool val);
void FK_Device_Audio_setNull(bool val);
void FK_Input_Audio_setNull(bool val);
void FK_Device_Video_setNull(bool val);
void FK_Input_Video_setNull(bool val);
void FK_Device_ControlledVia_setNull(bool val);
void FK_Interface_setNull(bool val);
void IPaddress_setNull(bool val);
void MACaddress_setNull(bool val);
void FK_Device_SlaveTo_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
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
class Row_Device* FK_Device_Audio_getrow();
class Row_Input* FK_Input_Audio_getrow();
class Row_Device* FK_Device_Video_getrow();
class Row_Input* FK_Input_Video_getrow();
class Row_Device* FK_Device_ControlledVia_getrow();
class Row_Device* FK_Device_SlaveTo_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Command_FK_Device_getrows(vector <class Row_CommandGroup_Command*> *rows);
void CommandGroup_EntGroup_FK_Device_getrows(vector <class Row_CommandGroup_EntGroup*> *rows);
void Device_FK_Device_Audio_getrows(vector <class Row_Device*> *rows);
void Device_FK_Device_Video_getrows(vector <class Row_Device*> *rows);
void Device_FK_Device_ControlledVia_getrows(vector <class Row_Device*> *rows);
void Device_FK_Device_SlaveTo_getrows(vector <class Row_Device*> *rows);
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
void InfraredCode_FK_Device_getrows(vector <class Row_InfraredCode*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Device+ m_Disabled+ m_FK_Room+ m_FK_Installation+ m_FK_DesignObj+ m_Description+ m_FK_DeviceTemplate+ m_FK_Device_Audio+ m_FK_Input_Audio+ m_FK_Device_Video+ m_FK_Input_Video+ m_FK_Device_ControlledVia+ m_FK_Interface+ m_IPaddress+ m_MACaddress+ m_IgnoreOnOff+ m_FK_Device_SlaveTo+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
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
string FK_Device_Audio_asSQL();
string FK_Input_Audio_asSQL();
string FK_Device_Video_asSQL();
string FK_Input_Video_asSQL();
string FK_Device_ControlledVia_asSQL();
string FK_Interface_asSQL();
string IPaddress_asSQL();
string MACaddress_asSQL();
string IgnoreOnOff_asSQL();
string FK_Device_SlaveTo_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

