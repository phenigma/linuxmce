#ifndef __Table_DeviceTemplate_AV_H__
#define __Table_DeviceTemplate_AV_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_AV.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DeviceTemplate_AV : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_AV(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_AV")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_AV();

private:		
	friend class Row_DeviceTemplate_AV;
	struct Key
	{
		friend class Row_DeviceTemplate_AV;
		long int pk_FK_DeviceTemplate;

		
		Key(long int in_FK_DeviceTemplate);
	
		Key(class Row_DeviceTemplate_AV *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_AV::Key &key1, const Table_DeviceTemplate_AV::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_AV*> *rows);
	class Row_DeviceTemplate_AV* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_AV* GetRow(long int in_FK_DeviceTemplate);
	

private:	
	
		
	class Row_DeviceTemplate_AV* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_AV : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_AV::Key;
		friend class Table_DeviceTemplate_AV;
	private:
		Table_DeviceTemplate_AV *table;
		
		long int m_FK_DeviceTemplate;
long int m_IR_PowerDelay;
long int m_IR_ModeDelay;
long int m_DigitDelay;
long int m_TogglePower;
long int m_ToggleDSP;
long int m_ToggleInput;
long int m_ToggleOutput;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[12];
	
	public:
		long int FK_DeviceTemplate_get();
long int IR_PowerDelay_get();
long int IR_ModeDelay_get();
long int DigitDelay_get();
long int TogglePower_get();
long int ToggleDSP_get();
long int ToggleInput_get();
long int ToggleOutput_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DeviceTemplate_set(long int val);
void IR_PowerDelay_set(long int val);
void IR_ModeDelay_set(long int val);
void DigitDelay_set(long int val);
void TogglePower_set(long int val);
void ToggleDSP_set(long int val);
void ToggleInput_set(long int val);
void ToggleOutput_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_AV(Table_DeviceTemplate_AV *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_AV *Table_DeviceTemplate_AV_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceTemplate+ m_IR_PowerDelay+ m_IR_ModeDelay+ m_DigitDelay+ m_TogglePower+ m_ToggleDSP+ m_ToggleInput+ m_ToggleOutput+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string IR_PowerDelay_asSQL();
string IR_ModeDelay_asSQL();
string DigitDelay_asSQL();
string TogglePower_asSQL();
string ToggleDSP_asSQL();
string ToggleInput_asSQL();
string ToggleOutput_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

