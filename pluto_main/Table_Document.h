#ifndef __Table_Document_H__
#define __Table_Document_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Document.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Document : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Document(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Document")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Document();

private:		
	friend class Row_Document;
	struct Key
	{
		friend class Row_Document;
		long int pk_PK_Document;

		
		Key(long int in_PK_Document);
	
		Key(class Row_Document *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Document::Key &key1, const Table_Document::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Document*> *rows);
	class Row_Document* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Document* GetRow(long int in_PK_Document);
	

private:	
	
		
	class Row_Document* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Document : public TableRow, public SerializeClass
	{
		friend struct Table_Document::Key;
		friend class Table_Document;
	private:
		Table_Document *table;
		
		long int m_PK_Document;
long int m_FK_Document_Parent;
short int m_Hidden;
long int m_Order;
string m_Title;
string m_Contents;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[11];
	
	public:
		long int PK_Document_get();
long int FK_Document_Parent_get();
short int Hidden_get();
long int Order_get();
string Title_get();
string Contents_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Document_set(long int val);
void FK_Document_Parent_set(long int val);
void Hidden_set(short int val);
void Order_set(long int val);
void Title_set(string val);
void Contents_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Document_Parent_isNull();
bool Contents_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Document_Parent_setNull(bool val);
void Contents_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Document(Table_Document *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Document *Table_Document_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Document* FK_Document_Parent_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObj_FK_Document_getrows(vector <class Row_DesignObj*> *rows);
void Document_FK_Document_Parent_getrows(vector <class Row_Document*> *rows);
void Document_Comment_FK_Document_getrows(vector <class Row_Document_Comment*> *rows);
void Package_FK_Document_getrows(vector <class Row_Package*> *rows);
void Package_FK_Document_UsersManual_getrows(vector <class Row_Package*> *rows);
void Package_FK_Document_ProgrammersGuide_getrows(vector <class Row_Package*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Document+ m_FK_Document_Parent+ m_Hidden+ m_Order+ m_Title+ m_Contents+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Document_asSQL();
string FK_Document_Parent_asSQL();
string Hidden_asSQL();
string Order_asSQL();
string Title_asSQL();
string Contents_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

