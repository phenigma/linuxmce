#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#ifndef WIN32 // we only have signals on Linux and hte global var is only used there. so we ifndef it..
#include "App_Server.h"

using namespace DCE;

void SignalHandler_Start(App_Server *pApp_Server);
void SignalHandler_Stop();
#else /*ifndef WIN32*/
#define SignalHandler_Start (void)
#define SignalHandler_Stop (void)
#endif /*ifndef WIN32*/

#endif /*ifndef SIGNAL_HANDLER_H*/
