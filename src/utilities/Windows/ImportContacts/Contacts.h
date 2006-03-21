#pragma once

#include "string"
#include "vector"

using namespace std;

struct Contact
{
public:
	string firstName, middleName, lastName;
	string title, companyName;

	string phoneHome, homeBusiness, phoneMobile;
	string homeAddress, businessAddress;
	string email;
};

class ContactsList : public vector<Contact *>
{
public:

	~ContactsList();
	void addContact( Contact *pContact);
};

