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
#ifndef LDAPENTRY_H
#define LDAPENTRY_H

/**
@author igor
*/

#include <ldap.h>
#include <memory>
#include <string>

#include "ldapattribute.h"

namespace LDAPSERVER {

typedef 
	std::vector<LDAPAttribute> LDAPATTRIBUTESVEC;

class LDAPEntry{
	friend class LDAPEntryCollection;

protected:
    LDAPEntry(LDAP* pldap, LDAPMessage* pe);
public:
    ~LDAPEntry();

public:
	inline LDAPMessage* 
	GetLDAPMsg() {
		return pe_;
	}
	
	inline const std::string&
	GetDN() {
		return dn_;
	}
	
	inline const LDAPATTRIBUTESVEC&
	GetAttrs() {
		return attrs_;
	}
	
protected:
	LDAPMessage *pe_;

	std::string dn_;
	LDAPATTRIBUTESVEC attrs_;
};

typedef std::auto_ptr<LDAPEntry> LDAPEntryPtr;

};

#endif
