#ifndef __Table_FloorplanObjectType_H__
#define __Table_FloorplanObjectType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_FloorplanObjectType.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_FloorplanObjectType
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_FloorplanObjectType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("FloorplanObjectType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_FloorplanObjectType();

private:		
	friend class Row_FloorplanObjectType;
	struct Key
	{
		friend class Row_FloorplanObjectType;
		long int pk_PK_FloorplanDesignObjType;

		
		Key(long int in_PK_FloorplanDesignObjType);
	
		Key(class Row_FloorplanObjectType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_FloorplanObjectType::Key &key1, const Table_FloorplanObjectType::Key &key2) const;
	};	

	map<Table_FloorplanObjectType::Key, class Row_FloorplanObjectType*, Table_FloorplanObjectType::Key_Less> cachedRows;
	map<Table_FloorplanObjectType::Key, class Row_FloorplanObjectType*, Table_FloorplanObjectType::Key_Less> deleted_cachedRows;
	vector<class Row_FloorplanObjectType*> addedRows;
	vector<class Row_FloorplanObjectType*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_FloorplanObjectType*> *rows);
	class Row_FloorplanObjectType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_FloorplanObjectType* GetRow(long int in_PK_FloorplanDesignObjType);
	

private:	
	
		
	class Row_FloorplanObjectType* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_FloorplanObjectType : public TableRow, public SerializeClass
	{
		friend struct Table_FloorplanObjectType::Key;
		friend class Table_FloorplanObjectType;
	private:
		Table_FloorplanObjectType *table;
		
		long int m_PK_FloorplanDesignObjType;
long int m_FK_FloorplanType;
string m_Description;
string m_Define;
string m_Direction;
long int m_FK_DesignObj_Control;
string m_Filename;
long int m_FillX;
long int m_FillY;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[13];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_FloorplanDesignObjType_get();
long int FK_FloorplanType_get();
string Description_get();
string Define_get();
string Direction_get();
long int FK_DesignObj_Control_get();
string Filename_get();
long int FillX_get();
long int FillY_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_FloorplanDesignObjType_set(long int val);
void FK_FloorplanType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Direction_set(string val);
void FK_DesignObj_Control_set(long int val);
void Filename_set(string val);
void FillX_set(long int val);
void FillY_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_DesignObj_Control_isNull();
bool Filename_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_DesignObj_Control_setNull(bool val);
void Filename_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_FloorplanObjectType(Table_FloorplanObjectType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_FloorplanObjectType *Table_FloorplanObjectType_get() { return table; };

		// Return the rows for foreign keys 
		class Row_FloorplanType* FK_FloorplanType_getrow();
class Row_DesignObj* FK_DesignObj_Control_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_FloorplanDesignObjType+ m_FK_FloorplanType+ m_Description+ m_Define+ m_Direction+ m_FK_DesignObj_Control+ m_Filename+ m_FillX+ m_FillY+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_FloorplanDesignObjType_asSQL();
string FK_FloorplanType_asSQL();
string Description_asSQL();
string Define_asSQL();
string Direction_asSQL();
string FK_DesignObj_Control_asSQL();
string Filename_asSQL();
string FillX_asSQL();
string FillY_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

