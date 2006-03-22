//#include "StdAfx.h"
#include ".\contacts.h"

ContactsList::~ContactsList()
{
	vector<Contact *>::iterator it;
	for(it=begin();it!=end();it++)
		delete *it;

	clear();
}

void ContactsList::addContact( Contact *pContact)
{
	push_back( pContact );
};

