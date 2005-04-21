#ifndef __Table_Device_DeviceData_H__
#define __Table_Device_DeviceData_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Device_DeviceData.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Device_DeviceData : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Device_DeviceData(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Device_DeviceData();

private:		
	friend class Row_Device_DeviceData;
	struct Key
	{
		friend class Row_Device_DeviceData;
		long int pk_FK_Device;
long int pk_FK_DeviceData;

		
		Key(long int in_FK_Device, long int in_FK_DeviceData);
	
		Key(class Row_Device_DeviceData *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Device_DeviceData::Key &key1, const Table_Device_DeviceData::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Device_DeviceData*> *rows);
	class Row_Device_DeviceData* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Device_DeviceData* GetRow(long int in_FK_Device, long int in_FK_DeviceData);
	

private:	
	
		
	class Row_Device_DeviceData* FetchRow(DoubleLongKey &key);
		
			
};

class DLL_EXPORT Row_Device_DeviceData : public TableRow, public SerializeClass
	{
		friend struct Table_Device_DeviceData::Key;
		friend class Table_Device_DeviceData;
	private:
		Table_Device_DeviceData *table;
		
		long int m_FK_Device;
long int m_FK_DeviceData;
string m_IK_DeviceData;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int FK_Device_get();
long int FK_DeviceData_get();
string IK_DeviceData_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_Device_set(long int val);
void FK_DeviceData_set(long int val);
void IK_DeviceData_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Device_DeviceData(Table_Device_DeviceData *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Device_DeviceData *Table_Device_DeviceData_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Device* FK_Device_getrow();
class Row_DeviceData* FK_DeviceData_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Device+ m_FK_DeviceData+ m_IK_DeviceData+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_Device_asSQL();
string FK_DeviceData_asSQL();
string IK_DeviceData_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

