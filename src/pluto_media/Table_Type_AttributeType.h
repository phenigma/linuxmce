#ifndef __Table_Type_AttributeType_H__
#define __Table_Type_AttributeType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Type_AttributeType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Type_AttributeType : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_Type_AttributeType(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_Type_AttributeType();

private:		
	friend class Row_Type_AttributeType;
	struct Key
	{
		friend class Row_Type_AttributeType;
		long int pk_FK_Type;
long int pk_FK_AttributeType;

		
		Key(long int in_FK_Type, long int in_FK_AttributeType);
	
		Key(class Row_Type_AttributeType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Type_AttributeType::Key &key1, const Table_Type_AttributeType::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Type_AttributeType*> *rows);
	class Row_Type_AttributeType* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Type_AttributeType* GetRow(long int in_FK_Type, long int in_FK_AttributeType);
	

private:	
	
		
	class Row_Type_AttributeType* FetchRow(DoubleLongKey &key);
		
			
};

class DLL_EXPORT Row_Type_AttributeType : public TableRow, public SerializeClass
	{
		friend struct Table_Type_AttributeType::Key;
		friend class Table_Type_AttributeType;
	private:
		Table_Type_AttributeType *table;
		
		long int m_FK_Type;
long int m_FK_AttributeType;
short int m_Identifier;
short int m_CombineAsOne;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int FK_Type_get();
long int FK_AttributeType_get();
short int Identifier_get();
short int CombineAsOne_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_Type_set(long int val);
void FK_AttributeType_set(long int val);
void Identifier_set(short int val);
void CombineAsOne_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool CombineAsOne_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void CombineAsOne_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Type_AttributeType(Table_Type_AttributeType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Type_AttributeType *Table_Type_AttributeType_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Type* FK_Type_getrow();
class Row_AttributeType* FK_AttributeType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Type+ m_FK_AttributeType+ m_Identifier+ m_CombineAsOne+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_Type_asSQL();
string FK_AttributeType_asSQL();
string Identifier_asSQL();
string CombineAsOne_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

