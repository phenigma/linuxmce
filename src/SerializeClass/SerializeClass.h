/**
 *
 * @file SerializeClass.h
 * @brief header file for the SerializeClass
 * @author
 * @todo notcommented
 *
 */


#ifndef SERIALIZECLASS_H
#define SERIALIZECLASS_H

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/MyStl.h"

#ifndef SYMBIAN
	#include "PlutoUtils/CommonIncludes.h"
	#include <iostream>
	#include <map>
	#include <list>
	using namespace std;
#else
	#include <f32file.h>
	#include <eikenv.h>
	#include <e32std.h>

	#define u_int64_t TInt64
	#define malloc(x) User::Alloc(x)
	#define free(x) User::Free(x)
	#define realloc(x,y) User::ReAlloc(x,y)
	#define memcpy Mem::Copy
#endif

/** @todo check comment */
// Include this define to get debug information if the serialization is ever off
//#define DEBUG_SERIALIZATION


/**
 *
 * @see SerializeClass.txt before using !!!
 *
 */

class PlutoColor;
class PlutoPoint;
class PlutoSize;
class PlutoRectangle;
class PlutoDataBlock
{
public:
	unsigned long m_dwSize;  char *m_pBlock;
	PlutoDataBlock(unsigned long dwSize, char *pBlock) { m_dwSize=dwSize; m_pBlock=pBlock; }
	PlutoDataBlock() { m_dwSize=0; m_pBlock=NULL; }
};

#define BLOCK_SIZE 50000

// serializing constants for the ellementary types
// values over 2000 will be reserved for user objects
#define SERIALIZE_DATA_TYPE_CHAR			1
#define SERIALIZE_DATA_TYPE_SHORT			2
#define SERIALIZE_DATA_TYPE_LONG			3
#define SERIALIZE_DATA_TYPE_UNSIGNED_LONG	4
#define SERIALIZE_DATA_TYPE_INT64			5
#define SERIALIZE_DATA_TYPE_STRING			6
#define SERIALIZE_DATA_TYPE_VECT_STRING		7
#define SERIALIZE_DATA_TYPE_INT_STRING		8
#define SERIALIZE_DATA_TYPE_VECT_INT		9
#define SERIALIZE_DATA_TYPE_STRING_STRING	10

#define SERIALIZE_DATA_TYPE_COLOR			50
#define SERIALIZE_DATA_TYPE_POINT			51
#define SERIALIZE_DATA_TYPE_SIZE			52
#define SERIALIZE_DATA_TYPE_RECTANGLE		53
#define SERIALIZE_DATA_TYPE_BLOCK			54

/**
 * @brief class containing info about the item to serialize
 */
class ItemToSerialize
{

public:

	int m_iSerializeDataType; /** < type of item to serialize. can be user defined (over 2000) or one of the predefined types */
	void *m_pItem; /** < pointer to the item to be serialized */

	/**
	 * @brief basic constructor
	 * @param iSerializeDataType used to initialize the member
	 * @param pItem used to initialize the member
	 */
	ItemToSerialize(int iSerializeDataType, void *pItem)
	{
		m_iSerializeDataType = iSerializeDataType;
		m_pItem = pItem;
	}
};

#ifdef SYMBIAN

#include <e32std.h>
#include "../PlutoUtils/MyString.h"

class SerializeClass : public CBase

#else

class SerializeClass

#endif

