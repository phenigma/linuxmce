#ifndef VIPINCLUDES_H
#define VIPINCLUDES_H

#include "PlutoUtils/MyStl.h"

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/MultiThreadIncludes.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#else // Symbian stuff
#include <eikenv.h>
#include <e32std.h>
//#define string TBuf8<255>
#define u_int64_t TInt64
#define malloc(x) User::Alloc(x)
#define free(x) User::Free(x)
#define realloc(x,y) User::ReAlloc(x,y)
#define memcpy Mem::Copy
#endif

// Used by Customer and VR_IdentifyPhone
class VIPPhoneNumber
{
public:
	unsigned char m_iPhoneNumberType;
	string m_sNumber;
};

class MiscVIPAttribute
{
public:
	string m_sType,m_sValue;
	MiscVIPAttribute(string Type,string Value) { m_sType=Type; m_sValue=Value; }
};


class BasketOption
{
public:
	string m_sID,m_sDescription;
	long m_iCost,m_iQty;

	BasketOption() {};
	BasketOption (string ID,string Description,long Qty, long Cost) { m_sID=ID; m_sDescription=Description; m_iQty=Qty; m_iCost=Cost; }
};

class BasketItem
{
public:
	string m_sID,m_sDescription;
	long m_iCost,m_iQty;

	MYSTL_CREATE_LIST(m_listBasketOptions,BasketOption);

	BasketItem() {};
	BasketItem (string ID,string Description,long Qty, long Cost) { m_sID=ID; m_sDescription=Description; m_iQty=Qty; m_iCost=Cost; }

	~BasketItem() {
		MYSTL_ITERATE_LIST(m_listBasketOptions,BasketOption,pBO,it)
		{
			delete pBO;
		}
	}
};

typedef list<class BasketItem *> ListBasketItem;

// Phone Status Codes
#define C_PHONESTATUS_NORMAL_CONST		1
#define C_PHONESTATUS_LOST_CONST		2
#define C_PHONESTATUS_RELEASED_CONST	3
#define C_PHONESTATUS_NEW_PHONE_CONST	4
#endif

