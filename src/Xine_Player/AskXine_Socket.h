#ifndef AskXine_Socket_h
#define AskXine_Socket_h

class AskXine_Socket : public ClientSocket
{
public:
	// more simple c-tor: we don't need deviceID here
	AskXine_Socket(string sIPAddress, string sName ):ClientSocket(0, sIPAddress, sName){};
	// overriding on connect event handler, because we use custom protocol (actually just reading)
	virtual bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" ){return true;};
};

#endif
