#ifndef SerializeClass_H
#define SerializeClass_H

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#ifndef SYMBIAN
	#include "PlutoUtils/CommonIncludes.h"	
	#include <iostream>
	#include <map>
	#include <list>
	using namespace std;
#endif

// Include this define to get debug information if the serialization is ever off
//#define DEBUG_SERIALIZATION

/*

Derive a class you want to serialize from this class, and then use it in a combination of 2 ways.  
You can call StartReading/StartWriting and then call all the Read_/Write_
functions directly.  This will allocate a block of memory, pointed to by m_pcDataBlock, and CurrentSize() will always have the size.

Or, this can be done automatically.  Add the members that you want to have serialized when the object is created, either
by calling to AddToAutoSerializeList(...), or with the syntax: 
StartSerializeList() + m_iMember1 + m_vectorMember2, etc.  (see operator +)

Call SerializeWrite and SerializeRead to serialize the data members.  Pass in a filename to have them save to the file.

If your derived class has objects that are not of a standard type that SerializeClass knows how to handle, you must create
your own methods to add them to the serialize list, like this a typedef list<DesignObj_Data *> DesignObj_DataList object:

#define SERIALIZE_DATA_TYPE_INT_DESIGNOBJCHILDLIST	1000	// Be sure this doesn't conflict with any other class in the tree
void AddToAutoSerializeList(DesignObj_DataList &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT_DESIGNOBJCHILDLIST,(void *) &i)); }
DesignObj_Data &operator+ (DesignObjCommandList &i) { AddToAutoSerializeList(i); return (*this); }

Then, implement the UnknownSerialize method, like this:

virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition) s
{ 
	m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;
	if( bWriting )
	{	
		switch(pItem->m_iSerializeDataType)
		{
		case SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST:
			{
				DesignObjCommandList *pList = (DesignObjCommandList *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pList->size());
				for(DesignObjCommandList::iterator it=pList->begin();it!=pList->end();++it)
				{
					(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				}
				return true;  // We handled it
			}
			break;
	}
	else
	{
		switch(pItem->m_iSerializeDataType)
		{
		case SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST:
			{
				DesignObjCommandList *pList = (DesignObjCommandList *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for(unsigned long i=0;i<count;++i)
				{
					DesignObjCommand *pAction = new DesignObjCommand();
                    pAction->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					pList->push_back(pAction);
				}
				return true;  // We handled it
			}
			break;
	}

*/

class PlutoColor;
class PlutoPoint;
class PlutoSize;
class PlutoRectangle;

#define BLOCK_SIZE 50000

#include "../VIPShared/VIPIncludes.h"

#define SERIALIZE_DATA_TYPE_CHAR			1
#define SERIALIZE_DATA_TYPE_SHORT			2
#define SERIALIZE_DATA_TYPE_LONG			3
#define SERIALIZE_DATA_TYPE_UNSIGNED_LONG	4
#define SERIALIZE_DATA_TYPE_INT64			5
#define SERIALIZE_DATA_TYPE_STRING			6
#define SERIALIZE_DATA_TYPE_VECT_STRING		7
#define SERIALIZE_DATA_TYPE_INT_STRING		8
#define SERIALIZE_DATA_TYPE_COLOR			9
#define SERIALIZE_DATA_TYPE_POINT			10
#define SERIALIZE_DATA_TYPE_SIZE			11
#define SERIALIZE_DATA_TYPE_RECTANGLE		12

class ItemToSerialize
{
public:
	int m_iSerializeDataType;
	void *m_pItem;

	ItemToSerialize(int iSerializeDataType,void *pItem)
	{
		m_iSerializeDataType=iSerializeDataType;
		m_pItem=pItem;
	}
};

#ifdef SYMBIAN
#include <e32std.h>
#include "../VIPShared/MyString.h"

