#ifndef __Table_ModeChange_H__
#define __Table_ModeChange_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_security.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_ModeChange.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_ModeChange : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_security *database;
	struct Key;	//forward declaration
	
public:
	Table_ModeChange(Database_pluto_security *pDatabase):database(pDatabase)
	{
	};
	~Table_ModeChange();

private:		
	friend class Row_ModeChange;
	struct Key
	{
		friend class Row_ModeChange;
		long int pk_PK_ModeChange;

		
		Key(long int in_PK_ModeChange);
	
		Key(class Row_ModeChange *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_ModeChange::Key &key1, const Table_ModeChange::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_ModeChange*> *rows);
	class Row_ModeChange* AddRow();
	Database_pluto_security *Database_pluto_security_get() { return database; }
	
		
	class Row_ModeChange* GetRow(long int in_PK_ModeChange);
	

private:	
	
		
	class Row_ModeChange* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_ModeChange : public TableRow, public SerializeClass
	{
		friend struct Table_ModeChange::Key;
		friend class Table_ModeChange;
	private:
		Table_ModeChange *table;
		
		long int m_PK_ModeChange;
long int m_EK_HouseMode;
long int m_EK_DeviceGroup;
string m_ChangeTime;
long int m_EK_Users;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
	public:
		long int PK_ModeChange_get();
long int EK_HouseMode_get();
long int EK_DeviceGroup_get();
string ChangeTime_get();
long int EK_Users_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_ModeChange_set(long int val);
void EK_HouseMode_set(long int val);
void EK_DeviceGroup_set(long int val);
void ChangeTime_set(string val);
void EK_Users_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool EK_DeviceGroup_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void EK_DeviceGroup_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_ModeChange(Table_ModeChange *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_ModeChange *Table_ModeChange_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_ModeChange+ m_EK_HouseMode+ m_EK_DeviceGroup+ m_ChangeTime+ m_EK_Users+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_ModeChange_asSQL();
string EK_HouseMode_asSQL();
string EK_DeviceGroup_asSQL();
string ChangeTime_asSQL();
string EK_Users_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

