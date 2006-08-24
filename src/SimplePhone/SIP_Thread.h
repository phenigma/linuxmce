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