class SerializeClass : public CBase
#else
class SerializeClass
#endif
{
protected:
	MYSTL_CREATE_LIST(m_listItemToSerialize,ItemToSerialize);

public:
	SerializeClass()
	{
		m_pcDataBlock=m_pcCurrentPosition=NULL;
	}
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

	char *m_pcDataBlock;
	unsigned long m_dwAllocatedSize;
	char *m_pcCurrentPosition;
	
	void *m_pExtraSerializationData;  // This is only used in the Serialize method to store misc data
	
	unsigned long CurrentSize() { return (unsigned long) (m_pcCurrentPosition-m_pcDataBlock); }

	// It's recommended that all classes derived from this override this function so that the debug output will be meaningful
	virtual string SerializeClassClassName() { return "**not specified"; }

	// When the class wants to start writing to a new data block it will call this
	void StartWriting()
	{
		// We should not delete m_pcDataBlock because if there are several devices nested that 
		// were serialized, then will all have the same datablock.  It is the responsibility
		// of whoever called SerializeWrite to be sure they delete the datablock.  Just start
		// with a new one
		m_pcDataBlock = (char *)malloc(BLOCK_SIZE);
		m_dwAllocatedSize = BLOCK_SIZE;
		m_pcCurrentPosition = m_pcDataBlock;
	}

	void StartReading(unsigned long Size,char *DataPtr)
	{
		m_pcDataBlock = m_pcCurrentPosition = DataPtr;
		m_dwAllocatedSize = Size;
	}
	void FreeSerializeMemory() { delete m_pcDataBlock; m_pcDataBlock=m_pcCurrentPosition=NULL; }

	// The various AddToAutoSerializeList functions for all the supported types
	SerializeClass &StartSerializeList() { return (*( (SerializeClass *) this)); };

	// Create these for easily adding items to the list.  Now a constructor can just
	// do (*this) + m_iMyInt + m_sMyString + m_bMyBool, etc.
	SerializeClass &operator+ (int &i) {  MYSTL_ADDTO_LIST(m_listItemToSerialize, new ItemToSerialize(SERIALIZE_DATA_TYPE_LONG, (void *) &i)); return (*this); }
	SerializeClass &operator+ (long &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new ItemToSerialize(SERIALIZE_DATA_TYPE_LONG,(void *) &i));  return (*this); }
	SerializeClass &operator+ (unsigned long &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_UNSIGNED_LONG,(void *) &i)); return (*this); }
	SerializeClass &operator+ (bool &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_CHAR,(void *) &i)); return (*this); }
	SerializeClass &operator+ (char &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_CHAR,(void *) &i)); return (*this); }
	SerializeClass &operator+ (short &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_SHORT,(void *) &i)); return (*this); }
	SerializeClass &operator+ (u_int64_t &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_INT64,(void *) &i)); return (*this); }
	SerializeClass &operator+ (string &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_STRING,(void *) &i)); return (*this); }

#ifndef SYMBIAN
	SerializeClass &operator+ (vector<string> &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_VECT_STRING,(void *) &i)); return (*this); }
	SerializeClass &operator+ (map<int,string> &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT_STRING,(void *) &i)); return (*this); }
