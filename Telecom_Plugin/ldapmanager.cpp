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

#include "ldapmanager.h"

namespace LDAPSERVER {

LDAPManager::LDAPManager(const char *host, unsigned int port)
 : host_(host), port_(port) 
{
  pldap_ = ldap_init(host, port);
}

LDAPManager::~LDAPManager() {
}

void 
LDAPManager::SimpleBind(const char* who, const char* cred) 
	throw(LDAPException) 
{
	if(!pldap_ || ldap_simple_bind_s(pldap_, who, cred) != LDAP_SUCCESS) {
		throw LDAPException("Error binding");
	}
}

void
LDAPManager::Unbind() {
	if(pldap_ != NULL) {
		ldap_unbind(pldap_);
	}
}

LDAPEntryCollectionPtr
LDAPManager::Search(const char *base, unsigned scope, const char *filter, char *attrs[], bool attrsonly) 
	throw(LDAPException) 
{
	LDAPMessage *pres = NULL;
	if(pldap_ != NULL) {
  		if(ldap_search_s(pldap_, base, scope, filter, attrs, attrsonly, &pres)
	  		!= LDAP_SUCCESS) {
			throw LDAPException("Search failed");
		}
	}
	
	return LDAPEntryCollectionPtr(new LDAPEntryCollection(pldap_, pres));
}

};
