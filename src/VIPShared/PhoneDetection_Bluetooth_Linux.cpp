/*
 PhoneDetection_Bluetooth_Linux
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/MyStl.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "PhoneDetection_Bluetooth_Linux.h"
#include "PhoneDevice.h"

#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>
#include <sys/socket.h>
#include <list>
using namespace std;
using namespace DCE;

bdaddr_t bdaddr;

static volatile sig_atomic_t __io_canceled = 0;

static void sig_term(int sig)
{
	__io_canceled = 1;
}

PhoneDetection_Bluetooth_Linux::PhoneDetection_Bluetooth_Linux() 
	: PhoneDetectionEngine() 
{
}

bool PhoneDetection_Bluetooth_Linux::ScanningLoop()
{
	g_pPlutoLogger->Write(LV_STATUS, "Start of scan loop");
	//printf("Start of scan loop %p\n",g_pPlutoLogger);
	int num_rsp, length, flags, dev_id = 0;

	length  = 2 * 8;	/* ~20 seconds */
	num_rsp = 100;
	flags   = 0;
	const uint8_t *lap=NULL;

	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	inquiry_info_with_rssi result, *inq_list;
	inquiry_info *info;
	hci_event_hdr *hdr;
	evt_cmd_status *cs;
	inquiry_cp cp;
	struct hci_filter flt, old_flt;
	struct sigaction sa, sa_term, sa_int;
	struct pollfd p;
	int i, dd, len, err = 0, cancel = 1, cur_rsp = 0;

	/* Check for unlimited responses */
	if (num_rsp <= 0)
		num_rsp = 255;
