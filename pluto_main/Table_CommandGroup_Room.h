#ifndef __Table_CommandGroup_Room_H__
#define __Table_CommandGroup_Room_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CommandGroup_Room.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_CommandGroup_Room
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CommandGroup_Room(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CommandGroup_Room")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CommandGroup_Room();

private:		
	friend class Row_CommandGroup_Room;
	struct Key
	{
		friend class Row_CommandGroup_Room;
		long int pk_FK_CommandGroup;
long int pk_FK_Room;

		
		Key(long int in_FK_CommandGroup, long int in_FK_Room);
	
		Key(class Row_CommandGroup_Room *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CommandGroup_Room::Key &key1, const Table_CommandGroup_Room::Key &key2) const;
	};	

	map<Table_CommandGroup_Room::Key, class Row_CommandGroup_Room*, Table_CommandGroup_Room::Key_Less> cachedRows;
	map<Table_CommandGroup_Room::Key, class Row_CommandGroup_Room*, Table_CommandGroup_Room::Key_Less> deleted_cachedRows;
	vector<class Row_CommandGroup_Room*> addedRows;
	vector<class Row_CommandGroup_Room*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CommandGroup_Room*> *rows);
	class Row_CommandGroup_Room* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CommandGroup_Room* GetRow(long int in_FK_CommandGroup, long int in_FK_Room);
	

private:	
	
		
	class Row_CommandGroup_Room* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_CommandGroup_Room : public TableRow, public SerializeClass
	{
		friend struct Table_CommandGroup_Room::Key;
		friend class Table_CommandGroup_Room;
	private:
		Table_CommandGroup_Room *table;
		
		long int m_FK_CommandGroup;
long int m_FK_Room;
long int m_Sort;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_CommandGroup_get();
long int FK_Room_get();
long int Sort_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_CommandGroup_set(long int val);
void FK_Room_set(long int val);
void Sort_set(long int val);
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
	
		Row_CommandGroup_Room(Table_CommandGroup_Room *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CommandGroup_Room *Table_CommandGroup_Room_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CommandGroup* FK_CommandGroup_getrow();
class Row_Room* FK_Room_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_CommandGroup+ m_FK_Room+ m_Sort+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_CommandGroup_asSQL();
string FK_Room_asSQL();
string Sort_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

