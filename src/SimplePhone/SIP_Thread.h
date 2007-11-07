/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
/* Linphone SIP (LS) */

#ifndef SIP_THREAD_H
#define SIP_THREAD_H

#include "SimplePhone.h"
using namespace DCE;

/* C Object, public methods - forward declaration */
void * LS_Thread(void * arg);

bool LS_ActiveCall();
void LS_SentDTMF(char cDTMF);
void LS_InitiateCall(string sNumber);
void LS_DropCall();
void LS_AcceptCall();

/* C Object, public members - forward declaration */
extern bool LS_bQuit;

#endif /* SIP_THREAD_H */
