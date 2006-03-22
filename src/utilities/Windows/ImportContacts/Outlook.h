#ifndef  OutlookWraper_h
#define  OutlookWraper_h

#include "Contacts.h"

class OutlookWraper
{
private:
	string errDesc;
public:
	int readContacts(ContactsList &list);
};

#endif