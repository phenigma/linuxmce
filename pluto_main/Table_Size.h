#ifndef __Table_Size_H__
#define __Table_Size_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Size.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Size
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Size(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Size")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Size();

private:		
	friend class Row_Size;
	struct Key
	{
		friend class Row_Size;
		long int pk_PK_Size;

		
		Key(long int in_PK_Size);
	
		Key(class Row_Size *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Size::Key &key1, const Table_Size::Key &key2) const;
	};	

	map<Table_Size::Key, class Row_Size*, Table_Size::Key_Less> cachedRows;
	map<Table_Size::Key, class Row_Size*, Table_Size::Key_Less> deleted_cachedRows;
	vector<class Row_Size*> addedRows;
	vector<class Row_Size*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Size*> *rows);
	class Row_Size* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Size* GetRow(long int in_PK_Size);
	

private:	
	
		
	class Row_Size* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Size : public TableRow, public SerializeClass
	{
		friend struct Table_Size::Key;
		friend class Table_Size;
	private:
		Table_Size *table;
		
		long int m_PK_Size;
string m_Description;
long int m_Width;
long int m_Height;
long int m_ScaleX;
long int m_ScaleY;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[10];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Size_get();
string Description_get();
long int Width_get();
long int Height_get();
long int ScaleX_get();
long int ScaleY_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Size_set(long int val);
void Description_set(string val);
void Width_set(long int val);
void Height_set(long int val);
void ScaleX_set(long int val);
void ScaleY_set(long int val);
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
	
		Row_Size(Table_Size *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Size *Table_Size_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Orbiter_FK_Size_getrows(vector <class Row_Orbiter*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Size+ m_Description+ m_Width+ m_Height+ m_ScaleX+ m_ScaleY+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Size_asSQL();
string Description_asSQL();
string Width_asSQL();
string Height_asSQL();
string ScaleX_asSQL();
string ScaleY_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

