#ifndef __Table_Image_H__
#define __Table_Image_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Image.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Image
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Image(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Image")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Image();

private:		
	friend class Row_Image;
	struct Key
	{
		friend class Row_Image;
		string pk_PK_Image;

		
		Key(string in_PK_Image);
	
		Key(class Row_Image *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Image::Key &key1, const Table_Image::Key &key2) const;
	};	

	map<Table_Image::Key, class Row_Image*, Table_Image::Key_Less> cachedRows;
	map<Table_Image::Key, class Row_Image*, Table_Image::Key_Less> deleted_cachedRows;
	vector<class Row_Image*> addedRows;
	vector<class Row_Image*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Image*> *rows);
	class Row_Image* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Image* GetRow(string in_PK_Image);
	

private:	
	
		
	class Row_Image* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Image : public TableRow, public SerializeClass
	{
		friend struct Table_Image::Key;
		friend class Table_Image;
	private:
		Table_Image *table;
		
		string m_PK_Image;
string m_Date;
long int m_Width;
long int m_Height;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		string PK_Image_get();
string Date_get();
long int Width_get();
long int Height_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Image_set(string val);
void Date_set(string val);
void Width_set(long int val);
void Height_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Date_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Date_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Image(Table_Image *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Image *Table_Image_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Image+ m_Date+ m_Width+ m_Height+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Image_asSQL();
string Date_asSQL();
string Width_asSQL();
string Height_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

