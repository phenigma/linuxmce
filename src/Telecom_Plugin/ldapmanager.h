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
#ifndef LDAPMANAGER_H
#define LDAPMANAGER_H

/**
@author igor
*/

#include <string>
#include <ldap.h>

#include "ldapexception.h"
#include "ldapentrycollection.h"

namespace LDAPSERVER {

class LDAPManager
{
public:
    LDAPManager(const char *host, unsigned int port = 389);
    ~LDAPManager();
	
	void SimpleBind(const char* who, const char* cred) throw(LDAPException);
	void Unbind();
	
	LDAPEntryCollectionPtr Search(const char *base, unsigned scope, 
				const char *filter, char *attrs[], bool attrsonly) throw(LDAPException);
	
private:
	std::string host_;
	unsigned int port_;
	
	LDAP *pldap_;
};

};

#endif
