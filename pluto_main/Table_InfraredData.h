#ifndef __Table_InfraredData_H__
#define __Table_InfraredData_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_InfraredData.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_InfraredData
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_InfraredData(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("InfraredData")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_InfraredData();

private:		
	friend class Row_InfraredData;
	struct Key
	{
		friend class Row_InfraredData;
		long int pk_PK_InfraredData;

		
		Key(long int in_PK_InfraredData);
	
		Key(class Row_InfraredData *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_InfraredData::Key &key1, const Table_InfraredData::Key &key2) const;
	};	

	map<Table_InfraredData::Key, class Row_InfraredData*, Table_InfraredData::Key_Less> cachedRows;
	map<Table_InfraredData::Key, class Row_InfraredData*, Table_InfraredData::Key_Less> deleted_cachedRows;
	vector<class Row_InfraredData*> addedRows;
	vector<class Row_InfraredData*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_InfraredData*> *rows);
	class Row_InfraredData* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InfraredData* GetRow(long int in_PK_InfraredData);
	

private:	
	
		
	class Row_InfraredData* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_InfraredData : public TableRow, public SerializeClass
	{
		friend struct Table_InfraredData::Key;
		friend class Table_InfraredData;
	private:
		Table_InfraredData *table;
		
		long int m_PK_InfraredData;
string m_Manufacturer;
string m_Model;
string m_Device;
string m_Command;
string m_Pronto;
long int m_Frequency;
string m_Data;
string m_DataRepeat;
string m_Leadout;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[14];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_InfraredData_get();
string Manufacturer_get();
string Model_get();
string Device_get();
string Command_get();
string Pronto_get();
long int Frequency_get();
string Data_get();
string DataRepeat_get();
string Leadout_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_InfraredData_set(long int val);
void Manufacturer_set(string val);
void Model_set(string val);
void Device_set(string val);
void Command_set(string val);
void Pronto_set(string val);
void Frequency_set(long int val);
void Data_set(string val);
void DataRepeat_set(string val);
void Leadout_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Manufacturer_isNull();
bool Model_isNull();
bool Device_isNull();
bool Command_isNull();
bool Pronto_isNull();
bool Frequency_isNull();
bool Data_isNull();
bool DataRepeat_isNull();
bool Leadout_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Manufacturer_setNull(bool val);
void Model_setNull(bool val);
void Device_setNull(bool val);
void Command_setNull(bool val);
void Pronto_setNull(bool val);
void Frequency_setNull(bool val);
void Data_setNull(bool val);
void DataRepeat_setNull(bool val);
void Leadout_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_InfraredData(Table_InfraredData *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InfraredData *Table_InfraredData_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_InfraredData+ m_Manufacturer+ m_Model+ m_Device+ m_Command+ m_Pronto+ m_Frequency+ m_Data+ m_DataRepeat+ m_Leadout+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_InfraredData_asSQL();
string Manufacturer_asSQL();
string Model_asSQL();
string Device_asSQL();
string Command_asSQL();
string Pronto_asSQL();
string Frequency_asSQL();
string Data_asSQL();
string DataRepeat_asSQL();
string Leadout_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

