#ifndef __Table_DHCPDevice_H__
#define __Table_DHCPDevice_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DHCPDevice.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DHCPDevice : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DHCPDevice(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DHCPDevice();

private:		
	friend class Row_DHCPDevice;
	struct Key
	{
		friend class Row_DHCPDevice;
		long int pk_PK_DHCPDevice;

		
		Key(long int in_PK_DHCPDevice);
	
		Key(class Row_DHCPDevice *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DHCPDevice::Key &key1, const Table_DHCPDevice::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_DHCPDevice*> *rows);
	class Row_DHCPDevice* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DHCPDevice* GetRow(long int in_PK_DHCPDevice);
	

private:	
	
		
	class Row_DHCPDevice* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DHCPDevice : public TableRow, public SerializeClass
	{
		friend struct Table_DHCPDevice::Key;
		friend class Table_DHCPDevice;
	private:
		Table_DHCPDevice *table;
		
		long int m_PK_DHCPDevice;
long int m_FK_DeviceTemplate;
u_int64_t m_Mac_Range_Low;
u_int64_t m_Mac_Range_High;
long int m_FK_DeviceCategory;
long int m_FK_Manufacturer;
string m_Description;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[12];
	
	public:
		long int PK_DHCPDevice_get();
long int FK_DeviceTemplate_get();
u_int64_t Mac_Range_Low_get();
u_int64_t Mac_Range_High_get();
long int FK_DeviceCategory_get();
long int FK_Manufacturer_get();
string Description_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_DHCPDevice_set(long int val);
void FK_DeviceTemplate_set(long int val);
void Mac_Range_Low_set(u_int64_t val);
void Mac_Range_High_set(u_int64_t val);
void FK_DeviceCategory_set(long int val);
void FK_Manufacturer_set(long int val);
void Description_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_DeviceTemplate_isNull();
bool FK_DeviceCategory_isNull();
bool FK_Manufacturer_isNull();
bool Description_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_DeviceTemplate_setNull(bool val);
void FK_DeviceCategory_setNull(bool val);
void FK_Manufacturer_setNull(bool val);
void Description_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DHCPDevice(Table_DHCPDevice *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DHCPDevice *Table_DHCPDevice_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_DeviceCategory* FK_DeviceCategory_getrow();
class Row_Manufacturer* FK_Manufacturer_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DHCPDevice_DeviceData_FK_DHCPDevice_getrows(vector <class Row_DHCPDevice_DeviceData*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DHCPDevice+ m_FK_DeviceTemplate+ m_Mac_Range_Low+ m_Mac_Range_High+ m_FK_DeviceCategory+ m_FK_Manufacturer+ m_Description+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_DHCPDevice_asSQL();
string FK_DeviceTemplate_asSQL();
string Mac_Range_Low_asSQL();
string Mac_Range_High_asSQL();
string FK_DeviceCategory_asSQL();
string FK_Manufacturer_asSQL();
string Description_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

