#ifndef __Table_Style_H__
#define __Table_Style_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Style.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Style
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Style(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Style")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Style();

private:		
	friend class Row_Style;
	struct Key
	{
		friend class Row_Style;
		long int pk_PK_Style;

		
		Key(long int in_PK_Style);
	
		Key(class Row_Style *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Style::Key &key1, const Table_Style::Key &key2) const;
	};	

	map<Table_Style::Key, class Row_Style*, Table_Style::Key_Less> cachedRows;
	map<Table_Style::Key, class Row_Style*, Table_Style::Key_Less> deleted_cachedRows;
	vector<class Row_Style*> addedRows;
	vector<class Row_Style*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Style*> *rows);
	class Row_Style* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Style* GetRow(long int in_PK_Style);
	

private:	
	
		
	class Row_Style* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Style : public TableRow, public SerializeClass
	{
		friend struct Table_Style::Key;
		friend class Table_Style;
	private:
		Table_Style *table;
		
		long int m_PK_Style;
string m_Description;
long int m_FK_Style_Selected;
long int m_FK_Style_Highlighted;
long int m_FK_Style_Alt;
short int m_AlwaysIncludeOnOrbiter;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[10];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Style_get();
string Description_get();
long int FK_Style_Selected_get();
long int FK_Style_Highlighted_get();
long int FK_Style_Alt_get();
short int AlwaysIncludeOnOrbiter_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Style_set(long int val);
void Description_set(string val);
void FK_Style_Selected_set(long int val);
void FK_Style_Highlighted_set(long int val);
void FK_Style_Alt_set(long int val);
void AlwaysIncludeOnOrbiter_set(short int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Description_isNull();
bool FK_Style_Selected_isNull();
bool FK_Style_Highlighted_isNull();
bool FK_Style_Alt_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Description_setNull(bool val);
void FK_Style_Selected_setNull(bool val);
void FK_Style_Highlighted_setNull(bool val);
void FK_Style_Alt_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Style(Table_Style *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Style *Table_Style_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Style* FK_Style_Selected_getrow();
class Row_Style* FK_Style_Highlighted_getrow();
class Row_Style* FK_Style_Alt_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_Text_Skin_Language_FK_Style_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows);
void Skin_FK_Style_getrows(vector <class Row_Skin*> *rows);
void Style_FK_Style_Selected_getrows(vector <class Row_Style*> *rows);
void Style_FK_Style_Highlighted_getrows(vector <class Row_Style*> *rows);
void Style_FK_Style_Alt_getrows(vector <class Row_Style*> *rows);
void StyleVariation_FK_Style_getrows(vector <class Row_StyleVariation*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Style+ m_Description+ m_FK_Style_Selected+ m_FK_Style_Highlighted+ m_FK_Style_Alt+ m_AlwaysIncludeOnOrbiter+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Style_asSQL();
string Description_asSQL();
string FK_Style_Selected_asSQL();
string FK_Style_Highlighted_asSQL();
string FK_Style_Alt_asSQL();
string AlwaysIncludeOnOrbiter_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

