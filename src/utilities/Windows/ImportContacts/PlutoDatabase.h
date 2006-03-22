#pragma once


#include "Database.h"
#include "Contacts.h"

class PlutoDatabase
{
private:
	DatabaseWrapper *m_pContactsTable, *m_pPhoneTable;
	string m_connStr;

	// ContactTable
	vector<string> m_ContactParam, m_ContactValues;
	vector<bool> m_ContactType;

	// PhoneTable
	vector<string> m_PhoneParam, m_PhoneValues;
	vector<bool> m_PhoneType;

	bool writeContact(Contact *pContact);
	bool writePhoneNumber(string phoneNumber,int nType,int nUserID);

	// if exist return the id of Contact
	int existContact(Contact *pContact);

public:
	PlutoDatabase(void);
	~PlutoDatabase(void);

	int writeContacts(ContactsList &list);
};
