#ifndef __Table_Icon_H__
#define __Table_Icon_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Icon.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Icon
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Icon(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Icon")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Icon();

private:		
	friend class Row_Icon;
	struct Key
	{
		friend class Row_Icon;
		long int pk_PK_Icon;

		
		Key(long int in_PK_Icon);
	
		Key(class Row_Icon *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Icon::Key &key1, const Table_Icon::Key &key2) const;
	};	

	map<Table_Icon::Key, class Row_Icon*, Table_Icon::Key_Less> cachedRows;
	map<Table_Icon::Key, class Row_Icon*, Table_Icon::Key_Less> deleted_cachedRows;
	vector<class Row_Icon*> addedRows;
	vector<class Row_Icon*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Icon*> *rows);
	class Row_Icon* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Icon* GetRow(long int in_PK_Icon);
	

private:	
	
		
	class Row_Icon* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Icon : public TableRow, public SerializeClass
	{
		friend struct Table_Icon::Key;
		friend class Table_Icon;
	private:
		Table_Icon *table;
		
		long int m_PK_Icon;
string m_Define;
string m_Description;
long int m_TransparentColor;
string m_MainFileName;
string m_SelectedFileName;
string m_AltFileNames;
string m_BackgroundFileName;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[12];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Icon_get();
string Define_get();
string Description_get();
long int TransparentColor_get();
string MainFileName_get();
string SelectedFileName_get();
string AltFileNames_get();
string BackgroundFileName_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Icon_set(long int val);
void Define_set(string val);
void Description_set(string val);
void TransparentColor_set(long int val);
void MainFileName_set(string val);
void SelectedFileName_set(string val);
void AltFileNames_set(string val);
void BackgroundFileName_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool TransparentColor_isNull();
bool SelectedFileName_isNull();
bool AltFileNames_isNull();
bool BackgroundFileName_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void TransparentColor_setNull(bool val);
void SelectedFileName_setNull(bool val);
void AltFileNames_setNull(bool val);
void BackgroundFileName_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Icon(Table_Icon *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Icon *Table_Icon_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_FK_Icon_getrows(vector <class Row_CommandGroup*> *rows);
void Floorplan_FK_Icon_getrows(vector <class Row_Floorplan*> *rows);
void Room_FK_Icon_getrows(vector <class Row_Room*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Icon+ m_Define+ m_Description+ m_TransparentColor+ m_MainFileName+ m_SelectedFileName+ m_AltFileNames+ m_BackgroundFileName+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Icon_asSQL();
string Define_asSQL();
string Description_asSQL();
string TransparentColor_asSQL();
string MainFileName_asSQL();
string SelectedFileName_asSQL();
string AltFileNames_asSQL();
string BackgroundFileName_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

