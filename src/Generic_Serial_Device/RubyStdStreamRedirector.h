/***************************************************************************
 *   Copyright (C) 2005 by Igor Spac,,,                                    *
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
#ifndef EMBRUBYRUBYSTDSTREAMREDIRECTOR_H
#define EMBRUBYRUBYSTDSTREAMREDIRECTOR_H

namespace EMBRUBY {

/**
@author Igor Spac,,,
*/

#define HF_STDIN 	0
#define HF_STDOUT 	1
#define HF_STDERR 	2

class RubyStdStreamRedirector{
public:
    RubyStdStreamRedirector();
    RubyStdStreamRedirector(int stdstream);
    ~RubyStdStreamRedirector();
	
	void Open(int stdstream);
	void Close();
	
public:
	operator int() {
		return rstream_;
	}
	
	int getOriginalStream() {
		return savstream_;
	}

private:
	int rstream_; // redirected tail
	int ostream_; // the other tail
	
	int savstream_;
};

};

#endif
