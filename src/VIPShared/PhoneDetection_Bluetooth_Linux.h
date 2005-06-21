#ifndef PhoneDetection_Bluetooth_Linux_h
#define PhoneDetection_Bluetooth_Linux_h

#include "PhoneDetectionEngine.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


#ifndef EVT_INQUIRY_RESULT_WITH_RSSI
#define EVT_INQUIRY_RESULT_WITH_RSSI	0x22
typedef struct {
	bdaddr_t	bdaddr;
	uint8_t		pscan_rep_mode;
	uint8_t		pscan_period_mode;
	uint8_t		dev_class[3];
	uint16_t	clock_offset;
	uint8_t		rssi;
} __attribute__ ((packed)) inquiry_info_with_rssi;
#define INQUIRY_INFO_WITH_RSSI_SIZE 14
#else
#undef  EVT_INQUIRY_RESULT_WITH_RSSI
#define EVT_INQUIRY_RESULT_WITH_RSSI	0x22
#endif

class PhoneDetection_Bluetooth_Linux : public PhoneDetectionEngine
{
	struct hci_dev_info m_DevInfo;
	string m_sDongleMacAddress;

public:
	PhoneDetection_Bluetooth_Linux();

	virtual int GetLinkQuality(const char *addr);	   

protected:
	// This function needs to be overridden 
	virtual bool ScanningLoop();
};
#endif

