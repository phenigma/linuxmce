#ifndef __Table_Distro_H__
#define __Table_Distro_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Distro.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Distro
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Distro(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Distro")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Distro();

private:		
	friend class Row_Distro;
	struct Key
	{
		friend class Row_Distro;
		long int pk_PK_Distro;

		
		Key(long int in_PK_Distro);
	
		Key(class Row_Distro *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Distro::Key &key1, const Table_Distro::Key &key2) const;
	};	

	map<Table_Distro::Key, class Row_Distro*, Table_Distro::Key_Less> cachedRows;
	map<Table_Distro::Key, class Row_Distro*, Table_Distro::Key_Less> deleted_cachedRows;
	vector<class Row_Distro*> addedRows;
	vector<class Row_Distro*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Distro*> *rows);
	class Row_Distro* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Distro* GetRow(long int in_PK_Distro);
	

private:	
	
		
	class Row_Distro* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Distro : public TableRow, public SerializeClass
	{
		friend struct Table_Distro::Key;
		friend class Table_Distro;
	private:
		Table_Distro *table;
		
		long int m_PK_Distro;
string m_Description;
string m_Define;
long int m_FK_OperatingSystem;

		bool is_null[4];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Distro_get();
string Description_get();
string Define_get();
long int FK_OperatingSystem_get();

		
		void PK_Distro_set(long int val);
void Description_set(string val);
void Define_set(string val);
void FK_OperatingSystem_set(long int val);

		
		bool Define_isNull();

			
		void Define_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Distro(Table_Distro *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Distro *Table_Distro_get() { return table; };

		// Return the rows for foreign keys 
		class Row_OperatingSystem* FK_OperatingSystem_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_Package_FK_Distro_getrows(vector <class Row_DeviceTemplate_Package*> *rows);
void Package_Distro_FK_Distro_getrows(vector <class Row_Package_Distro*> *rows);
void RepositorySource_FK_Distro_getrows(vector <class Row_RepositorySource*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Distro+ m_Description+ m_Define+ m_FK_OperatingSystem;
		}
	private:
		void SetDefaultValues();
		
		string PK_Distro_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_OperatingSystem_asSQL();

	};

#endif

