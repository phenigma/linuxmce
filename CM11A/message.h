//
// C++ Interface: message
//
// Description: 
//
//
// Author: igor <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CM11ADEVMESSAGE_H
#define CM11ADEVMESSAGE_H

#include <string>

namespace CM11ADEV {

/**
@author igor
*/
class Message 
{
public:
    Message();
    ~Message();

public:
	void setAddress(unsigned char houseCode, unsigned char deviceCode) {
		houseCode_ = houseCode;
		deviceCode_ = deviceCode;
	}
	void setAddress(std::string address); // adress in the form of A1, B2,...
	
	unsigned char getHouseCode() const {
		return houseCode_; }
	unsigned char getDeviceCode() const {
		return deviceCode_; }
			
	void setFunctionCode(unsigned char functionCode) {
		functionCode_ = functionCode; }
	unsigned char getFunctionCode() const {
		return functionCode_; }
		
	void setDimmLevel(unsigned char dimmLevel) {
		dimmLevel_ = dimmLevel; }
	unsigned char getDimmLevel() const {
		return dimmLevel_; }
		

private:
	unsigned char houseCode_;
	unsigned char deviceCode_;
	unsigned char functionCode_;
	
	unsigned char dimmLevel_;	
	
private:
	static unsigned char s_MapHouseDeviceCodes[];
};

};

#endif
