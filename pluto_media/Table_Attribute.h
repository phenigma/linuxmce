#ifndef __Table_Attribute_H__
#define __Table_Attribute_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Attribute.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Attribute
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Attribute(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("Attribute")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Attribute();

private:		
	friend class Row_Attribute;
	struct Key
	{
		friend class Row_Attribute;
		long int pk_PK_Attribute;

		
		Key(long int in_PK_Attribute);
	
		Key(class Row_Attribute *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Attribute::Key &key1, const Table_Attribute::Key &key2) const;
	};	

	map<Table_Attribute::Key, class Row_Attribute*, Table_Attribute::Key_Less> cachedRows;
	map<Table_Attribute::Key, class Row_Attribute*, Table_Attribute::Key_Less> deleted_cachedRows;
	vector<class Row_Attribute*> addedRows;
	vector<class Row_Attribute*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Attribute*> *rows);
	class Row_Attribute* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Attribute* GetRow(long int in_PK_Attribute);
	

private:	
	
		
	class Row_Attribute* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Attribute : public TableRow, public SerializeClass
	{
		friend struct Table_Attribute::Key;
		friend class Table_Attribute;
	private:
		Table_Attribute *table;
		
		long int m_PK_Attribute;
long int m_FK_AttributeType;
string m_Name;
string m_FirstName;

		bool is_null[4];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Attribute_get();
long int FK_AttributeType_get();
string Name_get();
string FirstName_get();

		
		void PK_Attribute_set(long int val);
void FK_AttributeType_set(long int val);
void Name_set(string val);
void FirstName_set(string val);

		
		bool FK_AttributeType_isNull();
bool FirstName_isNull();

			
		void FK_AttributeType_setNull(bool val);
void FirstName_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Attribute(Table_Attribute *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Attribute *Table_Attribute_get() { return table; };

		// Return the rows for foreign keys 
		class Row_AttributeType* FK_AttributeType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void File_Attribute_FK_Attribute_getrows(vector <class Row_File_Attribute*> *rows);
void Picture_Attribute_FK_Attribute_getrows(vector <class Row_Picture_Attribute*> *rows);
void SearchToken_Attribute_FK_Attribute_getrows(vector <class Row_SearchToken_Attribute*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Attribute+ m_FK_AttributeType+ m_Name+ m_FirstName;
		}
	private:
		void SetDefaultValues();
		
		string PK_Attribute_asSQL();
string FK_AttributeType_asSQL();
string Name_asSQL();
string FirstName_asSQL();

	};

#endif

