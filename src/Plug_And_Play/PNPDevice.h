#ifndef PNPDEVICE_H_
#define PNPDEVICE_H_

#include <string>

#define PNP_DEVICE_UNKNOWN -1
#define PNP_DEVICE_DETECTED 0
#define PNP_DEVICE_CONFIRMING_POSSIBLE_TEMPLATES 1
#define PNP_DEVICE_PROMPT_FOR_TEMPLATE 2
#define PNP_DEVICE_PROMPT_FOR_PARAMETERS 3
#define PNP_DEVICE_ADD_DEVICE 4
#define PNP_DEVICE_ADD_SOFTWARE 5
#define PNP_DEVICE_START_DEVICE 6
#define PNP_DEVICE_DONE 7

/**Class that holds the informations and the functions asociated with a 
 * phisical device*/
class PNPDevice
{
	
public:
	
	PNPDevice(std::string deviceID);

	virtual ~PNPDevice();

	int getState();
	
	void setState(int newState);
	
	std::string getID();

	void setSerial(std::string newSerial);

	std::string getSerial();

	void setPath(std::string newPath);

	std::string getPath();

	void setPnpProtocol(int newPnpProtocol);

	int getPnpProtocol();
	
protected:

	/**some information needed to identify the device class*/
	std::string ID;

	/**serial number associated with each physical device (if possible)*/
	std::string serial;
	
	/**the exact message send parameters*/
	std::string messageParameters;
	
	/**the configuration parameter (how to find a device). for USB, this is the 'usb-2/1-2:0', for ethernet it's the MAC address*/
	std::string path;

	/**the state of the discovery*/
	int state;

	/**the PNP protocol this device has responded to*/
	int pnpProtocol;
};

#endif /*PNPDEVICE_H_*/
