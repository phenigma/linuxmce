/***************************************************************************
 *   Copyright (C) 2005 by igor                                            *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "ldapentrycollection.h"

namespace LDAPSERVER {

LDAPEntryCollection::LDAPEntryCollection(LDAP *pldap, LDAPMessage *pce)
	: pldap_(pldap), pce_(pce)
{
}


LDAPEntryCollection::~LDAPEntryCollection() {
	printf("LDAPEntryCollection destroyed!\n");
	if(pce_ != NULL) {
		ldap_msgfree(pce_);
	}
}

LDAPEntryPtr 
LDAPEntryCollection::First() {
	LDAPEntry* pret = NULL;
	
	if(pce_ != NULL)  {
		LDAPMessage *pe = ldap_first_entry( pldap_, pce_ );
		if(pe !=  NULL) {
			pret = new LDAPEntry(pldap_, pe);
		}
	}
	
	return LDAPEntryPtr(pret);
}

LDAPEntryPtr 
LDAPEntryCollection::Next(LDAPEntryPtr ple) {
	LDAPEntry* pret = NULL;
	
	if(pce_ != NULL)  {
		LDAPMessage *pe = ldap_next_entry( pldap_, ple->GetLDAPMsg() );
		if(pe !=  NULL) {
			pret = new LDAPEntry(pldap_, pe);
		}
	}
	
	return LDAPEntryPtr(pret);
}

};