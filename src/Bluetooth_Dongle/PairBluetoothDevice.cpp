#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>
#include <sys/socket.h> 

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h> 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#include <termios.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <netinet/in.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h> 

#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    bdaddr_t m_bdaddrDongle;

	if(argc != 2)
	{
		printf("PairBluetoothDevice <mac_address>\n\n");
		return 1;
	}

	string sMacAddressPhone(argv[1]);

	int dev_id = 0;
	struct hci_dev_info m_DevInfo; 
	int dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		printf("Error opening bluetooth device\n");
	}

	m_DevInfo.dev_id = dev_id;
	if (ioctl(dd, HCIGETDEVINFO, (void*) &m_DevInfo))
	{
		printf("Error opening bluetooth device\n");
		hci_close_dev(dd);
		return false;
	}
	char addr[18];
	ba2str(&m_DevInfo.bdaddr, addr);
	printf("Attached to BT adapter: %s\t%s\n", m_DevInfo.name, addr);
	hci_close_dev(dd);

	bacpy(&m_bdaddrDongle, &m_DevInfo.bdaddr);

	struct sockaddr_rc laddr, raddr;
	struct rfcomm_dev_req req;
	socklen_t alen;

	laddr.rc_family = AF_BLUETOOTH;
	bacpy(&laddr.rc_bdaddr, &m_bdaddrDongle);
	laddr.rc_channel = 0;

	printf("Mac address: %s\n", sMacAddressPhone.c_str());
	
	raddr.rc_family = AF_BLUETOOTH;
	str2ba(sMacAddressPhone.c_str(), &raddr.rc_bdaddr);
	raddr.rc_channel = 3;

	printf("Channel:  %d\n", raddr.rc_channel);

    bool m_bDead = false;
	int m_CommHandle;
	
	if ((m_CommHandle = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) 
	{
		printf("Can't create RFCOMM socketi\n");
		m_bDead=true;
		return -1;
	}

	if (bind(m_CommHandle, (struct sockaddr *)&laddr, sizeof(laddr)) < 0) 
	{
	    printf("Can't bind RFCOMM socket %s\n",sMacAddressPhone.c_str());
		close(m_CommHandle);
		m_bDead=true;
		return -1;
	}
	
    printf("Trying to connect...\n");
	connect(m_CommHandle, (struct sockaddr *)&raddr, sizeof(raddr)); 

	printf("Paring device... %s\n",sMacAddressPhone.c_str());
} 

