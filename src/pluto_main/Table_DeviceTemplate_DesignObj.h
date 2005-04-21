#ifndef __Table_DeviceTemplate_DesignObj_H__
#define __Table_DeviceTemplate_DesignObj_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_DesignObj.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DeviceTemplate_DesignObj : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DeviceTemplate_DesignObj(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DeviceTemplate_DesignObj();

private:		
	friend class Row_DeviceTemplate_DesignObj;
	struct Key
	{
		friend class Row_DeviceTemplate_DesignObj;
		long int pk_FK_DeviceTemplate;
long int pk_FK_DesignObj;

		
		Key(long int in_FK_DeviceTemplate, long int in_FK_DesignObj);
	
		Key(class Row_DeviceTemplate_DesignObj *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_DesignObj::Key &key1, const Table_DeviceTemplate_DesignObj::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_DesignObj*> *rows);
	class Row_DeviceTemplate_DesignObj* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_DesignObj* GetRow(long int in_FK_DeviceTemplate, long int in_FK_DesignObj);
	

private:	
	
		
	class Row_DeviceTemplate_DesignObj* FetchRow(DoubleLongKey &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_DesignObj : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_DesignObj::Key;
		friend class Table_DeviceTemplate_DesignObj;
	private:
		Table_DeviceTemplate_DesignObj *table;
		
		long int m_FK_DeviceTemplate;
long int m_FK_DesignObj;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[7];
	
	public:
		long int FK_DeviceTemplate_get();
long int FK_DesignObj_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_DeviceTemplate_set(long int val);
void FK_DesignObj_set(long int val);
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
	
		Row_DeviceTemplate_DesignObj(Table_DeviceTemplate_DesignObj *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_DesignObj *Table_DeviceTemplate_DesignObj_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_DesignObj* FK_DesignObj_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_DeviceTemplate+ m_FK_DesignObj+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string FK_DesignObj_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

