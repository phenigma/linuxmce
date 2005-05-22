#ifndef __Table_DeviceCommandGroup_H__
#define __Table_DeviceCommandGroup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceCommandGroup.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DeviceCommandGroup : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DeviceCommandGroup(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DeviceCommandGroup();

private:		
	friend class Row_DeviceCommandGroup;
	struct Key
	{
		friend class Row_DeviceCommandGroup;
		long int pk_PK_DeviceCommandGroup;

		
		Key(long int in_PK_DeviceCommandGroup);
	
		Key(class Row_DeviceCommandGroup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceCommandGroup::Key &key1, const Table_DeviceCommandGroup::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceCommandGroup*> *rows);
	class Row_DeviceCommandGroup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceCommandGroup* GetRow(long int in_PK_DeviceCommandGroup);
	

private:	
	
		
	class Row_DeviceCommandGroup* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DeviceCommandGroup : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceCommandGroup::Key;
		friend class Table_DeviceCommandGroup;
	private:
		Table_DeviceCommandGroup *table;
		
		long int m_PK_DeviceCommandGroup;
long int m_FK_DeviceCategory;
string m_Description;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int PK_DeviceCommandGroup_get();
long int FK_DeviceCategory_get();
string Description_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_DeviceCommandGroup_set(long int val);
void FK_DeviceCategory_set(long int val);
void Description_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_DeviceCategory_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_DeviceCategory_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceCommandGroup(Table_DeviceCommandGroup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceCommandGroup *Table_DeviceCommandGroup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCategory* FK_DeviceCategory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceCommandGroup_Command_FK_DeviceCommandGroup_getrows(vector <class Row_DeviceCommandGroup_Command*> *rows);
void DeviceCommandGroup_DeviceCommandGroup_Parent_FK_DeviceCommandGroup_getrows(vector <class Row_DeviceCommandGroup_DeviceCommandGroup_Parent*> *rows);
void DeviceCommandGroup_DeviceCommandGroup_Parent_FK_DeviceCommandGroup_Parent_getrows(vector <class Row_DeviceCommandGroup_DeviceCommandGroup_Parent*> *rows);
void DeviceTemplate_DeviceCommandGroup_FK_DeviceCommandGroup_getrows(vector <class Row_DeviceTemplate_DeviceCommandGroup*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DeviceCommandGroup+ m_FK_DeviceCategory+ m_Description+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceCommandGroup_asSQL();
string FK_DeviceCategory_asSQL();
string Description_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

