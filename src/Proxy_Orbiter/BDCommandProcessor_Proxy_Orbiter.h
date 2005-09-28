/**
 *
 * @file BDCommandProcessor_Proxy_Orbiter.h
 * @brief header file for the BDCommandProcessor_Proxy_Orbiter class
 * @author
 * @todo notcommented
 *
 */

#ifndef BDCOMMANDPROCESSORProxy_Orbiter_H
#define BDCOMMANDPROCESSORProxy_Orbiter_H

//-----------------------------------------------------------------------------------------------------

#include "../BD/BDCommandProcessor_Windows_Socket.h"

//-----------------------------------------------------------------------------------------------------

/**
 * @brief implementation for the BDCommandProcessor specific to the Proxy_Orbiter
 * @todo ask
 */
class BDCommandProcessor_Proxy_Orbiter : public BDCommandProcessor_Windows_Socket
{
public:

	class Proxy_Orbiter *m_pProxy_Orbiter; /**< points to the proxy orbiter object for this command processor @todo better comment */

	BDCommandProcessor_Proxy_Orbiter( class Proxy_Orbiter *pProxy_Orbiter, string sMacAddressPhone, class PhoneDevice *pPhoneDevice, class Socket *pSocket);
};

//-----------------------------------------------------------------------------------------------------

#endif // __BTCOMMANDPROCESSOR_Proxy_Orbiter_H__
