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

class DLL_EXPORT Table_Document
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

	map<Table_Document::Key, class Row_Document*, Table_Document::Key_Less> cachedRows;
	map<Table_Document::Key, class Row_Document*, Table_Document::Key_Less> deleted_cachedRows;
	vector<class Row_Document*> addedRows;
	vector<class Row_Document*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Document*> *rows);
	class Row_Document* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Document* GetRow(long int in_PK_Document);
	

private:	
	
		
	class Row_Document* FetchRow(Key &key);
		
			
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

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Document_get();
long int FK_Document_Parent_get();
short int Hidden_get();
long int Order_get();
string Title_get();
string Contents_get();

		
		void PK_Document_set(long int val);
void FK_Document_Parent_set(long int val);
void Hidden_set(short int val);
void Order_set(long int val);
void Title_set(string val);
void Contents_set(string val);

		
		bool FK_Document_Parent_isNull();
bool Contents_isNull();

			
		void FK_Document_Parent_setNull(bool val);
void Contents_setNull(bool val);
	
	
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
			StartSerializeList() + m_PK_Document+ m_FK_Document_Parent+ m_Hidden+ m_Order+ m_Title+ m_Contents;
		}
	private:
		void SetDefaultValues();
		
		string PK_Document_asSQL();
string FK_Document_Parent_asSQL();
string Hidden_asSQL();
string Order_asSQL();
string Title_asSQL();
string Contents_asSQL();

	};

#endif

