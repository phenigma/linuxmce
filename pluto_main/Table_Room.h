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

class DLL_EXPORT Table_Room
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

	map<Table_Room::Key, class Row_Room*, Table_Room::Key_Less> cachedRows;
	map<Table_Room::Key, class Row_Room*, Table_Room::Key_Less> deleted_cachedRows;
	vector<class Row_Room*> addedRows;
	vector<class Row_Room*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Room*> *rows);
	class Row_Room* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Room* GetRow(long int in_PK_Room);
	

private:	
	
		
	class Row_Room* FetchRow(Key &key);
		
			
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
long int m_FK_RoomMode;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[10];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Room_get();
long int FK_Installation_get();
long int FK_RoomType_get();
string Description_get();
long int FK_Icon_get();
long int FK_RoomMode_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Room_set(long int val);
void FK_Installation_set(long int val);
void FK_RoomType_set(long int val);
void Description_set(string val);
void FK_Icon_set(long int val);
void FK_RoomMode_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_RoomType_isNull();
bool FK_Icon_isNull();
bool FK_RoomMode_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_RoomType_setNull(bool val);
void FK_Icon_setNull(bool val);
void FK_RoomMode_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
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
class Row_RoomMode* FK_RoomMode_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Room_FK_Room_getrows(vector <class Row_CommandGroup_Room*> *rows);
void Device_FK_Room_getrows(vector <class Row_Device*> *rows);
void EntertainArea_FK_Room_getrows(vector <class Row_EntertainArea*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Room+ m_FK_Installation+ m_FK_RoomType+ m_Description+ m_FK_Icon+ m_FK_RoomMode+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Room_asSQL();
string FK_Installation_asSQL();
string FK_RoomType_asSQL();
string Description_asSQL();
string FK_Icon_asSQL();
string FK_RoomMode_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