{

public:

	char *m_pcDataBlock; /** < the data to be serialized */
	unsigned long m_dwAllocatedSize; /** < how much is allocated for the data */
	char *m_pcCurrentPosition; /** < current position in the DataBlock */
	void *m_pExtraSerializationData;  /** < this is only used in the Serialize method to store misc data */
	unsigned long m_iSC_Version; /** < A schema version.  Set this in your constructor to support multiple schemas. */

	/**
	 * @brief basic constructor
	 */
	SerializeClass()
	{
		m_pcDataBlock = m_pcCurrentPosition = NULL;
		m_iSC_Version = 1;
	}

	/**
	 * @brief destructor
	 * freeing allocated resources
	 */
	virtual ~SerializeClass()
	{
		MYSTL_ITERATE_LIST(m_listItemToSerialize,ItemToSerialize,pItem,it)
		{
			delete pItem;
		}
		// We should not delete m_pcDataBlock because if there are several devices nested that 
		// were serialized, then will all have the same datablock.  It is the responsibility
		// of whoever called SerializeWrite to be sure they delete the datablock
	}

	/**
	 * @brief free datablock - complements the destructor
	 */
	void FreeSerializeMemory() { delete m_pcDataBlock; m_pcDataBlock=m_pcCurrentPosition=NULL; }
	
	/**
	 * @brief computes the current size of processed data
	 * @return how much of the data block was processed
	 */
	unsigned long CurrentSize() { return (unsigned long)(m_pcCurrentPosition - m_pcDataBlock); }

	/**
	 * @brief name of the class to be serialized
	 * @return "**not specified" if not overriten
	 * @warning it's recommended that all classes derived from this override this function so that the debug output will be meaningful
	 */
	virtual string SerializeClassClassName() { return "**not specified"; }

	/**
	 * @brief when the class wants to start writing to a new data block it will call this
	 * used when you want to serialize a class manually
	 */
	void StartWriting()
	{
		// We should not delete m_pcDataBlock because if there are several devices nested that 
		// were serialized, then will all have the same datablock.  It is the responsibility
		// of whoever called SerializeWrite to be sure they delete the datablock.  Just start
		// with a new one
		m_pcDataBlock = new char[BLOCK_SIZE];

#ifndef SYMBIAN
		if(NULL == m_pcDataBlock)
			throw "Cannot allocate memory for m_pcDataBlock";
#endif

		m_dwAllocatedSize = BLOCK_SIZE;
		m_pcCurrentPosition = m_pcDataBlock;
	}


	/**
	 * @brief when the class wants to start reading will call this
	 * used when you want to serialize a class manually
	 */
	void StartReading(unsigned long Size,char *DataPtr)
	{
		m_pcDataBlock = m_pcCurrentPosition = DataPtr;
		m_dwAllocatedSize = Size;
	}

	// The various AddToAutoSerializeList functions for all the supported types

	/**
	 * @brief starts the list of items to be serialized
	 * should be used like: StartSerializeList() + ....
	 * @return the conversion is used in order to get the right methods called and not the overloaded ones
	 */
	SerializeClass &StartSerializeList() { return (*( (SerializeClass *) this)); };

	// Create these for easily adding items to the list.  Now a constructor can just
	// do (*this) + m_iMyInt + m_sMyString + m_bMyBool, etc.

	SerializeClass &operator+ (int &i) {  MYSTL_ADDTO_LIST(m_listItemToSerialize, new ItemToSerialize(SERIALIZE_DATA_TYPE_LONG, (void *) &i)); return (*this); } /** < @brief overloading + to take an int */
	SerializeClass &operator+ (long &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new ItemToSerialize(SERIALIZE_DATA_TYPE_LONG,(void *) &i));  return (*this); } /** < @brief overloading + to take an long */
	SerializeClass &operator+ (unsigned long &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_UNSIGNED_LONG,(void *) &i)); return (*this); } /** < @brief overloading + to take an unsigned int */
	SerializeClass &operator+ (bool &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_CHAR,(void *) &i)); return (*this); } /** < @brief overloading + to take an bool */
	SerializeClass &operator+ (char &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_CHAR,(void *) &i)); return (*this); } /** < @brief overloading + to take an char */
	SerializeClass &operator+ (short &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_SHORT,(void *) &i)); return (*this); } /** < @brief overloading + to take an short */
	SerializeClass &operator+ (string &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_STRING,(void *) &i)); return (*this); } /** < @brief overloading + to take a string */
	SerializeClass &operator+ (u_int64_t &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_INT64,(void *) &i)); return (*this); } /** < @brief overloading + to take an u_int64_t */

#ifndef SYMBIAN
	SerializeClass &operator+ (vector<string> &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_VECT_STRING,(void *) &i)); return (*this); } /** < @brief overloading + to take a vector of strings */
	SerializeClass &operator+ (map<int,string> &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT_STRING,(void *) &i)); return (*this); } /** < @brief overloading + to take a map<int,string> */
	SerializeClass &operator+ (map<string,string> &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_STRING_STRING,(void *) &i)); return (*this); } /** < @brief overloading + to take a map<string,string> */
	SerializeClass &operator+ (vector<int> &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_VECT_INT,(void *) &i)); return (*this); } /** < @brief overloading + to take a map<int,string> */