/*  this code would be needed if we wanted to support multiple dongles and specify the correct one by mac address
	dev_id = hci_devid(m_sDongleMacAddress.c_str());
	if (dev_id < 0) 
	{
		dev_id = hci_get_route(NULL);
		if (dev_id < 0)
		{
			printf("Can't get an ID for bluetooth dongle.");
			return;
		}
		else
		{
			printf("Bluetooth dongle %s not found, using hci0.", m_sDongleMacAddress.c_str());
		}
	}
*/
	/* Open the HCI device */
	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		//printf("Error opening bluetooth device");
		g_pPlutoLogger->Write(LV_CRITICAL, "Error opening bluetooth device");
		Sleep(15000);  // Don't get in a vicous loop, wait 15 seconds
	}

	m_DevInfo.dev_id = dev_id;
	if (ioctl(dd, HCIGETDEVINFO, (void*) &m_DevInfo))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Error opening bluetooth device");
		hci_close_dev(dd);
		return false;
	}
	char addr[18];
	ba2str(&m_DevInfo.bdaddr, addr);
	g_pPlutoLogger->Write(LV_STATUS,"Attached to BT adapter: %s\t%s\n", m_DevInfo.name, addr);
	hci_close_dev(dd);
  	
	dd = hci_open_dev(dev_id); // Todo: figure out why I need to keep opening and closing the device
	if (dd < 0) 
	{
		printf("Device open failed");
		return false;
	}

	//printf("opened device\n");

	/* Allocate memory for inquiry results */
	inq_list = (inquiry_info_with_rssi *) malloc(sizeof(inquiry_info_with_rssi) * num_rsp);
	if (!inq_list) {
		err = errno;
		goto close;
	}

	/* Store the current event filter of this HCI socket */
	len = sizeof(old_flt);
	if (getsockopt(dd, SOL_HCI, HCI_FILTER, &old_flt, (socklen_t *)&(len) ) < 0) {
		err = errno;
		goto close;
	}

	/* Replace the event filter with our needs */
	hci_filter_clear(&flt);
	hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
	hci_filter_set_event(EVT_INQUIRY_RESULT, &flt);
	hci_filter_set_event(EVT_INQUIRY_RESULT_WITH_RSSI, &flt);
	hci_filter_set_event(EVT_INQUIRY_COMPLETE, &flt);
	hci_filter_set_event(EVT_CMD_STATUS, &flt);
	if (setsockopt(dd, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
		err = errno;
		goto close;
	}

	/* Redirect SIGTERM and SIGINT to our function */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_term;
	sigaction(SIGTERM, &sa, &sa_term);
	sigaction(SIGINT,  &sa, &sa_int);

	/* Send an inquiry command to the device */
	memset(&cp, 0, INQUIRY_CP_SIZE);
	if (lap) {
		memcpy(cp.lap, lap, 3);
	} else {
		cp.lap[0] = 0x33;
		cp.lap[1] = 0x8b;
		cp.lap[2] = 0x9e;
	}

	printf("# ready to send command\n");

	cp.length = length;
	cp.num_rsp = num_rsp;
	if (hci_send_cmd(dd, OGF_LINK_CTL, OCF_INQUIRY, INQUIRY_CP_SIZE, &cp) < 0) {
		err = errno;
		goto done;
	}

	p.fd = dd;
	p.events = POLLIN | POLLERR | POLLHUP;

	m_mapDevicesDetectedThisScan.clear();

	g_pPlutoLogger->Write(LV_WARNING, "Inquiry started");	
	
	while (!__io_canceled && cancel) {
		p.revents = 0;
		if (poll(&p, 1, 100) > 0) {
			printf("# in poll\n");
			/* Read the next HCI event (in H4 format) */
			len = read(dd, buf, sizeof(buf));
			printf("# got: %d\n",len);
			if (len < HCI_EVENT_HDR_SIZE + 1)
				continue;

			hdr = (hci_event_hdr *) (void *) (buf + 1);
			ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
			len -= (1 + HCI_EVENT_HDR_SIZE);

			switch (hdr->evt) {
			case EVT_INQUIRY_RESULT:
				printf("# inquiry result: %d\n",(int) ptr[0]);
				/* Get standard inquiry result, inqmode = 0 */
				for (i = 0; i < ptr[0]; i++) {
					info = (inquiry_info *) ((char *) ptr + (sizeof(*info) * i) + 1);
					bacpy(&result.bdaddr, &info->bdaddr);
					result.pscan_rep_mode    = info->pscan_rep_mode;
					result.pscan_period_mode = info->pscan_period_mode;
					memcpy(result.dev_class, info->dev_class, 3);
					result.clock_offset = info->clock_offset;
					result.rssi = 0;

					char addr[18];
					ba2str(&(info+i)->bdaddr, addr);

					char name[248];
					memset(name, 0, sizeof(name));

// TODO - HACK !!!  SEE BELOW For some reason when I do this, I can only scan once and all future scans fail
//                  if (hci_read_remote_name(dd, &(info+i)->bdaddr, sizeof(name), name, 100000) < 0)

					PhoneDevice *pDNew = new PhoneDevice(name,addr,result.rssi);
					bacpy(&pDNew->m_bdaddrDongle, &m_DevInfo.bdaddr);
					g_pPlutoLogger->Write(LV_STATUS, "Device %s responded.", pDNew->m_sMacAddress.c_str());
					
					PLUTO_SAFETY_LOCK(mm,m_MapMutex);
					m_mapDevicesDetectedThisScan[pDNew->m_iMacAddress] = pDNew;

					PhoneDevice *pDExisting = m_mapPhoneDevice_Detected_Find(pDNew->m_iMacAddress);
					if( pDExisting && abs(pDExisting->m_iLinkQuality-pDNew->m_iLinkQuality)<10 )
					{
						// nothing to do
						delete pDNew;
						continue;
					}
					mm.Release();

					if( !pDExisting )
					{
						g_pPlutoLogger->Write(LV_STATUS, "Detected new device: %s  %d", pDNew->m_sMacAddress.c_str(), pDNew->m_iLinkQuality);						
// TODO - HACK !!!  SEE ABOVE.  HACK IN A NASTY FIX UNTIL WE FIGURE OUT WHY BLUEZ IS LOCKING UP
g_pPlutoLogger->Write(LV_WARNING, "Executing /tmp/hciscan hack");
system("hcitool scan > /tmp/hciscan");
vector<string> vectstr;
FileUtils::ReadFileIntoVector( "/tmp/hciscan", vectstr );
g_pPlutoLogger->Write(LV_WARNING, "got %d lines",(int) vectstr.size());
for(size_t s=0;s<vectstr.size();++s)
{
g_pPlutoLogger->Write(LV_WARNING, "comparing %s and %s",vectstr[s].c_str(),pDNew->m_sMacAddress.c_str());
string sLine = StringUtils::ToUpper(vectstr[s]);
if( sLine.find(StringUtils::ToUpper(pDNew->m_sMacAddress))!=string::npos )
{
g_pPlutoLogger->Write(LV_WARNING, "line %s matched %d",sLine.c_str(),(int) sLine.size());
size_t s = vectstr[s].length()-15;
if( s<0 )
s=0;
pDNew->m_sID = vectstr[s].substr(s);
g_pPlutoLogger->Write(LV_WARNING, "set name to %d %s",(int) s,pDNew->m_sID.c_str());
}
}
						Intern_NewDeviceDetected(pDNew);
					}
					else
					{
						g_pPlutoLogger->Write(LV_STATUS, "Detected change in strength device: %s  %d  from: %d",pDNew->m_sMacAddress.c_str(),pDNew->m_iLinkQuality);
						//printf("Detected change in strength device: %s  %d  from: %d\n",pDNew->m_sMacAddress.c_str(),pDNew->m_iLinkQuality,pDExisting->m_iLinkQuality);
						Intern_SignalStrengthChanged(pDNew);
					}

//					cur_rsp = add_result(list, cur_rsp, num_rsp, &result);
				}
				break;

			case EVT_INQUIRY_RESULT_WITH_RSSI:
				printf("inquiry result with rssi: %d\n",(int) ptr[0]);
				/* Get inquiry result with RSSI, inqmode = 1 */
				for (i = 0; i < ptr[0]; i++) {
					memcpy(&result, ptr + (sizeof(result) * i) + 1, sizeof(result));
//					cur_rsp = add_result(list, cur_rsp, num_rsp, &result);

					printf("got result with rssi\n");
					
					char addr[18];
					ba2str(&result.bdaddr, addr);

					char name[248];
					memset(name, 0, sizeof(name));
// For some reason when I do this, I can only scan once and all future scans fail
//                  if (hci_read_remote_name(dd, &(info+i)->bdaddr, sizeof(name), name, 100000) < 0)
                        strcpy(name, "see notes in code");

					PhoneDevice *pDNew = new PhoneDevice(name,addr,result.rssi);
					bacpy(&pDNew->m_bdaddrDongle, &m_DevInfo.bdaddr);
					
					PLUTO_SAFETY_LOCK(mm,m_MapMutex);
					m_mapDevicesDetectedThisScan[pDNew->m_iMacAddress] = pDNew;

					PhoneDevice *pDExisting = m_mapPhoneDevice_Detected_Find(pDNew->m_iMacAddress);
					if( pDExisting && abs(pDExisting->m_iLinkQuality-pDNew->m_iLinkQuality)<10 )
					{
						// nothing to do
						delete pDNew;
						continue;
					}
					mm.Release();

					if( !pDExisting )
					{
						g_pPlutoLogger->Write(LV_STATUS, "Detected new device: %s  %d\n", pDNew->m_sMacAddress.c_str(), pDNew->m_iLinkQuality);
						//printf("Detected new device: %s  %d\n",pDNew->m_sMacAddress.c_str(),pDNew->m_iLinkQuality);
						Intern_NewDeviceDetected(pDNew);
					}
					else
					{
  					    g_pPlutoLogger->Write(LV_STATUS, "Detected change in strength device: %s  %d  from: %d\n",pDNew->m_sMacAddress.c_str(),pDNew->m_iLinkQuality,pDExisting->m_iLinkQuality);
						Intern_SignalStrengthChanged(pDNew);
					}
				}
				break;

			case EVT_INQUIRY_COMPLETE:
				{
					/* The inquiry ended, because of time or number of responses */
					cancel = 0;
				
					list<PhoneDevice *> listDevicesLost;

					//Make a list of all the devices that were lost this scan
					PLUTO_SAFETY_LOCK(mm, m_MapMutex);

					printf("# Devices detected last scan: %d\n", m_mapPhoneDevice_Detected.size());
					printf("# Devices detected this scan: %d\n", m_mapDevicesDetectedThisScan.size());

					map<u_int64_t,class PhoneDevice *>::iterator itDevice;
					for(itDevice=m_mapPhoneDevice_Detected.begin();itDevice!=m_mapPhoneDevice_Detected.end();)
					{
						class PhoneDevice *pD = (*itDevice).second;
						map<u_int64_t,class PhoneDevice *>::iterator itDeviceNew = m_mapDevicesDetectedThisScan.find(pD->m_iMacAddress);
						if( /*itDeviceNew==m_mapDevicesDetectedThisScan.end() ||*/ !pD->m_bIsConnected)
						{
							listDevicesLost.push_back( (*itDevice).second );
							m_mapPhoneDevice_Detected.erase(itDevice++);
						}
						else
							itDevice++;
					}

					mm.Release();

					printf("# Devices lost this scan: %d\n", listDevicesLost.size());

					
					list<PhoneDevice *>::iterator itLost;
					for(itLost = listDevicesLost.begin();itLost != listDevicesLost.end();++itLost)
					{
						g_pPlutoLogger->Write(LV_STATUS, "Lost connection to device: %s", (*itLost)->m_sMacAddress.c_str());
						Intern_LostDevice(*itLost);
					}
	
					g_pPlutoLogger->Write(LV_WARNING, "Inquiry complete\n");
				}
				break;

			case EVT_CMD_STATUS:
				printf("# got a status\n");
				cs = (evt_cmd_status *) ptr;
				/* This happens when the inquiry command fails */
				if (cs->opcode == cmd_opcode_pack(OGF_LINK_CTL, OCF_INQUIRY)
						&& cs->status != 0) 
				{
					err = bt_error(cs->status); 
					cancel = 0;
				}
				break;
			}
		}
	}

	/* If we exit through a signal, cancel our current inquiry */
	if (cancel)
	{
		printf("# sending cancel\n");
		hci_send_cmd(dd, OGF_LINK_CTL, OCF_INQUIRY_CANCEL, 0, NULL);
	}

done:
	/* Restore the old event filter */
	setsockopt(dd, SOL_HCI, HCI_FILTER, &old_flt, sizeof(old_flt));

	/* Restore the old signal handlers */
	sigaction(SIGTERM, &sa_term, NULL);
	sigaction(SIGINT,  &sa_int,  NULL);

	//printf("restored signal handlers\n");

close:
	/* Close the HCI device */
	hci_close_dev(dd);

	printf("# closed device\n");

	/* Set the errno value for perror() */
	errno = err;

	/* On error, free the results and exit */
	if (err) {
		if (inq_list) free(inq_list);
		g_pPlutoLogger->Write(LV_WARNING, "Error in bluetooth scan: %d\n", err);
		//cerr << "Error " << err << " in bluetooth scan" << endl;
		Sleep(15000); // No fast looping
		return true;
	}

	/* Return the result pointer and the number of results */
	return true;
}
/*
void PhoneDetection_Bluetooth_Linux::GetListOfDetectedDevices()
{
	char addr[18];
	char name[248];
	
	struct sockaddr_rc loc_addr;
	struct sockaddr_in iploc_addr;
	int pf;
	int proto;
	int err;

	pf=PF_BLUETOOTH;                                 
	proto=BTPROTO_RFCOMM;

	system("hciconfig hci0 up");
	system("modprobe rfcomm");
	Sleep(1000);
	system("sdpd");
	Sleep(500);
	
	int dev_id;
	dev_id = hci_devid(m_sDongleMacAddress.c_str());
	if (dev_id < 0) 
	{
		dev_id = hci_get_route(NULL);
		if (dev_id < 0)
		{
			printf("Can't get an ID for bluetooth dongle.");
			return;
		}
		else
		{
			printf("Bluetooth dongle %s not found, using hci0.", m_sDongleMacAddress.c_str());
		}
	}

	int dd = hci_open_dev(dev_id);
	if (dd < 0) 
	{
		printf("Device open failed");
		return;
	}
	m_DevInfo.dev_id = dev_id;
	if (ioctl(dd, HCIGETDEVINFO, (void*) &m_DevInfo))
	{
		printf("Can't get info about device");
		return;
	}
	ba2str(&m_DevInfo.bdaddr, addr);
	printf("Attached to BT adapter: %s\t%s\n", m_DevInfo.name, addr);
	close(dd);
  	
	dd = hci_open_dev(dev_id); // Todo: figure out why I need to keep opening and closing the device
	if (dd < 0) 
	{
		printf("Device open failed");
		return;
	}
	
	inquiry_info *info = NULL;
	int num_rsp = hci_inquiry(dev_id, 8, 100, NULL, &info, 0);
	if (num_rsp < 0) 
	{
		printf("Inquiry failed");
		close(dd);
		return;
	}
	for (int i = 0; i < num_rsp; i++)
	{
		memset(name, 0, sizeof(name));
		if (hci_read_remote_name(dd, &(info+i)->bdaddr, sizeof(name), name, 100000) < 0)
			strcpy(name, "n/a");
		ba2str(&(info+i)->bdaddr, addr);
			printf("\t%s\t%s\t", addr, name);

		PhoneDevice *pD = new PhoneDevice(name,addr,0);
		bacpy(&pD->m_bdaddrDongle, &m_DevInfo.bdaddr);
		m_listPhoneDevice.push_back(pD);

		// Get the link quality
	}
	free(info);
	close(dd);
}
*/
	/*
void PhoneDetection_Bluetooth_Linux::GetLinkQuality()
{
	bool bMadeConn = false;
	time_t t = time(NULL);
	struct hci_conn_info_req *cr;
	struct hci_request rq;
	
	while(time(NULL) < t + 4)
	{
		cr = (hci_conn_info_req *)malloc(sizeof(*cr) + sizeof(struct hci_conn_info));
		bacpy(&cr->bdaddr, &(info+i)->bdaddr);
		cr->type = ACL_LINK;
		if (ioctl(dd, HCIGETCONNINFO, (unsigned long) cr) < 0) 
		{
			if (bMadeConn == false)
			{
				create_conn_cp cp;
				memset(&cp, 0, sizeof(cp));
				bacpy(&cp.bdaddr,  &(info+i)->bdaddr);
				cp.pkt_type =  HCI_DM1 | HCI_DM3 | HCI_DM5 | HCI_DH1 | HCI_DH3 | HCI_DH5;
				if (hci_send_cmd(dd, OGF_LINK_CTL, OCF_CREATE_CONN, CREATE_CONN_CP_SIZE, &cp)) 
				{
					printf("Create connection failed");
					continue;
				}
				bMadeConn = true;
				Sleep(1000);
			}
			free(cr);
			continue;					
		}
		get_link_quality_rp rp;
		memset(&rq, 0, sizeof(rq));
		rq.ogf    = OGF_STATUS_PARAM;
		rq.ocf    = OCF_GET_LINK_QUALITY;
		rq.cparam = &cr->conn_info->handle;
		rq.clen   = 2;
		rq.rparam = &rp;
		rq.rlen   = GET_LINK_QUALITY_RP_SIZE;
		free(cr);
				
		if (hci_send_req(dd, &rq, 100) < 0) {
			printf("HCI get_link_quality request failed");
			Sleep(500);
				continue;
		}
	
		if (rp.status) {
			//g_pPlutoLogger->Write(LV_WARNING,"HCI get_link_quality cmd failed (0x%2.2X)\n", 	rp.status);
			Sleep(500);
			close(dd);
			continue;
		}
		break;
	}
}
*/	
