#ifndef __Table_AttributeType_H__
#define __Table_AttributeType_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_AttributeType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_AttributeType : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_AttributeType(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_AttributeType();

private:		
	friend class Row_AttributeType;
	struct Key
	{
		friend class Row_AttributeType;
		long int pk_PK_AttributeType;

		
		Key(long int in_PK_AttributeType);
	
		Key(class Row_AttributeType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_AttributeType::Key &key1, const Table_AttributeType::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_AttributeType*> *rows);
	class Row_AttributeType* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_AttributeType* GetRow(long int in_PK_AttributeType);
	

private:	
	
		
	class Row_AttributeType* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_AttributeType : public TableRow, public SerializeClass
	{
		friend struct Table_AttributeType::Key;
		friend class Table_AttributeType;
	private:
		Table_AttributeType *table;
		
		long int m_PK_AttributeType;
string m_Description;
string m_Define;
long int m_PicPriority;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_AttributeType_get();
string Description_get();
string Define_get();
long int PicPriority_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_AttributeType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void PicPriority_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Define_isNull();
bool PicPriority_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Define_setNull(bool val);
void PicPriority_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_AttributeType(Table_AttributeType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_AttributeType *Table_AttributeType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Attribute_FK_AttributeType_getrows(vector <class Row_Attribute*> *rows);
void Type_AttributeType_FK_AttributeType_getrows(vector <class Row_Type_AttributeType*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_AttributeType+ m_Description+ m_Define+ m_PicPriority+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_AttributeType_asSQL();
string Description_asSQL();
string Define_asSQL();
string PicPriority_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

