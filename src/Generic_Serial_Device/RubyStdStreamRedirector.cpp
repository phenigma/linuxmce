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
#include "RubyStdStreamRedirector.h"

#include <unistd.h>

namespace EMBRUBY {

RubyStdStreamRedirector::RubyStdStreamRedirector() {
	savstream_ = rstream_ = ostream_ = -1;
}

RubyStdStreamRedirector::RubyStdStreamRedirector(int stdstream) {
	Open(stdstream);
}

void 
RubyStdStreamRedirector::Open(int stdstream) {
	int pipes[2];
	pipe(pipes);
	
	savstream_ = dup(stdstream);
	
	switch(stdstream) {
	case HF_STDIN:
		rstream_ = pipes[1];
		ostream_ = pipes[0];
		dup2(ostream_, HF_STDIN);
		break;
	case HF_STDOUT:
		rstream_ = pipes[0];
		ostream_ = pipes[1];
		dup2(ostream_, HF_STDOUT);
		break;
	case HF_STDERR:
		rstream_ = pipes[0];
		ostream_ = pipes[1];
		dup2(ostream_, HF_STDERR);
		break;
	default:
		rstream_ = pipes[0];
		ostream_ = pipes[1];
		break;
	}
}

RubyStdStreamRedirector::~RubyStdStreamRedirector() {
	Close();
}

void 
RubyStdStreamRedirector::Close() {
	close(rstream_);
	close(ostream_);
}

};
