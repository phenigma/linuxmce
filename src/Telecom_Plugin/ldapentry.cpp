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
#include "ldapentry.h"

using namespace std;

namespace LDAPSERVER {

LDAPEntry::LDAPEntry(LDAP* pldap, LDAPMessage* pe)
	: pe_(pe)
{
	char *pdn = ldap_get_dn( pldap, pe );
	dn_ = pdn;
	ldap_memfree( pdn );
	
	BerElement* pber = NULL;
	for(char* name = ldap_first_attribute(pldap, pe, &pber); 
						name != NULL; name = ldap_next_attribute(pldap, pe, pber)) 
	{
		attrs_.push_back(LDAPAttribute(pldap, pe, name));
		ldap_memfree( name );
	}
	if(pber != NULL) {
		ber_free(pber, 0);
	}
}

LDAPEntry::~LDAPEntry()
{
}

};
