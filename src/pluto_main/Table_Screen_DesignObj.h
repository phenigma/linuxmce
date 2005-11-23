#ifndef __Table_Screen_DesignObj_H__
#define __Table_Screen_DesignObj_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Screen_DesignObj.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Screen_DesignObj : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Screen_DesignObj(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Screen_DesignObj();

private:		
	friend class Row_Screen_DesignObj;
	struct Key
	{
		friend class Row_Screen_DesignObj;
		long int pk_PK_Screen_DesignObj;

		
		Key(long int in_PK_Screen_DesignObj);
	
		Key(class Row_Screen_DesignObj *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Screen_DesignObj::Key &key1, const Table_Screen_DesignObj::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Screen_DesignObj*> *rows);
	class Row_Screen_DesignObj* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Screen_DesignObj* GetRow(long int in_PK_Screen_DesignObj);
	

private:	
	
		
	class Row_Screen_DesignObj* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Screen_DesignObj : public TableRow, public SerializeClass
	{
		friend struct Table_Screen_DesignObj::Key;
		friend class Table_Screen_DesignObj;
	private:
		Table_Screen_DesignObj *table;
		
		long int m_PK_Screen_DesignObj;
long int m_FK_Screen;
long int m_FK_DesignObj;
long int m_FK_UI;
long int m_FK_Skin;
long int m_FK_DeviceTemplate;

		bool is_null[6];
	
	public:
		long int PK_Screen_DesignObj_get();
long int FK_Screen_get();
long int FK_DesignObj_get();
long int FK_UI_get();
long int FK_Skin_get();
long int FK_DeviceTemplate_get();

		
		void PK_Screen_DesignObj_set(long int val);
void FK_Screen_set(long int val);
void FK_DesignObj_set(long int val);
void FK_UI_set(long int val);
void FK_Skin_set(long int val);
void FK_DeviceTemplate_set(long int val);

		
		bool FK_UI_isNull();
bool FK_Skin_isNull();
bool FK_DeviceTemplate_isNull();

			
		void FK_UI_setNull(bool val);
void FK_Skin_setNull(bool val);
void FK_DeviceTemplate_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Screen_DesignObj(Table_Screen_DesignObj *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Screen_DesignObj *Table_Screen_DesignObj_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Screen* FK_Screen_getrow();
class Row_DesignObj* FK_DesignObj_getrow();
class Row_UI* FK_UI_getrow();
class Row_Skin* FK_Skin_getrow();
class Row_DeviceTemplate* FK_DeviceTemplate_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Screen_DesignObj+ m_FK_Screen+ m_FK_DesignObj+ m_FK_UI+ m_FK_Skin+ m_FK_DeviceTemplate;
		}
	private:
		void SetDefaultValues();
		
		string PK_Screen_DesignObj_asSQL();
string FK_Screen_asSQL();
string FK_DesignObj_asSQL();
string FK_UI_asSQL();
string FK_Skin_asSQL();
string FK_DeviceTemplate_asSQL();

	};

#endif