#endif

	SerializeClass &operator+ (PlutoColor &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_COLOR,(void *) &i)); return (*this); }
	SerializeClass &operator+ (PlutoPoint &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_POINT,(void *) &i)); return (*this); }
	SerializeClass &operator+ (PlutoSize &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_SIZE,(void *) &i)); return (*this); }
	SerializeClass &operator+ (PlutoRectangle &i) { MYSTL_ADDTO_LIST(m_listItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_RECTANGLE,(void *) &i)); return (*this); }

	bool SerializeWrite(void *pExtraSerializationData=NULL)
	{
		StartWriting();
		return Serialize(true,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
	}

	bool SerializeRead(unsigned long Size,char *DataPtr,void *pExtraSerializationData=NULL)
	{
		if( !DataPtr || !Size )
			return false;

		StartReading(Size,DataPtr);
		return Serialize(false,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
	}

	bool SerializeWrite(string sFilename,void *pExtraSerializationData=NULL)
	{
		bool bResult;

#ifndef SYMBIAN
		FILE *file = fopen(sFilename.c_str(),"wb");
		if( !file )
			return false;

		bResult=SerializeWrite(pExtraSerializationData);
		if( fwrite(m_pcDataBlock,1,CurrentSize(),file)!=CurrentSize() )
			bResult=false;

		fclose(file);
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
		base_str = NULL;		

		file.Close();
		aFs.Close();

#endif
		return bResult;
	}

	bool SerializeRead(string sFilename,void *pExtraSerializationData=NULL)
	{
#ifndef SYMBIAN
		long lSize = FileUtils::FileSize(sFilename.c_str());
		void *buf = malloc(lSize);

		FILE *file = fopen(sFilename.c_str(),"rb");
		if( !file )
			return false;

		fread(buf,1,lSize,file);
		fclose(file);
#else
		RFs aFs;
		aFs.Connect();
		
		RFile file;
		file.Open(aFs, sFilename.Des(), EFileShareReadersOnly | EFileStream | EFileRead);
		
		TInt lSize;
		file.Size(lSize);

		HBufC8* Buffer = HBufC8::NewL(lSize);
		TPtr8 pStr = Buffer->Des();

		if( KErrNone != file.Read(pStr, lSize))
			return false;

		char *buf = new char[lSize];

		for (int i = 0; i < lSize; i++)
			buf[i] = (char)pStr[i];

		file.Close();
		aFs.Close();

		delete Buffer;
		Buffer = NULL;
#endif

		return SerializeRead(lSize,(char *) buf,pExtraSerializationData);
	}


	// override this if you are adding other objects in your derived class
	virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition) { return false; } 

	// override this with your own list of members to serialize.  Don't forget to call your base class too
	virtual void SetupSerialization() {}; 

	// An easy way to add self-serializing classes 
	bool SelfSerializing(bool bWriting,ItemToSerialize *pItem); 

	virtual bool Serialize(bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition,void *pExtraSerializationData=NULL)
	{
		// Save the starting points
		m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;

		// In some cases a class may need extra data to properly serialize in UnknownSerialize.  This is an extra void pointer that can be cast to something useful
		m_pExtraSerializationData=pExtraSerializationData;

		MYSTL_ITERATE_LIST(m_listItemToSerialize,ItemToSerialize,pItem_del,it_del)
		{
			delete pItem_del;
		}
		MYSTL_CLEAR_LIST(m_listItemToSerialize);

		SetupSerialization();

#ifdef DEBUG_SERIALIZATION
		cout << "Schema for: " << SerializeClassClassName();
		MYSTL_ITERATE_LIST(m_listItemToSerialize,ItemToSerialize,pItem_cout,it_cout)
		{
			cout << " " << pItem_cout->m_iSerializeDataType;
		}
		cout << endl;
#endif
		MYSTL_ITERATE_LIST(m_listItemToSerialize,ItemToSerialize,pItem,it)
		{

			// These are self-serializing, we don't need the if( bWriting ) split.  However, these classes are 
			// derived from SerializeClass.  Therefore, we can't include the class definitions before binary
			// serialize.  We do a forward declaration, and have to put the actual switch block in a .cpp
			// file to avoid the cylcical dependencies
			if( SelfSerializing(bWriting,pItem) )
				continue;

			if( bWriting )
			{
#ifdef DEBUG_SERIALIZATION
				cout << "Writing type " << pItem->m_iSerializeDataType << " from class " << SerializeClassClassName() << " at pos: " << (int) CurrentSize() << endl;
#endif
				switch(pItem->m_iSerializeDataType)
				{
				case SERIALIZE_DATA_TYPE_CHAR:
					Write_char( *((char *) pItem->m_pItem) );
					break;
				case SERIALIZE_DATA_TYPE_SHORT:
					Write_short( *((short *) pItem->m_pItem) );
					break;
				case SERIALIZE_DATA_TYPE_LONG:
					Write_long( *((long *) pItem->m_pItem) );
					break;
				case SERIALIZE_DATA_TYPE_UNSIGNED_LONG:
					Write_unsigned_long( *((unsigned long *) pItem->m_pItem) );
					break;
				case SERIALIZE_DATA_TYPE_INT64:
					Write_int64( *((u_int64_t *) pItem->m_pItem) );
					break;
				case SERIALIZE_DATA_TYPE_STRING:
					Write_string( *((string *) pItem->m_pItem) );
					break;
#ifndef SYMBIAN
				case SERIALIZE_DATA_TYPE_VECT_STRING:
					{
						vector<string> *pVect = (vector<string> *) pItem->m_pItem;
						Write_unsigned_long((unsigned long) pVect->size());
#ifdef DEBUG_SERIALIZATION
						cout << "Writing " << (unsigned long) pVect->size() << " vector strings" << endl;
#endif
						for(size_t s=0;s<pVect->size();++s)
						{
							Write_string((*pVect)[s]);
						}
					}
					break;
				case SERIALIZE_DATA_TYPE_INT_STRING:
					{
						map<int,string> *pMap = (map<int,string> *) pItem->m_pItem;
						Write_unsigned_long((unsigned long) pMap->size());
#ifdef DEBUG_SERIALIZATION
						cout << "Writing " << (unsigned long) pMap->size() << " map int strings" << endl;
#endif
						map<int,string>::iterator it;
						for(it=pMap->begin();it!=pMap->end();++it)
						{
							Write_long( (*it).first );
							Write_string( (*it).second );
						}
					}
					break;
#endif
				default:
					{
						if( !UnknownSerialize(pItem,bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
						{
#ifdef DEBUG_SERIALIZATION
							cout << "Unknown Serialize failed" << endl;
#else
	
#ifndef SYMBIAN							
							throw ("Don't know how to serialize data type" + StringUtils::itos(pItem->m_iSerializeDataType)).c_str();
#else
							//TODO: Popup a error message
#endif

#endif
							return false; // We don't know how to serialize this type
						}
					}
				};
			}
			else
			{
#ifdef DEBUG_SERIALIZATION
				cout << "Reading type " << pItem->m_iSerializeDataType << " from class " << SerializeClassClassName() << " at pos:" << (int) CurrentSize() << endl;
#endif
				switch(pItem->m_iSerializeDataType)
				{
				case SERIALIZE_DATA_TYPE_CHAR:
					*((char *) pItem->m_pItem)=Read_char();
					break;
				case SERIALIZE_DATA_TYPE_SHORT:
					*((short *) pItem->m_pItem)=Read_short();
					break;
				case SERIALIZE_DATA_TYPE_LONG:
					*((long *) pItem->m_pItem)=Read_long();
					break;
				case SERIALIZE_DATA_TYPE_UNSIGNED_LONG:
					*((unsigned long *) pItem->m_pItem)=Read_unsigned_long();
					break;
				case SERIALIZE_DATA_TYPE_INT64:
					*((u_int64_t *) pItem->m_pItem)=Read_int64();
					break;
				case SERIALIZE_DATA_TYPE_STRING:
					Read_string( *((string *) pItem->m_pItem) );
					break;
#ifndef SYMBIAN
				case SERIALIZE_DATA_TYPE_VECT_STRING:
					{
						vector<string> *pVect = (vector<string> *) pItem->m_pItem;
						unsigned long count = Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
						cout << "Reading " << count << " vector strings" << endl;
#endif
						for(size_t s=0;s<count;++s)
						{
							string value;
							Read_string(value);
							pVect->push_back(value);
						}
					}
					break;
				case SERIALIZE_DATA_TYPE_INT_STRING:
					{
						map<int,string> *pMap = (map<int,string> *) pItem->m_pItem;
						unsigned long count=Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
						cout << "Reading " << count << " map int strings" << endl;
#endif
						for(size_t s=0;s<count;++s)
						{
							int Key = Read_long();
							string value;
							Read_string(value);
							(*pMap)[Key]=value;
						}
					}
					break;
#endif
				default:
					{
						if( !UnknownSerialize(pItem,bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
						{
#ifdef DEBUG_SERIALIZATION
							cout << "Unknown Serialize failed" << endl;
#else

#ifndef SYMBIAN							
							throw ("Don't know how to serialize data type" + StringUtils::itos(pItem->m_iSerializeDataType)).c_str();
#else
							//TODO: Popup a error message
#endif
							
#endif
							return false;
						}
					}
				};
			}
		}

		// We may have re-allocated the memory block and size, and the position will have changed
		pDataBlock=m_pcDataBlock; iAllocatedSize=m_dwAllocatedSize; pCurrentPosition=m_pcCurrentPosition;
		return true;
	}


	// When the class wants to continue reading/writing it will call this
	void ResumeReadWrite(char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)
	{
		m_pcDataBlock = pDataBlock;
		m_dwAllocatedSize = AllocatedSize;
		m_pcCurrentPosition = CurrentPosition;
	}

	void CheckWrite(unsigned long size)
	{
		unsigned long CurrentSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
		if( m_dwAllocatedSize-CurrentSize < size )
		{
			m_pcDataBlock = (char *) realloc(m_pcDataBlock,m_dwAllocatedSize + size + BLOCK_SIZE);
			m_dwAllocatedSize += size + BLOCK_SIZE;
			m_pcCurrentPosition = m_pcDataBlock + CurrentSize;
		}
	}

	bool CheckRead(unsigned long size)
	{
		unsigned long CurrentSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
		return  !(m_dwAllocatedSize-CurrentSize < size);
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
		*pl = v;
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
		*pl = v;
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

		//TODO!!!!!!!!!!!!!!
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

//TODO!!!!!!!!!!!!!!!		
unsigned char v1 = ((unsigned char *)pl)[0];
unsigned char v2 = ((unsigned char *)pl)[1];
unsigned char v3 = ((unsigned char *)pl)[2];
unsigned char v4 = ((unsigned char *)pl)[3];

long myval2 = (v4 << 24) + (v3 << 16) + (v2 << 8) + v1;

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
#ifdef DEBUG_SERIALIZATION
		cout << "Reading string " << str << " size: " << (int) CurrentSize() << endl;
#endif
	}

	char *Read_block(unsigned long size)
	{
		if( !CheckRead(size) )
			return NULL;

		char *ptr = (char *)malloc(size);
		memcpy(ptr,m_pcCurrentPosition,size);
		m_pcCurrentPosition += size;
		return ptr;
	}
};

class SafetyWrite
{
	SerializeClass *m_pSerializeClass;

public:
	SafetyWrite(SerializeClass *pSerializeClass) 
	{ 
		m_pSerializeClass=pSerializeClass;
		m_pSerializeClass->FreeSerializeMemory();
		m_pSerializeClass->StartWriting();
	}
	~SafetyWrite() 
	{ 
		m_pSerializeClass->FreeSerializeMemory();
	}
};


#endif
