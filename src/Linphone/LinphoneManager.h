//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCELINPHONEMANAGER_H
#define DCELINPHONEMANAGER_H

#include <string>

struct _LinphoneCore;

namespace DCE {

/**
@author Igor Spac,,,
*/

class LinphoneMangerInterceptor;

class LinphoneManager {
public:
    LinphoneManager();
    ~LinphoneManager();

public:
	void setHost(const char* host) {
		host_ = host;
	}
	const char* getHost() {
		return host_.c_str();
	}
	
	void setPhoneNum(const char* phonenum) {
		phonenum_ = phonenum;
	}
	const char* getPhoneNum() {
		return phonenum_.c_str();
	}

public: 
	// have only on one interceptor for speed :) reasons
	void regInterceptor(LinphoneMangerInterceptor* pint) { 
		pint_ = pint;
	}	
	
	void unregInterceptor(LinphoneMangerInterceptor* pint) {
		pint_ = NULL;
	}	

public:
	bool Open();
	void Close();

	bool Invite(std::string extension);
	bool Answer();
	void Drop();
	
public:
    static void Linphone_INVITE_CB(struct _LinphoneCore *lc, char *from);
    static void Linphone_BYE_CB(struct _LinphoneCore *lc, char *from);
    static void Linphone_STATUS_CB(struct _LinphoneCore *lc, char *something);
    static void Linphone_WARNING_CB(struct _LinphoneCore *lc, char *something);
    static void Linphone_URL_CB(struct _LinphoneCore *lc, char *something, char *url);
    static void Linphone_NOOP_CB();
		
private:
	LinphoneMangerInterceptor* pint_;
private:
	struct _LinphoneCore* linphone_;

	std::string host_;
	std::string phonenum_;
	std::string generateConfigFile();
};

class LinphoneMangerInterceptor {
	friend class LinphoneManger;
public:
	virtual void OnConnected() = 0;
	virtual void OnFailConnecting() = 0;
	virtual void OnInvite() = 0;
};

};

#endif