#endif

	SerializeClass &operator+ (PlutoColor &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_COLOR,(void *) &i)); return (*this); } /** < @brief overloading + to take a PlutoColor object */
	SerializeClass &operator+ (PlutoPoint &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_POINT,(void *) &i)); return (*this); } /** < @brief overloading + to take a PlutoPoint object */
	SerializeClass &operator+ (PlutoSize &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_SIZE,(void *) &i)); return (*this); } /** < @brief overloading + to take an PlutoSize object */
	SerializeClass &operator+ (PlutoRectangle &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_RECTANGLE,(void *) &i)); return (*this); } /** < @brief overloading + to take a PlutoRectangle object*/
	SerializeClass &operator+ (PlutoDataBlock &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_BLOCK,(void *) &i)); return (*this); } /** < @brief overloading + to take a PlutoDataBlock object*/
	
	/**
	 * @brief serializes data to be written
	 * @todo write something about the params
	 *
	 */
	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);
	
	/**
	 * @brief serialize writes the data to memory block
	 */
	bool SerializeWrite(void *pExtraSerializationData=NULL)
	{
		StartWriting();
		return Serialize(true, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition, pExtraSerializationData);
	}

	/**
	 * @brief serialize reads data from memory block
	 */
	bool SerializeRead(unsigned long Size,char *DataPtr,void *pExtraSerializationData=NULL)
	{
		StartReading(Size,DataPtr);
		return Serialize(false, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition, pExtraSerializationData);
	}

	/**
	 * @brief serialize writes the data to disk file
	 */
	bool SerializeWrite(string sFilename, void *pExtraSerializationData=NULL)
	{
		bool bResult;

#ifndef SYMBIAN
		FILE *pFile = fopen(sFilename.c_str(),"wb");
		if( !pFile )
			return false;

		bResult = SerializeWrite( pExtraSerializationData );
		if( fwrite(m_pcDataBlock, 1, CurrentSize(), pFile) != CurrentSize() )
			bResult=false;

		fclose( pFile );
#else
		RFs aFs;
		aFs.Connect();
		
		RFile file;
		file.Open(aFs, sFilename.Des(), EFileShareReadersOnly | EFileStream | EFileWrite);

		bResult=SerializeWrite(pExtraSerializationData);

		char *Buff = (char *)pExtraSerializationData;

		HBufC8 *base_str = HBufC8::NewL(CurrentSize());
		TPtr8 pStr = base_str->Des();

		for (int i = 0; i < CurrentSize(); i++)
			pStr.Append(TChar(Buff[i]));

		if ( KErrNone !=  file.Write(pStr, CurrentSize()) )
			bResult = false;

		delete base_str;
		base_str = NULL; /** @todo comment */	

		file.Close();
		aFs.Close();
#endif
		return bResult;
	}

	/**
	 * @brief serialize reads data from disk file
	 */
	bool SerializeRead( string sFilename, void *pExtraSerializationData=NULL );

	/**
	 * @brief override this if you are adding other objects in your derived class
	 */
	virtual bool UnknownSerialize(ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition) { return false; } 

	/**
	 * @brief override this with your own list of members to serialize.  Don't forget to call your base class too
	 */
	virtual void SetupSerialization(int iSC_Version) {}; 

	/**
	 * @brief Used for classes that know how to serialize themselves.  Only available in the base class
	 * @see the source file SerialiseClass.cpp
	 */
	bool SelfSerializing(bool bWriting, ItemToSerialize *pItem);

	/**
	 * @brief when the class wants to continue reading/writing it will call this
	 */
	void ResumeReadWrite(char *pcDataBlock, unsigned long dwAllocatedSize, char *pcCurrentPosition)
	{
		m_pcDataBlock = pcDataBlock;
		m_dwAllocatedSize = dwAllocatedSize;
		m_pcCurrentPosition = pcCurrentPosition;
	}

	/**
	 * @todo ask Aaron
	 */
	void CheckWrite(unsigned long dwSize)
	{
		unsigned long dwCurrentSize = (unsigned long) (m_pcCurrentPosition - m_pcDataBlock);
		if( m_dwAllocatedSize - dwCurrentSize < dwSize )
		{
			m_pcDataBlock = (char *) realloc(m_pcDataBlock, m_dwAllocatedSize + dwSize + BLOCK_SIZE);
			m_dwAllocatedSize += dwSize + BLOCK_SIZE;
			m_pcCurrentPosition = m_pcDataBlock + dwCurrentSize;
		}
	}

	/**
	 * @brief makes sure the reading was succesfull
	 */
	bool CheckRead(unsigned long dwSize)
	{
		unsigned long dwCurrentSize = (unsigned long) (m_pcCurrentPosition - m_pcDataBlock);
		return  !(m_dwAllocatedSize - dwCurrentSize < dwSize);
	}

	void Write_unsigned_int64(u_int64_t v) {
		CheckWrite(sizeof(u_int64_t));
		u_int64_t *pl = (u_int64_t *) m_pcCurrentPosition;
		*pl = v;
		m_pcCurrentPosition += sizeof(u_int64_t);
	}

	void Write_int64( u_int64_t v) {
		CheckWrite(sizeof( u_int64_t));
		 u_int64_t *pl = ( u_int64_t *) m_pcCurrentPosition;
		*pl = v;
		m_pcCurrentPosition += sizeof( u_int64_t);
	}

	void Write_unsigned_long(unsigned long v) {
		CheckWrite(sizeof(unsigned long));
		unsigned long *pl = (unsigned long *) m_pcCurrentPosition;

#ifndef WINCE
		*pl = v;
#else
		//try to avoid "Datatype misalignment" exception
		((unsigned char *)pl)[3] = (v >> 24)	& 0x000000FF;
		((unsigned char *)pl)[2] = (v >> 16)	& 0x000000FF;
		((unsigned char *)pl)[1] = (v >> 8)		& 0x000000FF;
		((unsigned char *)pl)[0] = v			& 0x000000FF;
#endif
		m_pcCurrentPosition += sizeof(unsigned long);
	}

	void Write_unsigned_short(unsigned short v) {
		CheckWrite(sizeof(unsigned short));
		unsigned short *ps = (unsigned short *) m_pcCurrentPosition;
		*ps = v;
		m_pcCurrentPosition += sizeof(unsigned short);
	}

	void Write_unsigned_char(unsigned char v) {
		CheckWrite(sizeof(unsigned char));
		unsigned char *ps = (unsigned char *) m_pcCurrentPosition;
		*ps = v;
		m_pcCurrentPosition += sizeof(unsigned char);
	}

	void Write_char(char v) {
		CheckWrite(sizeof(char));
		char *ps = (char *) m_pcCurrentPosition;
		*ps = v;
		m_pcCurrentPosition += sizeof(char);
	}

	void Write_long(long v) {
#ifdef DEBUG_SERIALIZATION
		cout << "Writing long " << v << " size: " << (int) CurrentSize() << endl;
#endif
		CheckWrite(sizeof(long));
		 long *pl = (long *) m_pcCurrentPosition;

#ifndef WINCE
		*pl = v;
#else
		//try to avoid "Datatype misalignment" exception
		((unsigned char *)pl)[3] = (v >> 24)	& 0x000000FF;
		((unsigned char *)pl)[2] = (v >> 16)	& 0x000000FF;
		((unsigned char *)pl)[1] = (v >> 8)		& 0x000000FF;
		((unsigned char *)pl)[0] = v			& 0x000000FF;
#endif


		m_pcCurrentPosition += sizeof(long);
	}

	void Write_short(short v) {
		CheckWrite(sizeof(short));
		 short *ps = (short *) m_pcCurrentPosition;
		*ps = v;
		m_pcCurrentPosition += sizeof(short);
	}

	void Write_string(string &str)
	{
#ifdef DEBUG_SERIALIZATION
		cout << "Writing string " << str << " size: " << (int) CurrentSize() << endl;
#endif
#ifndef SYMBIAN
		CheckWrite((unsigned long) str.length()+1);
		strcpy(m_pcCurrentPosition,str.c_str());
		m_pcCurrentPosition += str.length()+1;
#else
		CheckWrite((unsigned long) str.length()+1);
	    int i = 0;
		int len = str.length();
		while ( i < len ) 
		{
		    *(m_pcCurrentPosition++)= (char) str[i];
			 i++;
		}
	   *(m_pcCurrentPosition++) = 0;
#endif
	}

	void Write_block(char *ptr,unsigned long size)
	{
		CheckWrite((unsigned long) size);

		memcpy(m_pcCurrentPosition,ptr,size);
		m_pcCurrentPosition += size;
	}

	u_int64_t Read_unsigned_int64() {
		if( !CheckRead(sizeof(u_int64_t)) )
			return 0;
		u_int64_t *pl = (u_int64_t *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(u_int64_t);
		return *pl;
	}

	 u_int64_t Read_int64() {
		if( !CheckRead(sizeof( u_int64_t)) )
			return 0;
		 u_int64_t *pl = ( u_int64_t *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof( u_int64_t);
		return *pl;
	}

	unsigned long Read_unsigned_long() {

		/** @todo THIS */
		/*
		if( !CheckRead(sizeof(unsigned long)) )
			return 0;
		unsigned long *pl = (unsigned long *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(unsigned long);
		return *pl;
		*/

		return Read_long();
	}

	unsigned short Read_unsigned_short() {
		if( !CheckRead(sizeof(unsigned short)) )
			return 0;
		unsigned short *pl = (unsigned short *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(unsigned short);
		return *pl;
	}

	unsigned char Read_unsigned_char() {
		if( !CheckRead(sizeof(unsigned char)) )
			return 0;
		unsigned char *pl = (unsigned char *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(unsigned char);
		return *pl;
	}

	char Read_char() {
		if( !CheckRead(sizeof(char)) )
			return 0;
		char *pl = (char *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(char);
		return *pl;
	}

	long Read_long() {
		if( !CheckRead(sizeof(long)) )
			return 0;
		long *pl = (long *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(long);

		/** @todo this */	
		unsigned char v1 = ((unsigned char *)pl)[0];
		unsigned char v2 = ((unsigned char *)pl)[1];
		unsigned char v3 = ((unsigned char *)pl)[2];
		unsigned char v4 = ((unsigned char *)pl)[3];

		long myval2 = (v4 << 24) + (v3 << 16) + (v2 << 8) + v1;

		/** @todo check comment */
		//long myval = long(*pl);

#ifdef DEBUG_SERIALIZATION
		cout << "Reading long " << ((long) myval2) << " size: " << (int) CurrentSize() << endl;
#endif
		//return (long)*pl;
		return myval2;
	}

	short Read_short() {
		if( !CheckRead(sizeof(short)) )
			return 0;
		short *pl = (short *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(short);
		return *pl;
	}

	void Read_string(string &str)
	{
		str.SetLength(0);
		while ( m_pcCurrentPosition < m_pcDataBlock + m_dwAllocatedSize && *m_pcCurrentPosition) {
			 str.Append(*m_pcCurrentPosition++);
		}

		m_pcCurrentPosition++; //also skip '\0'

/*

#ifndef SYMBIAN
		// Don't bother checking
		str = m_pcCurrentPosition;
		m_pcCurrentPosition += str.length()+1;
#else
		str.SetLength(0);
		while ( *m_pcCurrentPosition ) 
		{
			 str.Append(*m_pcCurrentPosition++);
		}

		m_pcCurrentPosition++; //also skip '\0'
#endif

*/

#ifdef DEBUG_SERIALIZATION
		cout << "Reading string " << str << " size: " << (int) CurrentSize() << endl;
#endif
	}

	char *Read_block(unsigned long size)
	{
		if( !CheckRead(size) )
			return NULL;

		char *ptr = new char[size];
		memcpy(ptr,m_pcCurrentPosition,size);
		m_pcCurrentPosition += size;
		return ptr;
	}

protected:

	/**
     * @brief for Symbian compatibility; see Symbian.txt for detailes
     */
	MYSTL_CREATE_LIST(m_listItemToSerialize, ItemToSerialize);
};

#endif
