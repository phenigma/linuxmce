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

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Size : public TableBase , SingleLongKeyBase
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

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Size*> *rows);
	class Row_Size* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Size* GetRow(long int in_PK_Size);
	

private:	
	
		
	class Row_Size* FetchRow(SingleLongKey &key);
		
			
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
short int m_PreserveAspectRatio;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[12];
	
	public:
		long int PK_Size_get();
string Description_get();
long int Width_get();
long int Height_get();
long int ScaleX_get();
long int ScaleY_get();
short int PreserveAspectRatio_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Size_set(long int val);
void Description_set(string val);
void Width_set(long int val);
void Height_set(long int val);
void ScaleX_set(long int val);
void ScaleY_set(long int val);
void PreserveAspectRatio_set(short int val);
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
	
		Row_Size(Table_Size *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Size *Table_Size_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Orbiter_FK_Size_getrows(vector <class Row_Orbiter*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Size+ m_Description+ m_Width+ m_Height+ m_ScaleX+ m_ScaleY+ m_PreserveAspectRatio+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Size_asSQL();
string Description_asSQL();
string Width_asSQL();
string Height_asSQL();
string ScaleX_asSQL();
string ScaleY_asSQL();
string PreserveAspectRatio_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

