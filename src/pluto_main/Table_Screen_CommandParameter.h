#ifndef __Table_Screen_CommandParameter_H__
#define __Table_Screen_CommandParameter_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Screen_CommandParameter.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Screen_CommandParameter : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Screen_CommandParameter(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Screen_CommandParameter();

private:		
	friend class Row_Screen_CommandParameter;
	struct Key
	{
		friend class Row_Screen_CommandParameter;
		long int pk_FK_Screen;
long int pk_FK_CommandParameter;

		
		Key(long int in_FK_Screen, long int in_FK_CommandParameter);
	
		Key(class Row_Screen_CommandParameter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Screen_CommandParameter::Key &key1, const Table_Screen_CommandParameter::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Screen_CommandParameter*> *rows);
	class Row_Screen_CommandParameter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Screen_CommandParameter* GetRow(long int in_FK_Screen, long int in_FK_CommandParameter);
	

private:	
	
		
	class Row_Screen_CommandParameter* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Screen_CommandParameter : public TableRow, public SerializeClass
	{
		friend struct Table_Screen_CommandParameter::Key;
		friend class Table_Screen_CommandParameter;
	private:
		Table_Screen_CommandParameter *table;
		
		long int m_FK_Screen;
long int m_FK_CommandParameter;
string m_Description;

		bool is_null[3];
	
	public:
		long int FK_Screen_get();
long int FK_CommandParameter_get();
string Description_get();

		
		void FK_Screen_set(long int val);
void FK_CommandParameter_set(long int val);
void Description_set(string val);

		
		bool Description_isNull();

			
		void Description_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Screen_CommandParameter(Table_Screen_CommandParameter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Screen_CommandParameter *Table_Screen_CommandParameter_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Screen* FK_Screen_getrow();
class Row_CommandParameter* FK_CommandParameter_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Screen+ m_FK_CommandParameter+ m_Description;
		}
	private:
		void SetDefaultValues();
		
		string FK_Screen_asSQL();
string FK_CommandParameter_asSQL();
string Description_asSQL();

	};

#endif

