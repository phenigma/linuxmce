#ifndef __Table_Broadcast_H__
#define __Table_Broadcast_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Broadcast.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Broadcast
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Broadcast(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Broadcast")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Broadcast();

private:		
	friend class Row_Broadcast;
	struct Key
	{
		friend class Row_Broadcast;
		long int pk_PK_Broadcast;

		
		Key(long int in_PK_Broadcast);
	
		Key(class Row_Broadcast *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Broadcast::Key &key1, const Table_Broadcast::Key &key2) const;
	};	

	map<Table_Broadcast::Key, class Row_Broadcast*, Table_Broadcast::Key_Less> cachedRows;
	map<Table_Broadcast::Key, class Row_Broadcast*, Table_Broadcast::Key_Less> deleted_cachedRows;
	vector<class Row_Broadcast*> addedRows;
	vector<class Row_Broadcast*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Broadcast*> *rows);
	class Row_Broadcast* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Broadcast* GetRow(long int in_PK_Broadcast);
	

private:	
	
		
	class Row_Broadcast* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Broadcast : public TableRow, public SerializeClass
	{
		friend struct Table_Broadcast::Key;
		friend class Table_Broadcast;
	private:
		Table_Broadcast *table;
		
		long int m_PK_Broadcast;
string m_Description;
string m_Extensions;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Broadcast_get();
string Description_get();
string Extensions_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Broadcast_set(long int val);
void Description_set(string val);
void Extensions_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Extensions_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Extensions_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Broadcast(Table_Broadcast *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Broadcast *Table_Broadcast_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void MediaType_Broadcast_FK_Broadcast_getrows(vector <class Row_MediaType_Broadcast*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Broadcast+ m_Description+ m_Extensions+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Broadcast_asSQL();
string Description_asSQL();
string Extensions_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

