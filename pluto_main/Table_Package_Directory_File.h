#ifndef __Table_Package_Directory_File_H__
#define __Table_Package_Directory_File_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Package_Directory_File.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Package_Directory_File
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Package_Directory_File(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Package_Directory_File")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Package_Directory_File();

private:		
	friend class Row_Package_Directory_File;
	struct Key
	{
		friend class Row_Package_Directory_File;
		long int pk_PK_Package_Directory_File;

		
		Key(long int in_PK_Package_Directory_File);
	
		Key(class Row_Package_Directory_File *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Package_Directory_File::Key &key1, const Table_Package_Directory_File::Key &key2) const;
	};	

	map<Table_Package_Directory_File::Key, class Row_Package_Directory_File*, Table_Package_Directory_File::Key_Less> cachedRows;
	map<Table_Package_Directory_File::Key, class Row_Package_Directory_File*, Table_Package_Directory_File::Key_Less> deleted_cachedRows;
	vector<class Row_Package_Directory_File*> addedRows;
	vector<class Row_Package_Directory_File*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Package_Directory_File*> *rows);
	class Row_Package_Directory_File* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Package_Directory_File* GetRow(long int in_PK_Package_Directory_File);
	

private:	
	
		
	class Row_Package_Directory_File* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Package_Directory_File : public TableRow, public SerializeClass
	{
		friend struct Table_Package_Directory_File::Key;
		friend class Table_Package_Directory_File;
	private:
		Table_Package_Directory_File *table;
		
		long int m_PK_Package_Directory_File;
long int m_FK_Package_Directory;
long int m_FK_OperatingSystem;
long int m_FK_Distro;
string m_File;
string m_Search;
string m_MakeCommand;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Package_Directory_File_get();
long int FK_Package_Directory_get();
long int FK_OperatingSystem_get();
long int FK_Distro_get();
string File_get();
string Search_get();
string MakeCommand_get();

		
		void PK_Package_Directory_File_set(long int val);
void FK_Package_Directory_set(long int val);
void FK_OperatingSystem_set(long int val);
void FK_Distro_set(long int val);
void File_set(string val);
void Search_set(string val);
void MakeCommand_set(string val);

		
		bool FK_OperatingSystem_isNull();
bool FK_Distro_isNull();
bool Search_isNull();
bool MakeCommand_isNull();

			
		void FK_OperatingSystem_setNull(bool val);
void FK_Distro_setNull(bool val);
void Search_setNull(bool val);
void MakeCommand_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Package_Directory_File(Table_Package_Directory_File *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Package_Directory_File *Table_Package_Directory_File_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Package_Directory* FK_Package_Directory_getrow();
class Row_OperatingSystem* FK_OperatingSystem_getrow();
class Row_Distro* FK_Distro_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Package_Directory_File+ m_FK_Package_Directory+ m_FK_OperatingSystem+ m_FK_Distro+ m_File+ m_Search+ m_MakeCommand;
		}
	private:
		void SetDefaultValues();
		
		string PK_Package_Directory_File_asSQL();
string FK_Package_Directory_asSQL();
string FK_OperatingSystem_asSQL();
string FK_Distro_asSQL();
string File_asSQL();
string Search_asSQL();
string MakeCommand_asSQL();

	};

#endif

