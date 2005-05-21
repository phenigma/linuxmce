#ifndef __Table_Room_Users_H__
#define __Table_Room_Users_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Room_Users.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Room_Users : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Room_Users(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Room_Users();

private:		
	friend class Row_Room_Users;
	struct Key
	{
		friend class Row_Room_Users;
		long int pk_PK_Room_Users;

		
		Key(long int in_PK_Room_Users);
	
		Key(class Row_Room_Users *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Room_Users::Key &key1, const Table_Room_Users::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Room_Users*> *rows);
	class Row_Room_Users* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Room_Users* GetRow(long int in_PK_Room_Users);
	

private:	
	
		
	class Row_Room_Users* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Room_Users : public TableRow, public SerializeClass
	{
		friend struct Table_Room_Users::Key;
		friend class Table_Room_Users;
	private:
		Table_Room_Users *table;
		
		long int m_PK_Room_Users;
long int m_FK_Room;
long int m_FK_Users;
long int m_FK_Orbiter;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_Room_Users_get();
long int FK_Room_get();
long int FK_Users_get();
long int FK_Orbiter_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Room_Users_set(long int val);
void FK_Room_set(long int val);
void FK_Users_set(long int val);
void FK_Orbiter_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Users_isNull();
bool FK_Orbiter_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Users_setNull(bool val);
void FK_Orbiter_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Room_Users(Table_Room_Users *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Room_Users *Table_Room_Users_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Room* FK_Room_getrow();
class Row_Users* FK_Users_getrow();
class Row_Orbiter* FK_Orbiter_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Room_Users+ m_FK_Room+ m_FK_Users+ m_FK_Orbiter+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Room_Users_asSQL();
string FK_Room_asSQL();
string FK_Users_asSQL();
string FK_Orbiter_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

