#ifndef __Table_Room_H__
#define __Table_Room_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Room.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Room : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Room(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Room")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Room();

private:		
	friend class Row_Room;
	struct Key
	{
		friend class Row_Room;
		long int pk_PK_Room;

		
		Key(long int in_PK_Room);
	
		Key(class Row_Room *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Room::Key &key1, const Table_Room::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Room*> *rows);
	class Row_Room* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Room* GetRow(long int in_PK_Room);
	

private:	
	
		
	class Row_Room* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Room : public TableRow, public SerializeClass
	{
		friend struct Table_Room::Key;
		friend class Table_Room;
	private:
		Table_Room *table;
		
		long int m_PK_Room;
long int m_FK_Installation;
long int m_FK_RoomType;
string m_Description;
long int m_FK_Icon;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
	public:
		long int PK_Room_get();
long int FK_Installation_get();
long int FK_RoomType_get();
string Description_get();
long int FK_Icon_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Room_set(long int val);
void FK_Installation_set(long int val);
void FK_RoomType_set(long int val);
void Description_set(string val);
void FK_Icon_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_RoomType_isNull();
bool FK_Icon_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_RoomType_setNull(bool val);
void FK_Icon_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Room(Table_Room *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Room *Table_Room_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Installation* FK_Installation_getrow();
class Row_RoomType* FK_RoomType_getrow();
class Row_Icon* FK_Icon_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Room_FK_Room_getrows(vector <class Row_CommandGroup_Room*> *rows);
void Device_FK_Room_getrows(vector <class Row_Device*> *rows);
void EntertainArea_FK_Room_getrows(vector <class Row_EntertainArea*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Room+ m_FK_Installation+ m_FK_RoomType+ m_Description+ m_FK_Icon+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Room_asSQL();
string FK_Installation_asSQL();
string FK_RoomType_asSQL();
string Description_asSQL();
string FK_Icon_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

